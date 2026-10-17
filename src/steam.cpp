#include "steam.h"
#include "utils.h"
#include "log.h"

bool FFNx_SteamAPI_ValidateLibrary()
{
	static const bool is_valid = [] {
		const char *lib_path = "FFNx_steam_api.dll";
		bool is_genuine_steam_api = isFileSigned(lib_path);
		if (!is_genuine_steam_api)
		{
			is_genuine_steam_api = sha1_file(lib_path) == "772afa31bd0d16a79dd1d0036bb9a9cb8f2eaf30";
		}
		if (is_genuine_steam_api)
		{
			return true;
		}

		char message[256] = {};
		snprintf(message, sizeof(message), "Invalid %s detected. Please ensure your FFNx installation is not corrupted or tampered by unauthorized software.\n", lib_path);
		ffnx_unexpected(message);
		MessageBoxA(nullptr, message, "Error", MB_ICONERROR | MB_OK);
		return false;
	}();

	return is_valid;
}
