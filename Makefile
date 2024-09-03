debug: main.cpp
	g++ main.cpp -o debug -lraylib -std=c++20 -g

release: main.cpp
	g++ main.cpp -o release -lraylib -std=c++20 -O2

all: debug release
