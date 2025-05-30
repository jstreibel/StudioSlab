################################################################################
# Builder stage with CUDA
################################################################################
# pick the devel image that matches compatible  CUDA version (12.1, in our case)
#FROM nvidia/cuda:12.8.1-devel-ubuntu24.04 AS builder
FROM jstreibel/studioslab-base AS builder

WORKDIR /opt/StudioSlab

# copy, configure, build & test
COPY CMakeLists.txt .
COPY Lib ./Lib
COPY Studios ./Studios
RUN mkdir Build
WORKDIR /opt/StudioSlab/Build
RUN cmake -DCMAKE_BUILD_TYPE=Release ..
RUN make -j$(nproc)
RUN ctest --output-on-failure

################################################################################
# Runtime stage with CUDA runtime
################################################################################
FROM jstreibel/studioslab-base-deploy AS runtime

COPY --from=builder /opt/StudioSlab/Build/bin/* /usr/local/bin/

USER nobody:nogroup
ENTRYPOINT ["/usr/local/bin/Fields-RtoR"]
CMD ["--help"]
