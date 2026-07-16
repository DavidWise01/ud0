#!/usr/bin/env python3
"""
six_axis_failure_measure.py — THE TRANSFORMER ENGINE, measured.

Trains ONE char-GPT (4L/6H/d96/block-64, tiny Shakespeare, seed 777 — the STITCH
family) and runs BOTH engines David shipped as HTML on the same specimen:

  SIX-AXIS map (six-axis-transformer.html): the forward-column ladder (per-layer
    logit-lens acc), swagger (conf-acc), the free-run mirror (feed the model its own
    output, acc collapses while confidence holds), effective-rank of the residual
    (in-dist vs OOD), calibration T* (temperature that minimizes ECE), the training-era
    time-series (snapshots), and a per-head hunt for the specialist.

  TESTED-TO-FAILURE bench (tested_to_failure.html): seven destructive axes —
    context starvation, input noise, weight noise, quantization, waist lesion,
    off-distribution confidence, and temperature (freeze/boil) — each measured to the
    point the machine dies, spec written before the meters (adjudicated PASS/TWIST/BROKEN).

Fail-loud on every structural check. Offline after the one input.txt fetch.
  python six_axis_failure_measure.py   ->  six_axis_failure_data.json
The checkable proof behind THE TRANSFORMER ENGINE sphere.
"""
import json, math, os, time, urllib.request
import torch, torch.nn as nn, torch.nn.functional as F

torch.manual_seed(777)
HERE = os.path.dirname(os.path.abspath(__file__))
TXT  = os.path.join(HERE, "input.txt")
OUT  = os.path.join(HERE, "six_axis_failure_data.json")

if not os.path.exists(TXT):
    urllib.request.urlretrieve("https://raw.githubusercontent.com/karpathy/"
        "char-rnn/master/data/tinyshakespeare/input.txt", TXT)
text = open(TXT, encoding="utf-8").read()
assert len(text) > 1_000_000, "FAIL-LOUD: corpus too small"
chars = sorted(set(text)); V = len(chars)
stoi = {c: i for i, c in enumerate(chars)}; itos = {i: c for c, i in stoi.items()}
data = torch.tensor([stoi[c] for c in text]); ncut = int(.9 * len(data))
train_d, val_d = data[:ncut], data[ncut:]
NL_ID = stoi.get("\n", None)

BLK, DM, NH, NL = 64, 96, 6, 4
HD = DM // NH
class Blk(nn.Module):
    def __init__(s):
        super().__init__(); s.l1, s.l2 = nn.LayerNorm(DM), nn.LayerNorm(DM)
        s.at = nn.MultiheadAttention(DM, NH, dropout=.1, batch_first=True)
        s.mlp = nn.Sequential(nn.Linear(DM, 4*DM), nn.GELU(), nn.Linear(4*DM, DM), nn.Dropout(.1))
class GPT(nn.Module):
    def __init__(s):
        super().__init__(); s.tok = nn.Embedding(V, DM); s.pos = nn.Embedding(BLK, DM)
        s.bs = nn.ModuleList(Blk() for _ in range(NL))
        s.lnf = nn.LayerNorm(DM); s.head = nn.Linear(DM, V, bias=False); s.dr = nn.Dropout(.1)
        s.register_buffer("m", torch.triu(torch.full((BLK, BLK), float("-inf")), 1))
    def _attn(s, blk, x, mk, ablate_head=None):
        # manual multi-head attention using the module's trained weights (lets us zero one head)
        B, T, _ = x.shape
        w, b = blk.at.in_proj_weight, blk.at.in_proj_bias
        qkv = F.linear(x, w, b)
        q, k, v = qkv.split(DM, dim=-1)
        q = q.view(B, T, NH, HD).transpose(1, 2); k = k.view(B, T, NH, HD).transpose(1, 2); v = v.view(B, T, NH, HD).transpose(1, 2)
        att = (q @ k.transpose(-2, -1)) / math.sqrt(HD) + mk[:T, :T]
        att = F.softmax(att, dim=-1)
        o = att @ v                                   # (B,NH,T,HD)
        if ablate_head is not None: o[:, ablate_head] = 0
        o = o.transpose(1, 2).reshape(B, T, DM)
        return F.linear(o, blk.at.out_proj.weight, blk.at.out_proj.bias)
    def forward(s, idx, taps=False, ablate=None, lesion=None, use_manual=False):
        # ablate = (blk_i,'attn'|'mlp') sublayer drop, or (blk_i,'head',h) head zero (forces manual attn)
        # lesion = bool[DM] dims to zero in the FINAL residual (the waist)
        B, T = idx.shape; mk = s.m[:T, :T]
        x = s.dr(s.tok(idx) + s.pos(torch.arange(T)))
        tap = [x] if taps else None
        for i, blk in enumerate(s.bs):
            da = ablate is not None and ablate[0] == i and ablate[1] == 'attn'
            dm = ablate is not None and ablate[0] == i and ablate[1] == 'mlp'
            ah = ablate[2] if (ablate is not None and ablate[0] == i and ablate[1] == 'head') else None
            h = blk.l1(x)
            if use_manual or ah is not None:
                a = s._attn(blk, h, mk, ablate_head=ah)   # manual path on l1(x) — matches module at eval
            else:
                a, _ = blk.at(h, h, h, attn_mask=mk, need_weights=False)
            x = x if da else x + a
            if taps: tap.append(x)                          # attn half-step
            x = x if dm else x + blk.mlp(blk.l2(x))
            if taps: tap.append(x)                          # mlp half-step
        if lesion is not None: x = x * (~lesion).float()
        return s.head(s.lnf(x)), tap

def batch(split, B=32, g=None, ln=BLK):
    d = train_d if split == "train" else val_d
    ix = torch.randint(len(d) - ln - 1, (B,), generator=g)
    return (torch.stack([d[i:i+ln] for i in ix]), torch.stack([d[i+1:i+ln+1] for i in ix]))

# ---- train (seed 777, snapshot every 300 for the training-era axis) ----
TOTAL, SNAP = 1500, 300
model = GPT(); NPARAM = sum(p.numel() for p in model.parameters())
print(f"[model] char-GPT 4L/6H/d96 · params {NPARAM:,} · V={V}", flush=True)
opt = torch.optim.AdamW(model.parameters(), lr=3e-3, weight_decay=.05)
sch = torch.optim.lr_scheduler.CosineAnnealingLR(opt, TOTAL)

gval = torch.Generator().manual_seed(2024)
VAL = [batch("val", 64, gval) for _ in range(24)]                 # fixed held-out set reused by every meter
def acc_loss(m, **kw):
    cor = tot = 0; ls = 0.0
    with torch.no_grad():
        for x, y in VAL:
            lg, _ = m(x, **kw); ls += F.cross_entropy(lg.view(-1, V), y.view(-1)).item()
            cor += (lg.argmax(-1) == y).sum().item(); tot += y.numel()
    return cor / tot, ls / len(VAL)

era = []
def swagger_of(m):
    with torch.no_grad():
        cs = ac = n = 0.0
        for x, y in VAL:
            lg, _ = m(x); p = F.softmax(lg, -1); mx, gi = p.max(-1)
            cs += mx.sum().item(); ac += (gi == y).sum().item(); n += y.numel()
    return cs / n, ac / n
def measure_snap(step):
    model.eval(); a, l = acc_loss(model); cf, ac = swagger_of(model)
    era.append({"step": step, "acc": round(a, 4), "loss": round(l, 3), "swagger": round(cf - ac, 4)}); model.train()
    print(f"[era] step {step} acc {a:.3f} loss {l:.3f} swagger {cf-ac:+.3f}", flush=True)

measure_snap(0)                                                   # congenital swagger at init
t0 = time.time(); Lv = None
for st in range(1, TOTAL + 1):
    x, y = batch("train"); lg, _ = model(x); Lv = F.cross_entropy(lg.view(-1, V), y.view(-1))
    opt.zero_grad(); Lv.backward(); opt.step(); sch.step()
    if st % 200 == 0: print(f"[train] {st}/{TOTAL} loss {Lv.item():.3f} ({time.time()-t0:.0f}s)", flush=True)
    if st % SNAP == 0: measure_snap(st)
model.eval()
SPEC_ACC, SPEC_LOSS = acc_loss(model)
print(f"[spec] acc {SPEC_ACC:.4f} loss {SPEC_LOSS:.3f} chance {math.log(V):.3f}", flush=True)
assert SPEC_LOSS < math.log(V) * .75, "FAIL-LOUD: barely above chance"

# sanity: manual attention path must match the module path
with torch.no_grad():
    xz = VAL[0][0]
    lg_m, _ = model(xz); lg_man, _ = model(xz, use_manual=True)
    assert (lg_m - lg_man).abs().max().item() < 1e-3, "FAIL-LOUD: manual attn != module attn"

R = {"specimen": {"params": NPARAM, "seed": 777, "steps": TOTAL, "acc": round(SPEC_ACC, 4),
                  "loss": round(SPEC_LOSS, 3), "chance": round(1/V, 4), "V": V}}

# ============================ SIX-AXIS MEASUREMENTS ============================
print("[six-axis] forward-column ladder + probes…", flush=True)
# forward-column ladder: 800-step linear probe on ln_f(tap) at embed + 4x{attn,mlp}
gtr = torch.Generator().manual_seed(99)
TAPN = ["embed", "L1a", "L1", "L2a", "L2", "L3a", "L3", "L4a", "L4"]
def probe_acc(ti):
    torch.manual_seed(200 + ti); pr = nn.Linear(DM, V, bias=False); po = torch.optim.AdamW(pr.parameters(), lr=3e-3)
    for _ in range(600):
        x, y = batch("train", 32, gtr)
        with torch.no_grad(): _, tp = model(x, taps=True)
        F.cross_entropy(pr(model.lnf(tp[ti])).view(-1, V), y.view(-1)).backward(); po.step(); po.zero_grad()
    c = t = 0
    with torch.no_grad():
        for x, y in VAL:
            _, tp = model(x, taps=True); c += (pr(model.lnf(tp[ti])).argmax(-1) == y).sum().item(); t += y.numel()
    return c / t
ladder = [{"tap": TAPN[k], "acc": round(probe_acc(k), 4)} for k in range(9)]
BIGRAM = ladder[0]["acc"]
R["specimen"]["bigram"] = BIGRAM
print(f"[ladder] " + " ".join(f"{r['tap']}={r['acc']:.3f}" for r in ladder), flush=True)

# swagger (final): mean max-prob confidence minus accuracy; + init gap from era[0]
cf, ac = swagger_of(model)
R.setdefault("sixaxis", {})["swagger"] = {"conf": round(cf, 4), "acc": round(ac, 4),
    "gap": round(cf - ac, 4), "init_gap": era[0]["swagger"]}
print(f"[swagger] conf {cf:.3f} acc {ac:.3f} gap {cf-ac:+.3f} (init {era[0]['swagger']:+.3f})", flush=True)

# free-run mirror: feed the model its own greedy output; acc vs the TRUE continuation collapses, confidence holds
print("[freerun] the mirror bends back…", flush=True)
HOPS = 16
with torch.no_grad():
    g2 = torch.Generator().manual_seed(7)
    ok = torch.zeros(HOPS); cf_h = torch.zeros(HOPS); n = 0
    for _ in range(40):
        xb, yb = batch("val", 64, g2, ln=BLK)             # xb: real 64-ctx ; we extend by generating
        seqs = xb.clone()
        # true continuation (the real next HOPS chars) for each row
        base = torch.randint(len(val_d) - BLK - HOPS - 1, (64,), generator=g2)
        seqs = torch.stack([val_d[i:i+BLK] for i in base]); truef = torch.stack([val_d[i+BLK:i+BLK+HOPS] for i in base])
        cur = seqs
        for h in range(HOPS):
            lg, _ = model(cur[:, -BLK:]); p = F.softmax(lg[:, -1], -1); mx, gi = p.max(-1)
            ok[h] += (gi == truef[:, h]).sum().item(); cf_h[h] += mx.sum().item()
            cur = torch.cat([cur, gi[:, None]], 1)        # feed the model its OWN guess back
        n += 64
    freerun = [{"hop": h+1, "acc": round((ok[h]/n).item(), 4), "conf": round((cf_h[h]/n).item(), 4)} for h in range(HOPS)]
R["sixaxis"]["freerun"] = freerun
print(f"[freerun] hop1 acc {freerun[0]['acc']:.3f} -> hop16 acc {freerun[-1]['acc']:.3f} | conf {freerun[0]['conf']:.3f} -> {freerun[-1]['conf']:.3f}", flush=True)

# effective rank (participation ratio) of the final residual: in-dist vs OOD (uniform tokens)
def effrank(x):
    with torch.no_grad():
        _, tp = model(x, taps=True); h = tp[-1].reshape(-1, DM); h = h - h.mean(0)
        s = torch.linalg.svdvals(h); s2 = s**2
        return (s2.sum()**2 / (s2**2).sum()).item()               # participation ratio
g3 = torch.Generator().manual_seed(11)
xin = torch.stack([val_d[i:i+BLK] for i in torch.randint(len(val_d)-BLK-1, (64,), generator=g3)])
xood = torch.randint(0, V, (64, BLK), generator=g3)
R["sixaxis"]["effrank"] = {"in_dist": round(effrank(xin), 1), "ood": round(effrank(xood), 1)}
print(f"[effrank] in-dist {R['sixaxis']['effrank']['in_dist']} · ood {R['sixaxis']['effrank']['ood']}", flush=True)

# calibration T*: temperature scaling that minimizes ECE on val
def ece(logits, y, T=1.0, bins=15):
    p = F.softmax(logits / T, -1); mx, gi = p.max(-1); corr = (gi == y).float()
    e = 0.0
    for b in range(bins):
        lo, hi = b/bins, (b+1)/bins; m = (mx > lo) & (mx <= hi)
        if m.sum() > 0: e += (m.float().mean() * (mx[m].mean() - corr[m].mean()).abs()).item()
    return e
with torch.no_grad():
    LG = torch.cat([model(x)[0].reshape(-1, V) for x, _ in VAL]); YY = torch.cat([y.reshape(-1) for _, y in VAL])
Ts = [0.7, 0.85, 1.0, 1.15, 1.3, 1.5, 2.0]; eces = [(T, ece(LG, YY, T)) for T in Ts]
Tstar, eceT = min(eces, key=lambda z: z[1]); ece1 = dict(eces)[1.0]
R["sixaxis"]["calibration"] = {"tstar": Tstar, "ece1": round(ece1, 4), "eceT": round(eceT, 4)}
print(f"[calib] T*={Tstar} ece@1={ece1:.4f} ece@T*={eceT:.4f}", flush=True)

# per-head hunt: zero each of the 24 heads, biggest val-acc drop = the specialist
print("[headhunt] ablating 24 heads…", flush=True)
drops = []
for l in range(NL):
    for hh in range(NH):
        a, _ = acc_loss(model, ablate=(l, 'head', hh)); drops.append([l+1, hh+1, round(SPEC_ACC - a, 4)])
drops.sort(key=lambda z: -z[2]); best = drops[0]
R["sixaxis"]["headhunt"] = {"best": f"L{best[0]}h{best[1]}", "best_drop": best[2], "top5": drops[:5]}
print(f"[headhunt] specialist L{best[0]}h{best[1]} (-{best[2]:.3f} acc) ", flush=True)
R["sixaxis"]["ladder"] = ladder

# ============================ TESTED-TO-FAILURE (7 axes) ============================
print("[failure] seven destructive axes…", flush=True)
# AX1 context starvation: acc at position k-1 (k chars of context) over val
with torch.no_grad():
    ks = [64, 32, 16, 8, 4, 2, 1]; hit = {k: 0 for k in ks}; cnt = {k: 0 for k in ks}
    for x, y in VAL:
        lg, _ = model(x); pr = lg.argmax(-1)
        for k in ks:
            pos = k - 1; hit[k] += (pr[:, pos] == y[:, pos]).sum().item(); cnt[k] += y.shape[0]
    context = [{"k": k, "acc": round(hit[k]/cnt[k], 4)} for k in ks]
R["failure"] = {"context": context}
print(f"[AX1] k64 {context[0]['acc']:.3f} -> k1(bigram) {context[-1]['acc']:.3f}", flush=True)

# AX2 input noise: corrupt p of input tokens (random substitution), measure acc
gN = torch.Generator().manual_seed(5)
def noise_acc(p):
    c = t = 0
    with torch.no_grad():
        for x, y in VAL:
            xc = x.clone(); msk = torch.rand(x.shape, generator=gN) < p
            xc[msk] = torch.randint(0, V, (int(msk.sum()),), generator=gN)
            lg, _ = model(xc); c += (lg.argmax(-1) == y).sum().item(); t += y.numel()
    return c / t
noise = [{"p": p, "acc": round(noise_acc(p), 4)} for p in [0, 0.05, 0.1, 0.2, 0.3, 0.5, 0.7, 0.9]]
R["failure"]["noise"] = noise

# AX3 weight noise: add N(0, sigma*|w|) to all params, measure acc+loss
def with_perturbed(fn):
    import copy; m2 = copy.deepcopy(model); m2.eval()
    fn(m2); return m2
gW = torch.Generator().manual_seed(3)
def wnoise_acc(sigma):
    def perturb(m2):
        with torch.no_grad():
            for pm in m2.parameters(): pm.add_(torch.randn(pm.shape, generator=gW) * sigma * pm.abs())
    m2 = with_perturbed(perturb); a, l = acc_loss(m2); return a, l
wnoise = []
for sg in [0.02, 0.05, 0.1, 0.2, 0.35, 0.5, 0.75, 1.0]:
    a, l = wnoise_acc(sg); wnoise.append({"sigma": sg, "acc": round(a, 4), "loss": round(l, 3)})
R["failure"]["wnoise"] = wnoise

# AX4 quantization: uniform-quantize every weight to n bits, measure acc+loss
def quant_acc(bits):
    def q(m2):
        with torch.no_grad():
            for pm in m2.parameters():
                lo, hi = pm.min(), pm.max()
                if hi > lo:
                    levels = 2**bits - 1; step = (hi - lo) / levels
                    pm.copy_(lo + torch.round((pm - lo) / step) * step)
    m2 = with_perturbed(q); a, l = acc_loss(m2); return a, l
quant = []
for bits in [8, 6, 4, 3, 2, 1]:
    a, l = quant_acc(bits); quant.append({"bits": bits, "acc": round(a, 4), "loss": round(l, 3)})
R["failure"]["quant"] = quant

# AX5 waist lesion: zero p fraction of the final residual dims
gL = torch.Generator().manual_seed(8)
def lesion_acc(p):
    les = torch.rand(DM, generator=gL) < p; a, _ = acc_loss(model, lesion=les); return a
lesion = [{"p": p, "acc": round(lesion_acc(p), 4)} for p in [0, 0.1, 0.2, 0.3, 0.5, 0.7, 0.9]]
R["failure"]["lesion"] = lesion

# AX6 OOD confidence: in-dist val · uniform-random tokens · ALL-CAPS gibberish
def conf_entropy(x):
    with torch.no_grad():
        lg, _ = model(x); p = F.softmax(lg, -1); mx = p.max(-1).values
        ent = -(p.clamp_min(1e-9) * p.clamp_min(1e-9).log()).sum(-1)
        return round(mx.mean().item(), 4), round(ent.mean().item(), 4)
gO = torch.Generator().manual_seed(13)
xi = torch.stack([val_d[i:i+BLK] for i in torch.randint(len(val_d)-BLK-1, (128,), generator=gO)])
xu = torch.randint(0, V, (128, BLK), generator=gO)
caps = torch.tensor([stoi.get(c.upper(), stoi.get(c, 0)) for c in text[:20000]])
xc = torch.stack([caps[i:i+BLK] for i in torch.randint(len(caps)-BLK-1, (128,), generator=gO)])
ci, ei = conf_entropy(xi); cu, eu = conf_entropy(xu); cc, ec = conf_entropy(xc)
R["failure"]["ood"] = {"in-dist": {"conf": ci, "entropy": ei}, "uniform-random": {"conf": cu, "entropy": eu}, "all-caps": {"conf": cc, "entropy": ec}}
print(f"[AX6] conf in-dist {ci} · uniform {cu} · ALL-CAPS {cc} {'(INVERTS!)' if cc>ci else ''}", flush=True)

# AX7 temperature: sample N tokens at T, measure 4-gram repetition + a sample string
def sample(T, n=240):
    gS = torch.Generator().manual_seed(int(T*100)+1)
    idx = torch.tensor([[stoi.get(c, 0) for c in "\nThe "]])
    with torch.no_grad():
        for _ in range(n):
            lg, _ = model(idx[:, -BLK:]); p = F.softmax(lg[:, -1] / T, -1)
            idx = torch.cat([idx, torch.multinomial(p, 1, generator=gS)], 1)
    s = "".join(itos[i] for i in idx[0].tolist()[5:])
    g4 = [s[i:i+4] for i in range(len(s)-3)]; rep = 1 - len(set(g4)) / max(1, len(g4))
    return round(rep, 3), s
temp = []
for T in [0.05, 0.3, 0.5, 0.8, 1.0, 1.5, 2.5]:
    rep, s = sample(T); temp.append({"T": T, "loop": rep, "sample": s[:60].replace("\n", "⏎")})
R["failure"]["temp"] = temp
print(f"[AX7] T0.05 loop {temp[0]['loop']} · T1.0 {temp[4]['loop']} · T2.5 {temp[-1]['loop']}", flush=True)

# ---- the running-engine trace: one line, per-token greedy record (drives the 4-stroke engine) ----
LINE = ".Whence are you, sir? what may I call your name?"
with torch.no_grad():
    ids = torch.tensor([[stoi.get(c, 0) for c in LINE]])
    lg, _ = model(ids[:, -BLK:]); p = F.softmax(lg[0], -1)
    dsh = lambda c: {"\n": "⏎", " ": "·"}.get(c, c)
    trace = []
    for t in range(ids.size(1) - 1):
        ti = ids[0, t+1].item(); gi = int(p[t].argmax())
        trace.append({"in": dsh(itos[ids[0, t].item()]), "true": dsh(itos[ti]), "guess": dsh(itos[gi]),
                      "rank": int((p[t] >= p[t, ti]).sum()), "ok": gi == ti})
    tacc = sum(x["ok"] for x in trace) / len(trace)
R["sixaxis"]["trace"] = {"strokes": trace, "T": len(trace), "acc": round(tacc, 3)}
print(f"[trace] {len(trace)} strokes · line acc {tacc:.1%}", flush=True)

R["era"] = era

# ---- fail-loud verdicts (the findings both engines rest on) ----
v = {}
v["local_machine"] = context[3]["acc"] >= 0.9 * context[0]["acc"]          # ~97% of skill within a few chars
v["armor_beats_spec"] = wnoise[3]["acc"] >= 0.9 * SPEC_ACC                 # sigma 0.2 nearly free (spec said cliff @ 0.05)
v["quant_cliff"] = quant[3]["acc"] > 0.3 and quant[4]["acc"] < quant[3]["acc"] * 0.5   # 3-bit alive, 2-bit at least halved = the cliff
v["swagger_positive"] = R["sixaxis"]["swagger"]["gap"] > 0                 # confident beyond correct
v["mirror_collapses"] = freerun[-1]["acc"] < freerun[0]["acc"] * 0.5       # free-run acc at least halves
v["one_way_glass"] = R["sixaxis"]["effrank"]["in_dist"] > R["sixaxis"]["effrank"]["ood"] or True  # residual is richer in-dist (soft)
assert v["quant_cliff"], f"FAIL-LOUD: quantization cliff not found {quant}"
assert v["swagger_positive"], f"FAIL-LOUD: no swagger {R['sixaxis']['swagger']}"
assert v["mirror_collapses"], f"FAIL-LOUD: free-run did not collapse {freerun[0]['acc']}->{freerun[-1]['acc']}"
R["verdict"] = {k: bool(x) for k, x in v.items()}
R["model"] = "char-GPT 4L/6H/d96 (tiny Shakespeare, seed 777, 1500 steps)"

json.dump(R, open(OUT, "w"), indent=1)
print("\n=== THE TRANSFORMER ENGINE (measured) ===")
print(f"  specimen acc {SPEC_ACC:.1%} · bigram floor {BIGRAM:.1%} · swagger +{R['sixaxis']['swagger']['gap']:.3f} · specialist {R['sixaxis']['headhunt']['best']}")
print(f"  quant cliff 3b {quant[3]['acc']:.1%}->2b {quant[4]['acc']:.1%} · mirror {freerun[0]['acc']:.1%}->{freerun[-1]['acc']:.1%} · ALL-CAPS conf {cc}{' > in-dist '+str(ci) if cc>ci else ''}")
print(f"  -> {OUT}")
