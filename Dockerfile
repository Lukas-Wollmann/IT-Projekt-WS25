FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y \
    cmake zlib1g-dev libzstd-dev ninja-build make llvm llvm-dev clang clang-format gdb git \
    && rm -rf /var/lib/apt/lists/*

ARG UID=1001
ARG GID=1001

RUN groupadd -g ${GID} ocean \
    && useradd -u ${UID} -g ${GID} -m -s /bin/bash ocean \
    && mkdir -p  /workspace \
    && chown -R ocean:ocean /workspace

USER ocean

WORKDIR /workspace