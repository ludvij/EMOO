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
	PWSTR path;

	auto err = SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &path);
	if (err != S_OK)
	{
		CoTaskMemFree(path);
		throw std::runtime_error("Could not find appdata");
	}
	root = current_folder = path;
	CoTaskMemFree(path);
}

#else
#error Platform not supported
#endif // FILEMANAGER_WINDOWS
