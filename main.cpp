#include <stdio.h>
#include <iostream>
#include <queue>
#include <string>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "sound.h"
#include "led.h"
#include "SDL.h"
#include "SDL_thread.h"
#include "SDL_mutex.h"
#include "error.h"

LED led;

int dark_color[3] = { 0, 0, 0 };
int bright_color[3] = { 255, 63, 0 };
int silent_color[3] = { 0, 0, 0 };
int loud_color[3] = { 128, 224, 255 };

volatile bool g_interrupted = false;
void sig_handler(int signo)
{
    g_interrupted = true;
}

// max intensity is 32767 (16-bit signed audio)
// the source colors (above) have an 8-bit range
// the mixed color elements have a 9-bit range (0..511)
void mix_colors(int dest_colors[3], int low_color[3], int high_color[3], int sound_intensity)
{
    for(int i = 0; i < 3; ++i)
    {
        int bright, dark;
        if (high_color[i] >= low_color[i]) {
            bright = high_color[i];
            dark = low_color[i];
        } else {
            bright = low_color[i];
            dark = high_color[i];
            sound_intensity = 32767 - sound_intensity;
        }
        dest_colors[i] = dark + (((bright - dark) * sound_intensity) / 16384);
    }
}

void sound_callback(short sound_intensity, void *context)
{
    int colors[3];
    mix_colors(colors, silent_color, loud_color, sound_intensity);
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
        std::cerr << "warning: failed to parse color " << arg << std::endl;
    }
}

int led_test()
{
    led.set_color(511, 0, 0);
    SDL_Delay(1000);
    led.set_color(0, 0, 0);
    SDL_Delay(100);
    led.set_color(0, 511, 0);
    SDL_Delay(1000);
    led.set_color(0, 0, 0);
    SDL_Delay(100);
    led.set_color(0, 0, 511);
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

int candle(int duration = 0)
{
    int colors[3], intensity;
    srand((unsigned)time(NULL));

    int delta = 0;
    int cycles = 0;
    while(!g_interrupted)
    {
        delta += (rand() % 2048) - 1024;
        if (delta < -8192 || delta > 8192)
            delta = 0;
        intensity = 16384 + delta;
        mix_colors(colors, dark_color, bright_color, intensity);
        led.set_color(colors[0], colors[1], colors[2]);
        SDL_Delay(5);
        if (duration != 0 && ++cycles >= duration) break;
    }

    return 0;
}

bool g_candle_on = true;

SDL_mutex *mutex;
class ScopedMutexLock
{
public:
    ScopedMutexLock(SDL_mutex *mutex) : m_mutex(mutex)
    { SDL_LockMutex(m_mutex); }
    ~ScopedMutexLock() { SDL_UnlockMutex(m_mutex); }
private:
    SDL_mutex *m_mutex;
};

std::queue<std::string> command_queue;

void queue_command(const std::string &command)
{
    ScopedMutexLock lock(mutex);
    command_queue.push(command);
    g_interrupted = true;
}

bool dequeue_command(std::string &command)
{
    if (!g_interrupted) {
        // idle with candle
        if (g_candle_on) {
            candle();
        } else {
            while(!g_interrupted)
                SDL_Delay(50);
        }
    }

    {
        ScopedMutexLock lock(mutex);
        if (command_queue.empty())
            return false;
        command = command_queue.front();
        command_queue.pop();
        g_interrupted = !command_queue.empty();
    }

    return true;
}

volatile bool interactive_thread_exited = false;

static int interactive_thread(void *param)
{
    try
    {
        Sound sound;
        sound.set_callback(sound_callback, NULL);

        std::string line;
        while(dequeue_command(line))
        {
            std::string command, arg;
            std::stringstream ss(line);
            ss >> command;
            if (command == "candle")
            {
                ss >> arg;
                g_candle_on = (arg == "on");
            }
            else if (command == "color")
            {
                int *target = NULL;

                ss >> arg;
                if (arg == "dark")
                    target = dark_color;
                else if (arg == "bright")
                    target = bright_color;
                else if (arg == "silent")
                    target = silent_color;
                else if (arg == "loud")
                    target = loud_color;

                ss >> arg;
                parse_color(target, arg.c_str());
            }
            else if (command == "play")
            {
                try
                {
                    std::string filename;
                    while(ss.peek() == ' ')
                        ss.get();
                    getline(ss, filename);
                    sound.load_file(filename.c_str());
                    sound.play();
                }
                catch(SDLError &error)
                {
                    std::cerr << error.what() << std::endl;
                }
            }
        }
    }
    catch(SDLError &error)
    {
        std::cerr << error.what() << std::endl;
        interactive_thread_exited = true;
        return 1;
    }

    interactive_thread_exited = true;
    return 0;
}

// candle {on|off}
// color {dark|bright|silent|loud} #rrggbb
// play filename
// exit
int interactive()
{
    SDL_Thread *thread = SDL_CreateThread(interactive_thread, "interactive_thread", 0);   
    mutex = SDL_CreateMutex();

    std::string line;
    while(!interactive_thread_exited && std::getline(std::cin, line))
    {
        if (line == "exit") {
            g_interrupted = true;
            break;
        } else {
            queue_command(line);
        }
    }

    int ret;
    SDL_WaitThread(thread, &ret);
    SDL_DestroyMutex(mutex);
    return ret;
}

int main(int argc, char **argv)
{
    signal(SIGINT, sig_handler);

    if (argc < 2) {
        std::cerr <<
            "Usage:\n"
            "  --interactive\n"
            "  --led-test\n"
            "  --candle [bright_color [dark_color]]\n"
            "  sound_file.mp3 [loud_color [silent_color]]\n"
            "    (where colors are given in #rrggbb format)";
        return 1;
    }

    if (0 == strcmp(argv[1], "--interactive")) {
        return interactive();
    } else if (0 == strcmp(argv[1], "--led-test")) {
        return led_test();
    } else if (0 == strcmp(argv[1], "--candle")) {
        if (argc >= 3)
            parse_color(bright_color, argv[2]);
        if (argc >= 4)
            parse_color(dark_color, argv[3]);
        int duration = 0;
        if (argc >= 5)
            duration = atoi(argv[4]);
        std::cout << "candle duration is " << duration << std::endl;
        return candle(duration);
    }

    const char *sound_file = argv[1];
    if (argc >= 3)
        parse_color(loud_color, argv[2]);
    if (argc >= 4)
        parse_color(silent_color, argv[3]);

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
