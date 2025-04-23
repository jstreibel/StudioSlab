# Dockerfile.base
FROM nvidia/cuda:12.8.1-devel-ubuntu24.04

# Copy and run your install script
COPY ../Scripts/install-deps.sh /install-deps.sh
RUN chmod +x /install-deps.sh \
 && /install-deps.sh --dev \
 # clean up apt cache to keep image small
 && rm -rf /var/lib/apt/lists/*

CMD ["bash"]