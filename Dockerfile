############################################################
# Build Stage
############################################################

FROM ubuntu:24.04 AS builder
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake ninja-build git \
    pkg-config protobuf-compiler libprotobuf-dev \
    libgrpc++-dev libgrpc-dev \
    #libsoci-dev \
    #libsoci-postgresql-dev \
    #libsoci-postgresql-dev \
    #libsoci-sqlite3-dev \
    librdkafka-dev \
    libssl-dev \
    zlib1g-dev \
    curl wget uuid-dev \
    libjsoncpp-dev  \
    sqlite3 \
    libsqlite3-dev \
    default-libmysqlclient-dev \
    libpq-dev \
    ca-certificates

#
# Google Cloud SDK (placeholder)
#

# RUN ...

#
# libtorrent (optional later)
#

# RUN apt install libtorrent-rasterbar-dev

WORKDIR /app

# Build soci from source and install. Long story.
RUN git clone --recurse-submodules https://github.com/SOCI/soci
WORKDIR /app/soci
RUN mkdir build
WORKDIR /app/soci/build
RUN cmake .. \
    -DSOCI_CXX11=ON \
    -DSOCI_TESTS=OFF \
    -DWITH_BOOST=OFF \
    -DSOCI_MYSQL=ON  \
    -DSOCI_POSTGRESQL=ON  \
    -DSOCI_SQLITE3=ON
RUN cmake --build . -j$(nproc)
RUN make install
RUN ldconfig


# Building own repo
WORKDIR /app/logos
RUN mkdir -p build
COPY . .
RUN ./manage.sh generate-cpp   
# && ./manage.sh generate-py   // TODO: Python stuff needs work

WORKDIR /app/logos/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Debug
RUN cmake --build . -j$(nproc)

EXPOSE 50051

WORKDIR /app/logos/build/code/service
ENTRYPOINT ["./logos-service", "-c", "./docker.settings.json"]