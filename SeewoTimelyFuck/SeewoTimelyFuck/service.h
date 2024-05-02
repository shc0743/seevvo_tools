#pragma once
// service.h  

#ifndef SERVICE_H  
#define SERVICE_H  

#include <windows.h>  
#include <string>  
#include <thread>  

class SeewoTimelyFuckService {
public:
    SeewoTimelyFuckService(const std::wstring& serviceName);
    virtual ~SeewoTimelyFuckService();

    bool StartAsService();
    static void WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
    static void WINAPI ServiceCtrlHandler(DWORD ctrlCode);

    // TODO: Declare additional methods and members as needed for your service logic.  

    static SeewoTimelyFuckService* pCurrentService;

protected:
    SERVICE_STATUS_HANDLE serviceStatusHandle;
    SERVICE_STATUS serviceStatus;
    HANDLE stopServiceEvent;
    std::wstring serviceName;
    DWORD last_stat;


    static DWORD WINAPI StoppingThrd(PVOID);
    static void End_UI_Process();
    static BOOL UpdateServiceStatus();


    // Disallow copying and assignment.  
    SeewoTimelyFuckService(const SeewoTimelyFuckService&) = delete;
    SeewoTimelyFuckService& operator=(const SeewoTimelyFuckService&) = delete;
};

#endif // SERVICE_H