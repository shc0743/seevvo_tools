#include <iostream>
#include "../../resource/tool.h"
#include "service.h"
using namespace std;

#include "./cnw_user.h"
#pragma comment(lib, "customized-notification-window_x64.lib")
#pragma comment(lib, "comctl32.lib")

int ixui(int type) {
	// 0- closed   1- opened

	// 创建*.exe.prevent_ixmessages即可阻止出现此消息
	if (file_exists(GetProgramDirW() + L".prevent_ixmessages")) return 0;

	if (!CustomNotifications_Init()) return GetLastError();

	auto obj = CustomNotifications_Create();
	CustomNotifications_SetTitle(obj, L"Seewo Service");
	CustomNotifications_SetText(obj, (type == 1) ?
		L"Seewo Core Service reopened" : L"Seewo Core Service closed");

	CustomNotifications_SetShownState(obj, SW_NORMAL);

	CloseHandleIfOk( CustomNotifications_DelayDestroy(obj, 3 * 1000) );
	Sleep(5000);
	CustomNotifications_Destroy(obj);
	CustomNotifications_Unload();

	return 0;
}


