
include_directories(utils)

set(SRCS 
	safe_iop/safe_iop.c 
	cutils/log.c 
	cutils/threads.c
	cutils/ashmem-dev.c
	cutils/sched_policy.c
	cutils/native_handle.c
	cutils/process_name.c
	utils/RefBase.cpp 
	utils/VectorImpl.cpp 
	utils/String16.cpp 
	utils/String8.cpp 
	utils/SharedBuffer.cpp 
	utils/Unicode.cpp
	utils/misc.cpp
	utils/Static.cpp
	utils/Threads.cpp
)

add_library(android ${SRCS})
