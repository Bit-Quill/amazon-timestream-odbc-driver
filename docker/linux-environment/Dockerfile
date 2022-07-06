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
    && apt-get -y install libcurl4-openssl-dev \
                          libssl-dev \
                          uuid-dev \
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
    && rm -r /var/lib/apt/lists/*

#installing dependecies for JDK
RUN apt-get -y update \
    && apt-get -y install software-properties-common \
    && rm -r /var/lib/apt/lists/*

#setup JDK
RUN  wget -O- https://apt.corretto.aws/corretto.key | apt-key add - \
     && apt-add-repository 'deb https://apt.corretto.aws stable main' \
     && apt-get update \
     && apt-get install -y java-17-amazon-corretto-jdk \
     && rm -r /var/lib/apt/lists/*

ENV JAVA_HOME /usr/lib/jvm/java-17-amazon-corretto/
ENV ODBC_LIB_PATH /documentdb-odbc/build/odbc/lib/

WORKDIR /documentdb-odbc
