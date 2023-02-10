FROM ubuntu:20.04

#set DEBIAN_FRONTEND=noninteractive to ignore user prompt
ENV DEBIAN_FRONTEND=noninteractive

# installing basic dependecies
RUN apt-get -y update \
    && apt-get -y install wget \
                          curl \
    && rm -r /var/lib/apt/lists/*

#installing latest cmake
RUN wget https://github.com/Kitware/CMake/releases/download/v3.22.3/cmake-3.22.3-linux-x86_64.sh \
      -q -O /tmp/cmake-install.sh \
      && chmod u+x /tmp/cmake-install.sh \
      && mkdir /usr/bin/cmake \
      && /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
      && rm /tmp/cmake-install.sh

ENV PATH="/usr/bin/cmake/bin:${PATH}"

#installing dependicies
RUN apt-get -y update \
    && apt-get -y install uuid-dev \
                          zlib1g-dev \
                          libpulse-dev \
                          gcc \
                          gcc-multilib  \
                          g++ \
                          g++-multilib \
                          build-essential \
                          valgrind \
                          libboost-all-dev \
                          libsasl2-dev \
                          lcov \
                          git \
                          unixodbc-dev \
                          gdb \
                          curl \
                          zip \
                          unzip \
                          tar \
    && rm -r /var/lib/apt/lists/*

#install aws-sdk-dependencies
RUN git clone https://github.com/Microsoft/vcpkg.git \
    && cd vcpkg \
    && git checkout 2022.09.27 \
    && ./bootstrap-vcpkg.sh \
    && ./vcpkg install "aws-sdk-cpp[core,sts,timestream-query,timestream-write]" --recurse

ENV REPOSITORY_ROOT /timestream-odbc/
ENV VCPKG_ROOT /timestream-odbc/vcpkg
ENV ODBC_LIB_PATH /timestream-odbc/build/odbc/lib/

WORKDIR /timestream-odbc
