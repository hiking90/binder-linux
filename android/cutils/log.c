#include <stdarg.h>
#include <stdio.h>

#include "log.h"

void alog(int level, const char *tag, const char *fmt, ...)
{
    va_list ap;

    fprintf(stderr, "%s: ", tag);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");
}
