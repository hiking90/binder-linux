#ifndef __LOG_H__
#define __LOG_H__

#include <stdbool.h>
#include <android/log.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_NDEBUG
#ifdef NDEBUG
#define LOG_NDEBUG  1
#else
#define LOG_NDEBUG  0
#endif
#endif

enum {
    LOG_ERROR = 1,
    LOG_WARN,
    LOG_DEBUG,
    LOG_VERBOSE,
    LOG_INFO,
};


#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

// ---------------------------------------------------------------------

#ifndef __predict_false
#define __predict_false(exp) __builtin_expect((exp) != 0, 0)
#endif


// void __android_log_print(int, const char *, const char *, ...);
// void __android_log_assert(const char *cond, const char *tag, const char *fmt, ...);

/* Returns 2nd arg.  Used to substitute default value if caller's vararg list
 * is empty.
 */
#define __android_second(dummy, second, ...)     second

#define __android_rest(first, ...)               , ## __VA_ARGS__

#define android_printAssert(cond, tag, ...) \
    __android_log_assert(cond, tag, \
        __android_second(0, ## __VA_ARGS__, NULL) __android_rest(__VA_ARGS__))

/*
 *    IF_ALOG uses android_testLog, but IF_ALOG can be overridden.
 *    android_testLog will remain constant in its purpose as a wrapper
 *        for Android logging filter policy, and can be subject to
 *        change. It can be reused by the developers that override
 *        IF_ALOG as a convenient means to reimplement their policy
 *        over Android.
 */
#if LOG_NDEBUG /* Production */
#define android_testLog(prio, tag) \
    (__android_log_is_loggable(prio, tag, ANDROID_LOG_DEBUG) != 0)
#else
#define android_testLog(prio, tag) \
    (__android_log_is_loggable(prio, tag, ANDROID_LOG_VERBOSE) != 0)
#endif

/*
 * Use the per-tag properties "log.tag.<tagname>" to generate a runtime
 * result of non-zero to expose a log. prio is ANDROID_LOG_VERBOSE to
 * ANDROID_LOG_FATAL. default_prio if no property. Undefined behavior if
 * any other value.
 */
int __android_log_is_loggable(int prio, const char *tag, int default_prio);

/*
 * ===========================================================================
 *
 * The stuff in the rest of this file should not be used directly.
 */

#define android_printLog(prio, tag, ...) \
    __android_log_print(prio, tag, __VA_ARGS__)

#define android_vprintLog(prio, cond, tag, ...) \
    __android_log_vprint(prio, tag, __VA_ARGS__)

/*
 * Basic log message macro.
 *
 * Example:
 *  ALOG(LOG_WARN, NULL, "Failed with error %d", errno);
 *
 * The second argument may be NULL or "" to indicate the "global" tag.
 */
#ifndef ALOG
#define ALOG(priority, tag, ...) \
    LOG_PRI(ANDROID_##priority, tag, __VA_ARGS__)
#endif

/*
 * Log macro that allows you to specify a number for the priority.
 */
#ifndef LOG_PRI
#define LOG_PRI(priority, tag, ...) \
    android_printLog(priority, tag, __VA_ARGS__)
#endif

/*
 * Log macro that allows you to pass in a varargs ("args" is a va_list).
 */
#ifndef LOG_PRI_VA
#define LOG_PRI_VA(priority, tag, fmt, args) \
    android_vprintLog(priority, NULL, tag, fmt, args)
#endif

/*
 * Conditional given a desired logging priority and tag.
 */
#ifndef IF_ALOG
#define IF_ALOG(priority, tag) \
    if (android_testLog(ANDROID_##priority, tag))
#endif


#ifndef ALOGE
#if LOG_NDEBUG
#define ALOGE(...)  ((void)0)
#else
#define ALOGE(...)  ((void)ALOG(LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif
#endif

#ifndef ALOGW
#if LOG_NDEBUG
#define ALOGW(...)  ((void)0)
#else
#define ALOGW(...)  ((void)ALOG(LOG_WARN, LOG_TAG, __VA_ARGS__))
#endif
#endif

#ifndef ALOGW_IF
#define ALOGW_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)ALOG(LOG_WARN, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif


#ifndef ALOGD
#if LOG_NDEBUG
#define ALOGD(...)  ((void)0)
#else
#define ALOGD(...)  ((void)ALOG(LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif
#endif

#ifndef ALOGV
#if LOG_NDEBUG
#define ALOGV(...)  ((void)0)
#else
#define ALOGV(...)  ((void)ALOG(LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#endif
#endif

#ifndef ALOGI
#if LOG_NDEBUG
#define ALOGI(...)  ((void)0)
#else
#define ALOGI(...)  ((void)ALOG(LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif
#endif

/*
 * Log a fatal error.  If the given condition fails, this stops program
 * execution like a normal assertion, but also generating the given message.
 * It is NOT stripped from release builds.  Note that the condition test
 * is -inverted- from the normal assert() semantics.
 */
#ifndef LOG_ALWAYS_FATAL_IF
#define LOG_ALWAYS_FATAL_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)android_printAssert(#cond, LOG_TAG, ## __VA_ARGS__)) \
    : (void)0 )
#endif

#ifndef LOG_ALWAYS_FATAL
#define LOG_ALWAYS_FATAL(...) \
    ( ((void)android_printAssert(NULL, LOG_TAG, ## __VA_ARGS__)) )
#endif

/*
 * Versions of LOG_ALWAYS_FATAL_IF and LOG_ALWAYS_FATAL that
 * are stripped out of release builds.
 */
#if LOG_NDEBUG

#ifndef LOG_FATAL_IF
#define LOG_FATAL_IF(cond, ...) ((void)0)
#endif
#ifndef LOG_FATAL
#define LOG_FATAL(...) ((void)0)
#endif

#else

#ifndef LOG_FATAL_IF
#define LOG_FATAL_IF(cond, ...) LOG_ALWAYS_FATAL_IF(cond, ## __VA_ARGS__)
#endif
#ifndef LOG_FATAL
#define LOG_FATAL(...) LOG_ALWAYS_FATAL(__VA_ARGS__)
#endif

#endif

/*
 * Assertion that generates a log message when the assertion fails.
 * Stripped out of release builds.  Uses the current LOG_TAG.
 */
#ifndef ALOG_ASSERT
#define ALOG_ASSERT(cond, ...) LOG_FATAL_IF(!(cond), ## __VA_ARGS__)
//#define ALOG_ASSERT(cond) LOG_FATAL_IF(!(cond), "Assertion failed: " #cond)
#endif

 // ---------------------------------------------------------------------

/*
 * Conditional given a desired logging priority and tag.
 */
#ifndef IF_ALOG
#define IF_ALOG(priority, tag) \
    if (android_testLog(ANDROID_##priority, tag))
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * verbose priority.
 */
#ifndef IF_ALOGV
#if LOG_NDEBUG
#define IF_ALOGV() if (false)
#else
#define IF_ALOGV() IF_ALOG(LOG_VERBOSE, LOG_TAG)
#endif
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * debug priority.
 */
#ifndef IF_ALOGD
#define IF_ALOGD() IF_ALOG(LOG_DEBUG, LOG_TAG)
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * info priority.
 */
#ifndef IF_ALOGI
#define IF_ALOGI() IF_ALOG(LOG_INFO, LOG_TAG)
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * warn priority.
 */
#ifndef IF_ALOGW
#define IF_ALOGW() IF_ALOG(LOG_WARN, LOG_TAG)
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * error priority.
 */
#ifndef IF_ALOGE
#define IF_ALOGE() IF_ALOG(LOG_ERROR, LOG_TAG)
#endif


#ifdef __cplusplus
}
#endif

#endif /* __LOG_H__ */
