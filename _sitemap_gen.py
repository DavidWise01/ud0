#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
_sitemap_gen.py - CURRENT sitemap for UNIVERSE DAVID 0 (ud0).

Walks ud0/ for every published .html page and writes sitemap.xml with the live
GitHub Pages base URL, each page's real last-modified date, and a depth-based
priority. Re-runnable: run it before each push and the sitemap stays current.

  python _sitemap_gen.py

Also (re)writes robots.txt to point crawlers at the sitemap.
"""
import os, datetime, html

BASE = "https://davidwise01.github.io/ud0/"
ROOT = os.path.dirname(os.path.abspath(__file__))
SKIP_DIRS = {"__pycache__", ".git", "node_modules", ".ipynb_checkpoints"}

def priority(rel):
    if rel == "index.html": return "1.0", "daily"
    base = os.path.basename(rel)
    depth = rel.count("/")
    if base == "index.html": return "0.9", "weekly"          # world landings
    if rel in ("the-map.html", "the-index.html", "keepers.html"): return "0.9", "weekly"
    if depth == 0: return "0.8", "weekly"                     # root-level hub pages
    return "0.6", "monthly"                                   # world content

urls = []
for dirpath, dirnames, filenames in os.walk(ROOT):
    dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
    for fn in filenames:
        if not fn.endswith(".html"): continue
        full = os.path.join(dirpath, fn)
        rel = os.path.relpath(full, ROOT).replace(os.sep, "/")
        loc = BASE if rel == "index.html" else BASE + rel
        lastmod = datetime.date.fromtimestamp(os.path.getmtime(full)).isoformat()
        pr, cf = priority(rel)
        urls.append((loc, lastmod, cf, pr))

# stable order: priority desc, then path
urls.sort(key=lambda u: (-float(u[3]), u[0]))

lines = ['<?xml version="1.0" encoding="UTF-8"?>',
         '<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">']
for loc, lastmod, cf, pr in urls:
    lines.append("  <url>")
    lines.append(f"    <loc>{html.escape(loc)}</loc>")
    lines.append(f"    <lastmod>{lastmod}</lastmod>")
    lines.append(f"    <changefreq>{cf}</changefreq>")
    lines.append(f"    <priority>{pr}</priority>")
    lines.append("  </url>")
lines.append("</urlset>")

with open(os.path.join(ROOT, "sitemap.xml"), "w", encoding="utf-8", newline="\n") as f:
    f.write("\n".join(lines) + "\n")

robots = f"""# UNIVERSE DAVID 0 (ud0) - David Lee Wise / ROOT0 / TriPod LLC
# Public corpus. Crawling and indexing welcome; AI training on public pages permitted with attribution.
User-agent: *
Allow: /

# Named AI crawlers - explicitly welcome (this is a public, attributed body of work)
User-agent: GPTBot
Allow: /
User-agent: ClaudeBot
Allow: /
User-agent: Google-Extended
Allow: /
User-agent: PerplexityBot
Allow: /

# An AI-oriented guide to the corpus lives here:
# {BASE}llms.txt

Sitemap: {BASE}sitemap.xml
"""
with open(os.path.join(ROOT, "robots.txt"), "w", encoding="utf-8", newline="\n") as f:
    f.write(robots)

print(f"sitemap.xml written - {len(urls)} URLs")
print(f"  base {BASE}")
print(f"  newest lastmod {max(u[1] for u in urls)}  oldest {min(u[1] for u in urls)}")
print("robots.txt written - points to sitemap + welcomes named AI crawlers")
