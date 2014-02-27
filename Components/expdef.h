#ifndef BEATS_COMPONENTS_EXPDEF_H__INCLUDE
#define BEATS_COMPONENTS_EXPDEF_H__INCLUDE

#ifdef COMPONENTS_EXPORTS
#define COMPONENTS_DLL_DECL __declspec(dllexport)
#else
#define COMPONENTS_DLL_DECL __declspec(dllimport)
#endif

#endif
