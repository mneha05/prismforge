FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential openmpi-bin libopenmpi-dev iverilog ghdl perl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY . .
RUN make ci

RUN mkdir -p /output
VOLUME ["/output"]

CMD ["mpirun", "--allow-run-as-root", "--oversubscribe", "-np", "2", "./build/prismforge-mpi", "--width", "960", "--height", "540", "--samples", "4", "--threads", "2", "--scene", "scenes/neon.scene", "--output", "/output/render.ppm"]
