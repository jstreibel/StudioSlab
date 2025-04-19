################################################################################
# Builder stage with CUDA
################################################################################
# pick the devel image that matches compatible  CUDA version (12.1, in our case)
FROM nvidia/cuda:12.1.0-devel-ubuntu22.04 AS builder

WORKDIR /opt/StudioSlab

# install all dev‐deps (thanks JohnW)
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
      build-essential cmake git \
      libsfml-dev \
      libgl1-mesa-dev \
      libegl1-mesa-dev \
      libfreetype6-dev \
      libglew-dev \
      libfontconfig1-dev \
      libcairomm-1.0-dev \
      libpangomm-1.4-dev \
      libboost-locale-dev \
      libboost-random-dev \
      libboost-timer-dev \
      freeglut3-dev \
      libglfw3-dev \
      libcxxopts-dev \
      libfftw3-dev  \
      libtinyxml2-dev  \
      libglm-dev \
      libfreeimageplus-dev \
    && rm -rf /var/lib/apt/lists/*

# copy, configure, build & test
COPY CMakeLists.txt .
RUN mkdir Build && cd Build && cmake .. && cd ..
COPY . .
RUN cd Build && cmake -DCMAKE_BUILD_TYPE=Release .. \
             && make -j$(nproc) \
             && ctest --output-on-failure

################################################################################
# Runtime stage with CUDA runtime
################################################################################
FROM nvidia/cuda:12.1.0-runtime-ubuntu22.04 AS runtime

WORKDIR /app

# only the shared‐libs you need (same list as before, but non‑dev)
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
      libsfml2.5  \
      libgl1-mesa-glx \
      libegl1-mesa \
      libfreetype6 \
      libglew2.2  \
      libfontconfig1 \
      libcairomm-1.0-1  \
      libpangomm-1.4-1 \
      libboost-locale1.74.0 \
      libboost-random1.74.0 \
      libboost-timer1.74.0 \
      freeglut3  \
      libglfw3  \
      libfftw3-3  \
      libtinyxml2-6 \
      libglm0  \
      libfreeimageplus3 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /opt/StudioSlab/Build/bin/* /usr/local/bin/

USER joao:joao
# ENTRYPOINT ["/usr/local/bin/StudioSlab"]
CMD ["--help"]
