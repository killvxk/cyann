#include "cyann.h"

void    get_peb(CINT *peb)
{
        if (peb)
        #if defined(_WIN64)
                *peb = __readgsqword( 0x60 );
        #else
                *peb = __readfsdword( 0x30 );
        #endif
}

DWORD   shash(char *s)
{
        CINT   i;

        i = 0;
        while (*s)
        {
                i *= 16777619;
                i ^= *s++;
        }
        return (i);
}

DWORD   whash(PWSTR s)
{
        CINT    i;

        i = 0;
        while (*s)
        {
                i *= 16777619;
                i ^= *s++;
        }
        return (i);
}


CINT get_exported_symbol(CINT baseaddr, CINT symbolhash)
{
	PIMAGE_NT_HEADERS	nthdr;
	PIMAGE_EXPORT_DIRECTORY	exportdirectory;
	PDWORD			address;
	PDWORD			name;
	PWORD 			ordinal;
	DWORD			i;

	nthdr = (PIMAGE_NT_HEADERS)(((PIMAGE_DOS_HEADER)baseaddr)->e_lfanew + baseaddr);
	exportdirectory = (PIMAGE_EXPORT_DIRECTORY)(baseaddr+nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress); 
	address=(PDWORD)(baseaddr+exportdirectory->AddressOfFunctions);
	name=(PDWORD)(baseaddr+exportdirectory->AddressOfNames);
	ordinal=(PWORD)(baseaddr+exportdirectory->AddressOfNameOrdinals);
	for(i=0;i<exportdirectory->NumberOfNames;i++)
	{
		if(shash((char*)baseaddr+name[i]) == symbolhash)
            		return (CINT)(baseaddr+address[ordinal[i]]);
    	}
 	return (0);
}

CINT   get_dllbase_by_peb(CINT peb, CINT modulehash)
{
        CINT   head;
        CINT   last;
        PWSTR   fullmodulename;

        if (peb && modulehash)
        {
                head = *(CINT *)((*(CINT*)(peb + PEB_LDR_DATA_OFFSET)) + INMEMORYORDERMODULELIST_OFFSET);
		last = *(CINT *)(head + sizeof(PVOID));
		while (last != head)
                {
                        fullmodulename = (PWSTR)(*(CINT *)(head + FULLDLLNAME_OFFSET));
			if (whash(_wcslwr(fullmodulename)) == modulehash)
                                return (*(CINT *)(head + DLLBASE_OFFSET));
                        head = *(CINT *)(head);
                }
        }
        return (0);
}

CINT   resolve_symbol(t_fonction *fct)
{
        if (g_module[fct->index].addr == 0)
        {
                if ((g_module[fct->index].addr = get_dllbase_by_peb(g_peb, g_module[fct->index].hash)))
                {
                        if ((fct->addr = get_exported_symbol(g_module[fct->index].addr, fct->hash)))
                                return (fct->addr);
                        else
                                return (0);
                }
                else
                        return (0);
        }
        else
        {
                if (fct->addr == 0)
                {
                        if ((fct->addr = get_exported_symbol(g_module[fct->index].addr, fct->hash)))
                                return (fct->addr);
                        else
                                return (0);
                }
                else
                        return (fct->addr);
        }
}
