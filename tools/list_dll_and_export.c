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
        PIMAGE_NT_HEADERS       nthdr;
        PIMAGE_EXPORT_DIRECTORY exportdirectory;
        PDWORD                  address;
        PDWORD                  name;
        char			*n0;
	PWORD                   ordinal;
        DWORD                   i;

        nthdr = (PIMAGE_NT_HEADERS)(((PIMAGE_DOS_HEADER)baseaddr)->e_lfanew + baseaddr);
        exportdirectory = (PIMAGE_EXPORT_DIRECTORY)(baseaddr+nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
        address=(PDWORD)(baseaddr+exportdirectory->AddressOfFunctions);
        name=(PDWORD)(baseaddr+exportdirectory->AddressOfNames);
        ordinal=(PWORD)(baseaddr+exportdirectory->AddressOfNameOrdinals);
        for(i=0;i<exportdirectory->NumberOfNames;i++)
        {
		n0 = (char*)baseaddr+name[i];
		if (dllname)
			printf("%S -> %d =>%s -> %d\n", dllname, whash(dllname), n0, shash(n0));
		else
			printf("%s -> %d\n", n0, shash(n0));
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

int	main(int argc, char **argv)
{
	if (argc == 1)
	{
		DWORD peb = 0;
		get_peb(&peb);
		list(peb);
	}
	HANDLE lib = LoadLibrary(argv[1]);
	if (lib)
	{
		printf("Lib at %p\n", lib);
		list_and_print((DWORD)lib, NULL);
	}
	else
		printf("Can't load lib!\n");
	return (0);
}
