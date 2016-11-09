# About
Goal of this project is to use Android Binder at Linux desktop environment.
The most of source code was come from Android Source Repositories. But, the source code was modified slightly for Linux dekstop.

## Prerequisites to build
1. Linux kernel's Binder must be enabled.
2. libselinux must be installed. http://userspace.selinuxproject.org/
3. CMake is used as a build system.
4. If you want to build with aidl-cpp, please do "git clone https://github.com/hiking90/aidl-cpp.git" as a subdirectory of the project. Ex) $ cd binder-linux && git clone ... 

## Build
<pre>
$ cmake .
$ make
</pre>

## Testing
<pre>
$ ./servicemanager &
$ ./test_main server &
$ ./test_main
</pre>

## Aidl for Cpp

## AOSP Git Comments
- native/include/binder   115e6ec7e93a68b753c9e859221785bd798ab5f8
- native/libs/binder      59e086f536e1dbbbfe9cb6a1826aaddef999d5ce
- libutils                0f39fe22ae3886cff53e48a3de0c9bf306919a9d
- libcutils               5f851752d36eb6b15ecf4e18871624810f771810
