FROM ubuntu:latest

ENV TZ=Europe/London
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

ARG GIT_REPO_USER=a
ARG GIT_REPO_NAME=DigitalTradingExchange
ARG ROOT_SRC_DIR=/app
ARG SSH_PRIVATE_KEY

RUN apt-get update && apt-get install -y                \
    build-essential autoconf cmake git pkg-config       \
    automake libtool curl make g++                      \
    libgrpc-dev libgrpc++-dev protobuf-compiler-grpc    \
    python3-grpcio python3-grpc-tools                   \
    librdkafka-dev librdkafka-dev libcpputest-dev       \
    libsoci-dev libsoci-mysql4.0 libspdlog-dev


WORKDIR ${ROOT_SRC_DIR}
COPY . .
RUN manage.sh generate-cpp && manage.sh generate-py
RUN mkdir build && cd build/ && cmake -j10 .. && cmake --build .

ENTRYPOINT ["/bin/bash"]
