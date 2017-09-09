#!/usr/bin/env python

import pefile
from struct import pack as p
from capstone import *
import sys

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
		except:
			print "Wont patch"
	nw += insb
jmptable.append(p("<Q", 0) * 3)
table= "".join(jmptable)
pe.set_bytes_at_rva(addr, nw.decode('hex'))
f=open(sys.argv[1]+"-table", "w")
f.write(table)
f.close()
pe.write(sys.argv[1]+"-mited")
