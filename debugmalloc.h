#ifndef DEBUGMALLOC_H
#define DEBUGMALLOC_H

#include <stdbool.h>
#include "debugmalloc-impl.h"

/* These macro-like functions forward all allocation/free
 * calls to debugmalloc. Usage is the same, malloc(size)
 * gives the address of a new memory block, free(ptr)
 * deallocates etc.
 * 
 * If you use this file, make sure that you include this
 * in *ALL* translation units (*.c) of your source. The
 * builtin free() function cannot deallocate a memory block
 * that was allocated via debugmalloc, yet the name of
 * the functions are the same!
 * 
 * If there is a conflict with the macro names used here,
 * you can include debugmalloc-nomacro.h instead. */

#define malloc(S) debugmalloc_malloc_full((S), "malloc", #S, __FILE__, __LINE__, false)
#define calloc(N,S) debugmalloc_malloc_full((N)*(S), "calloc", #N ", " #S, __FILE__, __LINE__, true)
#define realloc(P,S) debugmalloc_realloc_full((P), (S), "realloc", #S, __FILE__, __LINE__)
#define free(P) debugmalloc_free_full((P), "free", __FILE__, __LINE__)

#endif
