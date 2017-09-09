#include "cyann.h"
#include "api_function.h"
#include "zlib.h"

PUCHAR	extract_rsrc(LPWSTR rname)
{
	HRSRC		hres;
	HGLOBAL		hglob;
	PUCHAR		res;
	FINDRESOURCE	findres;
	LOCKRESOURCE	lockres;
	LOADRESOURCE	loadres;
//	DECOMPRESSBUFF	decomp;
	ULONG		size;
	ULONG		csize;
//	ULONG		fsize;
	PUCHAR		out;		
int x;	
	if (!(findres = (FINDRESOURCE)resolve_symbol(&g_fonctions[4])))
		return (NULL);
	if (!(hres = findres(NULL, rname, RT_RCDATA)))
		return (NULL);
	if (!(loadres = (LOADRESOURCE)resolve_symbol(&g_fonctions[5])))
		return (NULL);
	if (!(hglob = loadres(NULL, hres)))
		return (NULL);
	if (!(lockres = (LOCKRESOURCE)resolve_symbol(&g_fonctions[6])))
		return (NULL);
	if (!(res = lockres(hglob)))
		return (NULL);
	size = *(ULONG *)res;
	csize = *(ULONG *)(res + 4);
	res = res + 8;
/*	if (!(decomp = (DECOMPRESSBUFF)resolve_symbol(&g_fonctions[7])))
		return (NULL);*/
	if (!(out = (PUCHAR)malloc(sizeof(UCHAR) * size))) // Fucking bad leaks above
		return (NULL);
	/*if ((*/x = uncompress(out, &size, res, csize);//))
	//{
		printf("%d\n", x);
		if (x != Z_OK)
		{
			if (x == Z_DATA_ERROR)
				printf("Data corrupted!\n");
			printf("uncompress failed!\n");
			return (NULL);
		}
	//}
/*	if (decomp(COMPRESSION_FORMAT_LZNT1, out, size, res, csize, &fsize))
		return (NULL);*/
	return (out);
}

t_pe	*parse_pe(PUCHAR in)//Todo: gerer les free si un malloc pete avant de ret NULL
{
	t_pe	*pe;
	PUCHAR	pehdr;
	PUCHAR	fsecthdr;

	if (!(pe = (t_pe *)malloc(sizeof(t_pe))))
		return (NULL);
	memcpy_s(&pe->doshdr, sizeof(IMAGE_DOS_HEADER), in, sizeof(IMAGE_DOS_HEADER));
	memcpy_s(&pe->nthdr, sizeof(IMAGE_NT_HEADERS64), in + pe->doshdr.e_lfanew, sizeof(IMAGE_NT_HEADERS64));
	pe->dosstubsize = pe->doshdr.e_lfanew - PE_HEADER_OFFSET - 0x4;
	if (!(pe->dosstub = (PUCHAR)malloc(sizeof(UCHAR) * pe->dosstubsize)))
		return (NULL);
	memcpy_s(pe->dosstub, pe->dosstubsize, in + PE_HEADER_OFFSET + 0x4, pe->dosstubsize);
	pe->nbsect = pe->nthdr.FileHeader.NumberOfSections;
	if (!(pe->secthdr = (IMAGE_SECTION_HEADER*)malloc(sizeof(IMAGE_SECTION_HEADER) * (pe->nbsect + 1))))
		return (NULL);
	if (!(pe->sections = (UCHAR **)malloc(sizeof(UCHAR*) * (pe->nbsect + 1))))
		return (NULL);
	pehdr = in + pe->doshdr.e_lfanew;
	fsecthdr = pehdr + 0x18 + pe->nthdr.FileHeader.SizeOfOptionalHeader;
	for (size_t i = 0; i < pe->nbsect; i++)
	{
		memcpy_s(&pe->secthdr[i], sizeof(IMAGE_SECTION_HEADER), fsecthdr + (i * sizeof(IMAGE_SECTION_HEADER)), sizeof(IMAGE_SECTION_HEADER));
		if (!(pe->sections[i] = (UCHAR*)malloc(sizeof(UCHAR) * pe->secthdr[i].SizeOfRawData)))
			return (NULL);
		memcpy_s(pe->sections[i], pe->secthdr[i].SizeOfRawData, in + pe->secthdr[i].PointerToRawData, pe->secthdr[i].SizeOfRawData);
		pe->sectsize += pe->secthdr[i].SizeOfRawData;
	}
	pe->sections[pe->nbsect] = NULL;
	pe->buffer = in;
	return (pe);
}

void	free2d(void **tab)
{
	void **bk = tab;

	while (*tab)
	{
		free(*tab);
		tab++;
	}
	free(bk);
}

void	free_pe(t_pe *pe)
{
	free(pe->dosstub);
	free(pe->secthdr);
	free2d((void**)pe->sections);
	free(pe);
}
