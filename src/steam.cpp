#include "steam.h"
#include "utils.h"
#include "log.h"

#include <libloaderapi.h>
#include <sysinfoapi.h>
#include <shlwapi.h>

HMODULE hDll = nullptr;
FARPROC procRunCallbacks = nullptr;
FARPROC procRegisterCallback = nullptr;
FARPROC procUnregisterCallback = nullptr;
FARPROC procSteamUser = nullptr;
FARPROC procSteamUtils = nullptr;
FARPROC procUserStats = nullptr;

typedef bool(*LPRESTARTAPPIFNECESSARY)(uint32);
typedef bool(*LPINIT)();
typedef void(*LPRUNCALLBACKS)();
typedef void(*LPSHUTDOWN)();
typedef void(*LPREGISTERCALLBACK)(class CCallbackBase *pCallback, int iCallback);
typedef void(*LPUNREGISTERCALLBACK)(class CCallbackBase *pCallback);
typedef ISteamUser*(*LPUSER)();
typedef ISteamUtils*(*LPUTILS)();
typedef ISteamUserStats*(*LPUSERSTATS)();

bool load_library()
{
	if (hDll != nullptr) {
		return true;
	}

	char lib_path[MAX_PATH] = {};

	strncpy(lib_path, "ffnx_steam_api.dll", sizeof(lib_path));

	if (!fileExists(lib_path)) {
		strncpy(lib_path, "steam_api.dll", sizeof(lib_path));
	}

	bool is_genuine_steam_api = isFileSigned(lib_path);
	if (!is_genuine_steam_api) is_genuine_steam_api = sha1_file(lib_path) == "03bd9f3e352553a0af41f5fe006f6249a168c243";
	if (!is_genuine_steam_api)
	{
		char message[256] = {};
		snprintf(message, sizeof(message), "Invalid %s detected. Please ensure your FFNx installation is not corrupted or tampered by unauthorized software.\n", lib_path);
		ffnx_unexpected(message);
		MessageBoxA(nullptr, message, "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	hDll = LoadLibraryA(lib_path);

	if (hDll != nullptr) {
		return true;
	}

	ffnx_error("%s: cannot load library %s\n", __func__, lib_path);

	return false;
}

bool steam_api_restart_app_if_necessary(uint32 unOwnAppID)
{
	if (!load_library()) {
		return false;
	}

	FARPROC proc = GetProcAddress(hDll, "SteamAPI_RestartAppIfNecessary");
	if (proc == nullptr) {
		ffnx_error("%s: Function not found in Steam lib\n", __func__);

		return false;
	}

	return LPRESTARTAPPIFNECESSARY(proc)(unOwnAppID);
}

bool steam_api_init()
{
	if (!load_library()) {
		return false;
	}

	FARPROC proc = GetProcAddress(hDll, "SteamAPI_Init");
	if (proc == nullptr) {
		ffnx_error("%s: Function not found in Steam lib\n", __func__);

		return false;
	}

	return LPINIT(proc)();
}

void steam_api_run_callbacks()
{
	if (!load_library()) {
		return;
	}

	if (procRunCallbacks == nullptr) {
		procRunCallbacks = GetProcAddress(hDll, "SteamAPI_RunCallbacks");
		if (procRunCallbacks == nullptr) {
			ffnx_error("%s: Function not found in Steam lib\n", __func__);

			return;
		}
	}

	return LPRUNCALLBACKS(procRunCallbacks)();
}

void steam_api_shutdown()
{
	if (!load_library()) {
		return;
	}

	FARPROC proc = GetProcAddress(hDll, "SteamAPI_Shutdown");
	if (proc == nullptr) {
		ffnx_error("%s: Function not found in Steam lib\n", __func__);

		return;
	}

	return LPSHUTDOWN(proc)();
}

void steam_api_register_callback(class CCallbackBase *pCallback, int iCallback)
{
	if (!load_library()) {
		return;
	}

	if (procRegisterCallback == nullptr) {
		procRegisterCallback = GetProcAddress(hDll, "SteamAPI_RegisterCallback");
		if (procRegisterCallback == nullptr) {
			ffnx_error("%s: Function not found in Steam lib\n", __func__);

			return;
		}
	}

	return LPREGISTERCALLBACK(procRegisterCallback)(pCallback, iCallback);
}

void steam_api_unregister_callback(class CCallbackBase *pCallback)
{
	if (!load_library()) {
		return;
	}

	if (procUnregisterCallback == nullptr) {
		procUnregisterCallback = GetProcAddress(hDll, "SteamAPI_UnregisterCallback");
		if (procUnregisterCallback == nullptr) {
			ffnx_error("%s: Function not found in Steam lib\n", __func__);

			return;
		}
	}

	return LPUNREGISTERCALLBACK(procUnregisterCallback)(pCallback);
}

ISteamUser *steam_user()
{
	if (!load_library()) {
		return nullptr;
	}

	if (procSteamUser == nullptr) {
		procSteamUser = GetProcAddress(hDll, "SteamUser");
		if (procSteamUser == nullptr) {
			ffnx_error("%s: Function not found in Steam lib\n", __func__);

			return nullptr;
		}
	}

	return LPUSER(procSteamUser)();
}

ISteamUtils *steam_utils()
{
	if (!load_library()) {
		return nullptr;
	}

	if (procSteamUtils == nullptr) {
		procSteamUtils = GetProcAddress(hDll, "SteamUtils");
		if (procSteamUtils == nullptr) {
			ffnx_error("%s: Function not found in Steam lib\n", __func__);

			return nullptr;
		}
	}

	return LPUTILS(procSteamUtils)();
}

ISteamUserStats *steam_user_stats()
{
	if (!load_library()) {
		return nullptr;
	}

	if (procUserStats == nullptr) {
		procUserStats = GetProcAddress(hDll, "SteamUserStats");
		if (procUserStats == nullptr) {
			ffnx_error("%s: Function not found in Steam lib\n", __func__);

			return nullptr;
		}
	}

	return LPUSERSTATS(procUserStats)();
}
