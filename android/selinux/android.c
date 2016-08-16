#include <stdarg.h>
#include <stdio.h>

#include "cutils/log.h"

#include "android.h"

struct selabel_handle* selinux_android_service_context_handle(void)
{
    return NULL;
}

int selinux_log_callback(int type, const char *fmt, ...)
{
    va_list ap;
    int prio;

    switch (type) {
        case SELINUX_WARNING:
            prio = LOG_WARN;
            break;
        case SELINUX_INFO:
            prio = LOG_INFO;
            break;
        default:
            prio = LOG_ERROR;
            break;
    }

    fprintf(stderr, "SELINUX: ");

    va_start (ap, fmt);
    vfprintf (stderr, fmt, ap);
    va_end (ap);

    fprintf(stderr, "\n");

    return 0;
}
