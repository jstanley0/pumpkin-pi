#include "sound.h"
#include "error.h"
#include "SDL.h"
#include "SDL_mixer.h"

Sound::Sound()
{
    if (0 != SDL_Init(SDL_INIT_AUDIO))
        throw SDLError();

    if (0 != Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024))
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
    Mix_UnregisterAllEffects(MIX_CHANNEL_POST);
    m_callback = cb;
    m_context = context;
    if (!Mix_RegisterEffect(MIX_CHANNEL_POST, &Sound::effect_proc, NULL, this))
        throw MixError();
}

/* static */ void Sound::effect_proc(int chan, void *stream, int len, void *udata)
{
    short *samples = (short *)stream;
    int count = len / 2;
    short max = 0;
    for(int i = 0; i < count; ++i) {
        short sample = samples[i];
        if (sample < 0)
            sample = -sample;
        if (samples[i] > max)
            max = samples[i];
    }

    Sound *self = (Sound *)udata;
    self->m_callback(max, self->m_context);
}

void Sound::load_file(const char *filename)
{
    m_sound_file = Mix_LoadMUS(filename);
    if (!m_sound_file)
        throw MixError();
}

void Sound::play()
{
    if (0 != Mix_PlayMusic(m_sound_file, 0))
        throw MixError();

    while(Mix_PlayingMusic())
        SDL_Delay(20);
}
