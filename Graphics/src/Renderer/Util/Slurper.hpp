#ifndef LUD_SLURPER_HEADER
#define LUD_SLURPER_HEADER

#include <ostream>
#include <string_view>
#include <string>
#include <optional>

#include <fstream>

#include <vector>
#include <bit>



namespace Lud {

class Slurper 
{
public:
	Slurper();
	Slurper(const std::string_view& filename, std::ios_base::openmode mode = std::ios_base::in);
	Slurper(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in);
	Slurper(const char* filename, std::ios_base::openmode mode = std::ios_base::in);

	Slurper(const Slurper& slurper) = delete;
	Slurper(Slurper&& slurper) noexcept = default;


	~Slurper();

	// return remaining file as vector
	std::vector<std::string> ReadLines();

	// reads a single line
	std::string ReadLine();
	// reads n chars, returns nullopt if not enough space
	std::optional<std::string> Read(size_t chars);

	// reads size of structure from file
	template<class T> std::optional<T> ReadStructure();
	// reads size of structure from fiel to provided structure
	template<class T> void ReadToStructure(T& t);

	// returns current pos in file
	size_t Where();
	// moves current file pos to provided pos
	Slurper& Move(size_t pos);
	// moves current file pos by an offset in a direction
	Slurper& Move(size_t offset, std::ios_base::seekdir dir);

	// returns file pointer to pos 0
	Slurper& Reset();

	// returns true if file was opened
	bool IsOpen() const;

	// opens file 
	Slurper& Open(const std::string_view& filename, std::ios_base::openmode mode = std::ios_base::in);
	Slurper& Open(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in);
	Slurper& Open(const char* filename, std::ios_base::openmode mode = std::ios_base::in);

	// returns file as vector of lines without you needing to creating a slurper
	static std::vector<std::string> Slurp(const std::string_view& filename, std::ios_base::openmode mode = std::ios_base::in);
	static std::vector<std::string> Slurp(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in);
	static std::vector<std::string> Slurp(const char* filename, std::ios_base::openmode mode = std::ios_base::in);


	// reads whole file to char vector
	std::vector<char> ReadChars();

	static std::vector<char> SlurpChars(const std::string_view& filename, std::ios_base::openmode mode = std::ios_base::in);
	static std::vector<char> SlurpChars(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in);
	static std::vector<char> SlurpChars(const char* filename, std::ios_base::openmode mode = std::ios_base::in);

	// closes file
	void Close();

	bool HasSpace(size_t size);

private:
	std::ifstream file;
};
}

#ifdef LUD_SLURPER_IMPLEMENTATION
namespace Lud {


inline Slurper::Slurper()
{

}

inline Slurper::Slurper(const std::string_view& filename, const std::ios_base::openmode mode) 
	: Slurper()
{ 
	Open(filename, mode); 
}
inline Slurper::Slurper(const std::string& filename, const std::ios_base::openmode mode) 
	: Slurper()
{ 
	Open(filename, mode); 
}
inline Slurper::Slurper(const char* filename, const std::ios_base::openmode mode ) 
	: Slurper()
{ 
	Open(filename, mode); 
}

inline Slurper::~Slurper()
{
	Close();
}

inline std::vector<std::string> Slurper::Slurp(const std::string& filename, const std::ios_base::openmode mode)
{
	Slurper file(filename, mode);
	return file.ReadLines();
}

inline std::vector<std::string> Slurper::Slurp(const char* filename, const std::ios_base::openmode mode)
{
	Slurper file(filename, mode);
	return file.ReadLines();
}

inline std::vector<std::string> Slurper::Slurp(const std::string_view& filename, const std::ios_base::openmode mode)
{
	Slurper file(filename, mode);
	return file.ReadLines();
}

inline Slurper& Lud::Slurper::Open(const std::string_view& filename, const std::ios_base::openmode mode) 
{
	if (IsOpen()) {
		Close();
	}
	file.open(std::string(filename), mode);
	return *this;
}
inline Slurper& Lud::Slurper::Open(const std::string& filename, const std::ios_base::openmode mode) 
{ 
	if (IsOpen()) {
		Close();
	}
	file.open(filename, mode);
	return *this;
} 
inline Slurper& Lud::Slurper::Open(const char* filename, const std::ios_base::openmode mode) 
{ 
	if (IsOpen()) {
		Close();
	}
	file.open(filename, mode);
	return *this;
}

inline bool Slurper::IsOpen() const
{
	return file.is_open();
}

inline Slurper& Slurper::Reset() 
{
	file.clear();
	file.seekg(0, std::ios::beg);

	return *this;
}


inline void Slurper::Close()
{
	file.close();
}

inline std::optional<std::string> Slurper::Read(const size_t chars)
{
	if (!HasSpace(chars)) {
		return std::nullopt;
	}
	char* buffer = new char[chars];

	file.read(buffer, chars);
	std::string text(buffer);

	delete[] buffer;

	return text;
}

template<class T> std::optional<T> Slurper::ReadStructure()
{
	T t;
	if (!HasSpace(sizeof(t))) {
		return std::nullopt;
	}
	ReadToStructure<T>(t);
	return t;

}

template<class T> void Slurper::ReadToStructure(T& t)
{
	file.read(std::bit_cast<char*>(&t), sizeof(t));
}

inline std::string Slurper::ReadLine()
{
	std::string line;
	std::getline(file, line);

	return line;
}

inline std::vector<std::string> Slurper::ReadLines()
{
	std::vector<std::string> lines;

	for(std::string line; std::getline(file, line);) {
		lines.emplace_back(line);
	}
	return lines;
}

inline size_t Slurper::Where()  
{
	return file.tellg();
}

inline Slurper& Slurper::Move(const size_t pos)
{
	file.seekg(pos);
	return *this;
}

inline Slurper& Slurper::Move(const size_t offset, const std::ios_base::seekdir dir)
{
	file.seekg(offset, dir);
	return *this;
}

inline bool Slurper::HasSpace(const size_t size)
{
	const size_t current = Where();
	Move(0, std::ios::end);
	const size_t length = Where();

	return size < length - current;
}

inline std::vector<char> Slurper::ReadChars()
{
	size_t file_size = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(file_size);
	Move(0);
	file.read(buffer.data(), file_size);
	
	return buffer;
}

inline std::vector<char> Slurper::SlurpChars(const std::string_view& filename, std::ios_base::openmode mode)
{
	Slurper file(filename, mode | std::ios::ate);
	return file.ReadChars();
}

inline std::vector<char> Slurper::SlurpChars(const std::string& filename, std::ios_base::openmode mode)
{
	Slurper file(filename, mode | std::ios::ate);
	return file.ReadChars();
}

inline std::vector<char> Slurper::SlurpChars(const char* filename, std::ios_base::openmode mode)
{
	Slurper file(filename, mode | std::ios::ate);
	return file.ReadChars();
}

}

#endif
#endif