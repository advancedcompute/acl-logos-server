# syntax=docker/dockerfile:1.5

############################################################
# Builder Stage
############################################################
FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake ninja-build git \
    pkg-config protobuf-compiler libprotobuf-dev \
    libgrpc++-dev libgrpc-dev \
    librdkafka-dev \
    libabsl-dev \
    libssl-dev \
    libfmt-dev \
    zlib1g-dev \
    curl wget uuid-dev \
    libjsoncpp-dev \
    sqlite3 libsqlite3-dev \
    default-libmysqlclient-dev \
    libpq-dev \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Build SOCI
RUN git clone --recurse-submodules https://github.com/SOCI/soci
WORKDIR /app/soci/build
RUN cmake .. \
    -DSOCI_CXX11=ON \
    -DSOCI_TESTS=OFF \
    -DWITH_BOOST=OFF \
    -DSOCI_MYSQL=ON \
    -DSOCI_POSTGRESQL=ON \
    -DSOCI_SQLITE3=ON

RUN cmake --build . -j$(nproc)
RUN make install && ldconfig

############################################################
# Application build
############################################################
WORKDIR /app/logos
COPY . .
RUN mkdir -p build .config/acl/logos
RUN rm -rf build/*

RUN ./manage.sh generate-cpp && ./manage.sh generate-py

WORKDIR /app/logos/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN cmake --build . -j$(nproc)
RUN ldconfig

############################################################
# Runtime Stage (minimal)
############################################################
#FROM ubuntu:24.04 AS runtime
#ENV DEBIAN_FRONTEND=noninteractive

# Only runtime dependencies
#RUN apt-get update && apt-get install -y \
#    libprotobuf-dev \
#    #libgrpc++1 \
#    libgrpc++1.51t64    \
#    librdkafka1 \
#    libssl3 \
#    libfmt9 \
#    zlib1g \
#    libjsoncpp25 \
#    libsqlite3-0 \
#    libmysqlclient21 \
#    libpq5 \
#    ca-certificates \
#    libabsl20220623t64  \
#    && rm -rf /var/lib/apt/lists/*

# Copy compiled SOCI
#COPY --from=builder /usr/local /usr/local

# Copy app binary
#WORKDIR /app
#COPY --from=builder /app/logos/build/dep/acl-libcpputils/code/lib/libcpputils.so    /app/libcpputils.so
#COPY --from=builder /app/logos/build/code/service/logos-service                     /app/logos-service
#COPY --from=builder /app/logos/etc/docker.settings.json                             /app/docker.settings.json

# Certs (temporary approach)
RUN --mount=type=secret,id=mysql_ca \
    --mount=type=secret,id=mysql_cert \
    --mount=type=secret,id=mysql_key \
    mkdir -p /etc/logos/certs && \
    cp /run/secrets/mysql_ca /etc/logos/certs/ca.crt && \
    cp /run/secrets/mysql_cert /etc/logos/certs/client.crt && \
    cp /run/secrets/mysql_key /etc/logos/certs/client.key && \
    chmod 600 /etc/logos/certs/*

WORKDIR /app/logos/build/code/service/

EXPOSE 50051
#ENTRYPOINT ["/bin/bash"]
ENTRYPOINT ["./logos-service", "-c", "./settings.json"]