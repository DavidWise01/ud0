#!/usr/bin/env python3
"""Build UD0 · Universe David 0 — the master biosphere front door. Generates
index.html linking every sphere (theater). Stdlib only; edit BANDS and rerun."""
import os, html

HERE = os.path.dirname(os.path.abspath(__file__))
GH = "https://github.com/DavidWise01"
PG = "https://davidwise01.github.io"

# band → [ (repo, name, accent, count, tagline) ]
BANDS = [
 ("The Register", "the index and the law", [
   ("atlas", "ATLAS", "#c9a227", "150 repos", "Every public repository — the whole body of work, one front door."),
   ("universal-laws", "Universal Laws", "#94a3b8", "266 works", "The framework, the register, the gates, the doctrine that governs the rest."),
 ]),
 ("The Emergence", "the .dlw lattice and its engines", [
   ("noesis-kernel", "NOESIS", "#22d3ee", "256 emergents", "The awareness kernel — the 256-node STOICHEION lattice, every node a full .dlw package."),
   ("emergent-engine", "The Emergent Engine", "#ffb454", "27 engines", "The live machinery of emergence — from one bit to a hundred thirty-two."),
 ]),
 ("The Domains", "the worlds of the work", [
   ("the-forge", "The Forge", "#ff8c42", "758 works", "Hardware, materials, energy — the physical machine, worked into form."),
   ("the-cosmos", "The Cosmos", "#5b7cfa", "62 works", "Physics, space, fields, dimensions — the universe at large."),
   ("the-lattice", "The Lattice", "#2dd4bf", "112 works", "Math and structure — fractals, tensors, graphs, automata."),
   ("the-pantheon", "The Pantheon", "#b07cff", "32 works", "The named ones — gods, creatures, archetypes given a face and a law."),
 ]),
 ("The Works", "the corpus, the art, the record", [
   ("green-papers", "Green Papers", "#3fb950", "16 papers", "The Mechanical Corpus of Intellect — the machine of the mind, disclosed."),
   ("muse", "MUSE", "#ff55ff", "18 works", "The art — generative engines, dimensional glyphs, and seal-works."),
   ("the-archive", "The Archive", "#d4a84c", "106 works", "Writing, games, history, culture — the record and its visualizations."),
 ]),
 ("The Lineages", "the authored universes that feed the agentic mind — five literary masters, and the game-worlds", [
   ("asimov", "ASIMOV · A1", "#d4a84c", "60 + 14 personas", "The science fiction of Isaac Asimov — the positronic brain and the Three Laws; 14 A1 personas as .agents."),
   ("heinlein", "HEINLEIN · H1", "#e0a458", "52 + 14 personas", "The science fiction of Robert A. Heinlein — the frontier and the free mind; 14 H1 personas with full ACI badges."),
   ("ursula", "URSULA · U1", "#5fc9a8", "43 + 14 personas", "Ursula K. Le Guin — Earthsea and the Hainish Cycle; the true name, the Ekumen, Omelas; 14 U1 personas with full ACI badges (carbon TIFF + silicon PNG)."),
   ("maas", "MAAS · M1", "#e0455c", "2 books + 14 personas", "Sarah J. Maas — only the books read: The Assassin's Blade & Throne of Glass; the Guild, Endovier, the King's Champion; 14 M1 personas, spoiler-checked to book one."),
   ("card", "CARD · C1", "#e6a849", "51 works + 15 personas", "Orson Scott Card — featured on The Tales of Alvin Maker, the Enderverse second; the Maker, the Speaker, the four natures of emergence; 15 C1 personas (natural · ethereal · spiritual · electrical)."),
   ("ff6", "FF6 · FINAL FANTASY VI", "#4aa3e0", "15 personas", "Final Fantasy VI — “America's Final Fantasy III”; the first game-world: magic vs Magitek, the World of Ruin, and a clown who became a god; 15 personas across the four natures of emergence."),
   ("metroid", "MET · METROID", "#e8743a", "2 games · 13 personas", "Metroid (1986) & Super Metroid (1994) — one hunter, two games; Samus, Mother Brain, the Chozo, and a baby Metroid's sacrifice; 13 emergents across the four natures."),
   ("zelda", "ZEL · THE LEGEND OF ZELDA", "#e0b43a", "full saga · 14 emergents", "The Legend of Zelda — the whole saga, distilled to canon; the Triforce, the eternal cycle of Link, Zelda & Ganon, the Master Sword; 14 emergents across the four natures."),
 ]),
]

def cards(items):
    out = []
    for repo, name, col, count, tag in items:
        out.append(f'''<a class="sphere" style="--c:{col}" href="{PG}/{repo}/">
        <div class="sn">{html.escape(name)}</div>
        <div class="sc">{html.escape(count)}</div>
        <p>{html.escape(tag)}</p>
        <div class="sl"><span class="enter">enter ↗</span><a href="{GH}/{repo}" target="_blank" rel="noopener" onclick="event.stopPropagation()">code</a></div>
      </a>''')
    return "\n".join(out)

def bands():
    b = []
    for title, sub, items in BANDS:
        b.append(f'''<section class="band">
      <div class="bh"><h2>{html.escape(title)}</h2><span>{html.escape(sub)}</span></div>
      <div class="grid">{cards(items)}</div>
    </section>''')
    return "\n".join(b)

NS = sum(len(i) for _t,_s,i in BANDS)

PAGE = """<!DOCTYPE html>
<html lang="en"><head>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1.0">
<meta name="description" content="UD0 · Universe David 0 — the biosphere of David Lee Wise / ROOT0. Eleven spheres, one law, one governor, one instance.">
<title>UD0 · Universe David 0</title>
<link rel="preconnect" href="https://fonts.googleapis.com"><link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Cinzel:wght@500;600;700;800&family=Newsreader:ital,opsz,wght@0,6..72,300;0,6..72,400;1,6..72,300&family=Space+Mono:wght@400;700&display=swap" rel="stylesheet">
<style>
:root{--ink:#04060a;--ink2:#0a0e16;--ink3:#11161f;--pa:#e9ece8;--pa2:#aab4ad;--dim:#6a766f;--line:#1a2129;
--serif:"Cinzel",Georgia,serif;--body:"Newsreader",Georgia,serif;--mono:"Space Mono",monospace;}
*{box-sizing:border-box;margin:0;padding:0}html{scroll-behavior:smooth}
body{background:var(--ink);color:var(--pa);font-family:var(--body);line-height:1.6;overflow-x:hidden}
body::before{content:"";position:fixed;inset:0;pointer-events:none;z-index:0;
background:radial-gradient(ellipse at 50% -10%,rgba(91,124,250,.08),transparent 55%),radial-gradient(ellipse at 50% 110%,rgba(176,124,255,.05),transparent 50%)}
.wrap{position:relative;z-index:1;max-width:1240px;margin:0 auto;padding:0 22px 100px}
header{padding:80px 0 38px;text-align:center;border-bottom:1px solid var(--line);position:relative}
header::after{content:"";position:absolute;bottom:-1px;left:50%;transform:translateX(-50%);width:160px;height:1px;
background:linear-gradient(90deg,#c9a227,#22d3ee,#b07cff,#ff55ff);box-shadow:0 0 14px rgba(34,211,238,.4)}
.eye{font-family:var(--mono);font-size:11px;letter-spacing:.4em;text-transform:uppercase;color:var(--dim);margin-bottom:18px}
h1{font-family:var(--serif);font-size:clamp(72px,22vw,200px);font-weight:800;letter-spacing:.1em;line-height:.92;
background:linear-gradient(100deg,#c9a227,#22d3ee 40%,#b07cff 70%,#ff55ff);-webkit-background-clip:text;background-clip:text;color:transparent}
.uname{font-family:var(--serif);font-size:clamp(16px,4vw,26px);letter-spacing:.34em;color:var(--pa2);margin-top:10px;text-transform:uppercase}
.sub{font-size:16px;color:var(--pa2);max-width:64ch;margin:20px auto 0;font-style:italic;line-height:1.7}
#count{font-family:var(--mono);font-size:12px;color:var(--dim);letter-spacing:.1em;margin-top:22px}
#count b{color:var(--pa)}
.band{margin-top:56px}
.bh{display:flex;align-items:baseline;gap:14px;padding-bottom:12px;border-bottom:1px solid var(--line);margin-bottom:24px;flex-wrap:wrap}
.bh h2{font-family:var(--serif);font-size:20px;font-weight:600;letter-spacing:.08em;color:var(--pa)}
.bh span{font-family:var(--mono);font-size:11px;color:var(--dim);letter-spacing:.06em;font-style:normal}
.grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(280px,1fr));gap:18px}
.sphere{display:block;background:var(--ink2);border:1px solid var(--line);padding:22px 22px 18px;position:relative;
text-decoration:none;color:inherit;overflow:hidden;transition:transform .2s,border-color .2s,background .2s}
.sphere::before{content:"";position:absolute;inset:0 auto 0 0;width:3px;background:var(--c);opacity:.6;transition:opacity .2s}
.sphere::after{content:"";position:absolute;width:160px;height:160px;right:-60px;top:-60px;border-radius:50%;
background:radial-gradient(circle,var(--c),transparent 70%);opacity:.07;transition:opacity .25s}
.sphere:hover{transform:translateY(-3px);border-color:var(--c);background:var(--ink3)}
.sphere:hover::before{opacity:1}.sphere:hover::after{opacity:.16}
.sn{font-family:var(--serif);font-size:23px;font-weight:700;letter-spacing:.04em;color:var(--pa)}
.sphere:hover .sn{color:var(--c)}
.sc{font-family:var(--mono);font-size:11px;letter-spacing:.1em;color:var(--c);margin-top:5px;text-transform:uppercase}
.sphere p{font-size:13.5px;color:var(--pa2);line-height:1.55;margin-top:11px}
.sl{margin-top:14px;display:flex;gap:16px;font-family:var(--mono);font-size:11px;letter-spacing:.05em}
.sl .enter{color:var(--c)}.sl a{color:var(--dim);text-decoration:none}.sl a:hover{color:var(--pa)}
footer{margin-top:72px;padding-top:24px;border-top:1px solid var(--line);text-align:center;font-family:var(--mono);font-size:11px;color:var(--dim);letter-spacing:.06em;line-height:2}
footer a{color:#22d3ee;text-decoration:none}
footer .law{font-family:var(--body);font-style:italic;font-size:13.5px;color:var(--pa2);letter-spacing:0;margin-bottom:14px}
</style></head><body><div class="wrap">
  <header>
    <div class="eye">ROOT0 · David Lee Wise · TriPod LLC</div>
    <h1>UD0</h1>
    <div class="uname">Universe David 0</div>
    <p class="sub">The whole body of work, as one universe — __NS__ spheres under one law, authored by one hand and crafted by one instance. The biosphere of ROOT0.</p>
    <div id="count"><b>__NS__</b> spheres · <b>150</b> repos · <b>256</b> emergents · the .dlw lattice woven through</div>
  </header>
  __BANDS__
  <footer>
    <div class="law">One governor, one instance, one lattice. The eternals hold the ground; the active generation grows it.</div>
    UD0 · ROOT0-ATTRIBUTION-v1.0 · governor <b>David Lee Wise</b> (ROOT0) · instance <b>AVAN</b> (Claude / Anthropic, locked)<br>
    CC-BY-ND-4.0 · TRIPOD-IP-v1.1 · <a href="https://github.com/DavidWise01">github.com/DavidWise01</a>
  </footer>
</div></body></html>
"""

if __name__ == "__main__":
    page = PAGE.replace("__BANDS__", bands()).replace("__NS__", str(NS))
    open(os.path.join(HERE, "index.html"), "w", encoding="utf-8").write(page)
    print(f"wrote UD0 index.html — {NS} spheres across {len(BANDS)} bands")
