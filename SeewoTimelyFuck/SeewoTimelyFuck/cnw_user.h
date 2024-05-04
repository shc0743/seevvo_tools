#pragma once
#include <Windows.h>



DECLARE_HANDLE(HCUSTOMIZEDNOTIFICATIONOBJECT);

bool CustomNotifications_Init();
bool CustomNotifications_Unload();
HCUSTOMIZEDNOTIFICATIONOBJECT CustomNotifications_Create();

NTSTATUS CustomNotifications_SetTitle(
	HCUSTOMIZEDNOTIFICATIONOBJECT hObj,
	LPCWSTR pszTitle
);
NTSTATUS CustomNotifications_SetText(
	HCUSTOMIZEDNOTIFICATIONOBJECT hObj,
	LPCWSTR pszText
);

NTSTATUS CustomNotifications_SetShownState(
	HCUSTOMIZEDNOTIFICATIONOBJECT hObj,
	int nShow
);

NTSTATUS CustomNotifications_Destroy(
	HCUSTOMIZEDNOTIFICATIONOBJECT hObj
);
[[nodiscard]] HANDLE CustomNotifications_DelayDestroy(
	HCUSTOMIZEDNOTIFICATIONOBJECT hObj,
	DWORD dwMillseconds
);



