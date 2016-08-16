#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "cutils/log.h"

static const char * LOG_LEVEL[] = {
    "UNKNOWN", "DEFAULT", "VERBOSE", "DEBUG  ", "INFO   ", "WARN   ", "ERROR  ", "FATAL  ", "SILENT "
};

static int _default_log_level = ANDROID_LOG_INFO;

__attribute__ ((constructor))
static void __check_log_level() 
{
    const char *level = getenv("LOG_LEVEL");
    if (level != NULL) {
        _default_log_level = atoi(level);
    }
}

int __android_log_print(int level, const char *tag, const char *fmt, ...)
{
    va_list ap;
    int ret;
    const char *ltag = LOG_LEVEL[0];

    if (level < _default_log_level)
        return 0;

    if (level > 0 && level <= ANDROID_LOG_SILENT)
        ltag = LOG_LEVEL[level];

    fprintf(stderr, "[%s] %s: ", ltag, tag);
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

    abort();
}

int __android_log_is_loggable(int prio, const char *tag, int def)
{
    int logLevel = def;
    return logLevel >= _default_log_level && prio >= logLevel;
}
