#pragma once

class LED
{
    int m_pi;
    void init_pin(int gpio);
    
public:
    LED();
    ~LED();

    // brightness range is 0..255
    void set_color(int r, int g, int b);
    void set_color(int l[3], int r[3]);
};
