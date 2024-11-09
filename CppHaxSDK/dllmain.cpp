#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "logger/logger.h"
#include "gui/gui.h"
#include "user.h"
#include "mono/mono.h"

#define MONO_GAME_FUNC(r, n, p, s) extern r(*n)p
#include "mono/mono_game_functions.h"
#undef MONO_GAME_FUNC

void Start() {
	LOG_INIT(DEBUG, true);
	mono::Initialize();

    uintptr_t __this = 0x000001BF25433B40;
    LOG_DEBUG << "PREPARE1" << LOG_FLUSH;
    Vector3 vec3 = *(Vector3*)Transform_getPosition((MonoObject*)__this);
    LOG_DEBUG << "PREPARE2" << LOG_FLUSH;
    LOG_DEBUG << "VEC3: " << vec3.x << ' ' << vec3.y << ' ' << vec3.z << LOG_FLUSH;
    // 14.63 6.51 0.00

    /*ImplementationDetails details;
    details.ApplyStyleProc = ApplyStyle;
	haxsdk::ImplementImGui(details);*/
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}