#include "opengl.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <logger.h>
#include <detours.h>
#include <imgui.h>
#include <backend/imgui_impl_opengl3.h>
#include <backend/imgui_impl_win32.h>

using swapBuffers_t = bool(WINAPI*)(HDC);
static swapBuffers_t oSwapBuffers;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static WNDPROC oWndproc;
static LRESULT WINAPI HookedWndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	return CallWindowProc(oWndproc, hWnd, msg, wParam, lParam);
}

bool WINAPI HookedSwapBuffers(HDC hdc) {
	static bool g_inited = false;
	if (!g_inited) {
		HWND hwnd = WindowFromDC(hdc);
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplOpenGL3_Init();
		oWndproc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)HookedWndproc);
		g_inited = true;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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