#!/usr/bin/env python3
import hashlib,zlib,struct,json,zipfile,re,pathlib
root=pathlib.Path(__file__).resolve().parent.parent
b=(root/'build/bridge.dex').read_bytes()
assert b[:8]==b'dex\n035\x00'
assert struct.unpack_from('<I',b,8)[0]==zlib.adler32(b[12:])&0xffffffff
assert b[12:32]==hashlib.sha1(b[32:]).digest()
assert struct.unpack_from('<I',b,32)[0]==len(b)
assert struct.unpack_from('<I',b,36)[0]==0x70
assert struct.unpack_from('<I',b,40)[0]==0x12345678
# DEX table and class definition count.
strings_n,strings_off,types_n,types_off,protos_n,protos_off,fields_n,fields_off,methods_n,methods_off,class_n,class_off=struct.unpack_from('<12I',b,56)
assert class_n==1 and methods_n==10 and fields_n==1
assert 0<strings_n<100 and 0<types_n<50 and 0<protos_n<20
s=[]
def unleb(off):
 n=0;shift=0
 while True:
  x=b[off];off+=1;n|=(x&127)<<shift;shift+=7
  if x<128:return n,off
for i in range(strings_n):
 ptr=struct.unpack_from('<I',b,strings_off+i*4)[0]
 ln,p=unleb(ptr);end=b.index(0,p);s.append(b[p:end].decode('utf8'))
 assert len(s[-1])==ln
assert all(x in s for x in ['Lwindui/Bridge;','nativeTouch','nativeRun','nativeUrl','onClick','onTouch','run','shouldOverrideUrlLoading'])
assert s==sorted(s)
assert struct.unpack_from('<I',b,class_off)[0]<types_n
with zipfile.ZipFile(root/'dist/libWindUI_2.0.1.levipack') as z:
 assert not z.testzip();assert z.namelist().count('libWindUI.so')==1
 assert z.read('libWindUI.so')==(root/'dist/libWindUI.so').read_bytes()
manifest=json.loads((root/'manifest.json').read_text());assert manifest['minecraft_versions']==['1.26.*','1.26.6*']
assert (root/'dist/libWindUI.so').stat().st_size>30000
print('PASS: valid DEX checksum/structure, UI method names, manifest, packager checks')
