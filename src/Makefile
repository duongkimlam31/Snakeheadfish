# Compiler and compilation flags
CXX = g++
CXXFLAGS = -std=c++20 -g

# Build rule for the target
all: main.cc nnue.cpp misc.cpp
	$(CXX) $(CXXFLAGS) -pthread -Ofast -g -o main main.cc nnue.cpp misc.cpp

start:
	./main

.PHONY: clean

clean:
	rm -f build/main
