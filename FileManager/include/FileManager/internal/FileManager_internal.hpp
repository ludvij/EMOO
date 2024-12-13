#ifndef FILE_MANAGER_INTERNAL_HEADER
#define FILE_MANAGER_INTERNAL_HEADER

#include "Core.hpp"

#include <deque>
#include <filesystem>
#include <fstream>
#include <unordered_map>


namespace FILEMANAGER_NAMESPACE::detail
{

class Context
{
public:
	Context();
	~Context();

private:
public:
	std::deque<std::filesystem::path> folders;
	std::filesystem::path root;
	std::filesystem::path current_folder;
	std::fstream current_file;

	std::vector<char*> allocations;

	std::string serialize_filename="srl.dat";

	std::unordered_map<std::string, std::filesystem::path> known_paths;
};


}

#endif