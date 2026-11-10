#!/usr/bin/env python3
"""Generate self-contained static WindUIBE documentation. Uses mistune only."""
from pathlib import Path
import html, json, re, unicodedata
import mistune

ROOT = Path(__file__).resolve().parents[1]
DOCS = ROOT / 'docs'
PAGES = DOCS / 'pages'
items = [
 ('01-overview.md','index.html','Overview','house'),
 ('02-quickstart.md','quickstart.html','Quick start','terminal'),
 ('03-window.md','window.html','Windows & tabs','panels'),
 ('04-elements.md','elements.html','Controls & elements','sliders'),
 ('05-themes.md','themes.html','Themes','palette'),
 ('06-overlays.md','overlays.html','Dialogs & popups','bell'),
 ('07-native.md','native.html','Native C++ ABI','code'),
 ('08-build.md','build.html','Build & limitations','wrench'),
]
md=mistune.create_markdown(escape=True,plugins=['table','strikethrough'])
search=[]
for fname,outname,title,icon in items:
 source=(PAGES/fname).read_text(encoding='utf-8')
 rendered=md(source)
 hidx=0
 def heading(m):
  global hidx
  tag,attrs,body=m.group(1),m.group(2),m.group(3)
  slug=re.sub(r'[^a-z0-9]+','-',html.unescape(re.sub(r'<[^>]+>','',body)).lower()).strip('-')
  hidx+=1
  return f'<h{tag} id="{html.escape(slug)}">{body}<a class="anchor" href="#{html.escape(slug)}" aria-label="Link to heading">#</a></h{tag}>'
 rendered=re.sub(r'<h([1-4])(.*?)>(.*?)</h\1>',heading,rendered,flags=re.S)
 headings=[html.unescape(re.sub(r'<[^>]+>','',x)).replace('#','').strip() for x in re.findall(r'<h[1-4][^>]*>(.*?)</h[1-4]>',rendered,re.S)]
 nav=''.join(f'<a class="nav-item {"active" if o==outname else ""}" href="{o}"><span class="nav-dot">{i+1:02}</span>{html.escape(t)}</a>' for i,(_,o,t,_) in enumerate(items))
 cleaned=re.sub(r'\s+',' ',re.sub(r'[`*#]|\[[^]]+\]\([^)]*\)',' ',source))
 search.append({'title':title,'href':outname,'headings':headings,'text':cleaned[:4500]})
 page=f'''<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><meta name="color-scheme" content="dark light"><meta name="description" content="WindUIBE v2.0.1 native C++17 UI for Levi Launchroid: {html.escape(title)}"><title>{html.escape(title)} — WindUIBE Docs</title><link rel="stylesheet" href="assets/site.css"></head>
<body><button id="mobile-nav" aria-label="Toggle navigation">☰</button><div class="layout"><aside id="sidebar"><div class="brand"><span class="brand-icon">≋</span><div><strong>WindUIBE</strong><small>C++ API · v2.0.1</small></div></div><label class="search-box"><span>⌕</span><input type="search" id="doc-search" placeholder="Search docs…" aria-label="Search documentation"></label><div id="search-results" hidden></div><div id="nav-list"><p class="nav-caption">DOCUMENTATION</p>{nav}</div><div class="sidebar-footer"><a href="https://github.com/RealBatu20/WindUIBE">GitHub repository</a><small>Unofficial WindUI port · MIT</small></div></aside><main class="main"><header class="header"><div class="crumb">WINDUIBE / <span>{html.escape(title.upper())}</span></div><div class="header-links"><a href="https://github.com/RealBatu20/WindUIBE">Source</a><a href="https://github.com/RealBatu20/WindUIBE/issues">Issues</a></div></header><div class="body"><div class="meta">C++17 / ANDROID ARM64 / LEVI LAUNCHROID</div><article>{rendered}</article><div class="doc-foot">Last documented for v2.0.1 · On-device compatibility not yet verified. <a href="build.html">Read limitations</a>.</div><nav class="prevnext">{('<a href="'+items[i-1][1]+'">← '+items[i-1][2]+'</a>') if (i:=next(i for i,x in enumerate(items) if x[1]==outname))>0 else '<span></span>'}{('<a href="'+items[i+1][1]+'">'+items[i+1][2]+' →</a>') if i<len(items)-1 else '<span></span>'}</nav></div></main></div><script defer src="assets/site.js"></script></body></html>'''
 (DOCS/outname).write_text(page,encoding='utf-8')
(DOCS/'assets/search-index.json').write_text(json.dumps(search,ensure_ascii=False,indent=1),encoding='utf-8')
print('Generated',len(items),'pages in',DOCS)
