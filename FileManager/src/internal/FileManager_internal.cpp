#include "internal/FileManager_internal.hpp"

#ifdef FILEMANAGER_PLATFORM_WINDOWS
#include <ShlObj.h>
#endif//FILE_MANAGER_PLATFORM_WINDOWS

Fman::detail::Context::~Context()
{
	for (const auto& alloc : allocations)
	{
		delete[] alloc;
	}
}

Fman::detail::Context::Context()
{
	// I need to clan this
#if defined(FILEMANAGER_PLATFORM_WINDOWS)
	auto add_knonw_path = [&](const std::string& name, GUID id)
		{
			PWSTR path;

			auto err = SHGetKnownFolderPath(id, KF_FLAG_CREATE, NULL, &path);
			if (err != S_OK)
			{
				CoTaskMemFree(path);
				throw std::runtime_error("path not found");
			}
			known_paths.emplace(name, path);

			CoTaskMemFree(path);
		};
	add_knonw_path("HOME", FOLDERID_Profile);
	add_knonw_path("APPDATA", FOLDERID_RoamingAppData);
	add_knonw_path("DOCUMENTS", FOLDERID_Documents);
#elif defined(FILEMANAGER_PLATFORM_LINUX)
	known_paths.emplace("HOME", "~");
	known_paths.emplace("APPDATA", "~");
	known_paths.emplace("DOCUMENTS", "~/Documents");
#else
#error Unsupported platform
#endif

	known_paths.emplace("PWD", std::filesystem::current_path());

	root = current_folder = known_paths.at("PWD");
}



