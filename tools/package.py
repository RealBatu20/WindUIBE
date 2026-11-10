#!/usr/bin/env python3
import json,zipfile,struct,pathlib,hashlib
root=pathlib.Path(__file__).resolve().parent.parent
v=json.loads((root/'manifest.json').read_text()); assert v['type']=='preload-native' and v['entry']=='libWindUI.so'
assert v['minecraft_versions']==['1.26.*','1.26.6*']
so=root/'dist/libWindUI.so';assert so.exists() and so.read_bytes()[:4]==b'\x7fELF'
data=so.read_bytes();assert struct.unpack_from('<H',data,18)[0]==183
out=root/'dist/libWindUI_2.0.1.levipack'
with zipfile.ZipFile(out,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
 for item in ['manifest.json','LICENSE','ATTRIBUTION.txt']:
  z.write(root/item,item)
 z.write(so,'libWindUI.so')
with zipfile.ZipFile(out) as z:
 assert z.testzip() is None
 assert [n for n in z.namelist() if n.endswith('.so')]==['libWindUI.so']
 assert z.read('libWindUI.so')==data
 assert json.loads(z.read('manifest.json'))==v
print('packaged',out,out.stat().st_size,'bytes SHA256',hashlib.sha256(out.read_bytes()).hexdigest())
