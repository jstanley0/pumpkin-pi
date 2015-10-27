CFLAGS := -O2
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs) -lSDL2_mixer

all:
	g++ $(CFLAGS) $(SDL_CFLAGS) $(SDL_LDFLAGS) main.cpp led.cpp sound.cpp error.cpp -o pumpkin-pi

.PHONY: all
