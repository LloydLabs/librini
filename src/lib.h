//
// Created by LloydLabs on 9/25/2017.
// The purpose of this is to not re-implement libc, but to provide the user
// without the burden of having libc as a dependency.
//

#ifndef RINI_LIB_H
#define RINI_LIB_H

#ifdef NO_LIBC

#ifndef NULL
#define NULL ((void*)0)
#endif

#define memset rini_memset

unsigned rini_strnlen(const char* str, unsigned max);
inline void* rini_memset(void* dest, char set, unsigned size);
int rini_strncmp(const char* first, const char* second, unsigned max);
int rini_memcmp(void* first, void* second, unsigned size);
inline void* rini_memcpy(const void* dest, const void* src, unsigned size);

#else
#include <string.h>
#endif

#endif // LIB_H
