#include "opengl.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <logger.h>
#include <detours.h>
#include <imgui.h>
#include <backend/imgui_impl_opengl2.h>
#include <backend/imgui_impl_win32.h>

#ifdef __WIN64__
#define __decl __stdcall
#else
#define __decl __cdecl
#endif

using swapBuffers_t = bool(__decl*)(HDC);
swapBuffers_t oSwapBuffers;

bool g_inited = false;

void InitOpenGL(HDC hdc) {
	static HWND prevHwnd = nullptr;
	HWND curHwnd = WindowFromDC(hdc);

	if (prevHwnd == curHwnd && g_inited) {
		return;
	}
	prevHwnd = curHwnd;

	if (g_inited) {
		ImGui_ImplWin32_Init(WindowFromDC(hdc));
		ImGui_ImplOpenGL2_Init();

		LOG_INFO << "Init again" << LOG_FLUSH;
		return;
	}

	ImGui::CreateContext();
	ImGui_ImplWin32_Init(WindowFromDC(hdc));
	ImGui_ImplOpenGL2_Init();
	LOG_INFO << "First init" << LOG_FLUSH;
	g_inited = true;
}

bool __decl HookedSwapBuffers(HDC hdc) {
	InitOpenGL(hdc);

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	return oSwapBuffers(hdc);
}

namespace haxsdk::opengl {
	void HookOpenGL() {
		HMODULE module = GetModuleHandleA("opengl32.dll");
		if (module == 0) {
			LOG_ERROR << "Unable to get opengl32.dll handle" << LOG_FLUSH;
			return;
		}

		oSwapBuffers = (swapBuffers_t)GetProcAddress(module, "wglSwapBuffers");
		if (oSwapBuffers == 0) {
			LOG_ERROR << "Unable to find wglSwapBuffers" << LOG_FLUSH;
			return;
		}
		else {
			LOG_INFO << "wglSwapBuffers address " << oSwapBuffers << LOG_FLUSH;
		}

		DetourTransactionBegin();
		DetourAttach(&(PVOID&)oSwapBuffers, HookedSwapBuffers);
		DetourTransactionCommit();
	}
}