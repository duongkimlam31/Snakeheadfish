# Compiler and compilation flags
CXX = g++
CXXFLAGS = -std=c++14 -g

# Build rule for the target
chess: src/main.cc
	$(CXX) $(CXXFLAGS) -o chess src/main.cc

.PHONY: clean

clean:
	rm -f chess
