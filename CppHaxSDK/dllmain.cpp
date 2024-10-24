#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "logger/logger.h"

void Start() {
	LOG_INIT();
	LOG_INFO << "INFO" << LOG_FLUSH;
	LOG_WARNING << "WARNING" << LOG_FLUSH;
	LOG_ERROR << "ERROR" << LOG_FLUSH;
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}

