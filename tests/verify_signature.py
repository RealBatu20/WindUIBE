#!/usr/bin/env python3
"""Exact-file, wildcard-aware diagnostic signature validator (never patches)."""
from pathlib import Path
import sys,re,mmap,hashlib
root=Path(__file__).resolve().parent.parent
src=(root/'src/loader.cpp').read_text()
body=src.split('static const char kMinecraft126621DiagnosticSignature[] =',1)[1].split(';',1)[0]
pat=' '.join(re.findall(r'"([0-9A-F? ]+)"',body)).split()
assert len(pat)==64 and pat.count('?')==24
pattern=re.compile(b''.join(b'.' if x=='?' else re.escape(bytes([int(x,16)])) for x in pat),re.DOTALL)
if len(sys.argv)<2:raise SystemExit('Usage: python tests/verify_signature.py /path/to/Minecraft-1.26.62.1-libminecraftpe.so')
path=Path(sys.argv[1]);hash=hashlib.sha256(path.read_bytes()).hexdigest()
with path.open('rb') as f:
 mapped=mmap.mmap(f.fileno(),0,access=mmap.ACCESS_READ)
 positions=[x.start() for x in pattern.finditer(mapped)]
 mapped.close()
print('Minecraft ELF SHA256:',hash)
print('Pattern:',len(pat),'bytes,',pat.count('?'),'wildcards,',len(pat)-pat.count('?'),'fixed bytes')
print('Raw matches:',len(positions),'; offsets:',[hex(x) for x in positions])
if hash=='2a915cf07bd165c5f939b02b36ee756878a63c4489f4971fb14e6e7bdf39216b':
 assert positions==[0x7934c80]
 print('PASS: supplied Minecraft 1.26.62.1 fingerprint')
else:
 print('WARNING: this is not the supplied verified ELF; matches alone are NOT proof of build compatibility')
