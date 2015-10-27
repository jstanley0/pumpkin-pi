#include "error.h"
#include "SDL.h"
#include "SDL_mixer.h"

const char *SDLError::what()
{
    return SDL_GetError();
}

const char *MixError::what()
{
    return Mix_GetError();
}
