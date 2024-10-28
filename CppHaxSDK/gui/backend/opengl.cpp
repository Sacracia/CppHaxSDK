#include "opengl.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <detours.h>
#include <imgui.h>
#include <backend/imgui_impl_opengl3.h>
#include <backend/imgui_impl_win32.h>

#include "user.h"
#include <logger.h>

// global variables
static bool g_visible = true;
static unsigned int g_key = 0;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static WNDPROC oWndproc;
static LRESULT WINAPI HookedWndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDrawCursor = g_visible && io.WantCaptureMouse;
	if (g_key > 0 && uMsg == WM_KEYUP && wParam == g_key) {
		g_visible = !g_visible;
	}
	io.MouseDrawCursor = g_visible && io.WantCaptureMouse;

	if (g_visible && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return true;
	}

	return CallWindowProc(oWndproc, hWnd, uMsg, wParam, lParam);
}

using swapBuffers_t = bool(WINAPI*)(HDC);
static swapBuffers_t oSwapBuffers;

bool WINAPI HookedSwapBuffers(HDC hdc) {
	static bool inited = false;
	if (!inited) {
		LOG_INFO << "Hello from hooked wglSwapBuffers!" << LOG_FLUSH;
		inited = true;
		HWND hwnd = WindowFromDC(hdc);
		ImGui::CreateContext();
		ApplyStyle();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplOpenGL3_Init();

		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = true;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
		oWndproc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)HookedWndproc);
	}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		if (g_visible)
			ImGui::ShowDemoWindow();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return oSwapBuffers(hdc);
}

namespace haxsdk::opengl {
	void HookOpenGL(unsigned int key) {
		g_key = key;

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
		LOG_INFO << "wglSwapBuffers address " << oSwapBuffers << LOG_FLUSH;
		DetourTransactionBegin();
		DetourAttach(&(PVOID&)oSwapBuffers, HookedSwapBuffers);
		DetourTransactionCommit();
	}
}