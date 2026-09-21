CXX ?= g++
MPICXX ?= mpicxx
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -Wpedantic -Iinclude
OMP_FLAGS := -fopenmp -DPRISMFORGE_USE_OPENMP
MPI_FLAGS := -DPRISMFORGE_USE_MPI
SOURCES := src/main.cpp src/scene.cpp src/tracer.cpp src/parallel.cpp
CORE_SOURCES := src/scene.cpp src/tracer.cpp

.PHONY: all local mpi test rtl-verilog clean

all: local

build:
	mkdir -p build

local: build
	$(CXX) $(CXXFLAGS) $(OMP_FLAGS) $(SOURCES) -o build/prismforge

mpi: build
	$(MPICXX) $(CXXFLAGS) $(OMP_FLAGS) $(MPI_FLAGS) $(SOURCES) -o build/prismforge-mpi

test: build
	$(CXX) $(CXXFLAGS) $(OMP_FLAGS) tests/core_tests.cpp $(CORE_SOURCES) -o build/core_tests
	./build/core_tests

rtl-verilog: build
	iverilog -g2012 -Wall -o build/ray_pipeline_tb \
		rtl/verilog/ray_sphere_discriminant.v \
		rtl/systemverilog/ray_packet_pipeline.sv \
		rtl/systemverilog/tb_ray_pipeline.sv
	vvp build/ray_pipeline_tb +VECTORS=rtl/test_vectors.txt

clean:
	rm -rf build
