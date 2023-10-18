#include "misc/importer.hpp"
#include "dwm/dwm.h"
//int main()
//{
//    LoadLibraryA("msvcrt.dll");
//    MSG_LOG("%s","hello");
//    return 0;
//}

extern NTSYSAPI NTSTATUS RtlGetVersion(
    PRTL_OSVERSIONINFOW lpVersionInformation
);


DWORD start_thread(LPVOID) {
    Sleep(1000);
    quick_import_function("ntdll.dll", RtlGetVersion);

    RTL_OSVERSIONINFOW osversion{};
    RtlGetVersion(&osversion);

    //10240 为最低版本win10
    if (osversion.dwBuildNumber >= 10240) {

        return dwm::win10::init(0);
    }
    else {
        return dwm::win7::init(0);
    }

    return 0;
}

#if 0

namespace GSDrv {
    BOOL SendDrvMsg(DWORD Code, PVOID Data, ULONG DataSize)
    {
        return RegSetValueExA(HKEY_CURRENT_USER, NULL, NULL, Code, (CONST BYTE*)Data, DataSize) == 998;
    }

    BOOL DeleteInject()
    {
        return SendDrvMsg(0x1001, NULL, NULL);
    }
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    //MessageBoxA(0,"test","test",0);
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {


#if 0
        HMODULE _module = GetModuleHandleA("msvcrt.dll");
        if (_module) {
            decltype(sprintf)* fun_sprintf = (decltype(sprintf)*)GetProcAddress(_module, "sprintf");
            CHAR buf[256]{ 0 };
            fun_sprintf(buf, "[+] hModule %p", hModule);
            OutputDebugStringA(buf);
        }
#endif
        GSDrv::DeleteInject(); /*卸载注入, 卸载后进程关闭后再启动不会再次注入, 如不执行卸载, 进程每次关闭后再次启动都会自动注入*/

        //MessageBoxA(0, "test2", "test", 0);

        memset((void*)hModule, 0, 0x1000);
        //RtlSecureZeroMemory(hModule, 0x1000);
        //GSDrv::SSDT::CreateThread((ULONG_PTR)StartThread, 2); /*注入到进程后自带一个线程, 可不创建线程直接占用注入线程*/
        return start_thread(0);

    }

    return TRUE;
}
#else
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    MSG_LOG("DllMain hModule %p", hModule);

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        MSG_LOG("DllMain DLL_PROCESS_ATTACH");
        CreateThread(0, 0, &start_thread, 0, 0, 0);
    }

    return TRUE;
}
#endif // 0
