#ifndef __LOG_H__
#define __LOG_H__

#include <stdbool.h>

#ifndef LOG_NDEBUG
#ifdef NDEBUG
#define LOG_NDEBUG  1
#else
#define LOG_NDEBUG  0
#endif
#endif

enum {
    LOG_ERROR = 1,
    LOG_WARNING,
    LOG_DEBUG,
    LOG_VERBOSE,
    LOG_INFO,
};


#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

void alog(int, const char *, const char *, ...);

#ifndef ALOGE
#if LOG_NDEBUG
#define ALOGE(...)  ((void)0)
#else
#define ALOGE(...)  ((void)alog(LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif
#endif

#ifndef ALOGW
#if LOG_NDEBUG
#define ALOGW(...)  ((void)0)
#else
#define ALOGW(...)  ((void)alog(LOG_WARNING, LOG_TAG, __VA_ARGS__))
#endif
#endif

#ifndef ALOGD
#if LOG_NDEBUG
#define ALOGD(...)  ((void)0)
#else
#define ALOGD(...)  ((void)alog(LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif
#endif

#ifndef ALOGV
#if LOG_NDEBUG
#define ALOGV(...)  ((void)0)
#else
#define ALOGV(...)  ((void)alog(LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#endif
#endif

#ifndef ALOGI
#if LOG_NDEBUG
#define ALOGI(...)  ((void)0)
#else
#define ALOGI(...)  ((void)alog(LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif
#endif

#endif /* __LOG_H__ */
