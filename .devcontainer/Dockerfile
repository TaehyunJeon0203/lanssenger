FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
    build-essential \
    cmake \
    gdb \
    git \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# vscode 사용자 생성 (Dev Container에서 필요)
RUN useradd -m vscode
USER vscode
WORKDIR /workspace
