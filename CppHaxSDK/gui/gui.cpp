#include "gui.h"

#include <Windows.h>
#include <TlHelp32.h>

#include <vector>
#include <string>
#include <algorithm>

#include <logger.h>
#include <backend/opengl.h>

namespace haxsdk {
	void ImplementGui() {
		DWORD processId = GetProcessId(GetCurrentProcess());
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

		MODULEENTRY32 me{};
		me.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(snapshot, &me)) {
			do {
				std::string moduleName(me.szModule);
				std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::tolower);
				if (moduleName == "opengl32.dll") {
					LOG_INFO << "OPENGL32" << LOG_FLUSH;
					opengl::HookOpenGL();
				}
				if (moduleName == "d3d9.dll") {
					LOG_INFO << "DIRECTX9" << LOG_FLUSH;
				}
				else if (moduleName == "d3d10.dll") {
					LOG_INFO << "DIRECTX10" << LOG_FLUSH;
				}
				else if (moduleName == "d3d11.dll") {
					LOG_INFO << "DIRECTX11" << LOG_FLUSH;
				}
				else if (moduleName == "d3d12.dll") {
					LOG_INFO << "DIRECTX12" << LOG_FLUSH;
				}
				else if (moduleName == "vulkan-1.dll") {
					LOG_INFO << "VULKAN" << LOG_FLUSH;
				}
			} while (Module32Next(snapshot, &me));


		}

		CloseHandle(snapshot);
	}
}