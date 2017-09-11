#include "cyann.h"
#include "api_function.h"

int	lookup(LPPROCESS_INFORMATION rpi, CINT baseaddr)
{
	CINT			savedeip;
	t_nanomite		*ntable;
	LPCONTEXT               rcontext;
        GETTHREADCONTEXT        getthcontext;
        SETTHREADCONTEXT        setthcontext;
	WRITEPROCESSMEM         writeprocmem;

	printf("Lookup here!\n");
	ntable = g_ntable;
        if (!(rcontext = (CONTEXT *)malloc(sizeof(CONTEXT))))
                return (0);
        rcontext->ContextFlags = CONTEXT_FULL;
        if (!(getthcontext = (GETTHREADCONTEXT)resolve_symbol(&g_fonctions[10])))
                return (0);
        if (!(setthcontext = (SETTHREADCONTEXT)resolve_symbol(&g_fonctions[11])))
                return (0);
        if (!getthcontext(rpi->hThread, rcontext))
                return (0);
	while (ntable->offset)
	{
		if (ntable->offset == (long long)((rcontext->R_IP - baseaddr) - 1))
		{
			if (ntable->type == 1 && (rcontext->EFlags & (1 << 6)))
				rcontext->R_IP = baseaddr + ntable->dest;
			else if (ntable->type == 2 && !(rcontext->EFlags & (1 << 6)))
				rcontext->R_IP = baseaddr + ntable->dest;
			else if (ntable->type == 3)
				rcontext->R_IP = baseaddr + ntable->dest;
			else if (ntable->type == 4 && ((rcontext->EFlags & (1 << 11)) == (rcontext->EFlags & (1 << 7))) && !(rcontext->EFlags & (1 << 6)))
				rcontext->R_IP = baseaddr + ntable->dest;
			else if (ntable->type == 5 && (((rcontext->EFlags & (1 << 11)) == (rcontext->EFlags & (1 << 7))) || (rcontext->EFlags & (1 << 6))))
				rcontext->R_IP = baseaddr + ntable->dest;
			else if (ntable->type == 6 && !(rcontext->EFlags & 1) && !(rcontext->EFlags & (1 << 6)))
				rcontext->R_IP = baseaddr + ntable->dest;
			else if (ntable->type == 7 && (!(rcontext->EFlags & 1) || (rcontext->EFlags & (1 << 6))))
				rcontext->R_IP = baseaddr + ntable->dest;
			else if (ntable->type == 8 && ((rcontext->EFlags & (1 << 11)) != (rcontext->EFlags & (1 << 7))))
				rcontext->R_IP = baseaddr + ntable->dest;
			else if (ntable->type == 9 && (((rcontext->EFlags & (1 << 11)) != (rcontext->EFlags & (1 << 7))) || (rcontext->EFlags & (1 << 6))))
				rcontext->R_IP = baseaddr + ntable->dest;
			else if (ntable->type == 10 && (rcontext->EFlags & 1))
				rcontext->R_IP = baseaddr + ntable->dest;
			else if (ntable->type == 11 && ((rcontext->EFlags & 1) || (rcontext->EFlags & (1 << 6))))
				rcontext->R_IP = baseaddr + ntable->dest;
			else if (ntable->type == 12)
			{
				savedeip = rcontext->R_IP + 4;
				if (!(writeprocmem = (WRITEPROCESSMEM)resolve_symbol(&g_fonctions[9])))
					return (0);
				rcontext->R_SP -= 4;
				writeprocmem(rpi->hProcess, (LPVOID)rcontext->R_SP, &savedeip, 4, NULL);
				rcontext->R_IP = baseaddr + ntable->dest;
			}
			break;
		}
		ntable++;
	}
	if (!setthcontext(rpi->hThread, rcontext))
                return (0);
        free(rcontext);
	return (1);
}

int	debug_son(LPPROCESS_INFORMATION rpi, CINT baseaddr)
{
	int			cont;
	CINT			ccstatus;
	DEBUG_EVENT		event;
	RESUMETHREAD		resumeth;
	DEBUGACTIVEPROC		debugproc;
	WAITFORDEBUGEVENT	waitforit;
	CONTINUEDEBUGEVENT	continuedbg;

	cont = 1;
	if (!(debugproc = (DEBUGACTIVEPROC)resolve_symbol(&g_fonctions[13])))
		return (0);
	if (!debugproc(rpi->dwProcessId))
        	return (0);
	if (!(resumeth = (RESUMETHREAD)resolve_symbol(&g_fonctions[12])))
                return (0);
        if (!(resumeth(rpi->hThread)))
                return (0);
	if (!(waitforit = (WAITFORDEBUGEVENT)resolve_symbol(&g_fonctions[14])))
		return (0);
	if (!(continuedbg = (CONTINUEDEBUGEVENT)resolve_symbol(&g_fonctions[15])))
		return (0);
	ccstatus = DBG_CONTINUE;
	while (cont)
	{
		waitforit(&event, INFINITE);
		switch (event.dwDebugEventCode)
		{
			case EXCEPTION_DEBUG_EVENT:
				if (event.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT)
				{
					lookup(rpi, baseaddr);
					ccstatus = DBG_CONTINUE;
				}
				else
					ccstatus = DBG_EXCEPTION_NOT_HANDLED;
				break;
			case EXIT_PROCESS_DEBUG_EVENT:
				cont = 0;
				break;
		}
		continuedbg(event.dwProcessId, event.dwThreadId, ccstatus);
	}
	return (0);
}

t_nanomite	*make_nanomited_table(LPWSTR rsrc)
{
	t_nanomite	*table;

	if (!(table =(t_nanomite*) extract_rsrc(rsrc)))
		return (NULL);
	else
		return (table);
}
