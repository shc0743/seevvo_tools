#include <iostream>
#include "../../resource/tool.h"
#include "service.h"
using namespace std;

wchar_t SERVICE_NAMEDPIPE_NAME[256] = L"\\\\.\\pipe\\seewo-servicing-{ad7ca1b0-"
"48f4-468a-a16c-81725e86f637}-";

wstring szSvcName;
vector<wstring> szStartTimes;

int ixui(int type);

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


int main()
{
	CmdLineW cl(GetCommandLineW());
	wstring svcName;
	SetCurrentDirectoryW((GetProgramPathW()).c_str());

	if ((file_exists(GetProgramInfo().name + ".data"))) {
		fstream fp(GetProgramInfo().name + ".data");
		char tt[256]{};
		while (fp.getline(tt, 256))
			if (tt[0] != 0 && tt[0] != '\r' && tt[0] != '\n' && tt[0] != ' ')
				szStartTimes.push_back(s2ws(tt));
		fp.close();
	}

	if (cl.getopt(L"s", svcName) != 0 && (!svcName.empty())) {
		if (szStartTimes.size() < 1) return ERROR_INVALID_CONFIG_VALUE;

		::szSvcName = svcName;
		wcscat_s(SERVICE_NAMEDPIPE_NAME, svcName.c_str());
		SeewoTimelyFuckService ctsvc(svcName);
		return ctsvc.StartAsService() ? 0 : GetLastError();
	}

	if (cl.getopt(L"testsigning")) {
		fstream fp(GetProgramInfo().name + ".data", ios::app);
		fp << "00:00:00" << endl;
		fp.close();

		return Process.StartAndWait(L"\"" + GetProgramDirW() + L"\" --testsigningv2");
	}

	if (cl.getopt(L"x", svcName) != 0 && (!svcName.empty())) {
		wstring fn = L"1";
		WCHAR pipeName[256]{};
		::szSvcName = svcName;
		wcscat_s(SERVICE_NAMEDPIPE_NAME, svcName.c_str());
		wcscpy_s(pipeName, SERVICE_NAMEDPIPE_NAME);

		HANDLE hPipe = CreateFile(
			pipeName,
			GENERIC_READ | GENERIC_WRITE,
			0, NULL, OPEN_EXISTING, 0, NULL
		);

		if (hPipe == INVALID_HANDLE_VALUE) {
			return 1;
		}

		DWORD bytesWritten;
		WriteFile(hPipe, fn.c_str(), DWORD((fn.length() + 1) * 2), &bytesWritten, NULL);

		wchar_t buffer[2048]{};
		DWORD bytesRead = 0;
		BOOL result = ReadFile(hPipe, buffer, sizeof(buffer) - 2, &bytesRead, NULL);
		if (result && bytesRead > 0) {
			wcout << buffer;
		}
		CloseHandle(hPipe);
		return 0;
	}

	if (cl.getopt(L"d")) {
		fstream fp(GetProgramInfo().name + ".log", ios::app);
		fp << "seewo tried to start when " << time(0) << ", cmd= " << GetCommandLineA() << endl;
		fp.close();
		return 0;
	}

	if (cl.getopt(L"ix1")) {
		FreeConsole();
		(void)CoInitialize(NULL);
		LoadLibraryW(L"Comctl32.lib");
		//INITCOMMONCONTROLSEX icce{};
		//icce.dwSize = sizeof(icce);
		//icce.dwICC = ICC_ALL_CLASSES;
		//{ (void)0; }
		//if (!InitCommonControlsEx(&icce)) {
		//	return GetLastError();
		//}

		return ixui((cl.getopt(L"ix1n")) ? 1 : 0);
	}

	PCWSTR sstip = L"";
	DWORD ddtip = 0;
	auto w = [&] {WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), sstip,
		(DWORD)wcslen(sstip), &ddtip, 0); };

	if (szStartTimes.empty()) {
		if (cl.getopt(L"1")) {
			cerr << "Invalid input!! ";
			pause;
			return -1;
		}
		char tx[32]{};
		fstream fp(GetProgramInfo().name + ".data", ios::app);
		do {
			sstip = L"请输入时间(HH:mm:ss)，^Z完成: ";
			w();
			cin.getline(tx, 32);
			fp << (tx) << endl;
		}while (cin.eof() == false);
		fp.close();
		cout << endl;
		return Process.StartAndWait(L"\"" + GetProgramDirW() + L"\" -1");
	}

	if (!IsRunAsAdmin()) {
		ShellExecuteW(NULL, L"runas", (L"\"" + GetProgramDirW() + L"\"").c_str(), L" ", 0, 1);
		return 0;
	}
	
	sstip = L"即将安装服务 SeewoTimelyServicingObjectiveTool ";
	w();
	for (int i = 0; i < 5; ++i) {
		Sleep(1000);
		_putch('.');
	}
	cout << endl;

	wstring sname = L"SeewoTimelyServicingObjectiveTool";
	ServiceManager.Remove(ws2s(sname));
	auto r = ServiceManager.New(sname, L"\"" + GetProgramDirW() + L"\" -s\"" +
		sname + L"\"", ServiceManager.Auto, L"", L"Seewo Timely Service Object"
		"ive Model Tool.", SERVICE_WIN32_OWN_PROCESS);
	if (r) {
		auto ss = L"失败：" + LastErrorStrW() + L" (" + to_wstring(r) + L")";
		sstip = ss.c_str();
		w();
		pause;
		return GetLastError();
	}

	ServiceManager.Start(ws2s(sname));
	sstip = L"成功。";
	w();
	Sleep(2000);
	if (cl.getopt(L"testsigningv2")) {
		Process.StartOnly(L"\"" + GetProgramDirW() + L"\" -x" + sname);
	}
	return 0;
}





void run() {
	HKEY hkey = 0;

	if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\"
		"Image File Execution Options\\SeewoCore.exe", 0, 0, 0, KEY_READ | KEY_WRITE,
		0, &hkey, 0) == ERROR_SUCCESS) {
		RegCloseKey(hkey); hkey = 0;
	}

	MySetRegistryValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\"
		"Image File Execution Options\\SeewoCore.exe", L"Debugger", L"\"" + GetProgramDirW() +
		L"\" -d -n\"" + szSvcName + L"\" -p ");

	Process.StartAndWait(L"taskkill.exe /f /im SeewoCore.exe /t");

	auto sess = WTSGetActiveConsoleSessionId();
	wstring cmdLine = L"--ix1";
	STARTUPINFO si{}; PROCESS_INFORMATION pi{};
	(void)Process.StartAsActiveUserT((GetProgramDirW())
		.c_str(), (LPTSTR)cmdLine.c_str(), NULL, NULL, FALSE, 0, 0, 0, &si, &pi);

	Sleep(60 * 1000);

	hkey = 0;
	(void)RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\"
		"Image File Execution Options\\SeewoCore.exe", 0, 0, 0, KEY_READ | KEY_WRITE,
		0, &hkey, 0);
	if (hkey) {
		RegDeleteValueW(hkey, L"Debugger");
		RegCloseKey(hkey);
	}
	else {

		fstream fp(GetProgramInfo().name + ".log", ios::app);
		fp << "[reopenSeewo] hkey is NULL" << endl; fp.close();
	}

	ServiceManager.Start("SeewoCoreService");
	Process.StartOnly_HiddenWindow(L"cmd.exe /c test.cmd");

	cmdLine = L"--ix1 --ix1n";
	(void)Process.StartAsActiveUserT((GetProgramDirW())
		.c_str(), (LPTSTR)cmdLine.c_str(), NULL, NULL, FALSE, 0, 0, 0, &si, &pi);

}



// Function to handle a client connection  
static void __stdcall OpenerPipe_ClientHandler(void* pParam) {
	HANDLE hPipe = (HANDLE)pParam;
	WCHAR buffer[2048]{};
	DWORD bytesRead = 0;
	BOOL result = ReadFile(hPipe, buffer, sizeof(buffer) - 2, &bytesRead, NULL);
	if (result && bytesRead > 0) {

		run();

		wstring str = L"ok";

		// Send the data back to the client  
		DWORD bytesWritten;
		WriteFile(hPipe, str.c_str(), DWORD((str.length() + 1) * 2), &bytesWritten, NULL);
	}
	else {
		/*std::cerr << "ReadFile failed with error: "
			<< GetLastError() << std::endl;*/
	}

	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	_endthread();
	return;
}
static DWORD WINAPI GlobalOpenerPipe(PVOID) {
	WCHAR pipeName[256]{};
	wcscpy_s(pipeName, SERVICE_NAMEDPIPE_NAME);
	//if (!LoadStringW(hInst, IDS_STRING_SERVICE_NAMEDPIPE_NAME,
	//	pipeName, 256)) return GetLastError();
	HANDLE hPipe;
	SECURITY_ATTRIBUTES sa{ 0 };
	SECURITY_DESCRIPTOR sd{ 0 };
	{
		SID_IDENTIFIER_AUTHORITY sia = SECURITY_WORLD_SID_AUTHORITY;
		PSID pSid = NULL;
		AllocateAndInitializeSid(&sia, 1, 0, 0, 0, 0, 0, 0, 0, 0, &pSid); // Everyone
		BYTE buf[0x400] = { 0 };
		PACL pAcl = (PACL)&buf;
		InitializeAcl(pAcl, 1024, ACL_REVISION);
		AddAccessAllowedAce(pAcl, ACL_REVISION,
			GENERIC_READ | GENERIC_WRITE | READ_CONTROL, pSid);
		InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd, TRUE, (PACL)pAcl, FALSE);
		sa.nLength = (DWORD)sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = (LPVOID)&sd;
		sa.bInheritHandle = TRUE;
		FreeSid(pSid);
	}

	while (true) { // Loop to accept multiple connections  
		hPipe = CreateNamedPipeW(
			pipeName,
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			64,
			1024, 1024, 0, &sa
		);

		if (hPipe == INVALID_HANDLE_VALUE || !hPipe) {
			DWORD err = GetLastError();
			fstream fp("error.log", ios::app);
			fp << "[" << time(0) << "] " << "CreateNamedPipe "
				"failed with error: " << err << "while pipe name is: " <<
				ws2s(pipeName) << std::endl;
			fp.close();
			Sleep(1000); // Wait for a second and retry
			continue;
		}


		ConnectNamedPipe(hPipe, NULL);
		//std::cout << "Client connected to the named pipe." << std::endl;

		// Start a new thread to handle the client  
		unsigned threadID = 0;
		_beginthread(OpenerPipe_ClientHandler, 0, (void*)hPipe);
	}

	return 0;
}
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")
static DWORD WINAPI ScheduledTaskCreator(PVOID) {
	wstring today;
	{
		SYSTEMTIME st{};
		//WCHAR buffer[512]{};
		WCHAR buffer2[512]{};
		GetLocalTime(&st);
		PCWSTR pDateFormat = NULL, pTimeFormat = NULL;
		static WCHAR dateformat[256] = L"yyyy-MM-dd";
		//static WCHAR timeformat[256] = L"HH:mm:ss";
		if (dateformat[0]) pDateFormat = dateformat;
		//if (timeformat[0]) pTimeFormat = timeformat;
		//GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, pTimeFormat, buffer, 512);
		GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, pDateFormat, buffer2, 512, 0);
		today = buffer2;
		today += L"T";
	}

	auto LOGGER_GENERATE_MESSAGE = [](int, LPCWSTR text, LPCWSTR title, int) {
		if (!text) return 2;
		fstream fp("error.log", ios::app);
		fp << ws2s(L"Error: [["s + (title ? title : L"untitled") + L"]] " +
			text + L" \r\n");
		return 1; // TODO
	};
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		LOGGER_GENERATE_MESSAGE(0, L"Co cannot init", 0, 0);
		return hr;
	}

	auto create = [&](size_t nn,wstring time) {

		WCHAR playStartTime[64]{};
		wcscpy_s(playStartTime, time.c_str());
		if (playStartTime[0] == 0) {
			return LOGGER_GENERATE_MESSAGE(NULL, L"无效参数", 0, MB_ICONERROR);
		}
		wstring wstrExecutablePath = L"-x\"" + szSvcName + L"\"";
		ITaskService* pService = nullptr;
		HRESULT hr = CoCreateInstance(
			CLSID_TaskScheduler,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_ITaskService,
			(void**)&pService
		);
		if (FAILED(hr)) {
			return LOGGER_GENERATE_MESSAGE(NULL, L"COM调用异常", 0, MB_ICONERROR);
		}
		//  Connect to the task service.
		hr = pService->Connect(VARIANT(), VARIANT(),
			VARIANT(), VARIANT());
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM调用异常2", 0, MB_ICONERROR);
			pService->Release();
			return 1;
		}
		ITaskFolder* pRootFolder = nullptr;
		WCHAR rt[] = L"\\";
		hr = pService->GetFolder(rt, &pRootFolder);
		if (FAILED(hr)) {
			LOGGER_GENERATE_MESSAGE(NULL, L"COM调用异常3", 0, MB_ICONERROR);
			pService->Release();
			return 1;
		}
		WCHAR wszTaskName[128] = L"{ad7ca1b0-48f4-468a-a16c-81725e86f637}-";
		wcscat_s(wszTaskName, (szSvcName + L"-" + to_wstring(nn)).c_str());
		//LOGGER_GENERATE_MESSAGE(0, wszTaskName, L"task", 0);
		//  If the same task exists, remove it.
		pRootFolder->DeleteTask((wszTaskName), 0);

		//  Create the task definition object to create the task.
		ITaskDefinition* pTask = NULL;
		hr = pService->NewTask(0, &pTask);

		pService->Release();  // COM clean up.  Pointer is no longer used.
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：NewTask", 0, MB_ICONERROR);
			pRootFolder->Release();
			return 1;
		}

		//  ------------------------------------------------------
		//  Get the registration info for setting the identification.
		IRegistrationInfo* pRegInfo = NULL;
		hr = pTask->get_RegistrationInfo(&pRegInfo);
		if (FAILED(hr))
		{
			printf("\nCannot get identification pointer: %x", hr);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		//hr = pRegInfo->put_Author(L"Author Name");
		pRegInfo->Release();
		/*if (FAILED(hr))
		{
			printf("\nCannot put identification info: %x", hr);
			pRootFolder->Release();
			pTask->Release();
			CoUninitialize();
			return 1;
		}*/

		//  ------------------------------------------------------
		//  Create the principal for the task - these credentials
		//  are overwritten with the credentials passed to RegisterTaskDefinition
		IPrincipal* pPrincipal = NULL;
		hr = pTask->get_Principal(&pPrincipal);
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：get_Principal", 0, MB_ICONERROR);
			printf("\nCannot get principal pointer: %x", hr);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		//  Set up principal logon type to interactive logon
		hr = pPrincipal->put_UserId(BSTR(L"NT AUTHORITY\\SYSTEM"));
		hr = pPrincipal->put_LogonType(TASK_LOGON_SERVICE_ACCOUNT);
		pPrincipal->Release();
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：put_LogonType", 0, MB_ICONERROR);
			printf("\nCannot put principal info: %x", hr);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		//  ------------------------------------------------------
		//  Create the settings for the task
		ITaskSettings* pSettings = NULL;
		hr = pTask->get_Settings(&pSettings);
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：get_Settings", 0, MB_ICONERROR);
			printf("\nCannot get settings pointer: %x", hr);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		//  Set setting values for the task.  
		hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
		pSettings->Release();
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：put_StartWhenAvailable", 0, MB_ICONERROR);
			printf("\nCannot put setting information: %x", hr);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		// Set the idle settings for the task.
		IIdleSettings* pIdleSettings = NULL;
		hr = pSettings->get_IdleSettings(&pIdleSettings);
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：get_IdleSettings", 0, MB_ICONERROR);
			printf("\nCannot get idle setting information: %x", hr);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		//hr = pIdleSettings->put_WaitTimeout(L"PT5M");
		pIdleSettings->Release();
		//if (FAILED(hr))
		//{
			//LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：put_WaitTimeout", 0, MB_ICONERROR);
			//printf("\nCannot put idle setting information: %x", hr);
			//pRootFolder->Release();
			//pTask->Release();
			//return 1;
		//}


		//  ------------------------------------------------------
		//  Get the trigger collection to insert the time trigger.
		ITriggerCollection* pTriggerCollection = NULL;
		hr = pTask->get_Triggers(&pTriggerCollection);
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：get_Triggers", 0, MB_ICONERROR);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		//  Add the time trigger to the task.
		ITrigger* pTrigger = NULL;
		hr = pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);
		pTriggerCollection->Release();
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：create trigger", 0, MB_ICONERROR);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		ITimeTrigger* pTimeTrigger = NULL;
		hr = pTrigger->QueryInterface(
			IID_ITimeTrigger, (void**)&pTimeTrigger);
		pTrigger->Release();
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：QueryInterface", 0, MB_ICONERROR);
			printf("\nQueryInterface call failed for ITimeTrigger: %x", hr);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		hr = pTimeTrigger->put_Id(BSTR(L"MyTrigger"));
		if (FAILED(hr))
			printf("\nCannot put trigger ID: %x", hr);

		//hr = pTimeTrigger->put_EndBoundary(BSTR(L"2099-12-31T08:00:00"));
		if (FAILED(hr))
			printf("\nCannot put end boundary on trigger: %x", hr);

		//  Set the task to start at a certain time. The time 
		//  format should be YYYY-MM-DDTHH:MM:SS(+-)(timezone).
		//  For example, the start boundary below
		//  is January 1st 2005 at 12:05
		hr = pTimeTrigger->put_StartBoundary(BSTR(playStartTime));
		pTimeTrigger->Release();
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：put_StartBoundary", 0, MB_ICONERROR);
			printf("\nCannot add start boundary to trigger: %x", hr);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}


		//  ------------------------------------------------------
		//  Add an action to the task. This task will execute notepad.exe.     
		IActionCollection* pActionCollection = NULL;

		//  Get the task action collection pointer.
		hr = pTask->get_Actions(&pActionCollection);
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：get_Actions", 0, MB_ICONERROR);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		//  Create the action, specifying that it is an executable action.
		IAction* pAction = NULL;
		hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
		pActionCollection->Release();
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：createaction", 0, MB_ICONERROR);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		IExecAction* pExecAction = NULL;
		//  QI for the executable task pointer.
		hr = pAction->QueryInterface(
			IID_IExecAction, (void**)&pExecAction);
		pAction->Release();
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：QueryInterface", 0, MB_ICONERROR);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		//  Set the path of the executable to notepad.exe.
		hr = pExecAction->put_Path(BSTR(GetProgramDirW().c_str()));
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：put_Path", 0, MB_ICONERROR);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}
		hr = pExecAction->put_Arguments(BSTR(wstrExecutablePath.c_str()));
		pExecAction->Release();
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：put_Arguments", 0, MB_ICONERROR);
			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		//  ------------------------------------------------------
		//  Save the task in the root folder.
		IRegisteredTask* pRegisteredTask = NULL;
		hr = pRootFolder->RegisterTaskDefinition(
			BSTR(wszTaskName),
			pTask,
			TASK_CREATE_OR_UPDATE,
			VARIANT(),
			VARIANT(),
			TASK_LOGON_SERVICE_ACCOUNT,
			VARIANT(),
			&pRegisteredTask);
		if (FAILED(hr))
		{
			LOGGER_GENERATE_MESSAGE(NULL, L"COM失败：RegisterTaskDefinition", 0, MB_ICONERROR);

			pRootFolder->Release();
			pTask->Release();
			return 1;
		}

		printf("\n Success! Task successfully registered. ");

		//  Clean up.
		pRootFolder->Release();
		pTask->Release();
		pRegisteredTask->Release();
		return 0;
	};
	for (size_t i = 0; i < szStartTimes.size(); ++i) {
		create(i, today + szStartTimes[i]);
	}

	return 0;
}

constexpr LPTHREAD_START_ROUTINE workerThreads[] = {
	GlobalOpenerPipe,
	ScheduledTaskCreator,
};
constexpr size_t workerThreadsCount =
sizeof(workerThreads) / sizeof(LPTHREAD_START_ROUTINE);
HANDLE hServiceWorkerThreads[min(32, workerThreadsCount)] = {};
DWORD WINAPI srv_main(PVOID) {
	EnableAllPrivileges();

	AutoZeroMemory(hServiceWorkerThreads);

	for (size_t i = 0; i < workerThreadsCount; ++i) {
		hServiceWorkerThreads[i] =
			CreateThread(0, 0, workerThreads[i], 0, 0, 0);
		if (!hServiceWorkerThreads[i]) {
			return GetLastError();
		}
	}


	WaitForMultipleObjects(workerThreadsCount,
		hServiceWorkerThreads, TRUE, INFINITE);
	for (size_t i = 0; i < workerThreadsCount; ++i)
		if (hServiceWorkerThreads[i]) CloseHandle(hServiceWorkerThreads[i]);

	return 0;
}


