# About
Goal of this project is to use Android Binder at Linux desktop environment.
The most of source code was come from Android Source Repositories. But, the source code was modified slightly for Linux dekstop.

## Prerequisites to build & run
1. Linux kernel's Binder must be enabled.
2. libselinux must be installed. http://userspace.selinuxproject.org/
3. CMake is used as a build system.

## Prepare
<pre>
$ cd binder-linux
$ source ./env.sh
$ android_clone
</pre>

## Build
<pre>
$ cd binder-linux
$ source ./env.sh
$ m
</pre>

## Testing
<pre>
$ cout                  # Change to output directory
$ ./add_binder /dev/binderfs/binder-control binder
$ chmod a+rw /dev/binderfs/binder
$ ./servicemanager /dev/binderfs/binder &
$ ./test_main server &
$ ./test_main
</pre>

## Aidl

## TODO
Remove unnecesary libraries from build.