#include "haxsdk.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

static HaxGlobals g_globals;

static void DetermineBackend();

HaxGlobals& HaxSdk::GetGlobals() {
    return g_globals;
}

void HaxSdk::InitializeCore() {
    DetermineBackend();
    HAX_ASSERT(g_globals.backend != HaxBackend_None, "Unable to determine unity backend");
}

static void DetermineBackend() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    MODULEENTRY32W moduleEntry = {0};
    moduleEntry.dwSize = sizeof(moduleEntry);
    if (Module32FirstW(hSnapshot, &moduleEntry)) {
        do {
            if (wcsncmp(moduleEntry.szModule, L"mono", 4) == 0) {
                CloseHandle(hSnapshot);
                g_globals.backend = HaxBackend_Mono;
                g_globals.backendHandle = (void*)moduleEntry.hModule;
                return;
            }
            if (wcscmp(moduleEntry.szModule, L"GameAssembly.dll") == 0) {
                CloseHandle(hSnapshot);
                g_globals.backend = HaxBackend_IL2CPP;
                g_globals.backendHandle = (void*)moduleEntry.hModule;
                return;
            }
        } while (Module32NextW(hSnapshot, &moduleEntry));
    }
    CloseHandle(hSnapshot);
    g_globals.backend = HaxBackend_None;
}