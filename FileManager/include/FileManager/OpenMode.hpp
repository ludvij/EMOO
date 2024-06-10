#ifndef FILEMANAGER_OPENMODE_HEADER
#define FILEMANAGER_OPENMODE_HEADER

#include "internal/Config.hpp"
#include <fstream>

namespace FILEMANAGER_NAMESPACE
{
namespace mode
{

static constexpr int READ   = std::ios::in;
static constexpr int WRITE  = std::ios::out;
static constexpr int END    = std::ios::ate;
static constexpr int APPEND = std::ios::app;
static constexpr int BINARY = std::ios::binary;

}

using OpenMode = int;


}
#endif