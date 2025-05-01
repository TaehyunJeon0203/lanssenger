FROM ubuntu:24.04

RUN apt update && apt install -y \
    build-essential \
    cmake \
    gdb \
    clang-format \
    git \
    wget \
    curl \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
