
FROM ubuntu:22.04 AS builder

# TODO: Install g++, cmake, make, git

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    git
# TODO: Install C++ libraries you'll need

RUN apt-get install -y \
    libboost-all-dev
# Set working directory
WORKDIR /app

COPY . /app

# Standard CMake workflow:
#   mkdir build
#   cd build
#   cmake ..
#   make -j$(nproc)
RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)

FROM ubuntu:22.04 AS runtime
# Install only runtime dependencies (not build tools)
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app


COPY --from=builder /app/build/vector-search-engine /app/

EXPOSE 8080

CMD ["./vector-search-engine"]