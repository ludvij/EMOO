#include "FileManager.hpp"
#include "OpenMode.hpp"

#include "internal/Config.hpp"
#include "internal/FileManager_internal.hpp"

#include <lud_assert.hpp>

#include <string>

static FILEMANAGER_NAMESPACE::detail::Context context;


std::filesystem::path FILEMANAGER_NAMESPACE::GetCurrent()
{
	return context.current_folder;
}

std::filesystem::path FILEMANAGER_NAMESPACE::GetRoot()
{
	return context.root;
}

bool FILEMANAGER_NAMESPACE::SetRoot(const char* name)
{
	if (!name)
	{
		context.root = context.current_folder;
		context.folders.clear();
		return false;
	}
	else
	{
		context.root = context.current_folder = std::filesystem::path(name);
		context.folders.clear();

		return std::filesystem::create_directories(context.root);
	}
}


bool FILEMANAGER_NAMESPACE::PushFolder(const char* name)
{
	Lud::assert::that(!context.current_file.is_open(), "Can't push folder before popping file");
	context.folders.push_back(name);
	context.current_folder.append(name);

	return std::filesystem::create_directories(context.current_folder);
}

char* FILEMANAGER_NAMESPACE::AllocateFileName(const char* name)
{
	auto current = GetCurrent();
	current.append(name);
	auto repr = current.string();
	size_t sz = repr.size() + 1;

	char* s = new char[sz]();
	if (s)
	{
		strncpy_s(s, sz, repr.c_str(), sz);
	}
	context.allocations.push_back(s);

	return s;
}

void FILEMANAGER_NAMESPACE::PopFolder(int amount)
{
	if (amount < 0)
	{
		context.folders.clear();
		context.current_folder = context.root;
		return;
	}
	Lud::assert::lower(context.folders.size(), amount, "Can't pop more folders than pushed amount");
	Lud::assert::that(!context.current_file.is_open(), "Can't pop folder before popping file");

	for (size_t i = 0; i < amount; i++)
	{
		context.folders.pop_back();
	}
	context.current_folder = context.root;
	for (size_t i = 0; i < context.folders.size(); i++)
	{
		context.current_folder.append(context.folders[i]);
	}
}

bool FILEMANAGER_NAMESPACE::PushFile(const char* name, OpenMode mode)
{
	Lud::assert::that(!context.current_file.is_open(), "You need to call PopFile before calling PushFile again");

	std::filesystem::path path = context.current_folder;
	path.append(name);
	context.current_file.open(path, mode);

	return context.current_file.is_open();
}

void FILEMANAGER_NAMESPACE::PopFile()
{
	Lud::assert::that(context.current_file.is_open(), "You need to call PushFile before calling PopFile");

	context.current_file.close();
}

void FILEMANAGER_NAMESPACE::Write(const std::string_view text)
{
	Lud::assert::that(context.current_file.is_open(), "You need to call PushFile before writing to a file");

	context.current_file << text;
}
