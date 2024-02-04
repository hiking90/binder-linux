# About
Goal of this project is to use Android Binder at Linux desktop environment.
The most of source code was come from Android Source Repositories(Android 14). But, the source code was modified slightly for Linux dekstop.

## Prerequisites to build & run
1. Kernel 5.16.x is required because of the definition of BINDER_ENABLE_ONEWAY_SPAM_DETECTION.
1. Linux kernel's Binder must be enabled. https://www.kernel.org/doc/html/latest/admin-guide/binderfs.html
1. libselinux must be installed. http://userspace.selinuxproject.org/
1. 'libc++' and 'fmt' must be installed too.
1. CMake is used as a build system.

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

## Mounting binderfs
Please refer following two external documents.

https://www.kernel.org/doc/html/latest/admin-guide/binderfs.html#mounting-binderfs
https://wiki.archlinux.org/title/Anbox#Mounting_binderfs


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
binder-linux does not port AIDL compiler. Please use the AIDL compiler of Android build tools.
https://developer.android.com/studio/releases/build-tools

## TODO
Remove unnecesary libraries from build.
