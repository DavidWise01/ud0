# -*- coding: utf-8 -*-
"""
_suggestion_box.py -- PALINDROMEDA'S SUGGESTION BOX (a RUNNING ledger).

A running, append-only box of proposed REVISIONS to any part of Palindromeda
(David Lee Wise's name for the whole five-world palindrome corpus:
[ W1 CORPUS | W2 THE FOLD || W3 THE HALLWAY || W4 SONIA | W5 SONNY 5 ]).

This generator OWNS the canonical list (SUGGESTIONS below). To RUN it forward:
append a new dict to SUGGESTIONS and re-run -- it rewrites both
  ud0/palindromeda-suggestions.json   (the machine store)
  ud0/palindromeda-suggestion-box.html (the rendered page, data inlined)
Nothing is ever deleted: landed / declined items stay visible with their stamp.

Run: python _suggestion_box.py
"""
import json, os, html, datetime

ROOT = r"C:\Davids files\ud0"

# ------------------------------------------------------------------ the running list
# status: open | considered | held | landed | declined
# kind:   frontier | feature | gap | credit | structure | docs | performance | meta
# world:  I II III IV V  (or "*" for corpus-wide)
SUGGESTIONS = [
 dict(id="PS-001", raised="2026-08-12", target="World V / I-13 compiler", world="V", kind="frontier",
   status="held", raised_by="the darts: 045 quicksort, 048 union-find, 052 A*",
   title="Multiple return (a tuple / pair value)",
   detail="Three darts independently hit the same wall: quicksort's in-place partition wants to return (pivot-index, rewired array); union-find's path compression wants (root, rewired array); A*'s heap sift wants to swap a pair. I-13 returns one value. This is the last withheld frontier -- and, unlike bitwise/array/bignum which were ADDITIONS on top of the 13 counted symbols, changing what a return IS touches the value model, so it is the author's architectural call, flagged here, not smuggled in."),
 dict(id="PS-002", raised="2026-08-14", target="World V / I-13 compiler", world="V", kind="feature",
   status="landed", raised_by="the darts: 016 Boyer-Moore, 017 sieve, 018 brainfuck, + many",
   title="A bounded array",
   detail="Repeatedly the darts needed an indexed, fixed-length store (a sieve's flags, a tape, a DP row). LANDED: Value::Array is a handle into a per-run arena (keeps Value: Copy, no refactor); bounded literal [a,b,c]; checked indexing (E0501 on out-of-range); value-semantics functional update v[i]<-e. Three opcodes, OPCODE_COUNT 15->18. Proven by the live sieve dart."),
 dict(id="PS-003", raised="2026-08-15", target="World V / I-13 compiler", world="V", kind="feature",
   status="landed", raised_by="the darts: 032 Rabin-Miller, 035 Diffie-Hellman",
   title="Arbitrary-precision integers (bignum)",
   detail="Two crypto darts flagged f64's 53-bit ceiling as the wall. LANDED: Value::Bignum is a handle into an arena of sign-magnitude BigInt (base 2^32); big(x) intrinsic lowers to one ToBig op; Bin/Cmp dispatch on runtime type so no new arithmetic opcodes and the validator is unchanged (OPCODE_COUNT 18->19). Verified exact vs node BigInt. It paid off immediately in 050 RSA (round-trips) and 051 Fibonacci (F(100) exact)."),
 dict(id="PS-004", raised="2026-08-18", target="World V / I-13 compiler", world="V", kind="feature",
   status="open", raised_by="dart 053 knapsack",
   title="A 2-D array (items x capacity)",
   detail="The full knapsack DP is a 2-D table; the corpus array is 1-D, so the dart uses the space-optimized single-row form (which is what practitioners use anyway). Worth considering: a genuine 2-D array, or arrays-of-array-handles, would let the textbook 2-D recurrences run verbatim. Low urgency -- the 1-D form is honest and correct."),
 dict(id="PS-005", raised="2026-08-18", target="World V / I-13 compiler", world="V", kind="performance",
   status="open", raised_by="dart 052 A*",
   title="A binary-heap / priority-queue type",
   detail="A* pulls the lowest-f frontier cell every step. It RUNS today with a linear scan for the minimum (O(n) per step). A binary heap would make it O(log n). A heap is array-backed (so it already runs), but efficient sift-up/down is happiest with in-place swaps and paired returns -- so this is really a downstream want of PS-001 (multiple-return)."),
 dict(id="PS-006", raised="2026-08-18", target="World V / I-13 stdlib", world="V", kind="feature",
   status="open", raised_by="dart 050 RSA",
   title="Prime generation at scale (Rabin-Miller over bignum)",
   detail="Real 2048-bit RSA needs to GENERATE large probable primes, not just use toy ones. With bignum (PS-003) and the seeded PRNG already in the stdlib, a Rabin-Miller primality routine is now EXPRESSIBLE in I-13 -- it just has not been written as a std/*.i13 entry yet. A concrete, bounded next addition."),
 dict(id="PS-007", raised="2026-08-18", target="World V / I-13 stdlib", world="V", kind="feature",
   status="open", raised_by="dart 051 Fibonacci",
   title="Fast-doubling Fibonacci in the stdlib",
   detail="F(2k) and F(2k+1) can be computed from F(k) with a handful of bignum + - * operations -- O(log n) instead of O(n). It needs nothing new (bignum is enough), so it is a small, satisfying stdlib entry that would let F(1,000,000) return without a million iterations."),
 dict(id="PS-008", raised="2026-08-18", target="World V / I-13 wasm backend", world="V", kind="gap",
   status="open", raised_by="the campaign build",
   title="Bring the wasm backend to parity with the VM",
   detail="Array and bignum landed in the interpreter (VM) path; the wasm codegen currently errors cleanly on Array/Bignum rather than emitting them, so the VM is ahead of wasm. Either extend wasm codegen to cover the new value kinds, or document the VM-only scope explicitly so the gap is disclosed, not silent."),
 dict(id="PS-009", raised="2026-08-10", target="Corpus-wide", world="*", kind="structure",
   status="open", raised_by="the incomplete-spheres audit",
   title="Content pass on the shell spheres",
   detail="An audit found roughly 119 spheres that return HTTP 200 but ship no real instrument or content -- they load, but they are shells. A content-depth pass should either bring each to LIT (a real, verified instrument) or honestly retire it. Deferred, but tracked here so it is not forgotten."),
 dict(id="PS-010", raised="2026-08-08", target="World II / THE FOLD", world="II", kind="docs",
   status="considered", raised_by="the World II FOLD audit",
   title="Document THE FOLD's four by-design shapes inline",
   detail="Four intentional shapes in World II read like drift to a fresh auditor (they are not -- each has a proof). A short inline note per shape, at the shape, would stop every future audit from re-flagging the same four and having to re-derive that they are deliberate."),
 dict(id="PS-011", raised="2026-08-18", target="This box itself", world="*", kind="meta",
   status="landed", raised_by="David Lee Wise (ROOT0)",
   title="The box is itself revisable -- and running",
   detail="David asked for a RUNNING suggestion box open to revisions of any part of Palindromeda, the box included. v0 gives each item a provenance (who raised it), a status lifecycle (open -> considered -> held / landed / declined), and append-only history -- nothing is deleted, closed items stay visible with their stamp. Next revisions on the table: a public submission form, and per-item discussion threads. Meta by design: the box eats its own suggestions."),
]

# ------------------------------------------------------------------ write the store
os.makedirs(ROOT, exist_ok=True)
store = {
  "schema": "palindromeda-suggestions/1",
  "name": "PALINDROMEDA'S SUGGESTION BOX",
  "of": "the five-world palindrome corpus [ W1 | W2 || W3 || W4 | W5 ]",
  "running": True,
  "note": "Append-only. Nothing is deleted; landed and declined items stay visible with their stamp. This file is the machine store; palindromeda-suggestion-box.html renders it.",
  "author": "David Lee Wise / ROOT0 / TriPod LLC (with AVAN)",
  "updated": datetime.date.today().isoformat(),
  "count": len(SUGGESTIONS),
  "suggestions": SUGGESTIONS,
}
with open(os.path.join(ROOT, "palindromeda-suggestions.json"), "w", encoding="utf-8", newline="\n") as f:
    json.dump(store, f, indent=2, ensure_ascii=False)

# ------------------------------------------------------------------ render the page
STATUS = {
 "open":       ("#22d3ee", "OPEN"),
 "considered": ("#e8b923", "CONSIDERED"),
 "held":       ("#ff8a3d", "HELD - author's call"),
 "landed":     ("#59ffb0", "LANDED"),
 "declined":   ("#d0607a", "DECLINED"),
}
WORLD = {
 "I":  ("#a06bff", "W1 CORPUS"), "II": ("#7ad0ff", "W2 THE FOLD"),
 "III":("#e8b923", "W3 HALLWAY"), "IV": ("#e0616f", "W4 SONIA"),
 "V":  ("#22d3ee", "W5 SONNY 5"), "*": ("#ece9ff", "CORPUS-WIDE"),
}
def esc(s): return html.escape(str(s), quote=True)

def counts():
    c = {}
    for s in SUGGESTIONS: c[s["status"]] = c.get(s["status"], 0) + 1
    return c
C = counts()

def order_key(s):
    return (s["raised"], s["id"])
ordered = sorted(SUGGESTIONS, key=order_key, reverse=True)

cards = []
for s in ordered:
    scol, slab = STATUS[s["status"]]
    wcol, wlab = WORLD.get(s["world"], ("#ece9ff", s["world"]))
    cards.append(f'''<article class="sg" data-status="{esc(s['status'])}" data-world="{esc(s['world'])}" data-text="{esc((s['title']+' '+s['detail']+' '+s['target']+' '+s['raised_by']).lower())}">
  <div class="sgtop">
    <span class="id">{esc(s['id'])}</span>
    <span class="wtag" style="color:{wcol};border-color:{wcol}">{esc(wlab)}</span>
    <span class="kind">{esc(s['kind'])}</span>
    <span class="stat" style="color:{scol};border-color:{scol}">{esc(slab)}</span>
    <span class="date">{esc(s['raised'])}</span>
  </div>
  <h3 class="sgt">{esc(s['title'])}</h3>
  <p class="tgt">on <b>{esc(s['target'])}</b> &middot; raised by {esc(s['raised_by'])}</p>
  <p class="det">{esc(s['detail'])}</p>
</article>''')
cards_html = "\n".join(cards)

statbar = "".join(
  f'<span class="pill" style="color:{STATUS.get(k,("#ece9ff",k))[0]};border-color:{STATUS.get(k,("#ece9ff",k))[0]}">{C.get(k,0)} {STATUS.get(k,("",k))[1].split(" - ")[0]}</span>'
  for k in ["open","considered","held","landed","declined"]
)
inline_json = json.dumps(store, ensure_ascii=True)
today = datetime.date.today().isoformat()

HTML = f'''<!DOCTYPE html>
<html lang="en"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1.0">
<meta name="color-scheme" content="dark"><meta name="theme-color" content="#07060f">
<meta name="author" content="David Lee Wise / ROOT0 / TriPod LLC">
<meta name="description" content="Palindromeda's Suggestion Box -- a running, append-only ledger of proposed revisions to any part of the five-world palindrome corpus.">
<link rel="canonical" href="https://davidwise01.github.io/ud0/palindromeda-suggestion-box.html">
<link rel="icon" href="data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 64 64'%3E%3Ccircle cx='32' cy='32' r='26' fill='none' stroke='%23a06bff' stroke-width='4'/%3E%3C/svg%3E">
<title>Palindromeda's Suggestion Box</title>
<style>
:root{{--pa:#ece9ff;--dim:#8f8ac0;--ink:#cdc6f0;--card:rgba(20,14,40,.62);--line:rgba(160,107,255,.30);--c:#a06bff;--gold:#e8b923;--orange:#ff8a3d;--green:#59ffb0;--mono:ui-monospace,'SF Mono',Menlo,Consolas,monospace;--disp:'Iowan Old Style',Palatino,Georgia,serif}}
*{{box-sizing:border-box;margin:0;padding:0}}
html{{scroll-behavior:smooth}}
body{{background:#07060f;color:var(--pa);font-family:var(--mono);min-height:100vh;overflow-x:hidden;line-height:1.62;
 background-image:linear-gradient(rgba(150,110,255,.05) 1px,transparent 1px),linear-gradient(90deg,rgba(150,110,255,.05) 1px,transparent 1px),radial-gradient(130% 80% at 50% -12%,rgba(122,79,208,.26),transparent 62%);
 background-size:46px 46px,46px 46px,100% 100%;background-attachment:fixed}}
.floor{{position:fixed;left:-50%;right:-50%;bottom:-12vh;height:52vh;z-index:0;pointer-events:none;opacity:.26;
 background-image:linear-gradient(rgba(160,107,255,.7) 2px,transparent 2px),linear-gradient(90deg,rgba(160,107,255,.7) 2px,transparent 2px);
 background-size:60px 60px;transform:perspective(300px) rotateX(70deg);animation:floor 6s linear infinite;
 -webkit-mask:linear-gradient(transparent 6%,#000 74%);mask:linear-gradient(transparent 6%,#000 74%)}}
@keyframes floor{{to{{background-position:0 60px}}}}
@media(prefers-reduced-motion:reduce){{.floor{{animation:none}}}}
.wrap{{position:relative;z-index:2;max-width:940px;margin:0 auto;padding:26px 18px 90px}}
.crumb{{font-size:11px;letter-spacing:2px;color:var(--dim);text-transform:uppercase}}
.crumb a{{color:var(--c);text-decoration:none;font-weight:700}} .crumb a:hover{{color:var(--gold)}}
h1{{font-family:var(--disp);font-size:clamp(30px,6vw,46px);letter-spacing:1px;margin:14px 0 4px;line-height:1.05;
 background:linear-gradient(120deg,#fff,#c9b3ff 55%,#a06bff);-webkit-background-clip:text;background-clip:text;-webkit-text-fill-color:transparent;text-shadow:0 0 34px rgba(160,107,255,.25)}}
.tag{{color:var(--dim);font-size:13px;max-width:74ch;margin:2px 0 4px}}
.tag b{{color:#e7deff}}
.pbar{{display:inline-flex;align-items:center;gap:0;font-size:12.5px;letter-spacing:1px;color:#bcb2e6;border:1px solid var(--line);border-radius:8px;padding:7px 12px;margin:12px 0 6px;background:rgba(20,14,40,.5);flex-wrap:wrap}}
.pbar b{{color:#fff}} .pbar .sep{{color:var(--gold);font-weight:700;margin:0 8px}}
.stats{{display:flex;gap:8px;flex-wrap:wrap;margin:8px 0 4px}}
.pill{{font-size:11px;letter-spacing:1px;border:1px solid;border-radius:20px;padding:3px 11px;font-weight:700}}
.running{{font-size:11.5px;color:var(--dim);margin:10px 0 2px;border-left:2px solid var(--c);padding-left:11px}}
.running b{{color:#dcd2ff}}
.controls{{display:flex;gap:8px;flex-wrap:wrap;align-items:center;margin:16px 0 6px}}
.controls input{{flex:1;min-width:180px;font-family:var(--mono);font-size:13px;background:rgba(20,14,40,.7);color:var(--pa);border:1px solid var(--line);border-radius:7px;padding:9px 12px}}
.controls select{{font-family:var(--mono);font-size:12.5px;background:rgba(20,14,40,.85);color:var(--pa);border:1px solid var(--line);border-radius:7px;padding:9px 10px}}
.count{{font-size:11.5px;color:var(--dim);margin:4px 2px 14px;letter-spacing:1px}}
.sg{{border:1px solid var(--line);border-radius:11px;background:var(--card);padding:14px 16px 15px;margin:0 0 13px;box-shadow:0 0 20px rgba(0,0,0,.35),inset 0 0 30px rgba(160,107,255,.04)}}
.sgtop{{display:flex;gap:8px;align-items:center;flex-wrap:wrap;font-size:10.5px;letter-spacing:1px}}
.sgtop .id{{font-weight:700;color:#cdbcff;letter-spacing:2px}}
.wtag,.stat{{border:1px solid;border-radius:4px;padding:1px 7px;font-weight:700}}
.kind{{color:var(--dim);border:1px solid var(--line);border-radius:4px;padding:1px 7px;text-transform:uppercase}}
.date{{margin-left:auto;color:var(--dim)}}
.sgt{{font-size:16.5px;font-weight:600;color:#f2edff;margin:9px 0 3px;letter-spacing:.3px}}
.tgt{{font-size:11.5px;color:var(--dim);margin:0 0 7px}} .tgt b{{color:#c7b9f0}}
.det{{font-size:13px;color:var(--ink)}}
.empty{{color:var(--dim);font-size:13px;padding:20px 4px;text-align:center;display:none}}
footer{{margin-top:26px;border-top:1px solid var(--line);padding-top:15px;color:var(--dim);font-size:11.5px;line-height:1.85}}
footer a{{color:var(--gold);text-decoration:none}} footer b{{color:#c7b9f0}}
.worlds{{display:flex;gap:7px;flex-wrap:wrap;margin:8px 0}}
.worlds a{{font-size:10.5px;letter-spacing:1px;border:1px solid var(--line);border-radius:5px;padding:3px 9px;color:var(--c);text-decoration:none}}
.worlds a:hover{{border-color:var(--gold);color:var(--gold)}}
</style></head>
<body>
<div class="floor"></div>
<div class="wrap">
  <div class="crumb"><a href="./">&#9668; UNIVERSE DAVID 0</a> &middot; a running ledger</div>
  <h1>Palindromeda&rsquo;s Suggestion Box</h1>
  <p class="tag">A <b>running</b>, append-only ledger of proposed <b>revisions to any part of Palindromeda</b> &mdash; David Lee Wise&rsquo;s name for the whole five-world palindrome, <b>[ W1 &middot; CORPUS | W2 &middot; THE FOLD &#8214; W3 &middot; THE HALLWAY &#8214; W4 &middot; SONIA | W5 &middot; SONNY 5 ]</b>. The box itself is revisable. Nothing here is deleted: landed and declined items stay visible with their stamp.</p>
  <div class="pbar"><b>{len(SUGGESTIONS)}</b>&nbsp;suggestions<span class="sep">&#8214;</span>updated <b>{today}</b><span class="sep">&#8214;</span>store: <b>palindromeda-suggestions.json</b></div>
  <div class="stats">{statbar}</div>
  <p class="running">This is a <b>running box</b>: it grows as the corpus is worked. Each entry carries who raised it and a status &mdash; <b>open</b> &rarr; <b>considered</b> &rarr; <b>held</b> (the author&rsquo;s architectural call) / <b>landed</b> / <b>declined</b>. Several here have already <b>landed</b> (the array and the bignum), which is what a working box looks like.</p>

  <div class="controls">
    <input id="q" type="text" placeholder="filter suggestions..." autocomplete="off" aria-label="filter">
    <select id="fw" aria-label="filter by world">
      <option value="">all worlds</option>
      <option value="*">corpus-wide</option>
      <option value="I">W1 Corpus</option><option value="II">W2 The Fold</option>
      <option value="III">W3 Hallway</option><option value="IV">W4 Sonia</option><option value="V">W5 Sonny 5</option>
    </select>
    <select id="fs" aria-label="filter by status">
      <option value="">all statuses</option>
      <option value="open">open</option><option value="considered">considered</option>
      <option value="held">held</option><option value="landed">landed</option><option value="declined">declined</option>
    </select>
  </div>
  <div class="count" id="count"></div>

  <div id="list">
{cards_html}
  </div>
  <div class="empty" id="empty">no suggestions match that filter.</div>

  <div class="worlds">
    <a href="./">W1 &middot; Corpus</a><a href="world2/">W2 &middot; The Fold</a>
    <a href="https://0root.ai/">W3 &middot; Hallway</a><a href="world4/">W4 &middot; Sonia</a><a href="world5/">W5 &middot; Sonny 5</a>
  </div>
  <footer>
    <b>PALINDROMEDA&rsquo;S SUGGESTION BOX</b> &mdash; the corpus&rsquo;s own revision ledger, append-only and dated. Machine store: <b>palindromeda-suggestions.json</b>; rendered by <b>_suggestion_box.py</b> (append to its list and re-run to move the box forward). A suggestion is a proposal, not a promise; a <b>held</b> item is a deliberate architectural boundary, not a bug. David Lee Wise (ROOT0 / TriPod LLC) with AVAN.
  </footer>
</div>
<script type="application/json" id="data">{inline_json}</script>
<script>
"use strict";
(function(){{
  var q=document.getElementById("q"), fw=document.getElementById("fw"), fs=document.getElementById("fs");
  var cards=[].slice.call(document.querySelectorAll(".sg"));
  var count=document.getElementById("count"), empty=document.getElementById("empty");
  function apply(){{
    var t=q.value.trim().toLowerCase(), w=fw.value, s=fs.value, n=0;
    cards.forEach(function(c){{
      var ok = (!t || c.getAttribute("data-text").indexOf(t)>=0)
            && (!w || c.getAttribute("data-world")===w)
            && (!s || c.getAttribute("data-status")===s);
      c.style.display = ok ? "" : "none"; if(ok) n++;
    }});
    count.textContent = n + " of " + cards.length + " shown";
    empty.style.display = n ? "none" : "block";
  }}
  q.addEventListener("input",apply); fw.addEventListener("change",apply); fs.addEventListener("change",apply);
  apply();
}})();
</script>
</body></html>
'''

with open(os.path.join(ROOT, "palindromeda-suggestion-box.html"), "w", encoding="utf-8", newline="\n") as f:
    f.write(HTML)

print(f"wrote palindromeda-suggestions.json  ({len(SUGGESTIONS)} suggestions)")
print(f"wrote palindromeda-suggestion-box.html  ({len(HTML)} bytes)")
print("status counts:", C)
