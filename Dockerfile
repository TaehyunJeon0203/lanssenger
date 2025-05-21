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

# 빌드 디렉토리 생성 및 빌드 실행
RUN mkdir -p build && cd build && cmake .. && cmake --build .

# 빌드된 실행 파일 복사
RUN cp /app/build/bin/server /app/
RUN cp /app/build/bin/client /app/

# 기본 명령어 설정
CMD ["/bin/bash"] 