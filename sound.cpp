#include "sound.h"
#include "error.h"
#include "SDL.h"
#include "SDL_mixer.h"

extern volatile bool g_interrupted;

Sound::Sound()
{
    if (0 != SDL_Init(SDL_INIT_AUDIO))
        throw SDLError();

    if (0 != Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024))
        throw MixError();
}

Sound::~Sound()
{
    if (m_sound_file)
        Mix_FreeMusic(m_sound_file);
    Mix_CloseAudio();
}

void Sound::set_callback(callback cb, void *context)
{
    m_callback = cb;
    m_context = context;
}

/* static */ void Sound::postmix_proc(void *udata, Uint8 *stream, int len)
{
    short *samples = (short *)stream;
    int count = len / 2;
    short maxL = 0, maxR = 0;
    for(int i = 0; i < count; ++i) {
        short sample = samples[i];
        if (sample < 0)
            sample = -sample;
        short *max = ((i & 1) ? &maxL : &maxR);
        if (samples[i] > *max)
            *max = samples[i];
    }

    Sound *self = (Sound *)udata;
    self->m_callback(maxL, maxR, self->m_context);
}

void Sound::load_file(const char *filename)
{
    m_sound_file = Mix_LoadMUS(filename);
    if (!m_sound_file)
        throw MixError();
}

void Sound::play(char channel)
{
    Mix_SetPanning(MIX_CHANNEL_POST, channel == 'r' ? 0 : 255, channel == 'l' ? 0 : 255);
    Mix_SetPostMix(&Sound::postmix_proc, this);

    if (0 != Mix_PlayMusic(m_sound_file, 0))
        throw MixError();

    while(Mix_PlayingMusic() && !g_interrupted)
        SDL_Delay(20);

    if (g_interrupted)
        Mix_HaltMusic();

    Mix_SetPostMix(NULL, NULL);
}
