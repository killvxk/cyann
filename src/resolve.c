#include "cyann.h"

CINT get_exported_symbol(CINT baseaddr, CINT symbolhash)
{
	PIMAGE_NT_HEADERS64	nthdr;
	PIMAGE_EXPORT_DIRECTORY	exportdirectory;
	PDWORD			address;
	PDWORD			name;
	PWORD 			ordinal;
	DWORD			i;

	nthdr = (PIMAGE_NT_HEADERS64)(((PIMAGE_DOS_HEADER)baseaddr)->e_lfanew + baseaddr);
	exportdirectory = (PIMAGE_EXPORT_DIRECTORY)((LPBYTE)baseaddr+nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress); 
	address=(PDWORD)((LPBYTE)baseaddr+exportdirectory->AddressOfFunctions);
	name=(PDWORD)((LPBYTE)baseaddr+exportdirectory->AddressOfNames);
	ordinal=(PWORD)((LPBYTE)baseaddr+exportdirectory->AddressOfNameOrdinals);
	for(i=0;i<exportdirectory->AddressOfFunctions;i++)
	{
		if(shash((char*)baseaddr+name[i]) == symbolhash)
            		return (CINT)((LPBYTE)baseaddr+address[ordinal[i]]);
    	}
 	return (0);
	/*CINT   export_directory_table;
        CINT   name;
        CINT   nameptr;
        CINT   nbname;
        SHORT   address;

        if (baseaddr && symbolhash)
        {
                export_directory_table = *(CINT *)(baseaddr + PE_HEADER_OFFSET);
		export_directory_table = (*(CINT *)(export_directory_table + baseaddr + IMAGE_EXPORT_DIRECTORY_OFFSET)) + baseaddr;
                nbname = *(CINT *)(export_directory_table + NUMBER_OF_NAME_OFFSET);
                nameptr = (*(CINT *)(export_directory_table + ADDRESS_OF_NAME_OFFSET)) + baseaddr;
                while (nbname)
                {
                        name = (*(CINT *) (nameptr + nbname * 4)) + baseaddr;
                        if (shash((char *)name) == symbolhash)
                        {
                                nameptr = (*(CINT *)(export_directory_table + ADDRESSOFNAMEORDINAL_OFFSET)) + baseaddr;
                                address = (SHORT) (*(CINT *)(nameptr + nbname * 2));
                                export_directory_table = (*(CINT*)(export_directory_table + ADDRESSOFFUNCTION_OFFSET)) + baseaddr;
                                return ((*(CINT *)(export_directory_table + address * 4)) + baseaddr);
                        }
                        nbname--;
                }
        }
        return (0);*/
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
