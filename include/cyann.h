#ifndef CYANN_H
# define CYANN_H

# include <stdio.h>
# include <stdlib.h>
# include <windows.h>
# include "resource.h"
# include "fake_winhdr.h"

# define NBMODULE 2
# define NBFONCTION 17

# define PE_HEADER_OFFSET		0x3c

# if defined __x86_64__
	typedef DWORD64	CINT;
	# define PEB_LDR_DATA_OFFSET            0x18
	# define INMEMORYORDERMODULELIST_OFFSET 0x20
	# define FULLDLLNAME_OFFSET             0x50
	# define DLLBASE_OFFSET                 0x20
	# define GPR_BX 			Rbx
	# define GPR_AX				Rax
	# define R_IP				Rip
	# define R_SP				Rsp
# else
	typedef DWORD	CINT;
	# define PEB_LDR_DATA_OFFSET            0x0c
	# define INMEMORYORDERMODULELIST_OFFSET 0x14
	# define FULLDLLNAME_OFFSET             0x28
	# define DLLBASE_OFFSET                 0x10
	# define GPR_AX				Eax
	# define GPR_BX 			Ebx
	# define R_IP				Eip
	# define R_SP				Esp
# endif

typedef struct	s_module
{
	DWORD	hash;
	CINT	addr;
}		t_module;

typedef struct	s_fonction
{
	int		index;
	DWORD		hash;
	CINT		addr;
}		t_fonction;

typedef struct	s_pe
{
	size_t			dosstubsize;
	size_t			nbsect;
	size_t			sectsize;
	IMAGE_DOS_HEADER	doshdr;
	IMAGE_NT_HEADERS64	nthdr;
	IMAGE_SECTION_HEADER	*secthdr;
	PUCHAR			dosstub;
	UCHAR			**sections;
	PUCHAR			buffer;
}		t_pe;

typedef struct	s_nanomite
{
	long long	offset;
	long long	type;
	long long	dest;
}		t_nanomite;

extern CINT		g_peb;
extern t_module         g_module[NBMODULE];
extern t_fonction       g_fonctions[NBFONCTION];
extern t_nanomite	*g_ntable;

DWORD		shash(char *s);
DWORD		whash(PWSTR s);
void		get_peb(CINT *peb);
CINT		resolve_symbol(t_fonction *fct);
int		process_hollow(WCHAR *legitproc, t_pe *pe);
int		debug_son(LPPROCESS_INFORMATION rpi, CINT baseaddr);
PUCHAR		extract_rsrc(WORD id);
t_nanomite      *make_nanomited_table(WORD id);
t_pe		*parse_pe(PUCHAR in);
void		free_pe(t_pe *pe);

#endif
