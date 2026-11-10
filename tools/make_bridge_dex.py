#!/usr/bin/env python3
"""Deterministic minimal DEX 035 assembler for the 1-class JNI event bridge.
This intentionally emits a tiny, auditable Dalvik class without third-party D8.
The Java source in src/Bridge.java describes the same ABI/method bodies.
"""
import struct, hashlib, zlib, sys, pathlib
U16=lambda n: struct.pack('<H',n)
U32=lambda n: struct.pack('<I',n)
def leb(n):
    b=bytearray()
    while True:
        t=n&127;n>>=7;b.append(t|(128 if n else 0))
        if not n:return bytes(b)
class Build:
 def __init__(self):
    self.cls='Lwindui/Bridge;'
    self.ext='Landroid/webkit/WebViewClient;'
    self.interfaces=['Ljava/lang/Runnable;','Landroid/view/View$OnClickListener;','Landroid/view/View$OnTouchListener;']
    self.fields=[(self.cls,'id','I')]
    self.methods=[
      (self.ext,'<init>','V',()),
      (self.cls,'<init>','V',('I',)),
      (self.cls,'nativeClick','V',('I','Landroid/view/View;')),
      (self.cls,'nativeTouch','Z',('I','Landroid/view/View;','Landroid/view/MotionEvent;')),
      (self.cls,'nativeRun','V',('I',)),
      (self.cls,'nativeUrl','Z',('I','Landroid/webkit/WebView;','Ljava/lang/String;')),
      (self.cls,'onClick','V',('Landroid/view/View;',)),
      (self.cls,'onTouch','Z',('Landroid/view/View;','Landroid/view/MotionEvent;')),
      (self.cls,'run','V',()),
      (self.cls,'shouldOverrideUrlLoading','Z',('Landroid/webkit/WebView;','Ljava/lang/String;')),
    ]
    self.protos=sorted(set((ret,args) for _,_,ret,args in self.methods),key=lambda x:(x[0],x[1]))
    self.type_descriptors=sorted(set([self.cls,self.ext,'I','Z','V','Landroid/view/View;','Landroid/view/MotionEvent;','Landroid/webkit/WebView;','Ljava/lang/String;']+self.interfaces))
    s=set(self.type_descriptors+['id','<init>','nativeClick','nativeTouch','nativeRun','nativeUrl','onClick','onTouch','run','shouldOverrideUrlLoading','Bridge.java'])
    for ret,args in self.protos:s.add(''.join(('L' if a[0]=='L' else a[0]) for a in (ret,)+args))
    self.strings=sorted(s)
    self.S={v:i for i,v in enumerate(self.strings)}
    self.T={v:i for i,v in enumerate(self.type_descriptors)}
    self.P={v:i for i,v in enumerate(self.protos)}
    self.F={v:i for i,v in enumerate(sorted(self.fields,key=lambda f:(self.T[f[0]],self.S[f[1]],self.T[f[2]])))}
    self.methods=sorted(self.methods,key=lambda m:(self.T[m[0]],self.S[m[1]],self.P[(m[2],m[3])]))
    self.M={m:i for i,m in enumerate(self.methods)}
 def method(self,name):
    for m in self.methods:
      if m[0]==self.cls and m[1]==name:return self.M[m]
    raise ValueError(name)
 def inv(self,opcode, regs,method):
    assert len(regs)<=5 and all(0<=r<16 for r in regs)
    r=list(regs)+[0]*5
    return [opcode|(len(regs)<<12)|(r[4]<<8),self.method(method) if isinstance(method,str) else self.M[method],r[0]|r[1]<<4|r[2]<<8|r[3]<<12]
 def code(self,name):
    f=self.F[(self.cls,'id','I')]
    iget=lambda a,b:[0x52|(a<<8)|(b<<12),f]
    iput=lambda a,b:[0x59|(a<<8)|(b<<12),f]
    # p0 always starts at regs_size-ins_size, hence all methods use p0=1 except constructor.
    if name=='<init>':
      ins=2;reg=2;outs=1; code=self.inv(0x70,[0],(self.ext,'<init>','V',()))+iput(1,0)+[0x0e]
    elif name=='run':
      ins=1;reg=2;outs=1;code=iget(0,1)+self.inv(0x71,[0],'nativeRun')+[0x0e]
    elif name=='onClick':
      ins=2;reg=3;outs=2;code=iget(0,1)+self.inv(0x71,[0,2],'nativeClick')+[0x0e]
    elif name=='onTouch':
      ins=3;reg=4;outs=3;code=iget(0,1)+self.inv(0x71,[0,2,3],'nativeTouch')+[0x0a,0x0f]
    elif name=='shouldOverrideUrlLoading':
      ins=3;reg=4;outs=3;code=iget(0,1)+self.inv(0x71,[0,2,3],'nativeUrl')+[0x0a,0x0f]
    else:raise ValueError(name)
    return struct.pack('<HHHHII',reg,ins,outs,0,0,len(code))+b''.join(map(U16,code))
 def build(self):
    b=bytearray(b'\0'*112)
    def align(n=4):
      while len(b)%n:b.append(0)
    def alloc(size):o=len(b);b.extend(b'\0'*size);return o
    def patch(off,dat):b[off:off+len(dat)]=dat
    offsets={}
    for name,size,count in [('strings',4,len(self.strings)),('types',4,len(self.type_descriptors)),('protos',12,len(self.protos)),('fields',8,len(self.fields)),('methods',8,len(self.methods)),('classes',32,1)]:
      align();offsets[name]=alloc(size*count)
    data_off=len(b)
    str_off=[]
    for s in self.strings:
      str_off.append(len(b));bs=s.encode('utf8');b.extend(leb(len(s)));b.extend(bs);b.append(0)
    align()
    type_lists={};type_list_start=len(b)
    for args in sorted(set(args for _,args in self.protos if args),key=lambda a:(len(a),a)):
      type_lists[args]=len(b);b.extend(U32(len(args)))
      for a in args:b.extend(U16(self.T[a]))
      align()
    interface_ids=tuple(sorted(self.interfaces,key=lambda a:self.T[a]));interfaces_off=len(b);b.extend(U32(len(interface_ids)))
    for a in interface_ids:b.extend(U16(self.T[a]))
    align()
    code_offs={};code_start=len(b)
    for name in ['<init>','onClick','onTouch','run','shouldOverrideUrlLoading']:
      code_offs[name]=len(b);b.extend(self.code(name));align()
    cls_data_off=len(b)
    b.extend(leb(0)) # static_fields
    b.extend(leb(1)) # instance_fields
    b.extend(leb(5)) # direct_methods (constructor + 4 native)
    b.extend(leb(4)) # virtual_methods
    b.extend(leb(self.F[(self.cls,'id','I')])+leb(1))
    direct=sorted(['<init>','nativeClick','nativeTouch','nativeRun','nativeUrl'],key=self.method)
    previous=0
    for name in direct:
      mi=self.method(name);b.extend(leb(mi-previous));previous=mi
      b.extend(leb(0x10001 if name=='<init>' else 0x109));b.extend(leb(code_offs.get(name,0)))
    previous=0
    virtual=sorted(['onClick','onTouch','run','shouldOverrideUrlLoading'],key=self.method)
    for name in virtual:
      mi=self.method(name);b.extend(leb(mi-previous));previous=mi;b.extend(leb(1));b.extend(leb(code_offs[name]))
    align()
    map_off=len(b)
    entries=[(0x0000,1,0),(0x0001,len(self.strings),offsets['strings']),(0x0002,len(self.type_descriptors),offsets['types']),(0x0003,len(self.protos),offsets['protos']),(0x0004,len(self.fields),offsets['fields']),(0x0005,len(self.methods),offsets['methods']),(0x0006,1,offsets['classes']),(0x2002,len(self.strings),str_off[0]),(0x1001,len(type_lists)+1,type_list_start),(0x2001,5,code_start),(0x2000,1,cls_data_off),(0x1000,1,map_off)]
    entries.sort(key=lambda x:x[2]);b.extend(U32(len(entries)))
    for typ,count,off in entries:b.extend(U16(typ)+U16(0)+U32(count)+U32(off))
    for i,off in enumerate(str_off):patch(offsets['strings']+i*4,U32(off))
    for t,i in self.T.items():patch(offsets['types']+4*i,U32(self.S[t]))
    for (ret,args),i in self.P.items():
      short=''.join('L' if a.startswith('L') else a for a in (ret,)+args)
      patch(offsets['protos']+12*i,U32(self.S[short])+U32(self.T[ret])+U32(type_lists.get(args,0)))
    for (cls,name,t),i in self.F.items():patch(offsets['fields']+i*8, U16(self.T[cls])+U16(self.T[t])+U32(self.S[name]))
    for (cls,name,ret,args),i in self.M.items():patch(offsets['methods']+i*8, U16(self.T[cls])+U16(self.P[(ret,args)])+U32(self.S[name]))
    patch(offsets['classes'],U32(self.T[self.cls])+U32(0x21)+U32(self.T[self.ext])+U32(interfaces_off)+U32(self.S['Bridge.java'])+U32(0)+U32(cls_data_off)+U32(0))
    counts={'strings':len(self.strings),'types':len(self.type_descriptors),'protos':len(self.protos),'fields':len(self.fields),'methods':len(self.methods),'classes':1}
    def pair(n):return U32(counts[n]) + U32(offsets[n])
    patch(0, b'dex\n035\0')
    patch(32,U32(len(b))+U32(112)+U32(0x12345678)+U32(0)+U32(0)+U32(map_off)+pair('strings')+pair('types')+pair('protos')+pair('fields')+pair('methods')+pair('classes')+U32(len(b)-data_off)+U32(data_off))
    patch(12,hashlib.sha1(b[32:]).digest());patch(8,U32(zlib.adler32(b[12:])&0xffffffff))
    assert len(b)>=1100
    return bytes(b)
if __name__=='__main__':
 out=pathlib.Path(sys.argv[1]);data=Build().build();out.parent.mkdir(parents=True,exist_ok=True);out.write_bytes(data)
 print(f'Built {out}: {len(data)} bytes, dex SHA1 {hashlib.sha1(data).hexdigest()}')
