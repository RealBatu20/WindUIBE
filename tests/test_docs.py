#!/usr/bin/env python3
"""Offline public documentation link / inventory check."""
from pathlib import Path
import json,re
root=Path(__file__).resolve().parents[1]
docs=root/'docs'
items=json.loads((docs/'assets/search-index.json').read_text())
assert len(items)==8
for item in items:
 f=docs/item['href']; assert f.is_file(),f
 html=f.read_text(encoding='utf-8')
 assert '<article>' in html and 'site.css' in html and 'site.js' in html
 assert '<title>' in html and 'WindUIBE Docs' in html
 assert 'On-device compatibility not yet verified' in html
 for asset in ['assets/site.css','assets/site.js']:
  assert (docs/asset).exists()
 for link in re.findall(r'<a [^>]*href="([^"]+)"',html):
  if not link.startswith(('http:','https:','#','mailto:')):
   assert (docs/link.split('#')[0]).is_file(),(f,link)
assert (docs/'assets/preview.png').is_file()
assert (docs/'assets/dropdown-open.png').is_file()
print('PASS: public docs 8 pages, navigation links, offline assets, previews')
