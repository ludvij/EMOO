#include "FileManager.hpp"
#include "internal/Core.hpp"
#include "internal/FileManager_internal.hpp"
#include "Serializable.hpp"

#include <lud_assert.hpp>

#include <string>


FILEMANAGER_NAMESPACE::detail::Context context;

std::filesystem::path FILEMANAGER_NAMESPACE::GetCurrent()
{
	return context.current_folder;
}

std::filesystem::path FILEMANAGER_NAMESPACE::GetRoot()
{
	return context.root;
}

bool FILEMANAGER_NAMESPACE::SetRoot(const std::filesystem::path& name)
{
	if (name.empty())
	{
		context.root = context.current_folder;
		context.folders.clear();
		return false;
	}
	else
	{
		context.root = context.current_folder = name;
		context.folders.clear();

		return std::filesystem::create_directories(context.root);
	}
}

bool FILEMANAGER_NAMESPACE::SetRootToKnownPath(const std::string& name)
{
	if (auto path = context.known_paths.find(name); path != context.known_paths.end())
	{
		SetRoot(path->second);
	}
	return false;
}


bool FILEMANAGER_NAMESPACE::PushFolder(const std::filesystem::path& name)
{
	Lud::assert::that(!context.current_file.is_open(), "Can't push folder before popping file");
	context.folders.emplace_back(name);
	context.current_folder /= name;

	return std::filesystem::create_directories(context.current_folder);
}

bool FILEMANAGER_NAMESPACE::PushFolder(std::initializer_list<std::filesystem::path> name)
{
	uint32_t res = false;
	for (const auto& f : name)
	{
		res += PushFolder(f);
	}
	return res == name.size();
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
	Lud::assert::leq(context.folders.size(), amount, "Can't pop more folders than pushed amount");
	Lud::assert::that(!context.current_file.is_open(), "Can't pop folder before popping file");

	for (size_t i = 0; i < amount; i++)
	{
		context.folders.pop_back();
	}
	context.current_folder = context.root;
	for (size_t i = 0; i < context.folders.size(); i++)
	{
		context.current_folder /= context.folders[i];
	}
}

bool FILEMANAGER_NAMESPACE::PushFile(std::string_view name, OpenMode mode)
{
	Lud::assert::that(!context.current_file.is_open(), "You need to call PopFile before calling PushFile again");

	std::filesystem::path path = context.current_folder;
	path.append(name);
	if (!std::filesystem::exists(path) && ( mode & mode::READ ))
	{
		return false;
	}
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

void FILEMANAGER_NAMESPACE::Serialize(ISerializable* serial)
{
	if (PushFile(context.serialize_filename, mode::BINARY | mode::WRITE))
	{
		serial->Serialize(context.current_file);

		PopFile();
	}
}

void FILEMANAGER_NAMESPACE::Deserialize(ISerializable* serial)
{
	if (PushFile(context.serialize_filename, mode::BINARY | mode::READ))
	{
		serial->Deserialize(context.current_file);

		PopFile();
	}
}

void FILEMANAGER_NAMESPACE::SetSerializeFilename(std::string_view name)
{
	if (!name.empty())
	{
		context.serialize_filename = name;
	}
	else
	{
		context.serialize_filename = "srl.dat";
	}

}

void FILEMANAGER_NAMESPACE::SerializeData(const char* data, const size_t sz)
{
	auto& fs = context.current_file;
	Lud::assert::that(fs.is_open(), "Must be called inside Fman::Serialize");

	fs.write(data, sz);
}

void FILEMANAGER_NAMESPACE::DeserializeData(char* data, const size_t sz)
{
	auto& fs = context.current_file;
	Lud::assert::that(fs.is_open(), "Must be called inside Fman::Deserialize");

	fs.read(data, sz);
}
