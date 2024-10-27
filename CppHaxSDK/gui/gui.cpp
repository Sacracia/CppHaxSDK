#include "gui.h"

#include <Windows.h>
#include <TlHelp32.h>

#include <vector>
#include <string>
#include <algorithm>

#include <logger.h>
#include <backend/opengl.h>
#include <backend/directx9.h>

//static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam) {
//	const auto isMainWindow = [handle]() {
//		return GetWindow(handle, GW_OWNER) == nullptr && IsWindowVisible(handle);
//		};
//
//	DWORD pID = 0;
//	GetWindowThreadProcessId(handle, &pID);
//
//	if (GetCurrentProcessId() != pID || !isMainWindow() || handle == GetConsoleWindow())
//		return TRUE;
//
//	*reinterpret_cast<HWND*>(lParam) = handle;
//
//	return FALSE;
//}
//
//static HWND GetProcessWindow() {
//	HWND hwnd = nullptr;
//	EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&hwnd));
//
//	while (!hwnd) {
//		EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&hwnd));
//		std::this_thread::sleep_for(std::chrono::milliseconds(200));
//	}
//
//	char name[128];
//	GetWindowTextA(hwnd, name, RTL_NUMBER_OF(name));
//	return hwnd;
//}

namespace haxsdk {
	void ImplementGui(unsigned int key) {
		DWORD processId = GetProcessId(GetCurrentProcess());
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

		//HWND hwnd = GetProcessWindow();

		MODULEENTRY32 me{};
		me.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(snapshot, &me)) {
			do {
				std::string moduleName(me.szModule);
				std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::tolower);
				if (moduleName == "opengl32.dll") {
					LOG_INFO << "OPENGL graphics api found" << LOG_FLUSH;
					opengl::HookOpenGL(key);
				}
				if (moduleName == "d3d9.dll") {
					LOG_INFO << "DIRECTX9 graphics api found" << LOG_FLUSH;
					directx9::HookDirectx9(key);
				}
				else if (moduleName == "d3d10.dll") {
					LOG_INFO << "DIRECTX10 graphics api found" << LOG_FLUSH;
				}
				else if (moduleName == "d3d11.dll") {
					LOG_INFO << "DIRECTX11 graphics api found" << LOG_FLUSH;
				}
				else if (moduleName == "d3d12.dll") {
					LOG_INFO << "DIRECTX12 graphics api found" << LOG_FLUSH;
				}
				else if (moduleName == "vulkan-1.dll") {
					LOG_INFO << "VULKAN graphics api found" << LOG_FLUSH;
				}
			} while (Module32Next(snapshot, &me));
		}

		CloseHandle(snapshot);
	}
}