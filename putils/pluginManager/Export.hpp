#pragma once

//
// EXPORT define to indicate a function in a DLL
// can be called from the outside
//

#if defined(__unix__) || defined(__APPLE__)
# define EXPORT extern "C"
#endif
#ifdef _WIN32
# define EXPORT extern "C" __declspec(dllexport)
#endif
