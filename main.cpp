#include <stdio.h>
#include <iostream>
#include <string.h>
#include "sound.h"
#include "led.h"
#include "SDL.h"
#include "error.h"

void callback(short max_intensity, void *context)
{
    /*
    int max = max_intensity / 128;
    for(int c = 0; c < max; ++c)
        putchar('*');
    putchar('\n');
    */
    int c = max_intensity >> 6;    // 0 .. 511
    LED *led = (LED *)context;
    led->set_color(c, c / 2, 0);
}

int main(int argc, char **argv)
{
    LED led;
    if (argc < 2) {
        std::cerr << "Specify a sound file on the command line.\n";
        return 1;
    }
    if (0 == strcmp(argv[1], "--led-test")) {
        led.set_color(63, 63, 63);
        SDL_Delay(1000);
        led.set_color(0, 0, 0);
        SDL_Delay(100);
        led.set_color(511, 511, 511);
        SDL_Delay(1000);
        led.set_color(0, 0, 0);
        SDL_Delay(100);
        for(int x = 0; x < 512; ++x) {
            led.set_color(x, x, x);
            SDL_Delay(5);
        }
        for(int x = 511; x >= 0; --x) {
            led.set_color(x, x, x);
            SDL_Delay(5);
        }
        return 0;
    }

    const char *sound_file = argv[1];
    try
    {
        Sound sound;
        sound.set_callback(callback, &led);
        sound.load_file(sound_file);
        sound.play();
        return 0;
    }
    catch(SDLError &error)
    {
        std::cerr << error.what() << std::endl;
        return 1;
    }
}
