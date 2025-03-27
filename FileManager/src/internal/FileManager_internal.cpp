#include "internal/FileManager_internal.hpp"

#ifdef FILE_MANAGER_PLATFORM_WINDOWS
#include <ShlObj.h>
#endif//FILE_MANAGER_PLATFORM_WINDOWS

FILEMANAGER_NAMESPACE::detail::Context::~Context()
{
	for (const auto& alloc : allocations)
	{
		delete[] alloc;
	}
}

FILEMANAGER_NAMESPACE::detail::Context::Context()
{
	known_paths.emplace("PWD", std::filesystem::current_path());
#ifdef FILE_MANAGER_PLATFORM_WINDOWS
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

	add_knonw_path("APPDATA", FOLDERID_RoamingAppData);
	add_knonw_path("DOCUMENTS", FOLDERID_Documents);
#endif//FILE_MANAGER_PLATFORM_WINDOWS

	root = known_paths.at("PWD");
}



