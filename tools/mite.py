#!/usr/bin/env python2

import pefile
from struct import pack as p
from capstone import *
import sys
import random

topatch = ["je", "jne", "jmp", "jg", "jge", "ja", "jae", "jl", "jle", "jb", "jbe", "call"]

def	gettype(inst):
	return topatch.index(inst) + 1

pe = pefile.PE(sys.argv[1])
entry = pe.OPTIONAL_HEADER.AddressOfEntryPoint
print "[+] Entry point == "+hex(entry)
if pe.OPTIONAL_HEADER.Magic == 523:
	arch = CS_MODE_64
else:
	arch = CS_MODE_32

index = 0
for section in pe.sections:
	if '.text' in section.Name.strip():
		break;
	index += 1
textsec = pe.sections[index].get_data()

md = Cs(CS_ARCH_X86, arch)
nw = ""
addr = pe.sections[index].VirtualAddress
jmptable = []
f = 0
r = 0
j = 0
for i in md.disasm(textsec,0):
	insb = ''.join(format(x, '02x') for x in i.bytes)
	if i.mnemonic in topatch:
		try:
			dest = p("<Q", addr + int(i.op_str,16))
			nwinsb = "cc"
			n = 0
			while n != (len(i.bytes) - 1):
				nwinsb += "90"
				n += 1
			insb = nwinsb
			typ = gettype(i.mnemonic)
			jmptable.append(p("<Q", addr + i.address) + p("<Q", typ) + dest)
			r += 1
		except:
			print "Wont patch"
	else:
		if j % 2 == 0:
			jmptable.append(p("<Q", addr + i.address) + p("<Q", random.randint(0, len(topatch))) + p("<Q", addr + random.randint(0, len(textsec))))
			f += 1
	nw += insb
	j += 1

print "Added "+str(r)+" nanomites"
print "Added "+str(f)+" fake-nanomites"

jmptable.append(p("<Q", 0) * 3)
table= "".join(jmptable)
pe.set_bytes_at_rva(addr, nw.decode('hex'))
f=open(sys.argv[1]+"-table", "w")
f.write(table)
f.close()
pe.write(sys.argv[1]+"-mited")
