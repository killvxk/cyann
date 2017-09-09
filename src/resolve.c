#include "cyann.h"

DWORD get_exported_symbol(DWORD baseaddr, DWORD symbolhash)
{
        DWORD   export_directory_table;
        DWORD   name;
        DWORD   nameptr;
        DWORD   nbname;
        SHORT   address;

        if (baseaddr && symbolhash)
        {
                export_directory_table = *(DWORD *)(baseaddr + PE_HEADER_OFFSET);
                export_directory_table = (*(DWORD *)(export_directory_table + baseaddr + IMAGE_EXPORT_DIRECTORY_OFFSET)) + baseaddr;
                nbname = *(DWORD *)(export_directory_table + NUMBER_OF_NAME_OFFSET);
                nameptr = (*(DWORD *)(export_directory_table + ADDRESS_OF_NAME_OFFSET)) + baseaddr;
                while (nbname)
                {
                        name = (*(DWORD *) (nameptr + nbname * 4)) + baseaddr;
                        if (shash((char *)name) == symbolhash)
                        {
                                nameptr = (*(DWORD *)(export_directory_table + ADDRESSOFNAMEORDINAL_OFFSET)) + baseaddr;
                                address = (SHORT) (*(DWORD *)(nameptr + nbname * 2));
                                export_directory_table = (*(DWORD*)(export_directory_table + ADDRESSOFFUNCTION_OFFSET)) + baseaddr;
                                return ((*(DWORD *)(export_directory_table + address * 4)) + baseaddr);
                        }
                        nbname--;
                }
        }
        return (0);
}

DWORD   get_dllbase_by_peb(DWORD peb, DWORD modulehash)
{
        DWORD   head;
        DWORD   last;
        PWSTR   fullmodulename;

        if (peb && modulehash)
        {
                head = *(DWORD *)((*(DWORD*)(peb + PEB_LDR_DATA_OFFSET)) + INMEMORYORDERMODULELIST_OFFSET);
                last = *(DWORD *)(head + 4);
                while (last != head)
                {
                        fullmodulename = (PWSTR)(*(DWORD *)(head + FULLDLLNAME_OFFSET));
                        if (whash(_wcslwr(fullmodulename)) == modulehash)
                                return (*(DWORD *)(head + DLLBASE_OFFSET));
                        head = *(DWORD *)(head);
                }
        }
        return (0);
}

DWORD   resolve_symbol(t_fonction *fct)
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
