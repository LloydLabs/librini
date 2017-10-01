//
// Created by ExitProcess on 9/25/2017.
//

#include "lib.h"

unsigned rini_strnlen(const char* str, unsigned max)
{
    unsigned str_length = 0;
    for (char* buf = (char*)str; *buf != 0; buf++, str_length++)
    {
        if ((str_length + 1) >= max)
        {
            break;
        }
    }

    return str_length;
}

inline void* rini_memset(void* dest, char set, unsigned size)
{
    char* dest_buf = (char*)dest;
    for (unsigned i = 0; i < size; i++)
    {
        *dest_buf++ = set;
    }

    return dest;
}

int rini_strncmp(const char* first, const char* second, unsigned max)
{
    unsigned i = 0;

    char* first_buf = (char*)first;
    char* second_buf = (char*)second;

    for ( ; *first_buf != 0; first++, i++)
    {
	    if (i >= max)
	    {
		    return -1;
	    }
	    
        for ( ; *second_buf != 0; second_buf++)
        {
            if (*first_buf < *second_buf)
            {
                return -1;
            }
            else if (*first_buf > *second_buf)
            {
                return 1;
            }
        }
    }
	
    return 0; // TODO: *first_buf - *second_buf.
}

int rini_memcmp(void* first, void* second, unsigned size)
{
    unsigned char* first_buf = (unsigned char*)first;
    unsigned char* second_buf = (unsigned char*)second;

    for (unsigned i = 0; i < size; i++)
    {
        if (*first_buf++ != *second_buf++)
        {
            return 0;
        }
    }

    return 1;
}

inline void* rini_memcpy(const void* dest, const void* src, unsigned size)
{
    unsigned char* dest_buffer = (unsigned char*)dest;
    unsigned char* src_buffer = (unsigned char*)src;

    for (unsigned i = 0; i < size; i++)
    {
        *dest_buffer++ = *src_buffer++;
    }

    return (void*)dest;
}