################################################################################
# Builder stage with CUDA
################################################################################
# pick the devel image that matches compatible  CUDA version (12.1, in our case)
FROM nvidia/cuda:12.8.1-devel-ubuntu24.04 AS builder

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
RUN cmake -DCMAKE_BUILD_TYPE=Release ..
RUN make -j$(nproc)
RUN ctest --output-on-failure

################################################################################
# Runtime stage with CUDA runtime
################################################################################
FROM nvidia/cuda:12.8.1-devel-ubuntu24.04 AS runtime

WORKDIR /app

# shared‐libs only, need you (same list as before, but non‑dev)
COPY Scripts/install-deps.sh /install-deps.sh
RUN chmod +x /install-deps.sh \
    && /install-deps.sh --deploy

COPY --from=builder /opt/StudioSlab/Build/bin/* /usr/local/bin/

USER nobody:nogroup
ENTRYPOINT ["/usr/local/bin/FieldsRtoR"]
CMD ["--help"]
