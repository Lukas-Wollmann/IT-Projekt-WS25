FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y \
    cmake zlib1g-dev libzstd-dev ninja-build make llvm llvm-dev clang clang-format gdb git valgrind \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace