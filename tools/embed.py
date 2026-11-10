#!/usr/bin/env python3
"""Embed Android DEX and the offline HTML UI into the only packaged .so."""
from pathlib import Path
import sys
root=Path(__file__).resolve().parent.parent
bridge=root/'build/bridge.dex'
html=root/'assets/windui.html'
if not bridge.exists():
 raise SystemExit('Missing bridge.dex. First run tools/make_bridge_dex.py')
b=bridge.read_bytes();s=html.read_text(encoding='utf-8')
# Single-source the runtime script: modifications to windui.js must reach the
# embedded, installable release, not merely the desktop test page.
js=(root/'assets/windui.js').read_text(encoding='utf-8')
start=s.index('<script>'); end=s.index('</script>',start)
s=s[:start]+'<script>\n'+js+'\n'+s[end:]
assert ')winduidata"' not in s
out=root/'build/embedded.hpp'
out.parent.mkdir(parents=True,exist_ok=True)
with out.open('w',encoding='utf-8') as f:
 f.write('#pragma once\n#include <cstddef>\n')
 f.write('static const unsigned char kWindUiDex[] = {\n')
 for i in range(0,len(b),20):f.write('  '+', '.join('0x%02x'%c for c in b[i:i+20])+',\n')
 f.write('};\nstatic constexpr size_t kWindUiDexSize = sizeof(kWindUiDex);\n')
 f.write('static const char kWindUiHtml[] = R"winduidata('+s+')winduidata";\n')
print('generated',out,'DEX',len(b),'HTML',len(s.encode()))
