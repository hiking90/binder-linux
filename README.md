# About
Goal of this project is to use Android Binder at Linux desktop environment.
The most of source code was come from Android Source Repositories(Android 12). But, the source code was modified slightly for Linux dekstop.

## Prerequisites to build & run
1. Linux kernel's Binder must be enabled. https://www.kernel.org/doc/html/latest/admin-guide/binderfs.html
2. libselinux must be installed. http://userspace.selinuxproject.org/
3. 'libc++' and 'fmt' must be installed too.
4. CMake is used as a build system.

## Prepare
<pre>
$ cd binder-linux
$ source ./env.sh
$ android_clone
</pre>

## Build
<pre>
$ m
</pre>

## Testing
Change output directory.
<pre>
$ cout
</pre>

Create binder device file
<pre>
$ ./binder_device /dev/binderfs/binder-control binder
$ chmod a+rw /dev/binderfs/binder
</pre>

Run servicemanager
<pre>
$ ./binder_sm /dev/binderfs/binder &
</pre>

Run echo service
<pre>
$ ./binder_test server &
</pre>

Run echo client
<pre>
$ ./binder_test
</pre>

## Install
<pre>
$ ninja install
</pre>

## Aidl

## TODO
Remove unnecesary libraries from build.
