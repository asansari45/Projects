#pragma once
#include <new>

void* operator new(size_t size, const char* file, int line);
void* operator new[](size_t size, const char* file, int line);
void* operator new(size_t size);

#if defined(DEBUG_MEMORY)
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#if 0
#define DEBUG_DELETE delete(__FILE__, __LINE__)
#define delete DEBUG_DELETE
#endif
#endif

void PrintStatistics(bool NoUnknowns=true);


