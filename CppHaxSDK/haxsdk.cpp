#include "haxsdk.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <iomanip>
#include <iostream>
#include <filesystem>
#include <string_view>
#include <fstream>

#include "haxsdk_backend.h"

struct HaxLogger
{
    void Initialize(bool useConsole);
    void Log(std::string_view message, std::string_view tag);
    //void LogWarning(std::string_view message);
    void LogError(std::string_view message);

private:
    bool m_Initialized;
    bool m_UseConsole;
    std::filesystem::path m_LogPath;
};

static HaxGlobals g_Globals;
static HaxLogger g_Logger;

static bool GetBackend(HaxBackend& backend, void*& backendBaseAddr);

HaxGlobals& HaxSdk::GetGlobals()
{
	return g_Globals;
}

void HaxSdk::Initialize(bool useConsole)
{
    g_Logger.Initialize(useConsole);

	bool status = GetBackend(g_Globals.Backend, g_Globals.BackendHandle);
	HAX_ASSERT(status, "Unable to determine backend. Game is not Unity");

    HaxSdk::InitializeBackend();
    HaxSdk::AttachThread();
}

void HaxSdk::Log(std::string_view message)
{
    g_Logger.Log(message, "INFO");
}

void HaxSdk::LogError(std::string_view message)
{
    g_Logger.LogError(message);
}

void HaxLogger::Initialize(bool useConsole)
{
    if (m_Initialized)
        return;

    m_UseConsole = useConsole;
    if (m_UseConsole)
    {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    }

    char buff[MAX_PATH];
    GetModuleFileName(NULL, buff, MAX_PATH);
    const auto path = std::filesystem::path(buff);

    const auto logPath = path.parent_path() / "haxsdk-logs.txt";
    const auto prevLogPath = path.parent_path() / "haxsdk-prev-logs.txt";

    std::error_code errCode;
    std::filesystem::remove(prevLogPath, errCode);
    std::filesystem::rename(logPath, prevLogPath, errCode);
    std::filesystem::remove(logPath, errCode);

    m_LogPath = logPath;
    m_Initialized = true;
}

void HaxLogger::Log(std::string_view message, std::string_view tag)
{
    if (!m_Initialized)
        return;

    std::stringstream ss{};
    ss << '[' << std::left << std::setw(7) << tag << ":   HAXSDK] " << message << '\n';

    std::ofstream file(m_LogPath, std::ios_base::app);
    file << ss.str();
    file.close();

    if (m_UseConsole)
        std::cout << ss.str();
}

void HaxLogger::LogError(std::string_view message)
{
    if (!m_Initialized)
        return;

    if (m_UseConsole)
    {
        static HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        {
            WORD savedColor = info.wAttributes;
            SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
            Log(message, "ERROR");
            SetConsoleTextAttribute(console, savedColor);
            return;
        }
    }

    Log(message, "ERROR");
}

static bool GetBackend(HaxBackend& backend, void*& backendHandle)
{
	MODULEENTRY32W me = {0};
	me.dwSize = sizeof(MODULEENTRY32W);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
	if (Module32FirstW(snapshot, &me))
	{
		do
		{
			if (wcsstr(me.szModule, L"mono") != nullptr)
			{
				backend = HaxBackend_Mono;
				backendHandle = me.hModule;
				CloseHandle(snapshot);
				return true;
			}
			if (wcscmp(me.szModule, L"GameAssembly.dll") == 0)
			{
				backend = HaxBackend_Il2cpp;
				backendHandle = me.hModule;
				CloseHandle(snapshot);
				return true;
			}
		} while (Module32NextW(snapshot, &me));
	}

	CloseHandle(snapshot);
	return false;
}