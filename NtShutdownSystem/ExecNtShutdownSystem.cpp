
#include <Windows.h>
 
#define SeShutdownPrivilege (0x13)
 
int main(int argc, char* argv[]) {
	HMODULE ntdll = ::LoadLibrary("ntdll.dll");
	if (!ntdll) return 1;
	typedef int (* type_RtlAdjustPrivilege)(int, bool, bool, int*);
	typedef int (* type_NtShutdownSystem)(int);
	type_RtlAdjustPrivilege RtlAdjustPrivilege = (type_RtlAdjustPrivilege)GetProcAddress(ntdll, "RtlAdjustPrivilege");
	if (!RtlAdjustPrivilege) return 2;
	type_NtShutdownSystem NtShutdownSystem = (type_NtShutdownSystem)GetProcAddress(ntdll, "NtShutdownSystem");
	if (!NtShutdownSystem) return 3;
	int nEn = 0, nResult = 0, op = 2/*poweroff*/;
	if (argc >= 2 && (argv[1][0]=='r'&&argv[1][1]=='\0')) op = 1/*reboot*/;
	nResult = RtlAdjustPrivilege(SeShutdownPrivilege, true, false, &nEn);
	nResult = NtShutdownSystem(op);
	FreeLibrary(ntdll);
	return -1;
}