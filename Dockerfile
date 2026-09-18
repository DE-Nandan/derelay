# Build stage
FROM ubuntu:22.04 as builder

WORKDIR /build

# Install build dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    g++ \
    git \
    && rm -rf /var/lib/apt/lists/*

# Copy project files
COPY . /build

# Build the project
RUN cmake . && make

# Runtime stage
FROM ubuntu:22.04

WORKDIR /app

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Copy built binaries from builder
COPY --from=builder /build/derelay_server /app/derelay_server
COPY --from=builder /build/derelay_client /app/derelay_client

# Expose UDP port for the server
EXPOSE 5000/udp

# Run the server
ENTRYPOINT ["/app/derelay_server"]
