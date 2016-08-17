
cmake_minimum_required (VERSION 2.8)

enable_testing()

set(CMAKE_INSTALL_PREFIX "/usr/local")
#set(CMAKE_RULE_MESSAGES 0)

project(binder-linux)

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

## aidl-cpp
if (EXISTS "aidl-cpp/CMakeLists.txt")
add_subdirectory(aidl-cpp)
endif()

