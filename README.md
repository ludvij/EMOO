# EMOO
![logo](Application/resources/icon.ico "icon")  
Another NES emulator.  
Originally made as my bachelor's thesis

## Features
 - Live disassembly
 - Memory inspection
 - Mapper 0 ROMs
 - Graphical data representation
 - per ROM save-states


## Usage

In order to run this program you need to have a GPU driver that supports Vulkan 3  
Just run the binary file, it should work

---

## Compilation

list of requirements
 - python 3
 - Vulkan 3
 - C++20 compiler

Don't forget to clone with recursive as this project uses submodules

### Windows
If you are using MSVC, just run the makesln.bat file, it will generate a sln file.

### Linux
Currently not supported.

### Apple
I rather not bother.

---

## Dependencies
 - [SDL2](https://www.libsdl.org/)
 - [ImGui](https://github.com/ocornut/imgui)
 - [Vulkan](https://www.vulkan.org/)
 - [Portable file dialogs](https://github.com/samhocevar/portable-file-dialogs)
 - [GoogleTest](https://github.com/google/googletest)
 - [compile time regex](https://github.com/hanickadot/compile-time-regular-expressions)
 - [vkbootstrap](https://github.com/charles-lunarg/vk-bootstrap)
 - [cppicons](https://github.com/juliettef/IconFontCppHeaders)

## TODO
- [ ] Add Audio
- [ ] Support more mappers
- [ ] Add save-state feedback
- [ ] Clean up embedding
- [ ] Swap premake with cmake??????
- [ ] Make renderer stink less

---

## Contributing
Right now I'd rather not accept external contributions, as I really want to iron out this project, any suggestions are appreciated tho,
specially in the renderer department since right now is BAD