// src/lib/squirrel/squirrel/sqmem.cpp
// SPDX-License-Identifier: MIT

/*
    see copyright notice in squirrel.h
*/
#include "sqpcheader.h"
#ifndef SQ_EXCLUDE_DEFAULT_MEMFUNCTIONS

//no caller checks the result, so returning null hands back a pointer that gets
//stored and written through
static void sq_vm_nomemory(SQUnsignedInteger size)
{
    fprintf(stderr, "squirrel: out of memory allocating %llu bytes\n",
        (unsigned long long)size);
    abort();
}

void *sq_vm_malloc(SQUnsignedInteger size)
{
    void *p = malloc(size);
    //a zero-size request is allowed to return null
    if(!p && size) sq_vm_nomemory(size);
    return p;
}

void *sq_vm_realloc(void *p, SQUnsignedInteger SQ_UNUSED_ARG(oldsize), SQUnsignedInteger size)
{
    void *np = realloc(p, size);
    if(!np && size) sq_vm_nomemory(size);
    return np;
}

void sq_vm_free(void *p, SQUnsignedInteger SQ_UNUSED_ARG(size)){ free(p); }
#endif
