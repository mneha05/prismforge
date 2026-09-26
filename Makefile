CXX ?= g++
MPICXX ?= mpicxx
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -Wpedantic -Iinclude
OMP_FLAGS := -fopenmp -DPRISMFORGE_USE_OPENMP
MPI_FLAGS := -DPRISMFORGE_USE_MPI
SOURCES := src/main.cpp src/scene.cpp src/tracer.cpp src/parallel.cpp
CORE_SOURCES := src/scene.cpp src/tracer.cpp

.PHONY: all local mpi test generate rtl-verilog rtl-vhdl rtl-verilator rtl-all ci clean

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

generate: build
	perl tools/gen_scene.pl --scene build/generated.scene \
		--vectors build/generated_vectors.txt --count 128

rtl-verilog: generate
	iverilog -g2012 -Wall -o build/ray_pipeline_tb \
		rtl/verilog/ray_sphere_discriminant.v \
		rtl/systemverilog/ray_packet_pipeline.sv \
		rtl/systemverilog/tb_ray_pipeline.sv
	vvp build/ray_pipeline_tb +VECTORS=build/generated_vectors.txt

rtl-vhdl: generate
	ghdl -a --std=08 --workdir=build rtl/vhdl/ray_sphere_discriminant.vhd
	ghdl -a --std=08 --workdir=build rtl/vhdl/tb_ray_sphere_discriminant.vhd
	ghdl -e --std=08 --workdir=build -o build/vhdl_tb tb_ray_sphere_discriminant
	ghdl -r --std=08 --workdir=build tb_ray_sphere_discriminant \
		-gVECTOR_PATH=build/generated_vectors.txt --assert-level=error

rtl-verilator: generate
	verilator -Wall --cc --exe --build --top-module ray_packet_pipeline \
		rtl/verilog/ray_sphere_discriminant.v \
		rtl/systemverilog/ray_packet_pipeline.sv \
		rtl/verilator/ray_pipeline_main.cpp \
		-Mdir build/obj_dir -o ray_pipeline_verilated
	./build/obj_dir/ray_pipeline_verilated build/generated_vectors.txt

rtl-all: rtl-verilog rtl-vhdl rtl-verilator

ci: test mpi rtl-all

clean:
	rm -rf build
