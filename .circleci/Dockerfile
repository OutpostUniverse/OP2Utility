# For building on CircleCI with Google Test

# Build from the root project folder with:
#   docker build .circleci/ --tag outpostuniverse/op2utility:1.2
# Push image to DockerHub with:
#   docker login
#   docker push outpostuniverse/op2utility:1.2

# Run locally using the CircleCI command line interface:
#   circleci build
# Validate .circleci/config.yml file with:
#   circleci config validate

FROM ubuntu:24.04

# Install Development tools, Google Test library, and tools for primary CircleCI container
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    build-essential \
    clang \
    cmake \
    libgtest-dev \
    libgmock-dev \
    git \
    ssh \
    tar \
    gzip \
    ca-certificates \
  && rm -rf /var/lib/apt/lists/*
