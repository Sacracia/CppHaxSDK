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

    //HeroController* hero = (HeroController*)0x246c8809000;
    //LOG_DEBUG << hero->playerData() << LOG_FLUSH;

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