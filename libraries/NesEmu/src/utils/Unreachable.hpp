#pragma once

namespace Lud
{
[[noreturn]] inline void Unreachable()
{
#if defined(__GNUC__) // GCC, Clang, ICC
	__builtin_unreachable();
#elif defined(_MSC_VER) // MSVC
	__assume( false );
#elif _HAS_CXX23
	std::unreachable();
#endif
}
}