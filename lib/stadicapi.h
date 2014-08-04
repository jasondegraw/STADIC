#ifndef STADICAPI_H
#define STADICAPI_H

#if _WIN32 || _MSC_VER

#ifdef stadic_core_EXPORTS
#define STADIC_API __declspec(dllexport)
#else
#define STADIC_API __declspec(dllimport)
#endif
#else
#define STADIC_API
#endif

#endif // STADICAPI_H
