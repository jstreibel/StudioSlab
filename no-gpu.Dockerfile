# syntax=docker/dockerfile:1

################################################################################
# Builder stage with CUDA
################################################################################
# pick the devel image that matches compatible  CUDA version (12.1, in our case)
FROM ubuntu:24.04 AS builder

WORKDIR /opt/StudioSlab

# install all dev‐deps (thanks JohnW)
# copy the script
COPY Scripts/install-deps.sh /install-deps.sh
RUN chmod +x /install-deps.sh \
 && /install-deps.sh --dev

# copy, configure, build & test
COPY CMakeLists.txt .
COPY Lib ./Lib
COPY Studios ./Studios
RUN mkdir Build
WORKDIR /opt/StudioSlab/Build
RUN cmake -DSTUDIOSLAB_CUDA_SUPPORT=false -DCMAKE_BUILD_TYPE=Release ..
RUN make -j$(nproc)
RUN ctest --output-on-failure

################################################################################
# Runtime stage with CUDA runtime
################################################################################
FROM ubuntu:24.04 AS runtime

WORKDIR /app

# shared‐libs only, need you (same list as before, but non‑dev)
COPY Scripts/install-deps.sh /install-deps.sh
RUN chmod +x /install-deps.sh \
    && /install-deps.sh --deploy

COPY --from=builder /opt/StudioSlab/Build/bin/* /usr/local/bin/

USER nobody:nogroup
ENTRYPOINT ["/usr/local/bin/Fields-RtoR"]
CMD ["--help"]
