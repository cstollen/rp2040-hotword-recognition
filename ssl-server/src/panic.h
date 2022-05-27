#ifndef _PANIC_H
#define _PANIC_H

#include <stdio.h>

#define assert_panic(expr, mess) ({ if (!(expr)) {fprintf(stderr, "Panic: %s:%d %s\n", __FILE__, __LINE__, mess); exit(1);} })
#define panic_malloc(ptr) assert_panic((ptr) != 0, "cannot malloc")

#endif