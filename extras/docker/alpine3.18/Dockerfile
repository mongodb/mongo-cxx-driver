FROM alpine:3.18 AS builder

ARG MONGOCXX_VERSION
ARG MONGOC_VERSION=1.24.2
ARG MONGOCRYPT_VERSION=r1.8

WORKDIR /build

RUN apk add --no-cache \
    alpine-sdk \
    cmake \
    openssl-dev \
    python3

RUN mkdir /install

RUN git clone https://github.com/mongodb/libmongocrypt.git --branch ${MONGOCRYPT_VERSION} && \
    cd libmongocrypt && \
    mkdir cmake-build && \
    cd cmake-build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/install && \
    make -j $(nproc) && \
    make install -j $(nproc)

RUN wget https://github.com/mongodb/mongo-c-driver/releases/download/${MONGOC_VERSION}/mongo-c-driver-${MONGOC_VERSION}.tar.gz && \
    tar xzf mongo-c-driver-${MONGOC_VERSION}.tar.gz && \
    cd mongo-c-driver-${MONGOC_VERSION} && \
    mkdir cmake-build && \
    cd cmake-build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DMONGOC_ENABLE_CLIENT_SIDE_ENCRYPTION=ON -DCMAKE_INSTALL_PREFIX=/install -DCMAKE_PREFIX_PATH=/install .. && \
    make -j $(nproc) && \
    make install -j $(nproc)

RUN wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r${MONGOCXX_VERSION}/mongo-cxx-driver-r${MONGOCXX_VERSION}.tar.gz && \
    tar xf mongo-cxx-driver-r${MONGOCXX_VERSION}.tar.gz && \
    cd mongo-cxx-driver-r${MONGOCXX_VERSION}/build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_INSTALL_PREFIX=/install -DCMAKE_PREFIX_PATH=/install && \
    make -j $(nproc) && \
    make install -j $(nproc)

FROM alpine:3.18

RUN apk add --no-cache \
    openssl3 \
    libstdc++ \
    libc6-compat

COPY --from=builder /install /usr/local
