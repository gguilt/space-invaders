OUT = Space_Invaders
CXX = g++
SDL = -lSDL2 -lSDL2_image -lSDL2_ttf

OBJECTS = main.o

all: $(OUT)
$(OUT): $(OBJECTS)
	$(CXX) -o $@ $^ ${SDL}

