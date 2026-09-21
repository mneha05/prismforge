CXX ?= g++
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -Wpedantic -Iinclude
SOURCES := src/main.cpp src/scene.cpp src/tracer.cpp
CORE_SOURCES := src/scene.cpp src/tracer.cpp

.PHONY: all local test clean

all: local

build:
	mkdir -p build

local: build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o build/prismforge

test: build
	$(CXX) $(CXXFLAGS) tests/core_tests.cpp $(CORE_SOURCES) -o build/core_tests
	./build/core_tests

clean:
	rm -rf build
