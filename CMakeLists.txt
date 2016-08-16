
cmake_minimum_required (VERSION 2.8)

enable_testing()

set(CMAKE_INSTALL_PREFIX "/usr/local")
#set(CMAKE_RULE_MESSAGES 0)

project(binder-linux)

# find_package(PkgConfig)
# pkg_check_modules(LIBEVENT libevent)

# configure_file("cutils.pc.in" "cutils.pc" @ONLY)
# configure_file("module.modulemap.in" "module.modulemap" @ONLY)

set(CMAKE_CXX_FLAGS "-std=gnu++11 -fPIC -DPIC")
set(CMAKE_C_FLAGS "-fPIC -DPIC")

include_directories(android/safe_iop include android)

add_subdirectory(android)

set(SRCS
	binder/Binder.cpp
	binder/BpBinder.cpp
	binder/IPCThreadState.cpp
	binder/Parcel.cpp
	binder/TextOutput.cpp
	binder/Debug.cpp
    binder/Static.cpp
    binder/BufferedTextOutput.cpp
    binder/ProcessState.cpp
    binder/IInterface.cpp
    binder/Status.cpp
    binder/IServiceManager.cpp
    binder/IPermissionController.cpp
)

add_library(binder SHARED ${SRCS})
target_link_libraries(binder android pthread bsd)

add_executable(test_main test/test_main.cpp)
target_link_libraries(test_main binder)

add_executable(service_manager servicemanager/binder.c servicemanager/service_manager.c)
target_link_libraries(service_manager binder selinux)

add_executable(bctest servicemanager/binder.c servicemanager/bctest.c)
target_link_libraries(bctest binder selinux)

# add_test(NAME system_uptime COMMAND system_uptime)

# install (TARGETS cutils DESTINATION lib)
# install (FILES atomic/atomic_int.h utils/system_clock.h DESTINATION include/cutils)
# install (FILES cutils.pc DESTINATION lib/pkgconfig)
