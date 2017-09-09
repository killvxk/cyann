#include <stdio.h>
#include <windows.h>

# define PE_HEADER_OFFSET		0x3c
# define IMAGE_EXPORT_DIRECTORY_OFFSET	0x78
# define NUMBER_OF_NAME_OFFSET		0x18
# define ADDRESS_OF_NAME_OFFSET		0x20
# define ADDRESSOFNAMEORDINAL_OFFSET	0x24
# define ADDRESSOFFUNCTION_OFFSET	0x1c
# define PEB_LDR_DATA_OFFSET		0x0c
# define INMEMORYORDERMODULELIST_OFFSET	0x14
# define FULLDLLNAME_OFFSET		0x28
# define DLLBASE_OFFSET			0x10


DWORD   shash(char *s)
{
        DWORD   i;

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
        DWORD   i;

        i = 0;
        while (*s)
        {
                i *= 16777619;
                i ^= *s++;
        }
        return (i);
}


void    get_peb(DWORD *peb)
{
        if (peb)
                asm("push %%fs:0x18\n"
                "mov (%%esp), %%eax\n"
                "pop %%ebx\n"
                "mov 0x30(%%eax), %0"
                : "=a"(*peb) :
                );
}


DWORD list_and_print(DWORD baseaddr, WCHAR *dllname)
{
        DWORD   export_directory_table;
        DWORD   name;
        DWORD   nameptr;
        DWORD   nbname;
        SHORT   address;

        if (baseaddr)
        {
                export_directory_table = *(DWORD *)(baseaddr + PE_HEADER_OFFSET);
                export_directory_table = (*(DWORD *)(export_directory_table + baseaddr + IMAGE_EXPORT_DIRECTORY_OFFSET)) + baseaddr;
                nbname = *(DWORD *)(export_directory_table + NUMBER_OF_NAME_OFFSET);
                nameptr = (*(DWORD *)(export_directory_table + ADDRESS_OF_NAME_OFFSET)) + baseaddr;
                while (nbname)
                {
                        name = (*(DWORD *) (nameptr + nbname * 4)) + baseaddr;
                        printf("%S -> %d =>%s -> %d\n", dllname, whash(dllname), (char *)name, shash((char *)name));
                        nbname--;
                }
        }
        return (0);
}

DWORD   list(DWORD peb)
{
        DWORD   head;
        DWORD   last;
        PWSTR   fullmodulename;

        if (peb)
        {
                head = *(DWORD *)((*(DWORD*)(peb + PEB_LDR_DATA_OFFSET)) + INMEMORYORDERMODULELIST_OFFSET);
                last = *(DWORD *)(head + 4);
                while (last != head)
                {
                        fullmodulename = (PWSTR)(*(DWORD *)(head + FULLDLLNAME_OFFSET));
			if (wcsstr(_wcslwr(fullmodulename), L".dll"))
	                        list_and_print(*(DWORD *)(head + DLLBASE_OFFSET), fullmodulename);
                        head = *(DWORD *)(head);
                }
        }
        return (0);
}

int	main(void)
{
	DWORD peb = 0;
	get_peb(&peb);
	list(peb);
	return (0);
}
