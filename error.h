#pragma once

class SDLError
{
public:
    SDLError() {}
    virtual const char *what();
};

class MixError : public SDLError
{
public:
    MixError() {}
    virtual const char *what();
};
