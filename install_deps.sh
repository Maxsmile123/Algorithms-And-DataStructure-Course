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
        clang++-12 \
        clang-format \
        clang-tidy \
        libstdc++-12-dev
        gdb \
        python3 \
        python3-pip \
        python3-venv \
        vim \
        autoconf

pip3 install \
        click \
        gitpython \
        termcolor \
        virtualenv
