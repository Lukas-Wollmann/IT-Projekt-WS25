FROM alpine:3.23

RUN apk add --no-cache make llvm llvm-dev clang gdb git clang-extra-tools

WORKDIR /workspace
