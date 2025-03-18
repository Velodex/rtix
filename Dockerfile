FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

# Setup basic build environment
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    software-properties-common \ 
    python3-pip \
    git \
    && rm -rf /var/lib/apt/lists/*

# C++ core package dependencies
RUN apt-get update && \
    apt-get install -y \
    cmake \
    libnng-dev \
    libprotobuf-dev \
    libspdlog-dev \
    libyaml-cpp-dev \
    protobuf-compiler \
    && rm -rf /var/lib/apt/lists/*

# Additional build and test dependencies
RUN apt-get update && \
    apt-get install -y \
    lcov \
    gcovr \
    libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

# Build gtest
RUN cd /usr/src/gtest/ && \
    cmake . && \
    make && \
    cp lib/*.a /usr/lib

# Python Library dependencies
COPY requirements.txt .
RUN python3 -m pip install -r requirements.txt

# Add rtix to pythonpath
ENV PYTHONPATH=/rtix:$PYTHONPATH

# Set working directory
RUN mkdir /rtix
WORKDIR "/rtix"
