# Makefile for SFML project on Ubuntu

CXX = g++
CXXFLAGS = -std=c++17 -Wall
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRC = /home/iffatzahra/sfml-app/main.cpp
OBJ = main.o
TARGET = buzz-app

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LIBS)

$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $(SRC) -o $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)
