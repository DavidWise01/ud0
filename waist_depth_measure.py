#!/usr/bin/env python3
"""Logit-lens DEPTH sweep on the REAL distilgpt2 — CORRECTED lens application.
In transformers 5.x, hidden_states[-1] is ALREADY post-ln_f (head(hs[-1])==logits exactly),
while hs[0..n-1] are pre-ln_f residual states. So the logit lens = head(ln_f(hs[i])) for the
pre-norm states, and the head itself = head(hs[-1]). Reports: bigram baseline (intake floor),
per-depth top-1 accuracy + agreement-with-final, and the early-exit VALVE (exit-when-confident:
fraction / agreement / compute saved). Held-out test split. Offline, reproducible (seed fixed)."""
import os, json
os.environ["HF_HUB_OFFLINE"]="1"; os.environ["TRANSFORMERS_OFFLINE"]="1"
import torch, torch.nn.functional as F
from transformers import GPT2LMHeadModel, GPT2TokenizerFast
torch.manual_seed(0)

tok = GPT2TokenizerFast.from_pretrained("distilgpt2")
model = GPT2LMHeadModel.from_pretrained("distilgpt2").eval()
cfg = model.config; L = cfg.n_layer
ln_f, head = model.transformer.ln_f, model.lm_head

# ~1.3k tokens of varied general English (multi-register: narrative, factual, procedural, reflective)
TEXT = """The history of science is a long argument with nature, and each generation inherits the
questions its parents could not answer. A river never chooses its course; it follows the lowest
ground, and over centuries it carves the valley that then confines it. Cities grow the same way,
along the paths that were easiest to walk before anyone thought to pave them. In the morning the
market fills with the noise of bargaining, and by evening the same square is quiet enough to hear
a single pair of footsteps on the stone.
To make good bread you need only flour, water, salt, and time, but the last ingredient is the one
most people refuse to spend. Mix the dough in the evening, let it rest overnight, and by morning
the yeast will have done the work while you slept. The oven should be as hot as you can make it,
and the loaf is finished when it sounds hollow if you knock on the bottom.
The team was down by two goals at halftime, and the coach said nothing clever in the locker room;
he only reminded them to pass early and to keep their heads up. In the second half the younger
players stopped forcing the ball and started moving into space, and the crowd, which had gone
quiet, began to lean forward again. When the equalizer finally came it was almost an accident, a
deflection that fell to the right foot at the right moment.
Memory is not a recording but a reconstruction. Every time you recall a face you repaint it, and
the copy drifts a little further from the original, so that the people we remember most clearly
are often the ones we have quietly invented. The stars we see at night left their light behind
them long ago, and so the sky above the city is a museum of the past arranged as if it were the
present moment. A good map lies in useful ways: it flattens the round earth, shrinks the great
distances, and leaves out the thousand small streets that would only clutter the page.
When the tide goes out it leaves the shore rewritten, every footprint erased and the sand smooth
again and ready for the next day. The child learns to count on her fingers long before she learns
that the numbers do not need hands at all. Rain falls on the just and the unjust alike, the old
proverb says, and so does the sunlight, and so does the slow indifferent turning of the seasons
from summer into autumn and back. A promise is a rope thrown across time, binding the person you
are today to the person you will one day have to become, whether you remember making it or not.
The engineer who built the first great suspension bridge did not fully trust his own arithmetic,
so before he opened it he loaded the deck with far more weight than any crowd could ever bring,
and he stood beneath it while it held. That is one honest way to know a thing is true: you build
it, you test it past the point of ordinary use, and you watch what happens with your own eyes.
The sea does not care whether you can swim, and the mountain does not care whether you are tired,
and this indifference is not cruelty but simply the way the world is arranged. A sailor learns to
read the water the way a farmer reads the sky, not from books but from a thousand small mornings
of being wrong and then, slowly, less wrong. The old harbor still smells of salt and diesel and
rope, and the boats knock gently against the dock all night like animals shifting in their sleep.
A machine is only a way of borrowing force from one place and spending it in another. The lever
lets a child move a stone that a grown man could not budge; the wheel turns the long pull of a
horse into the smooth roll of a cart; the engine burns in a second the sunlight that a forest
stored for a hundred years. We call these things clever, but they are really just patient, and
patience is a kind of intelligence that does not need to think.
Dogs were the first animals to throw in their lot with us, following the firelight to the edge of
the camp and never quite leaving. Over the centuries we bred them into a hundred shapes, some for
running and some for guarding and some, in the end, only for company. A cat, by contrast, agreed
to nothing; it simply moved in, decided the house belonged to it, and allowed us to stay.
Weather is the argument the atmosphere has with itself, warm air rising and cold air falling and
the whole restless system trying and failing to reach a balance it can never keep. A storm is not
angry, though we describe it that way; it is only pressure finding the shortest path to relief.
After the rain the streets shine, the gutters run, and the air smells briefly of the clean edge
of things before the ordinary day closes over it again.
Childhood is a country you can visit but never live in twice, and its borders are drawn not on any
map but in the particular slant of afternoon light through a particular window. The games we
played then had rules we invented and enforced with a seriousness we would never again bring to
anything, and the summers were longer because we had not yet learned to count them. Growing up is
mostly the slow trading of that endless present for a future you can plan.
Work, done well, has a rhythm that carries you, and the hours pass without your noticing until the
light changes and you look up surprised. Done badly, or for no reason you believe in, the same
hours drag like a chain, and the clock becomes an enemy you watch. Most people spend their lives
somewhere between these two, and the whole art of a good life may be learning to spend a little
more of it on the near side of that line.
Language is a treaty between strangers, a shared agreement to let certain sounds stand in for
things that are not present, and the miracle is not that it sometimes fails but that it works at
all. Every word you know was handed to you by someone who is now, in most cases, dead, and you
will hand a slightly changed version of it to someone not yet born. To speak is to join a
conversation that started long before you and will go on long after, and to say something true in
it is the closest thing to a small immortality that most of us will ever get to touch."""

ids = tok(TEXT, return_tensors="pt").input_ids[:, :1024]   # distilgpt2 context cap
T = ids.shape[1]
# held-out split: fit bigram on train, evaluate everything on the test tail
cut = int(T*0.62)
print(f"model: distilgpt2  L={L}  d={cfg.n_embd}  heads={cfg.n_head} | tokens={T}  (train {cut} / test {T-cut})")

with torch.no_grad():
    out = model(ids, output_hidden_states=True)
hs = out.hidden_states            # len L+1; hs[-1] is POST ln_f (verified: head(hs[-1])==logits)
# sanity: head(hs[-1]) must equal the model logits
assert (head(hs[-1]) - out.logits).abs().max().item() < 1e-3, "lens identity broke"

# evaluation is on the test tail; predict token t+1 from position t
def acc_on_test(top1_full):
    # top1_full: [T] predictions at each position; score positions cut..T-2 against next token
    pred = top1_full[cut:T-1]; tgt = ids[0, cut+1:T]
    return (pred==tgt).float().mean().item()*100
final_top1_full = out.logits[0].argmax(-1)

# ---- bigram baseline (intake floor): P(next|cur) from TRAIN, argmax, eval on TEST ----
from collections import defaultdict, Counter
big = defaultdict(Counter)
tr = ids[0,:cut].tolist()
for a,b in zip(tr[:-1], tr[1:]): big[a][b]+=1
bg_top = {a:c.most_common(1)[0][0] for a,c in big.items()}
uni_top = Counter(tr).most_common(1)[0][0]
corr=tot=0
for t in range(cut, T-1):
    cur=ids[0,t].item(); pred=bg_top.get(cur, uni_top)
    corr += (pred==ids[0,t+1].item()); tot+=1
bigram_acc = corr/tot*100

# ---- depth sweep: logit lens = head(ln_f(hs[i])) for i<L ; head = head(hs[-1]) ----
def lens_top1(h, normed=False):
    lg = head(h if normed else ln_f(h))
    return lg.argmax(-1)[0]
def lens_full(h, normed=False):
    return head(h if normed else ln_f(h))[0]

stages=[]
prev=None
# embed + L1..L5 are pre-norm; the last (head) is post-norm
for i in range(0, L):                     # hs[0]=embed, hs[1..L-1]=blocks 1..L-1
    name = "embed" if i==0 else f"L{i}"
    t1 = lens_top1(hs[i])
    a  = acc_on_test(t1)
    ag = (t1[cut:T-1]==final_top1_full[cut:T-1]).float().mean().item()*100
    d = None if prev is None else a-prev
    stages.append({"stage":name,"acc":round(a,2),"agree":round(ag,2),"delta":None if d is None else round(d,2)})
    prev=a
# head (delivery) = the model's real output
ha = acc_on_test(final_top1_full)
stages.append({"stage":"head","acc":round(ha,2),"agree":100.0,"delta":round(ha-prev,2)})

print(f"\nintake bigram baseline (train->test): {bigram_acc:.2f}%")
print("=== DEPTH SWEEP (logit-lens top-1, held-out test) ===")
for s in stages:
    d="" if s["delta"] is None else f"  d{s['delta']:+.2f}"
    print(f"  {s['stage']:6s} acc={s['acc']:6.2f}%  agree_final={s['agree']:6.2f}%{d}")

# ---- TUNED LENS: per-layer affine (PCA-128 + ridge) fit on TRAIN to predict the final residual
#      hs[-1] (post-ln_f), read out through the model's own head; evaluated HELD-OUT on test.
#      This is the corpus's 'measured not assumed' lens (cheap Belrose-style translator). ----
print("\n=== TUNED LENS (PCA-128 + ridge -> final residual, held-out test) ===")
K, LAM = 128, 50.0
Yfin_tr = hs[-1][0,:cut]                       # target = final post-ln_f residual (head(Y)=logits)
tuned=[]; prevt=None; tuned_full={}
for i in range(0, L):
    Xtr = hs[i][0,:cut]; Xte = hs[i][0,cut:]
    mu = Xtr.mean(0)
    _,_,Vt = torch.linalg.svd(Xtr-mu, full_matrices=False)
    P = Vt[:K].T                               # 768xK principal directions (train)
    Ztr = (Xtr-mu)@P; Zte=(Xte-mu)@P
    Ztr1 = torch.cat([Ztr, torch.ones(Ztr.shape[0],1)],1)   # +bias
    Zte1 = torch.cat([Zte, torch.ones(Zte.shape[0],1)],1)
    G = Ztr1.T@Ztr1 + LAM*torch.eye(K+1)
    A = torch.linalg.solve(G, Ztr1.T@Yfin_tr)  # (K+1)x768
    predte = Zte1@A                            # test -> predicted final residual
    lg = head(predte)                          # model head on predicted residual
    t1 = lg.argmax(-1)
    tgt = ids[0, cut+1:T]; pr = t1[:-1]
    a = (pr==tgt).float().mean().item()*100
    ag = (t1[:-1]==final_top1_full[cut:T-1]).float().mean().item()*100
    d = None if prevt is None else a-prevt
    tuned.append({"stage":"embed" if i==0 else f"L{i}","acc":round(a,2),"agree":round(ag,2),"delta":None if d is None else round(d,2)})
    tuned_full[i]=lg; prevt=a
tuned.append({"stage":"head","acc":round(ha,2),"agree":100.0,"delta":round(ha-prevt,2)})
for s in tuned:
    d="" if s["delta"] is None else f"  d{s['delta']:+.2f}"
    print(f"  {s['stage']:6s} acc={s['acc']:6.2f}%  agree_final={s['agree']:6.2f}%{d}")
tclimb = ha - tuned[0]["acc"]
tw = next(s["acc"] for s in tuned if s["stage"]==f"L{L//2}")
print(f"  tuned waist(L{L//2}) share of climb: {(tw-tuned[0]['acc'])/tclimb*100:.1f}%")

# tuned-lens early-exit valve at the waist
print("\n=== TUNED-LENS VALVE at waist L3 ===")
tw_lg = tuned_full[L//2]; tw_prob=F.softmax(tw_lg,-1); tconf,ttop=tw_prob.max(-1)
tvalve=[]
for tau in [0.3,0.4,0.5,0.6,0.7,0.8]:
    m=(tconf[:-1]>=tau); idxs=torch.arange(m.shape[0])[m]
    frac=m.float().mean().item()*100
    agree=(ttop[:-1][idxs]==final_top1_full[cut:T-1][idxs]).float().mean().item()*100 if m.sum()>0 else float('nan')
    saved=frac/100*(L-L//2)/L*100
    tvalve.append({"tau":tau,"exit_pct":round(frac,1),"agree":round(agree,1),"saved_pct":round(saved,1)})
    print(f"  tau={tau}: exit {frac:5.1f}% | agree {agree:5.1f}% | saved ~{saved:4.1f}%")

# ---- EARLY-EXIT VALVE: exit at layer k when confident; agreement w/ final + compute saved ----
print("\n=== EARLY-EXIT VALVE (exit when max-softmax >= tau) ===")
valve={}
for k in [3,4,5]:
    lg = lens_full(hs[k]); prob=F.softmax(lg,-1)
    conf,top1 = prob.max(-1)
    fin = final_top1_full
    rows=[]
    for tau in [0.3,0.4,0.5,0.6,0.7]:
        m = (conf[cut:T-1] >= tau)
        idxs = torch.arange(cut,T-1)[m]
        frac = m.float().mean().item()*100
        agree = (top1[idxs]==fin[idxs]).float().mean().item()*100 if m.sum()>0 else float('nan')
        saved = frac/100*(L-k)/L*100
        rows.append({"tau":tau,"exit_pct":round(frac,1),"agree":round(agree,1),"saved_pct":round(saved,1)})
    valve[f"L{k}"]=rows
    print(f" exit@L{k} (skips {L-k}/{L} blocks):")
    for r in rows:
        print(f"    tau={r['tau']}: exit {r['exit_pct']:5.1f}% | agree {r['agree']:5.1f}% | saved ~{r['saved_pct']:4.1f}%")

climb = ha - stages[0]["acc"]
waist_k = L//2
waist_acc = next(s["acc"] for s in stages if s["stage"]==f"L{waist_k}")
summary={"model":"distilgpt2","L":L,"d":cfg.n_embd,"heads":cfg.n_head,"test_tokens":T-cut,
  "bigram_baseline":round(bigram_acc,2),"stages":stages,"head_acc":round(ha,2),
  "waist_layer":waist_k,"waist_acc":waist_acc,
  "climb_total":round(climb,2),"climb_to_waist":round(waist_acc-stages[0]["acc"],2),
  "waist_share_pct":round((waist_acc-stages[0]["acc"])/climb*100,1) if climb else None,
  "valve":valve}
json.dump(summary, open(os.path.join(os.path.dirname(__file__),"waist_result2.json"),"w"), indent=1)
print("\nwrote waist_result2.json")
