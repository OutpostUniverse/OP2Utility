# For building on CircleCI with Google Test

# Build from the root project folder with:
#   docker build .circleci/ --tag outpostuniverse/circleci-gcc-clang-googletest
# Push image to DockerHub with:
#   docker login
#   docker push outpostuniverse/circleci-gcc-clang-googletest

# Run locally using the CircleCI command line interface:
#   circleci build
# Validate .circleci/config.yml file with:
#   circleci config validate

FROM ubuntu:20.04

# Install Development tools, Google Test library, and tools for primary CircleCI container
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    build-essential \
    clang \
    cmake \
    libgtest-dev \
    git \
    ssh \
    tar \
    gzip \
    ca-certificates \
  && rm -rf /var/lib/apt/lists/*

# Compile and install Google Test
ARG gtestBuildDir=/tmp/gtest/
RUN mkdir -p ${gtestBuildDir} && \
  cd ${gtestBuildDir} && \
  cmake -DCMAKE_CXX_FLAGS="-std=c++17" /usr/src/gtest/ && \
  make -C ${gtestBuildDir} && \
  cp ${gtestBuildDir}lib*.a /usr/lib && \
  rm -rf ${gtestBuildDir}
