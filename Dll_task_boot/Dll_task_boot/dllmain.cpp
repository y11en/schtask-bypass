#define _WIN32_DCOM

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <comdef.h>
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")

using namespace std;

int login(LPWSTR filename)
{

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    hr = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        0,
        NULL);

    string wszTaskName = "WindowsWatchDog";

    ITaskService* pService = NULL;
    hr = CoCreateInstance(CLSID_TaskScheduler,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_ITaskService,
        (void**)&pService);

    hr = pService->Connect(_variant_t(), _variant_t(),
        _variant_t(), _variant_t());

    ITaskFolder* pRootFolder = NULL;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);

    pRootFolder->DeleteTask(_bstr_t(wszTaskName.c_str()), 0);

    ITaskDefinition* pTask = NULL;
    hr = pService->NewTask(0, &pTask);

    pService->Release();


    IRegistrationInfo* pRegInfo = NULL;
    hr = pTask->get_RegistrationInfo(&pRegInfo);

    hr = pRegInfo->put_Author(_bstr_t("Administrator"));
    pRegInfo->Release();

    ITaskSettings* pSettings = NULL;
    hr = pTask->get_Settings(&pSettings);

    hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
    pSettings->Release();

    ITriggerCollection* pTriggerCollection = NULL;
    hr = pTask->get_Triggers(&pTriggerCollection);

    ITrigger* pTrigger = NULL;
    hr = pTriggerCollection->Create(TASK_TRIGGER_BOOT, &pTrigger);
    pTriggerCollection->Release();

    IBootTrigger* pBootTrigger = NULL;
    hr = pTrigger->QueryInterface(
        IID_IBootTrigger, (void**)&pBootTrigger);
    pTrigger->Release();

    hr = pBootTrigger->put_Id(_bstr_t(L"Trigger1"));

    hr = pBootTrigger->put_StartBoundary(_bstr_t(L"2005-01-01T12:05:00"));

    hr = pBootTrigger->put_EndBoundary(_bstr_t(L"2035-05-02T08:00:00"));

    hr = pBootTrigger->put_Delay(_bstr_t("PT10S"));
    pBootTrigger->Release();

    IActionCollection* pActionCollection = NULL;

    hr = pTask->get_Actions(&pActionCollection);

    IAction* pAction = NULL;
    hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
    pActionCollection->Release();

    IExecAction* pExecAction = NULL;
    hr = pAction->QueryInterface(
        IID_IExecAction, (void**)&pExecAction);
    pAction->Release();

    hr = pExecAction->put_Path(filename);
    pExecAction->Release();

    IRegisteredTask* pRegisteredTask = NULL;
    VARIANT varPassword;
    varPassword.vt = VT_EMPTY;
    hr = pRootFolder->RegisterTaskDefinition(
        _bstr_t(wszTaskName.c_str()),
        pTask,
        TASK_CREATE_OR_UPDATE,
        _variant_t(L"SYSTEM"),
        varPassword,
        TASK_LOGON_SERVICE_ACCOUNT,
        _variant_t(L""),
        &pRegisteredTask);
    if (FAILED(hr))
    {
        printf("\nError saving the Task : %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    printf("Success!");

    pRootFolder->Release();
    pTask->Release();
    pRegisteredTask->Release();
    CoUninitialize();
    return 0;
}



extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPTSTR lpCmdLine,
    int nCmdShow
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:;
        LPWSTR* szArgList;
        int argCount;

        szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
        if (argCount != 3)
        {
            cout << "rundll.exe Dll_task_boot,DllMain filename" << endl;
            return 1;
        }
        login(szArgList[2]);
        LocalFree(szArgList);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

