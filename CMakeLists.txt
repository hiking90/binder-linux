cmake_minimum_required (VERSION 3.4)

find_package(PkgConfig)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-std=c++17 -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project (binder-linux)

set(ANDROID_DIR "${CMAKE_SOURCE_DIR}/android")
set(BINDER_DIR ${ANDROID_DIR}/native/libs/binder)
set(LIBUTILS_DIR ${ANDROID_DIR}/core/libutils)
set(LIBCUTILS_DIR ${ANDROID_DIR}/core/libcutils)
set(LIBBASE_DIR ${ANDROID_DIR}/libbase)
set(LIBLOG_DIR ${ANDROID_DIR}/logging/liblog)
set(LIBSYSTEM_DIR ${ANDROID_DIR}/core/libsystem)
set(LIBFMT_DIR ${ANDROID_DIR}/fmtlib)
set(LIBGTEST_DIR ${ANDROID_DIR}/googletest/googletest)
set(LIBPROCESSGROUP_DIR ${ANDROID_DIR}/core/libprocessgroup)
set(LIBSELINUX_DIR ${ANDROID_DIR}/selinux/libselinux)
set(LIBBACKTRACE_DIR ${ANDROID_DIR}/unwinding/libbacktrace)
set(LIBUNWINDSTACK_DIR ${ANDROID_DIR}/unwinding/libunwindstack)
set(LIBPROCINFO_DIR ${ANDROID_DIR}/libprocinfo)
set(LIBVINTF_DIR ${ANDROID_DIR}/libvintf)
set(LIBPCRE_DIR ${ANDROID_DIR}/pcre)
set(LIBKVER_DIR ${ANDROID_DIR}/Gki/libkver)
set(LIBSTATSPULL_DIR ${ANDROID_DIR}/StatsD/lib/libstatspull)
set(LIBSTATSSOCKET_DIR ${ANDROID_DIR}/StatsD/lib/libstatssocket)

set(BUILD_TOOLS_DIR "${CMAKE_SOURCE_DIR}/android/build-tools/linux-x86")
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
set(AIDL_DIR ${ANDROID_DIR}/aidl)
set(HIDL_DIR ${ANDROID_DIR}/hidl)
set(STATSD_AIDL_DIR     ${ANDROID_DIR}/StatsD)

include_directories(
    ${BINDER_DIR}/include
    ${LIBUTILS_DIR}/include
    ${LIBCUTILS_DIR}/include
    ${LIBBASE_DIR}/include
    ${LIBLOG_DIR}/include
    ${LIBSYSTEM_DIR}/include
)

macro(aidl_parser name base_dir sources)
    set(${name}_OUTPUTS "")
    foreach(src IN ITEMS ${sources})
        string(REGEX REPLACE "[.]aidl$" ".cpp" output_filename ${src})
        set(_OUTPUT "${GENERATED_DIR}/${output_filename}")
        add_custom_command(
            OUTPUT  ${_OUTPUT}
            COMMAND ${CMAKE_BINARY_DIR}/aidl --lang=cpp --ninja -I ${base_dir} "${base_dir}/${src}" --header_out ${GENERATED_DIR}/include -o ${GENERATED_DIR}
            DEPENDS ${base_dir}/${src}
            COMMENT "[AIDL] ${src} -> ${_OUTPUT}"
            VERBATIM
        )
        list(APPEND ${name}_OUTPUTS ${_OUTPUT})
    endforeach(src)
endmacro()

# Android log

add_library(log STATIC
    ${LIBLOG_DIR}/log_event_list.cpp
    ${LIBLOG_DIR}/log_event_write.cpp
    ${LIBLOG_DIR}/logger_name.cpp
    ${LIBLOG_DIR}/logger_read.cpp
    ${LIBLOG_DIR}/logger_write.cpp
    ${LIBLOG_DIR}/logprint.cpp
    ${LIBLOG_DIR}/properties.cpp
)

target_compile_options(log PUBLIC
    "-DLIBLOG_LOG_TAG=1006"
    "-DSNET_EVENT_LOG_TAG=1397638484"
)

# Android utils
add_library(utils STATIC
    ${LIBUTILS_DIR}/Errors.cpp
    ${LIBUTILS_DIR}/FileMap.cpp
    ${LIBUTILS_DIR}/JenkinsHash.cpp
    ${LIBUTILS_DIR}/NativeHandle.cpp
    ${LIBUTILS_DIR}/Printer.cpp
    ${LIBUTILS_DIR}/RefBase.cpp
    ${LIBUTILS_DIR}/SharedBuffer.cpp
    ${LIBUTILS_DIR}/StopWatch.cpp
    ${LIBUTILS_DIR}/String8.cpp
    ${LIBUTILS_DIR}/String16.cpp
    ${LIBUTILS_DIR}/StrongPointer.cpp
    ${LIBUTILS_DIR}/SystemClock.cpp
    ${LIBUTILS_DIR}/Threads.cpp
    ${LIBUTILS_DIR}/Timers.cpp
    ${LIBUTILS_DIR}/Tokenizer.cpp
    ${LIBUTILS_DIR}/Unicode.cpp
    ${LIBUTILS_DIR}/VectorImpl.cpp
    ${LIBUTILS_DIR}/misc.cpp
    ${LIBUTILS_DIR}/Looper.cpp
)

target_include_directories(utils PUBLIC
    ${LIBBACKTRACE_DIR}/include
)

# Android base
set(LIBASE_SRCS
    ${LIBBASE_DIR}/abi_compatibility.cpp
    ${LIBBASE_DIR}/chrono_utils.cpp
    ${LIBBASE_DIR}/cmsg.cpp
    ${LIBBASE_DIR}/file.cpp
    ${LIBBASE_DIR}/logging.cpp
    ${LIBBASE_DIR}/mapped_file.cpp
    ${LIBBASE_DIR}/parsebool.cpp
    ${LIBBASE_DIR}/parsenetaddress.cpp
    ${LIBBASE_DIR}/process.cpp
    ${LIBBASE_DIR}/properties.cpp
    ${LIBBASE_DIR}/stringprintf.cpp
    ${LIBBASE_DIR}/strings.cpp
    ${LIBBASE_DIR}/threads.cpp
    ${LIBBASE_DIR}/test_utils.cpp
    ${LIBBASE_DIR}/errors_unix.cpp
)

add_library(base STATIC ${LIBASE_SRCS})

# Android processgroup
# add_library(processgroup STATIC
#     ${LIBPROCESSGROUP_DIR}/cgroup_map.cpp
#     ${LIBPROCESSGROUP_DIR}/processgroup.cpp
#     ${LIBPROCESSGROUP_DIR}/sched_policy.cpp
#     ${LIBPROCESSGROUP_DIR}/task_profiles.cpp
#     ${LIBPROCESSGROUP_DIR}/cgrouprc/cgroup_controller.cpp
#     ${LIBPROCESSGROUP_DIR}/cgrouprc/cgroup_file.cpp
#     ${LIBPROCESSGROUP_DIR}/cgrouprc_format/cgroup_controller.cpp
# )

# target_include_directories(processgroup PUBLIC
#     ${LIBPROCESSGROUP_DIR}/cgrouprc_format/include
#     ${LIBPROCESSGROUP_DIR}/cgrouprc/include
#     ${LIBPROCESSGROUP_DIR}/include
#     ${LIBPROCESSGROUP_DIR}
# )

# target_link_libraries(processgroup PUBLIC base)

# Android selinux
# add_library(selinux STATIC
#     ${LIBSELINUX_DIR}/src/booleans.c
#     ${LIBSELINUX_DIR}/src/callbacks.c
#     ${LIBSELINUX_DIR}/src/freecon.c
#     ${LIBSELINUX_DIR}/src/label_backends_android.c
#     ${LIBSELINUX_DIR}/src/label.c
#     ${LIBSELINUX_DIR}/src/label_support.c
#     ${LIBSELINUX_DIR}/src/matchpathcon.c
#     ${LIBSELINUX_DIR}/src/setrans_client.c
#     ${LIBSELINUX_DIR}/src/sha1.c
#     ${LIBSELINUX_DIR}/src/android/android_host.c
#     ${LIBSELINUX_DIR}/src/avc.c
#     ${LIBSELINUX_DIR}/src/avc_internal.c
#     ${LIBSELINUX_DIR}/src/avc_sidtab.c
#     ${LIBSELINUX_DIR}/src/compute_av.c
#     ${LIBSELINUX_DIR}/src/compute_create.c
#     ${LIBSELINUX_DIR}/src/compute_member.c
#     ${LIBSELINUX_DIR}/src/context.c
#     ${LIBSELINUX_DIR}/src/deny_unknown.c
#     ${LIBSELINUX_DIR}/src/enabled.c
#     ${LIBSELINUX_DIR}/src/fgetfilecon.c
#     ${LIBSELINUX_DIR}/src/getenforce.c
#     ${LIBSELINUX_DIR}/src/getfilecon.c
#     ${LIBSELINUX_DIR}/src/get_initial_context.c
#     ${LIBSELINUX_DIR}/src/init.c
#     ${LIBSELINUX_DIR}/src/lgetfilecon.c
#     ${LIBSELINUX_DIR}/src/load_policy.c
#     ${LIBSELINUX_DIR}/src/lsetfilecon.c
#     ${LIBSELINUX_DIR}/src/mapping.c
#     ${LIBSELINUX_DIR}/src/procattr.c
#     ${LIBSELINUX_DIR}/src/reject_unknown.c
#     ${LIBSELINUX_DIR}/src/setenforce.c
#     ${LIBSELINUX_DIR}/src/setexecfilecon.c
#     ${LIBSELINUX_DIR}/src/setfilecon.c
#     ${LIBSELINUX_DIR}/src/stringrep.c
#     ${LIBSELINUX_DIR}/src/label_file.c
#     ${LIBSELINUX_DIR}/src/regex.c
# )

# target_include_directories(selinux PUBLIC
#     ${LIBSELINUX_DIR}/include
#     ${LIBPCRE_DIR}/include
# )

# target_compile_options(selinux PRIVATE
#     "-DNO_PERSISTENTLY_STORED_PATTERNS"
#     "-DDISABLE_SETRANS"
#     "-DDISABLE_BOOL"
#     "-D_GNU_SOURCE"
#     "-DNO_MEDIA_BACKEND"
#     "-DNO_X_BACKEND"
#     "-DNO_DB_BACKEND"
#     "-DBUILD_HOST"
#     "-DUSE_PCRE2"
#     "-Wall"
#     "-Werror"
#     "-Wno-error=missing-noreturn"
#     "-Wno-error=unused-function"
#     "-Wno-error=unused-variable"
# )

# # Android backtrace
# add_library(backtrace STATIC
#     ${LIBBACKTRACE_DIR}/Backtrace.cpp
#     ${LIBBACKTRACE_DIR}/BacktraceCurrent.cpp
#     ${LIBBACKTRACE_DIR}/BacktracePtrace.cpp
#     ${LIBBACKTRACE_DIR}/ThreadEntry.cpp
#     ${LIBBACKTRACE_DIR}/UnwindStack.cpp
#     ${LIBBACKTRACE_DIR}/UnwindStackMap.cpp
#     ${LIBBACKTRACE_DIR}/BacktraceMap.cpp
#     ${LIBPROCINFO_DIR}/process.cpp
# )

# target_include_directories(backtrace PUBLIC
#     ${LIBBACKTRACE_DIR}/include
#     ${LIBUNWINDSTACK_DIR}/include
#     ${LIBPROCINFO_DIR}/include
# )

# target_compile_options(backtrace PRIVATE
#     "-Wexit-time-destructors"
# )

# Android Binder
set(BINDER_SRCS
    ${BINDER_DIR}/Binder.cpp
    ${BINDER_DIR}/BpBinder.cpp
    ${BINDER_DIR}/BufferedTextOutput.cpp
    ${BINDER_DIR}/Debug.cpp
    ${BINDER_DIR}/IInterface.cpp
    ${BINDER_DIR}/IMemory.cpp
    ${BINDER_DIR}/IPCThreadState.cpp
    ${BINDER_DIR}/IResultReceiver.cpp
    ${BINDER_DIR}/IServiceManager.cpp
    ${BINDER_DIR}/IShellCallback.cpp
    ${BINDER_DIR}/LazyServiceRegistrar.cpp
    ${BINDER_DIR}/MemoryBase.cpp
    ${BINDER_DIR}/MemoryDealer.cpp
    ${BINDER_DIR}/MemoryHeapBase.cpp
    ${BINDER_DIR}/Parcel.cpp
    ${BINDER_DIR}/ParcelableHolder.cpp
    ${BINDER_DIR}/ParcelFileDescriptor.cpp
    ${BINDER_DIR}/PersistableBundle.cpp
    ${BINDER_DIR}/ProcessState.cpp
    ${BINDER_DIR}/RpcAddress.cpp
    ${BINDER_DIR}/RpcSession.cpp
    ${BINDER_DIR}/RpcServer.cpp
    ${BINDER_DIR}/RpcState.cpp
    ${BINDER_DIR}/Static.cpp
    ${BINDER_DIR}/Stability.cpp
    ${BINDER_DIR}/Status.cpp
    ${BINDER_DIR}/TextOutput.cpp
    ${BINDER_DIR}/Utils.cpp
)

set(aidl_srcs
    "android/os/IServiceManager.aidl"
    "android/os/IServiceCallback.aidl"
    "android/os/IClientCallback.aidl"
    "android/os/ServiceDebugInfo.aidl"
)
aidl_parser(binder_aidl "${BINDER_DIR}/aidl" "${aidl_srcs}")

add_library(binder STATIC
    ${binder_aidl_OUTPUTS}
    ${BINDER_SRCS}
)

add_dependencies(binder aidl)

target_include_directories(binder PUBLIC
    ${BINDER_DIR}/include
    ${LIBPROCESSGROUP_DIR}/include
    ${LIBSELINUX_DIR}/include
    ${LIBBACKTRACE_DIR}/include
    ${GENERATED_DIR}/include
    ${BINDER_DIR}/ndk/include_cpp
)

target_compile_options(binder PUBLIC
    # "-D__ANDROID__"
    # "-DANDROID_BASE_UNIQUE_FD_DISABLE_IMPLICIT_CONVERSION"
    # "-DANDROID_UTILS_REF_BASE_DISABLE_IMPLICIT_CONSTRUCTION"
)

# target_link_libraries(binder PUBLIC
#     base
#     utils
#     processgroup
#     selinux
#     backtrace
# )

# Android libbinder_ndk

add_library(binder_ndk STATIC
    ${BINDER_DIR}/ndk/ibinder.cpp
    # ${BINDER_DIR}/ndk/ibinder_jni.cpp
    ${BINDER_DIR}/ndk/parcel.cpp
    # ${BINDER_DIR}/ndk/parcel_jni.cpp
    ${BINDER_DIR}/ndk/process.cpp
    ${BINDER_DIR}/ndk/stability.cpp
    ${BINDER_DIR}/ndk/status.cpp
    ${BINDER_DIR}/ndk/service_manager.cpp
)

target_include_directories(binder_ndk PUBLIC
    ${BINDER_DIR}/ndk/include_cpp
    ${BINDER_DIR}/ndk/include_ndk
    ${BINDER_DIR}/ndk/include_platform
)

target_compile_options(binder_ndk PUBLIC
    "-D__INTRODUCED_IN(n)="
    "-D__ANDROID_API__=10000"
)

target_link_libraries(binder_ndk PUBLIC
    base
    binder
    utils
)


# Android libcutils
add_library(cutils STATIC
    ${LIBCUTILS_DIR}/socket_inaddr_any_server_unix.cpp
    ${LIBCUTILS_DIR}/socket_local_client_unix.cpp
    ${LIBCUTILS_DIR}/socket_local_server_unix.cpp
    ${LIBCUTILS_DIR}/socket_network_client_unix.cpp
    ${LIBCUTILS_DIR}/sockets_unix.cpp
    ${LIBCUTILS_DIR}/ashmem-host.cpp
    ${LIBCUTILS_DIR}/fs_config.cpp
    ${LIBCUTILS_DIR}/trace-host.cpp
    ${LIBCUTILS_DIR}/config_utils.cpp
    ${LIBCUTILS_DIR}/canned_fs_config.cpp
    ${LIBCUTILS_DIR}/iosched_policy.cpp
    ${LIBCUTILS_DIR}/load_file.cpp
    ${LIBCUTILS_DIR}/native_handle.cpp
    ${LIBCUTILS_DIR}/properties.cpp
    ${LIBCUTILS_DIR}/record_stream.cpp
    ${LIBCUTILS_DIR}/strlcpy.c
    ${LIBCUTILS_DIR}/threads.cpp
    ${LIBCUTILS_DIR}/fs.cpp
    ${LIBCUTILS_DIR}/hashmap.cpp
    ${LIBCUTILS_DIR}/multiuser.cpp
    ${LIBCUTILS_DIR}/str_parms.cpp
)

target_include_directories(cutils PUBLIC
    ${LIBCUTILS_DIR}/include
)

# Android FMT
add_library(fmt STATIC ${LIBFMT_DIR}/src/format.cc)

target_include_directories(fmt PUBLIC
    ${LIBFMT_DIR}/include
)

target_compile_options(fmt PRIVATE
    "-fno-exceptions"
    "-UNDEBUG"
)

# Android GTEST
add_library(gtest STATIC ${LIBGTEST_DIR}/src/gtest-all.cc)

target_include_directories(gtest PUBLIC
    ${LIBGTEST_DIR}
    ${LIBGTEST_DIR}/include
)

target_link_libraries(gtest PUBLIC "pthread")


# Android AIDL generator

find_package(FLEX)
find_package(BISON)

set(FLEX_FOUND "True")
set(FLEX_EXECUTABLE "${BUILD_TOOLS_DIR}/bin/flex")

set(FLEX_OUT "${GENERATED_DIR}/aidl_language_l.cpp")
set(FLEX_SRC "${AIDL_DIR}/aidl_language_l.ll")

FLEX_TARGET(lexer ${FLEX_SRC} ${FLEX_OUT})

set(BISON_FOUND "True")
set(BISON_EXECUTABLE "${BUILD_TOOLS_DIR}/bin/bison")

set(BISON_OUT       "${GENERATED_DIR}/aidl_language_y.cpp")
set(BISON_DEFINES   "${GENERATED_DIR}/aidl_language_y.h")
set(BISON_SRC       "${AIDL_DIR}/aidl_language_y.yy")

BISON_TARGET(parser ${BISON_SRC} ${BISON_OUT}
    COMPILE_FLAGS   "-d"
    DEFINES_FILE    ${BISON_DEFINES}
)

set(AIDL_SRCS
    ${AIDL_DIR}/aidl_checkapi.cpp
    ${AIDL_DIR}/aidl_const_expressions.cpp
    ${AIDL_DIR}/aidl_dumpapi.cpp
    ${AIDL_DIR}/aidl_language.cpp
    ${AIDL_DIR}/aidl_to_cpp_common.cpp
    ${AIDL_DIR}/aidl_to_cpp.cpp
    ${AIDL_DIR}/aidl_to_java.cpp
    ${AIDL_DIR}/aidl_to_ndk.cpp
    ${AIDL_DIR}/aidl_to_rust.cpp
    ${AIDL_DIR}/aidl_typenames.cpp
    ${AIDL_DIR}/aidl.cpp
    ${AIDL_DIR}/ast_cpp.cpp
    ${AIDL_DIR}/ast_java.cpp
    ${AIDL_DIR}/code_writer.cpp
    ${AIDL_DIR}/comments.cpp
    ${AIDL_DIR}/diagnostics.cpp
    ${AIDL_DIR}/generate_aidl_mappings.cpp
    ${AIDL_DIR}/generate_cpp.cpp
    ${AIDL_DIR}/generate_java_binder.cpp
    ${AIDL_DIR}/generate_java.cpp
    ${AIDL_DIR}/generate_ndk.cpp
    ${AIDL_DIR}/generate_rust.cpp
    ${AIDL_DIR}/import_resolver.cpp
    ${AIDL_DIR}/io_delegate.cpp
    ${AIDL_DIR}/line_reader.cpp
    ${AIDL_DIR}/location.cpp
    ${AIDL_DIR}/logging.cpp
    ${AIDL_DIR}/options.cpp
    ${AIDL_DIR}/parser.cpp
    ${AIDL_DIR}/main.cpp
)

add_executable(aidl
    ${FLEX_lexer_OUTPUTS}
    ${BISON_parser_OUTPUTS}
    ${AIDL_SRCS}
)

target_include_directories(aidl PUBLIC
    ${AIDL_DIR}
    ${LIBFMT_DIR}/include
    ${LIBGTEST_DIR}/include
    ${GENERATED_DIR}
)

# target_compile_definitions(aidl PRIVATE AIDL_CPP_BUILD)

target_link_libraries(aidl PUBLIC
    base
    fmt
    gtest
)

# Android vintf

add_library(vintf STATIC
    ${LIBVINTF_DIR}/parse_string.cpp
    ${LIBVINTF_DIR}/parse_xml.cpp
    ${LIBVINTF_DIR}/CompatibilityMatrix.cpp
    ${LIBVINTF_DIR}/FileSystem.cpp
    ${LIBVINTF_DIR}/HalManifest.cpp
    ${LIBVINTF_DIR}/HalInterface.cpp
    ${LIBVINTF_DIR}/KernelConfigTypedValue.cpp
    ${LIBVINTF_DIR}/KernelConfigParser.cpp
    ${LIBVINTF_DIR}/KernelInfo.cpp
    ${LIBVINTF_DIR}/RuntimeInfo.cpp
    ${LIBVINTF_DIR}/ManifestHal.cpp
    ${LIBVINTF_DIR}/ManifestInstance.cpp
    ${LIBVINTF_DIR}/MatrixInstance.cpp
    ${LIBVINTF_DIR}/MatrixKernel.cpp
    ${LIBVINTF_DIR}/MatrixHal.cpp
    ${LIBVINTF_DIR}/PropertyFetcher.cpp
    ${LIBVINTF_DIR}/Regex.cpp
    ${LIBVINTF_DIR}/SystemSdk.cpp
    ${LIBVINTF_DIR}/TransportArch.cpp
    ${LIBVINTF_DIR}/VintfObject.cpp
    ${LIBVINTF_DIR}/XmlFile.cpp
    ${LIBVINTF_DIR}/utils.cpp
    ${LIBVINTF_DIR}/RuntimeInfo-host.cpp
    ${ANDROID_DIR}/tinyxml2/tinyxml2.cpp

    ${LIBKVER_DIR}/kernel_release.cpp
    ${LIBKVER_DIR}/kmi_version.cpp
    ${LIBKVER_DIR}/utils.cpp

    ${HIDL_DIR}/utils/FQName.cpp
    ${HIDL_DIR}/utils/FqInstance.cpp

)

# target_compile_options(vintf PUBLIC
#     "-Wextra-semi"
# )

target_include_directories(vintf PUBLIC
    ${LIBVINTF_DIR}
    ${LIBVINTF_DIR}/include
    ${LIBVINTF_DIR}/include/vintf
    ${HIDL_DIR}/utils/include
    ${HIDL_DIR}/utils/include/hidl-util
    ${HIDL_DIR}/metadata/include
    ${AIDL_DIR}/metadata/include
    ${ANDROID_DIR}/tinyxml2
    ${LIBKVER_DIR}/include
)

# Android pcre

add_library(pcre STATIC
    ${LIBPCRE_DIR}/dist2/src/pcre2_auto_possess.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_compile.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_config.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_context.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_convert.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_dfa_match.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_error.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_extuni.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_find_bracket.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_maketables.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_match.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_match_data.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_jit_compile.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_newline.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_ord2utf.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_pattern_info.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_script_run.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_serialize.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_string_utils.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_study.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_substitute.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_substring.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_tables.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_ucd.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_valid_utf.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_xclass.c
    ${LIBPCRE_DIR}/dist2/src/pcre2_chartables.c
)

target_include_directories(pcre PUBLIC
    ${LIBPCRE_DIR}/include_internal
    ${LIBPCRE_DIR}/include
)

target_compile_options(pcre PUBLIC
    "-DHAVE_CONFIG_H"
)

# libbinder_linux

add_library(binder_linux SHARED
    $<TARGET_OBJECTS:base>
    $<TARGET_OBJECTS:binder>
    $<TARGET_OBJECTS:utils>
    $<TARGET_OBJECTS:log>
    $<TARGET_OBJECTS:cutils>
    $<TARGET_OBJECTS:vintf>
)

target_link_libraries(binder_linux
    pthread
)

# Android servicemanager daemon
set(SERVICE_MANAGER_DIR "android/native/cmds/servicemanager")

add_executable(servicemanager
    servicemanager/Access.cpp
    ${SERVICE_MANAGER_DIR}/ServiceManager.cpp
    ${SERVICE_MANAGER_DIR}/main.cpp
)

target_compile_definitions(servicemanager PRIVATE
    ANDROID_UTILS_REF_BASE_DISABLE_IMPLICIT_CONSTRUCTION
)

target_include_directories(servicemanager PUBLIC
    ${SERVICE_MANAGER_DIR}
    ${LIBSELINUX_DIR}/include
    ${GENERATED_DIR}/include
    ${BINDER_DIR}/ndk/include_cpp
    ${LIBVINTF_DIR}/include
    ${AIDL_DIR}/metadata/include
    ${HIDL_DIR}/metadata/include
    ${HIDL_DIR}/utils/include
)

target_link_libraries(servicemanager PUBLIC
    binder_linux
)

add_executable(add_binder
    tools/add_binder.c
)


aidl_parser(echo_aidl "${CMAKE_SOURCE_DIR}/test" "IBinderEcho.aidl")

add_executable(test_binder
    ${echo_aidl_OUTPUTS}
    test/test_main.cpp
)

target_include_directories(test_binder PUBLIC
    ${GENERATED_DIR}/include
)

target_link_libraries(test_binder PUBLIC
    binder_linux
)
