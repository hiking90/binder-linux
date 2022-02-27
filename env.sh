#!/bin/sh

function gettop
{
    local TOPFILE=servicemanager/Access.cpp
    if [ -n "$TOP" -a -f "$TOP/$TOPFILE" ] ; then
        # The following circumlocution ensures we remove symlinks from TOP.
        (cd "$TOP"; PWD= /bin/pwd)
    else
        if [ -f $TOPFILE ] ; then
            # The following circumlocution (repeated below as well) ensures
            # that we record the true directory name and not one that is
            # faked up with symlink names.
            PWD= /bin/pwd
        else
            local HERE=$PWD
            local T=
            while [ \( ! \( -f $TOPFILE \) \) -a \( "$PWD" != "/" \) ]; do
                \cd ..
                T=`PWD= /bin/pwd -P`
            done
            \cd "$HERE"
            if [ -f "$T/$TOPFILE" ]; then
                echo "$T"
            fi
        fi
    fi
}

function croot()
{
    local T=$(gettop)
    if [ "$T" ]; then
        if [ "$1" ]; then
            \cd $(gettop)/$1
        else
            \cd $(gettop)
        fi
    else
        echo "Couldn't locate the top of the tree.  Try setting TOP."
    fi
}

function ask_outdir() {
    read -p "Choose build output directory [out] > " DIR
    if [ -z "$DIR" ] ; then
        echo "set default"
        export BINDER_OUTDIR=$(gettop)/out
    else
        echo $DIR
        export BINDER_OUTDIR=$(eval echo $DIR)
    fi
}

function check_outdir() {
    if [ -z "$BINDER_OUTDIR" ] ; then
        ask_outdir
    fi
}

# export BINDER_OUTDIR=$HOME/binder-linux-out
export TOP=$(gettop)

function cout {
    check_outdir
    cd ${BINDER_OUTDIR}
}

function m {
    check_outdir
    echo "Build C/C++ binaries and libraries"
    if [ ! -f "${BINDER_OUTDIR}" ]; then
        mkdir -p ${BINDER_OUTDIR}
    fi

    cd ${BINDER_OUTDIR} && cmake -DCMAKE_INSTALL_PREFIX=$HOME/local ${TOP} -G Ninja && ninja -j `nproc`
    cd -
}

export ANDROID_DIR=$(gettop)/android

REPOSITORIES=(
    "https://android.googlesource.com/platform/frameworks/native"
    "https://android.googlesource.com/platform/system/libbase"
    "https://android.googlesource.com/platform/system/core"
    "https://android.googlesource.com/platform/system/logging"
    "https://android.googlesource.com/platform/system/unwinding"
    "https://android.googlesource.com/platform/system/tools/aidl"
    "https://android.googlesource.com/platform/system/tools/hidl"
    "https://android.googlesource.com/platform/system/libprocinfo"
    "https://android.googlesource.com/platform/system/libvintf"
    "https://android.googlesource.com/platform/prebuilts/build-tools"
    "https://android.googlesource.com/platform/external/fmtlib"
    "https://android.googlesource.com/platform/external/googletest"
    # "https://android.googlesource.com/platform/external/selinux"
    "https://android.googlesource.com/platform/external/pcre"
    "https://android.googlesource.com/platform/external/tinyxml2"
    "https://android.googlesource.com/platform/packages/modules/Gki"
)

TAG="android-12.0.0_r32"

function android_clone() {
    local PUSH_DIR=`pwd`
    cd $(gettop)
    rm -rf ${ANDROID_DIR}
    mkdir -p ${ANDROID_DIR} && cd ${ANDROID_DIR}
    for R in ${REPOSITORIES[*]}; do
        # git clone ${GIT_OPTIONS} -b ${TAG} ${R}
        git clone -c advice.detachedHead=false --depth 1 -b ${TAG} ${R}
        BASENAME=`basename ${R}`
        PATCH="$(gettop)/patches/${BASENAME}.patch"
        if [ -f ${PATCH} ]; then
            echo "Apply ${PATCH}"
            cd ${BASENAME}
            git apply ${PATCH}
            cd -
        fi
    done
    cd ${PUSH_DIR}
}

function android_pull() {
    local PUSH_DIR=`pwd`
    cd ${ANDROID_DIR}
    for R in ${REPOSITORIES[*]}; do
        D=`basename ${R}`
        if cd ${D} ; then
            echo ${R}
            git stash push
            git pull ${GIT_OPTIONS} --rebase
            git stash pop
            echo
            cd -
        fi
    done
    cd ${PUSH_DIR}
}

check_outdir
echo "Out directory is [${BINDER_OUTDIR}]"