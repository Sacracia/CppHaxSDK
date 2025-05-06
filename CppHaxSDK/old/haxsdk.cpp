#include "haxsdk.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <iomanip>
#include <iostream>
#include <filesystem>
#include <string_view>
#include <fstream>
#include <vector>

#ifdef _WIN64
    #if __has_include("third_party/detours/x64/detours.h")
        #include "third_party/detours/x64/detours.h"
    #else
        #include "../third_party/detours/x64/detours.h"
    #endif
#else
    #if __has_include("third_party/detours/x86/detours.h")
        #include "third_party/detours/x86/detours.h"
    #else
        #include "../third_party/detours/x86/detours.h"
    #endif
#endif

#include "haxsdk_unity.h"

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
static char g_Buffer[1024];

static bool GetBackend(HaxBackend& backend, void*& backendBaseAddr);

HaxGlobals& HaxSdk::GetGlobals()
{
	return g_Globals;
}

bool HaxSdk::IsMono()
{
    return g_Globals.m_Backend == HaxBackend_Mono;
}

bool HaxSdk::IsIl2Cpp()
{
    return g_Globals.m_Backend == HaxBackend_Il2cpp;
}

void HaxSdk::AttachThread()
{
    HaxSdk::IsMono() ? Mono::AttachThread() : Il2Cpp::AttachThread();
}

void HaxSdk::Hook(void** orig, void* hook)
{
    DetourAttach(orig, hook);
}

void HaxSdk::Assert(bool expr, std::string_view message)
{
    do {
        if (!expr) 
        {
            void* hwnd = HaxSdk::GetGlobals().m_GameHWND;
            HaxSdk::LogError(message);
            MessageBoxA(NULL, message.data(), "Hax assertion failed", MB_ICONERROR);
            TerminateProcess(GetCurrentProcess(), 0xDEAD);
        }
    } while (0);
}

void HaxSdk::Initialize(bool useConsole)
{
    g_Logger.Initialize(useConsole);

	bool status = GetBackend(g_Globals.m_Backend, g_Globals.m_BackendHandle);
	HAX_ASSERT(status, "Unable to determine backend. Game is not Unity");

    if (IsMono())
    {
        Mono::Initialize();
        Mono::AttachThread();
    }
    else
    {
        Il2Cpp::Initialize();
        Il2Cpp::AttachThread();
    }

    for (const HaxMethodInfo& info : g_Globals.m_PreCachedMethods)
    {
        System::Assembly assembly = System::AppDomain::GetDefaultDomain().Load(info.m_Assembly);
        HAX_ASSERT(assembly.m_NativePtr, std::format("Assembly {} not found", info.m_Assembly));
        System::Type type = assembly.GetType(info.m_Namespace, info.m_Class);
        HAX_ASSERT(type.m_ManagedPtr, std::format("Type {}.{} not found in {}", info.m_Namespace, info.m_Class, info.m_Assembly));
        System::MethodInfo method = type.GetMethod(info.m_Name, info.m_Signature);
        HAX_ASSERT(method.m_NativePtr, std::format("Method {} not found", info.m_Name));

        HAX_LOG("{}_{} = {}", info.m_Class, info.m_Name, method.GetFunctionPointer());
        info.m_Method = method;
    }

    for (const HaxFieldInfo& info : g_Globals.m_PreCachedFields)
    {
        System::Assembly assembly = System::AppDomain::GetDefaultDomain().Load(info.m_Assembly);
        HAX_ASSERT(assembly.m_NativePtr, std::format("Assembly {} not found", info.m_Assembly));
        System::Type type = assembly.GetType(info.m_Namespace, info.m_Class);
        HAX_ASSERT(type.m_ManagedPtr, std::format("Type {}.{} not found in {}", info.m_Namespace, info.m_Class, info.m_Assembly));
        info.m_Field = type.GetField(info.m_Name);
        HAX_ASSERT(info.m_Field.m_NativePtr, std::format("Field {} not found", info.m_Name));

        HAX_LOG("{}_{} = {}", info.m_Class, info.m_Name, info.m_Field.m_NativePtr);
    }

    g_Globals.m_Initialized = true;
}

void HaxSdk::Log(std::string_view message)
{
    g_Logger.Log(message, "INFO");
}

void HaxSdk::LogError(std::string_view message)
{
    g_Logger.LogError(message);
}

char* HaxSdk::ToUTF8(wchar_t* wstr, int length)
{
    memset(g_Buffer, 0, sizeof(g_Buffer));
    int nBytesNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr, length, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_UTF8, 0, wstr, length, g_Buffer, nBytesNeeded, NULL, NULL);
    return g_Buffer;
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