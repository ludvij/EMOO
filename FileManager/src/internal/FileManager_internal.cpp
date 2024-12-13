#include "internal/FileManager_internal.hpp"


FILEMANAGER_NAMESPACE::detail::Context::~Context()
{
	for (const auto& alloc : allocations)
	{
		delete[] alloc;
	}
}


#ifdef FILE_MANAGER_PLATFORM_WINDOWS
#include <ShlObj.h>

FILEMANAGER_NAMESPACE::detail::Context::Context()
{
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

	root = current_folder = known_paths.at("APPDATA");
}

#else
#error Platform not supported
#endif // FILEMANAGER_WINDOWS
