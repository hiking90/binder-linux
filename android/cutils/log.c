#include <stdarg.h>
#include <stdio.h>

#include "cutils/log.h"

int __android_log_print(int level, const char *tag, const char *fmt, ...)
{
    va_list ap;
    int ret;

    fprintf(stderr, "%s: ", tag);
    va_start(ap, fmt);
    ret = vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");

    return ret;
}

void __android_log_assert(const char *cond, const char *tag, const char *fmt, ...)
{
    va_list ap;

    fprintf(stderr, "%s - %s: ", cond, tag);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");	
}

int __android_log_is_loggable(int prio, const char *tag, int def)
{
    int logLevel = def;
    return logLevel >= 0 && prio >= logLevel;
}
