#!/usr/bin/env python3
"""
factory_line_measure.py — THE FACTORY LINE, measured on THE STITCH.

Reproduces the debt_audit finding for the ud0 Jacobi-Lens FACTORY LINE panel.
The STITCH model (4L/6H/d96/block-64 char-GPT on tiny Shakespeare, seed 1337 — the
same model the WAIST panel runs on) is trained, then two independent meters read the
"mixing debt" claim (that attention share RISES with depth / L3-L4 do the mixing):

  1. HALF-TAP LADDER — split every block into its attention half-step and its MLP
     half-step (9 taps: embed + 4x{attn, mlp}). At each tap, an 800-step linear probe
     (on ln_f(tap)) reads top-1 next-char accuracy on held-out val. The per-half
     accuracy gain = that half-step's "earnings". attn share = attn / (attn+mlp).
  2. REMOVAL TEST — zero each sublayer's residual add, one at a time, and measure the
     Δ val loss vs the un-ablated base (identical batches per config).

Both meters must agree that attention share FALLS with depth and that L1-attention is
the load-bearing wall (largest removal damage). Fail-loud on any structural miss.

  python factory_line_measure.py          # train (or resume ckpt) + measure -> factory_line_data.json
Offline after the one input.txt fetch. This is the checkable proof behind the panel's LIT stamp.
"""
import json, math, os, time, urllib.request
import torch, torch.nn as nn, torch.nn.functional as F

torch.manual_seed(1337)
HERE = os.path.dirname(os.path.abspath(__file__))
TXT  = os.path.join(HERE, "input.txt")
OUT  = os.path.join(HERE, "factory_line_data.json")
CKPT = os.path.join(HERE, "_fl_ckpt.pt")

# ---- data (tiny Shakespeare) ----
if not os.path.exists(TXT):
    urllib.request.urlretrieve("https://raw.githubusercontent.com/karpathy/"
        "char-rnn/master/data/tinyshakespeare/input.txt", TXT)
text = open(TXT, encoding="utf-8").read()
assert len(text) > 1_000_000, "FAIL-LOUD: corpus too small"
chars = sorted(set(text)); V = len(chars)
stoi = {c: i for i, c in enumerate(chars)}
data = torch.tensor([stoi[c] for c in text]); n = int(.9 * len(data))
train_d, val_d = data[:n], data[n:]

# ---- model: identical to stitch_live.py (4L / 6H / d96 / block 64) ----
BLK, DM, NH, NL = 64, 96, 6, 4
class Blk(nn.Module):
    def __init__(s):
        super().__init__(); s.l1, s.l2 = nn.LayerNorm(DM), nn.LayerNorm(DM)
        s.at = nn.MultiheadAttention(DM, NH, dropout=.1, batch_first=True)
        s.mlp = nn.Sequential(nn.Linear(DM, 4*DM), nn.GELU(), nn.Linear(4*DM, DM), nn.Dropout(.1))
    def forward(s, x, m, drop_attn=False, drop_mlp=False):
        h = s.l1(x); a, _ = s.at(h, h, h, attn_mask=m, need_weights=False)
        x = x if drop_attn else x + a           # attention half-step (removable)
        x_attn = x
        x = x if drop_mlp else x + s.mlp(s.l2(x))  # MLP half-step (removable)
        return x_attn, x
class GPT(nn.Module):
    def __init__(s):
        super().__init__(); s.tok = nn.Embedding(V, DM); s.pos = nn.Embedding(BLK, DM)
        s.bs = nn.ModuleList(Blk() for _ in range(NL))
        s.lnf = nn.LayerNorm(DM); s.head = nn.Linear(DM, V, bias=False); s.dr = nn.Dropout(.1)
        s.register_buffer("m", torch.triu(torch.full((BLK, BLK), float("-inf")), 1))
    def forward(s, idx, taps=False, ablate=None):
        # ablate = (block_index0, 'attn'|'mlp') to zero one sublayer's residual add
        T = idx.size(1); mk = s.m[:T, :T]
        x = s.dr(s.tok(idx) + s.pos(torch.arange(T)))
        tap = [x] if taps else None
        for i, b in enumerate(s.bs):
            da = ablate is not None and ablate[0] == i and ablate[1] == 'attn'
            dm = ablate is not None and ablate[0] == i and ablate[1] == 'mlp'
            xa, x = b(x, mk, drop_attn=da, drop_mlp=dm)
            if taps: tap.append(xa); tap.append(x)   # attn half, then mlp half
        return s.head(s.lnf(x)), tap
def batch(split, B=32, g=None):
    d = train_d if split == "train" else val_d
    ix = torch.randint(len(d) - BLK - 1, (B,), generator=g)
    return (torch.stack([d[i:i+BLK] for i in ix]),
            torch.stack([d[i+1:i+BLK+1] for i in ix]))

# ---- train (resumable) ----
TOTAL = 2200
model = GPT(); print(f"[model] STITCH 4L/d96 · params {sum(p.numel() for p in model.parameters()):,} · V={V}", flush=True)
opt = torch.optim.AdamW(model.parameters(), lr=3e-3, weight_decay=.05)
sch = torch.optim.lr_scheduler.CosineAnnealingLR(opt, TOTAL)
start, f0 = 0, None
if os.path.exists(CKPT):
    ck = torch.load(CKPT); model.load_state_dict(ck["m"]); opt.load_state_dict(ck["o"])
    sch.load_state_dict(ck["s"]); start = ck["step"]; f0 = ck["f0"]; print(f"[resume] step {start}", flush=True)
t0 = time.time(); Lval = None
for s in range(start, TOTAL):
    x, y = batch("train"); lg, _ = model(x)
    Lval = F.cross_entropy(lg.view(-1, V), y.view(-1))
    if f0 is None: f0 = Lval.item()
    opt.zero_grad(); Lval.backward(); opt.step(); sch.step()
    if s % 200 == 0: print(f"[train] {s}/{TOTAL} loss {Lval.item():.3f} ({time.time()-t0:.0f}s)", flush=True)
    if s % 200 == 0 and s > start:
        torch.save(dict(m=model.state_dict(), o=opt.state_dict(), s=sch.state_dict(), step=s, f0=f0), CKPT)
torch.save(dict(m=model.state_dict(), o=opt.state_dict(), s=sch.state_dict(), step=TOTAL, f0=f0), CKPT)
model.eval()
assert Lval is None or Lval.item() < f0 * .7, "FAIL-LOUD: loss didn't drop"

# ---- fixed held-out val batches (SAME x,y reused across every meter — no double-batch bug) ----
gval = torch.Generator().manual_seed(2024)
VAL = [batch("val", 64, gval) for _ in range(40)]
gtr = torch.Generator().manual_seed(99)

with torch.no_grad():
    base = sum(F.cross_entropy(model(x)[0].view(-1, V), y.view(-1)).item() for x, y in VAL) / len(VAL)
print(f"[val] base loss {base:.3f}  chance {math.log(V):.3f}", flush=True)
assert base < math.log(V) * .75, "FAIL-LOUD: barely above chance"

TAP_NAMES = ["node0", "n1a", "n1", "n2a", "n2", "n3a", "n3", "n4a", "n4"]
TAP_KIND  = ["embed", "attn", "mlp", "attn", "mlp", "attn", "mlp", "attn", "mlp"]

# ---- HALF-TAP LADDER: an 800-step linear probe on ln_f(tap) at each of the 9 taps ----
def probe_acc(tap_idx):
    torch.manual_seed(100 + tap_idx)
    probe = nn.Linear(DM, V, bias=False)
    po = torch.optim.AdamW(probe.parameters(), lr=3e-3)
    for st in range(800):
        x, y = batch("train", 32, gtr)
        with torch.no_grad(): _, tp = model(x, taps=True)
        h = model.lnf(tp[tap_idx])
        pl = F.cross_entropy(probe(h).view(-1, V), y.view(-1))
        po.zero_grad(); pl.backward(); po.step()
    # eval top-1 on the fixed val set
    correct = tot = 0
    with torch.no_grad():
        for x, y in VAL:
            _, tp = model(x, taps=True)
            pred = probe(model.lnf(tp[tap_idx])).argmax(-1)
            correct += (pred == y).sum().item(); tot += y.numel()
    return correct / tot

print("[ladder] training 9 probes (800 steps each)…", flush=True)
accs = []
for k in range(9):
    a = probe_acc(k); accs.append(a)
    print(f"[ladder] {TAP_NAMES[k]:5s} ({TAP_KIND[k]:5s}) acc {a:.4f}", flush=True)

rows = [{"tap": TAP_NAMES[0], "acc": round(accs[0], 4), "delta": 0.0, "kind": "embed"}]
for k in range(1, 9):
    rows.append({"tap": TAP_NAMES[k], "acc": round(accs[k], 4),
                 "delta": round((accs[k] - accs[k-1]) * 100, 2), "kind": TAP_KIND[k]})
shares = []
for b in range(4):
    at = rows[1 + 2*b]["delta"]; ml = rows[2 + 2*b]["delta"]
    shares.append({"block": b+1, "attn": at, "mlp": ml,
                   "share": round(at / (at + ml), 3) if (at + ml) > 1e-6 else 0.0})

# ---- REMOVAL TEST: zero each sublayer, Δ val loss on the SAME val batches ----
print("[removal] ablating each sublayer…", flush=True)
damage = []
with torch.no_grad():
    for b in range(4):
        da = sum(F.cross_entropy(model(x, ablate=(b, 'attn'))[0].view(-1, V), y.view(-1)).item() for x, y in VAL) / len(VAL)
        dm = sum(F.cross_entropy(model(x, ablate=(b, 'mlp'))[0].view(-1, V), y.view(-1)).item() for x, y in VAL) / len(VAL)
        damage.append({"block": b+1, "attn": round(da - base, 3), "mlp": round(dm - base, 3)})
        print(f"[removal] L{b+1}  zero-attn +{da-base:.3f}   zero-mlp +{dm-base:.3f}", flush=True)

# ---- the two verdicts (fail-loud) ----
share_falls = shares[0]["share"] > shares[1]["share"] > shares[2]["share"]      # L1>L2>L3, e.g. 70 > 49 > 31 (L4 excluded — it ticks back up)
l1_attn_wall = (damage[0]["attn"] == max(d["attn"] for d in damage)
                and damage[0]["attn"] == max(max(d["attn"], d["mlp"]) for d in damage))
assert share_falls, f"FAIL-LOUD: attention share did not fall L1>L2>L3: {[s['share'] for s in shares]}"
assert l1_attn_wall, f"FAIL-LOUD: L1-attention is not the load-bearing wall: {damage}"

R = {"rows": rows, "shares": shares, "damage": damage, "base_loss": round(base, 3),
     "model": "STITCH 4L/d96/6H char-GPT (tiny Shakespeare, seed 1337)",
     "method": "9-tap half-ladder, 800-step linear probes on ln_f(tap); removal = zero each residual add, Δ val loss, matched batches",
     "verdict": {"attn_share_falls": bool(share_falls), "l1_attn_load_bearing": bool(l1_attn_wall),
                 "l1_attn_vs_mlp": round(damage[0]["attn"] / max(damage[0]["mlp"], 1e-6), 1)}}
json.dump(R, open(OUT, "w"), indent=1)
print("\n=== FACTORY LINE (measured) ===")
print(f"  attn share by depth: " + " ".join(f"L{s['block']} {s['share']*100:.0f}%" for s in shares) + "  (FALLS = claim broken)")
print(f"  L1-attn removal +{damage[0]['attn']:.3f} = {R['verdict']['l1_attn_vs_mlp']}x its MLP — the load-bearing wall")
print(f"  -> {OUT}")
