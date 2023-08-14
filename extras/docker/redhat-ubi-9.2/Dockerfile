FROM registry.access.redhat.com/ubi9/ubi-minimal:9.2 AS builder

ARG MONGOCXX_VERSION
ARG MONGOC_VERSION=1.24.2
ARG MONGOCRYPT_VERSION=1.8.1

WORKDIR /build

RUN microdnf install -y \
    gcc \
    g++ \
    git \
    gzip \
    tar \
    wget \
    cmake \
    openssl-devel \
    python3

RUN mkdir /install

RUN git clone https://github.com/mongodb/libmongocrypt.git --branch ${MONGOCRYPT_VERSION} && \
    cd libmongocrypt && \
    mkdir cmake-build && \
    cd cmake-build && \
    cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/install && \
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

RUN wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r${MONGOCXX_VERSION}/mongo-cxx-driver-r${MONGOCXX_VERSION}.tar.gz && \
    tar xf mongo-cxx-driver-r${MONGOCXX_VERSION}.tar.gz && \
    cd mongo-cxx-driver-r${MONGOCXX_VERSION}/build && \
    cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_PREFIX=/install -DCMAKE_PREFIX_PATH=/install && \
    make -j $(nproc) && \
    make install -j $(nproc)

FROM registry.access.redhat.com/ubi9/ubi-minimal:9.2

COPY --from=builder /install /usr/local

RUN ldconfig

ENV LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/lib64/"
