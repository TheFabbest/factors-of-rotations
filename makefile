all: build

build:
	g++ -o factors_of_rotations src/main.cpp

build_optimized:
	g++ -O3 -flto=auto -o factors_of_rotations src/main.cpp

clean:
	rm -f factors_of_rotations