CFLAGS := -g
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs) -lSDL2_mixer

all: pumpkin-pi

pumpkin-pi: main.o led.o sound.o error.o
	g++ $(SDL_LDFLAGS) -pthread -lrt -lpigpiod_if2 main.o led.o sound.o error.o -o pumpkin-pi
    
main.o: main.cpp sound.h led.h
	g++ $(CFLAGS) $(SDL_CFLAGS) -c main.cpp

led.o: led.cpp led.h
	g++ $(CFLAGS) -c led.cpp

sound.o: sound.cpp sound.h error.h
	g++ $(CFLAGS) $(SDL_CFLAGS) -c sound.cpp

error.o: error.cpp error.h
	g++ $(CFLAGS) $(SDL_CFLAGS) -c error.cpp

clean:
	rm -f *.o pumpkin-pi
    
