#ifndef __BINDER_LINUX_ASSERT_H__

namespace binder_linux_assert {
    #undef __assert
    #include <assert.h>
}

#undef assert

#ifdef NDEBUG
#define	assert(x) (void)0
#else
#define assert(x) ((void)((x) || (binder_linux_assert::__assert_fail(#x, __FILE__, __LINE__, __func__),0)))
#endif

#endif // __BINDER_LINUX_ASSERT_H__