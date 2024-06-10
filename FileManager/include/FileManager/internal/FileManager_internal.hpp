#ifndef FILE_MANAGER_INTERNAL_HEADER
#define FILE_MANAGER_INTERNAL_HEADER

#include "Config.hpp"

#include <deque>
#include <filesystem>
#include <fstream>

namespace FILEMANAGER_NAMESPACE::detail
{
class Context
{
public:
	Context();
	~Context();

public:
	std::deque<const char*> folders;
	std::filesystem::path root;
	std::filesystem::path current_folder;
	std::fstream current_file;

	std::vector<char*> allocations;
};
}

#endif