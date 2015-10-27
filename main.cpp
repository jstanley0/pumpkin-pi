#include <stdio.h>
#include <iostream>
#include <string>
#include "sound.h"
#include "led.h"

void callback(short max_intensity, void *context)
{
    /*
    int max = max_intensity / 128;
    for(int c = 0; c < max; ++c)
        putchar('*');
    putchar('\n');
    */
    int c = max_intensity / 128;    // 0 .. 255
    LED *led = (LED *)context;
    led->set_color(c, 255 - c, 127);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cerr << "Specify a sound file on the command line.\n";
        return 1;
    }
    const char *sound_file = argv[1];
    try
    {
        LED led;
        Sound sound;
        sound.set_callback(callback, &led);
        sound.load_file(sound_file);
        sound.play();
        return 0;
    }
    catch(std::string &error)
    {
        std::cerr << error << std::endl;
        return 1;
    }
}
