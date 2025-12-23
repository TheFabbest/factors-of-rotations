all: build

build:
	g++ -O0 -o factors_of_rotations src/main.cpp

build_optimized:
	g++ -O3 -flto=auto -march=native -fno-rtti -o factors_of_rotations src/main.cpp

clean:
	rm -f factors_of_rotations