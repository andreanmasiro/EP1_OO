BINFOLDER := bin/
INCFOLDER := inc/
SRCFOLDER := src/
OBJFOLDER := obj/

CC := g++
CFLAGS := -Wall

SRCFILES := $(wildcard src/*.cpp)

all: $(SRCFILES:src/%.cpp=obj/%.o)
	$(CC) $(CFLAGS) obj/*.o -o bin/main  -std=c++11 -lssl -lcrypto

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -I./inc

.PHONY: clean
clean:
	rm -rf obj/*
	rm -rf bin/*

run:
	bin/main
