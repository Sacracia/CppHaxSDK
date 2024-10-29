#pragma once
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "D3d9.lib")
#ifdef _WIN64
#pragma comment(lib, "third_party/detours/x64/detours.lib")
#else
#pragma comment(lib, "third_party/detours/x86/detours.lib")
#endif

namespace haxsdk {
	void ImplementImGui();
}