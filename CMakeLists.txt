
cmake_minimum_required (VERSION 2.8)

enable_testing()

set(CMAKE_INSTALL_PREFIX "/usr/local")
#set(CMAKE_RULE_MESSAGES 0)

project(binder-linux)

# find_package(PkgConfig)
# pkg_check_modules(LIBEVENT libevent)

# configure_file("cutils.pc.in" "cutils.pc" @ONLY)
# configure_file("module.modulemap.in" "module.modulemap" @ONLY)

set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(android include)

add_subdirectory(android/cutils)

add_library(binder SHARED binder/binder.cpp)
target_link_libraries(binder cutils)

# add_executable(system_uptime test/system_uptime.c)
# target_link_libraries(system_uptime cutils)

# add_test(NAME system_uptime COMMAND system_uptime)

# install (TARGETS cutils DESTINATION lib)
# install (FILES atomic/atomic_int.h utils/system_clock.h DESTINATION include/cutils)
# install (FILES cutils.pc DESTINATION lib/pkgconfig)
