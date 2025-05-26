FROM ubuntu:22.04

# 기본 패키지 설치
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# 작업 디렉토리 설정
WORKDIR /app

# 소스 코드 복사
COPY . .

# 빌드 디렉토리 생성 및 서버만 빌드
RUN mkdir -p build && cd build && \
    cmake -DBUILD_SERVER_ONLY=ON .. && \
    cmake --build . --target server

# 빌드된 서버 실행 파일 복사
RUN cp /app/build/server /app/

# 실행 권한 부여
RUN chmod +x /app/server

# 기본 명령어 설정
CMD ["./server"] 