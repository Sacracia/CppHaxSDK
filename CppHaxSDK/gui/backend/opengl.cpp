#include "opengl.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <logger.h>
#include <detours.h>

using swapBuffers_t = BOOL(__stdcall*)(HDC);
swapBuffers_t oSwapBuffers;

bool g_inited = false;

BOOL __stdcall HookedSwapBuffers(HDC hdc) {
	if (!g_inited) {
		LOG_INFO << "Hook is activated!" << LOG_FLUSH;
		g_inited = true;
	}
	return oSwapBuffers(hdc);
}

namespace haxsdk::opengl {
	void HookOpenGL() {
		HMODULE module = GetModuleHandleA("opengl32.dll");
		if (module == 0) {
			LOG_ERROR << "Unable to get opengl32.dll handle" << LOG_FLUSH;
			return;
		}

		swapBuffers_t oSwapBuffers = (swapBuffers_t)GetProcAddress(module, "wglSwapBuffers");
		if (oSwapBuffers == 0) {
			LOG_ERROR << "Unable to find wglSwapBuffers" << LOG_FLUSH;
			return;
		}

		DetourTransactionBegin();
		DetourAttach(&(PVOID&)oSwapBuffers, HookedSwapBuffers);
		DetourTransactionCommit();
	}
}