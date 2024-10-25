#include "opengl.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <logger.h>
#include <detours.h>
#include <imgui.h>
#include <backend/imgui_impl_opengl3.h>
#include <backend/imgui_impl_win32.h>

#ifdef __WIN64__
#define __decl __stdcall
#else
#define __decl __cdecl
#endif

using swapBuffers_t = bool(__decl*)(HDC);
swapBuffers_t oSwapBuffers;

bool g_inited = false;

WNDPROC WndProc;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall Wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	return CallWindowProc(WndProc, hWnd, msg, wParam, lParam);
}

bool __decl HookedSwapBuffers(HDC hdc) {
	//InitOpenGL(hdc);
	if (ImGui::GetCurrentContext()) {
		if (!ImGui::GetIO().BackendRendererUserData) {
			ImGui_ImplOpenGL3_Init();
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	return oSwapBuffers(hdc);
}

static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam) {
	const auto isMainWindow = [handle]() {
		return GetWindow(handle, GW_OWNER) == nullptr && IsWindowVisible(handle);
		};

	DWORD pID = 0;
	GetWindowThreadProcessId(handle, &pID);

	if (GetCurrentProcessId() != pID || !isMainWindow() || handle == GetConsoleWindow())
		return TRUE;

	*reinterpret_cast<HWND*>(lParam) = handle;

	return FALSE;
}

HWND GetProcessWindow() {
	HWND hwnd = nullptr;
	EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&hwnd));

	while (!hwnd) {
		EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&hwnd));
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	char name[128];
	GetWindowTextA(hwnd, name, RTL_NUMBER_OF(name));

	return hwnd;
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

		HWND hwnd = GetProcessWindow();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hwnd);

		DetourTransactionBegin();
		DetourAttach(&(PVOID&)oSwapBuffers, HookedSwapBuffers);
		DetourTransactionCommit();

		WndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)Wndproc);
	}
}