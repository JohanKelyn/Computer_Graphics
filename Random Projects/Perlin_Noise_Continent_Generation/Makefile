PROJECT := build/app
CPP_FILES := src/main.cpp src/terrainCell.cpp
O_FILES := build/main.o build/terrainCell.o
COMPILED_FLAGS := -std=c++17 -Wall -O3
LINKED_FLAGS := -s

INCLUDE_DIRECTORIES := -Iinclude -I./../Include -I./../Include/eigen -I./../Include/stb 

LIB_FILES := $(wildcard ./../lib/*.a)

all: $(PROJECT)
$(PROJECT): $(O_FILES)
	g++ $^ -o $@ $(LIB_FILES)
build/%.o: src/%.cpp
	g++ -c $^ -o $@ $(INCLUDE_DIRECTORIES) $(COMPILED_FLAGS)
clean:
	del build\*.o build\*.exe /s