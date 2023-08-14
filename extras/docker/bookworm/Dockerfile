FROM debian:12-slim AS builder

ARG MONGOCXX_VERSION
ARG MONGOC_VERSION=1.24.2
ARG MONGOCRYPT_VERSION=1.8.1

WORKDIR /build

RUN apt update && apt install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    python3 \
    wget

RUN mkdir /install

RUN wget https://github.com/mongodb/libmongocrypt/archive/refs/tags/${MONGOCRYPT_VERSION}/libmongocrypt-${MONGOCRYPT_VERSION}.tar.gz -O libmongocrypt-${MONGOCRYPT_VERSION}.tar.gz && \
    tar xf libmongocrypt-${MONGOCRYPT_VERSION}.tar.gz && \
    cd libmongocrypt-${MONGOCRYPT_VERSION} && \
    mkdir cmake-build && \
    cd cmake-build && \
    cmake .. -DBUILD_VERSION=${MONGOCRYPT_VERSION} -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/install && \
    make -j $(nproc) && \
    make install -j $(nproc)

RUN wget https://github.com/mongodb/mongo-c-driver/archive/refs/tags/${MONGOC_VERSION}/mongo-c-driver-${MONGOC_VERSION}.tar.gz -O mongo-c-driver-${MONGOC_VERSION}.tar.gz && \
    tar xzf mongo-c-driver-${MONGOC_VERSION}.tar.gz && \
    cd mongo-c-driver-${MONGOC_VERSION} && \
    mkdir cmake-build && \
    cd cmake-build && \
    cmake -DBUILD_VERSION=${MONGOC_VERSION} -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DMONGOC_ENABLE_CLIENT_SIDE_ENCRYPTION=ON -DCMAKE_INSTALL_PREFIX=/install -DCMAKE_PREFIX_PATH=/install .. && \
    make -j $(nproc) && \
    make install -j $(nproc)

RUN wget https://github.com/mongodb/mongo-cxx-driver/archive/refs/tags/r${MONGOCXX_VERSION}/mongo-cxx-driver-${MONGOCXX_VERSION}.tar.gz -O mongo-cxx-driver-${MONGOCXX_VERSION}.tar.gz && \
    tar xf mongo-cxx-driver-${MONGOCXX_VERSION}.tar.gz && \
    cd mongo-cxx-driver-r${MONGOCXX_VERSION}/build && \
    cmake .. -DBUILD_VERSION=${MONGOCXX_VERSION} -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_PREFIX=/install -DCMAKE_PREFIX_PATH=/install && \
    make -j $(nproc) && \
    make install -j $(nproc)

FROM debian:12-slim

RUN apt update && \
    apt install -y libssl3 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=builder /install /usr/local

RUN ldconfig
