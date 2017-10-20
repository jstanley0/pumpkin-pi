#pragma once

struct _Mix_Music;

class Sound
{
public:
    Sound();
    ~Sound();
    
    typedef void (*callback)(short max_left, short max_right, void *context);
    
    void set_callback(callback cb, void *context);
    void load_file(const char *filename);
    void play();
    
private:
    _Mix_Music *m_sound_file;
    
    callback m_callback;
    void *m_context;
    
    static void effect_proc(int chan, void *stream, int len, void *udata);
};
