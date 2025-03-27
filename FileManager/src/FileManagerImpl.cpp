#include "FileManager.hpp"
#include "internal/Core.hpp"
#include "internal/FileManager_internal.hpp"
#include "Serializable.hpp"

#include <ludutils/lud_assert.hpp>

#include <algorithm>
#include <deque>
#include <string>
#include <string_view>

namespace fs = std::filesystem;
namespace ranges = std::ranges;

FILEMANAGER_NAMESPACE::detail::Context context;

fs::path FILEMANAGER_NAMESPACE::GetCurrent()
{
	return context.current_folder;
}

fs::path FILEMANAGER_NAMESPACE::GetRoot()
{
	return context.root;
}

bool FILEMANAGER_NAMESPACE::SetRoot(const fs::path& name)
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

		return fs::create_directories(context.root);
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


bool FILEMANAGER_NAMESPACE::PushFolder(const fs::path& name, bool create/*= true*/)
{
	Lud::assert::that(!context.current_file.is_open(), "Can't push folder before popping file");
	Lud::assert::that(!name.has_extension(), "Can't push file while pushing folder");
	const auto absolute = fs::absolute(name);

	if (create)
	{
		context.folders.emplace_back(absolute);
		context.current_folder = absolute;

		fs::create_directories(context.current_folder);
		return true;
	}
	else
	{
		const bool exists = fs::exists(absolute);
		if (!exists)
		{
			return false;
		}
		context.folders.emplace_back(absolute);
		context.current_folder = absolute;

		return true;
	}

}

bool FILEMANAGER_NAMESPACE::PushFolder(std::initializer_list<fs::path> name, bool create/*= true*/)
{
	uint32_t res = false;
	for (const auto& f : name)
	{
		res += PushFolder(f, create);
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
	Lud::assert::leq(amount, context.folders.size(), "Can't pop more folders than pushed amount");
	Lud::assert::that(!context.current_file.is_open(), "Can't pop folder before popping file");

	if (amount < 0 || amount == context.folders.size())
	{
		context.folders.clear();
		context.folders.push_back(context.root);
	}
	else
	{
		context.folders.resize(context.folders.size() - amount);
	}
	context.current_folder = context.folders.back();
}

bool FILEMANAGER_NAMESPACE::PushFile(fs::path name, OpenMode mode)
{
	Lud::assert::that(!context.current_file.is_open(), "You need to call PopFile before calling PushFile again");

	fs::path path = context.current_folder;
	path.append(name.string());
	if (!fs::exists(path) && ( mode & mode::read ))
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

std::vector<std::filesystem::path> FILEMANAGER_NAMESPACE::Traverse(const int depth, const TraverseMode trav_mode, const std::initializer_list<std::string_view> filters)
{
	Lud::assert::ne(depth, 0, "depth must be different from 0");
	std::vector<fs::path> result;

	int curr_depth = 0;
	std::deque<fs::directory_entry> iters;

	iters.emplace_back(context.current_folder);

	size_t size = 1;
	while (iters.size() > 0)
	{
		fs::directory_iterator it{ iters.front() };
		iters.pop_front();
		for (const auto& dir_entry : it)
		{
			if (fs::is_directory(dir_entry))
			{
				if (trav_mode & traverse::folders)
					result.emplace_back(dir_entry);
				iters.push_back(dir_entry);
			}
			if (!fs::is_directory(dir_entry) && ( trav_mode & traverse::files ))
			{
				if (filters.size() > 0)
				{
					const auto ext = dir_entry.path().extension();
					if (!( ranges::find(filters, ext) != filters.end() )) continue;
				}
				result.emplace_back(dir_entry);
			}
		}
		if (--size == 0)
		{
			size = iters.size();
			if (depth > FULL && ++curr_depth >= depth)
				return result;
		}
	}
	return result;
}

void FILEMANAGER_NAMESPACE::Write(const std::string_view text)
{
	Lud::assert::that(context.current_file.is_open(), "You need to call PushFile before writing to a file");

	context.current_file << text;
}

std::string FILEMANAGER_NAMESPACE::Slurp(std::filesystem::path path)
{
	std::string res;
	if (PushFile(path, mode::read | mode::end | mode::binary))
	{
		const size_t size = context.current_file.tellg();
		context.current_file.seekg(0, std::ios::beg);

		char* buf = new char[size];
		context.current_file.read(buf, size);
		res = std::string(buf, size);

		delete[] buf;
		PopFile();
	}

	return res;
}

void FILEMANAGER_NAMESPACE::Serialize(ISerializable* serial)
{
	if (PushFile(context.serialize_filename, mode::binary | mode::write))
	{
		serial->Serialize(context.current_file);

		PopFile();
	}
}

void FILEMANAGER_NAMESPACE::Deserialize(ISerializable* serial)
{
	if (PushFile(context.serialize_filename, mode::binary | mode::read))
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
