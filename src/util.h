#ifndef _UTIL_H_
#define _UTIL_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

// Infallible malloc, terminates program if allocation fails
void* xmalloc(size_t size);

#endif
