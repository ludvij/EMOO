#ifndef FILE_MANAGER_HEADER
#define FILE_MANAGER_HEADER
#include "internal/Config.hpp"

#include "OpenMode.hpp"

#include <filesystem>
#include <string>
#include <string_view>

namespace FILEMANAGER_NAMESPACE
{

/**
 * @brief Get the Current pushed path
 *
 * @return std::filesystem::path
 */
std::filesystem::path GetCurrent();
/**
 * @brief Get the Current root path
 *
 * @return std::filesystem::path
 */
std::filesystem::path GetRoot();

/**
 * @brief Set the Root object
 *
 * @param name (optional) name of the new root
 *             if not set will set root to current
 *
 * @return true if folder was created
 * @return false if folder exists
 */
bool SetRoot(const char* name=nullptr);

/**
 * @brief Pushes a nes folder on top of current
 *
 * @param name new folder name
 *
 * @return true if folder was created
 * @return false if folder exists
 */
bool PushFolder(const char* name);

/**
 * @brief Allocates and returns file name in current
 *        The user does not need to delete the allocated memory
 *
 * @param name name of file
 *
 * @return char* allocated 0 terminated string
 */
char* AllocateFileName(const char* name);

/**
 * @brief goes back folders
 *
 * @param amount the number of folders to go back
 *               if < 0 then all folders will be popped
 */
void PopFolder(int amount=1);


/**
* @brief Opens a file
*
* @param name, name of the file to be created
* @param mode, open mode of file
*/
bool PushFile(const char* name, OpenMode mode=mode::WRITE | mode::APPEND);

void PopFile();

void Write(const std::string_view text);


}
#endif