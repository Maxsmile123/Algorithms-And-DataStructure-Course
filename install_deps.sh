#!/bin/sh

set -e -x

echo "Install deps"

apt-get update

DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata

apt-get install -y software-properties-common

add-apt-repository -y ppa:ubuntu-toolchain-r/test

apt-get install -y wget rsync

# LLVM
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
add-apt-repository -y "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-14 main"

# CMake
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | apt-key add -
apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'

apt-get update

apt-get install -y \
        ssh \
        make \
        cmake \
        build-essential \
        ninja-build \
        git \
        linux-tools-common \
        linux-tools-generic \
        g++-12 \
        clang-14 \
        clang-format-14 \
        clang-tidy-14 \
        libclang-rt-14-dev \
        libc++-14-dev \
        libc++abi-14-dev \
        clangd-14 \
        lldb-14 \
        gdb \
        binutils-dev \
        libdwarf-dev \
        libdw-dev \
        python3 \
        python3-pip \
        python3-venv \
        ca-certificates \
        openssh-server \
        vim \
        autoconf

pip3 install \
        click \
        gitpython \
        python-gitlab \
        termcolor \
        virtualenv
