#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "sound.h"
#include "led.h"
#include "SDL.h"
#include "error.h"

LED led;

int dark_color[3] = { 0, 0, 0 };
int bright_color[3] = { 255, 96, 0 };

volatile bool g_interrupted = false;
void sig_handler(int signo)
{
    g_interrupted = true;
}

// max intensity is 32767 (16-bit signed audio)
// the source colors (above) have an 8-bit range
// the mixed color elements have a 9-bit range (0..511)
void mix_colors(int dest_colors[3], int sound_intensity)
{
    for(int i = 0; i < 3; ++i)
    {
        int bright, dark;
        if (bright_color[i] >= dark_color[i]) {
            bright = bright_color[i];
            dark = dark_color[i];
        } else {
            bright = dark_color[i];
            dark = bright_color[i];
            sound_intensity = 32767 - sound_intensity;
        }
        dest_colors[i] = dark + (((bright - dark) * sound_intensity) / 16384);
    }
}

void sound_callback(short sound_intensity, void *context)
{
    int colors[3];
    mix_colors(colors, sound_intensity);
    led.set_color(colors[0], colors[1], colors[2]);
}

void parse_color(int *colors, const char *arg)
{
    int color;
    if (1 == sscanf(arg, "#%x", &color)) {
        colors[0] = color >> 16;
        colors[1] = (color >> 8) & 0xFF;
        colors[2] = color & 0xFF;
    } else {
        fprintf(stderr, "warning: failed to parse color %s\n", arg);
    }
}

int led_test()
{
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

int candle()
{
    int colors[3], intensity;
    srand((unsigned)time(NULL));

    int delta = 0;

    while(!g_interrupted)
    {
        delta += (rand() % 2048) - 1024;
        if (delta < -8192 || delta > 8192)
            delta = 0;
        intensity = 16384 + delta;
        mix_colors(colors, intensity);
        led.set_color(colors[0], colors[1], colors[2]);
        SDL_Delay(5);
    }

    return 0;
}

int main(int argc, char **argv)
{
    signal(SIGINT, sig_handler);

    if (argc < 2) {
        std::cerr <<
            "Usage:\n"
            "  --led_test\n"
            "  --candle [bright_color [dark_color]]\n"
            "  sound_file.mp3 [loud_color [silent_color]]\n"
            "    (where colors are given in #rrggbb format)";
        return 1;
    }
    if (argc >= 3)
        parse_color(bright_color, argv[2]);
    if (argc >= 4)
        parse_color(dark_color, argv[3]);

    if (0 == strcmp(argv[1], "--led-test")) {
        return led_test();
    } else if (0 == strcmp(argv[1], "--candle")) {
        return candle();
    }

    const char *sound_file = argv[1];
    try
    {
        Sound sound;
        sound.set_callback(sound_callback, NULL);
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
