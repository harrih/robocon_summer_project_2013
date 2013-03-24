# Easier use of compiler
CC=g++
WXFLAGS=`wx-config --cxxflags` `wx-config --libs`

all:
	$(CC) main.cpp -o main.out $(WXFLAGS)

clean:
	rm -rf *o hello

run:
	./main.out
