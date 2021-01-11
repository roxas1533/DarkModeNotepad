#pragma once

#ifdef EXPORT
__declspec(dllexport)
#else
__declspec(dllimport)
#endif // EXPORT
void Inject();
#ifdef EXPORT
__declspec(dllexport)
#else
__declspec(dllimport)
#endif // EXPORT
void UnInject();
