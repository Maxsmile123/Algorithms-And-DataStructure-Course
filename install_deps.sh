#!/bin/sh

set -e -x

echo "Install deps"

sudo apt-get update

sudo apt-get install -y \
        wget \
        rsync \
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
