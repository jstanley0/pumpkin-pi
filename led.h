#pragma once

class LED
{
    int m_pi;
    void init_pin(int gpio);
    
public:
    LED();
    ~LED();

    // brightness range is 0..511, because reasons.
    // 9 bits are better than 8, right?
    void set_color(int r, int g, int b);
    void set_color(int rL, int gL, int bL, int rR, int gR, int bR);
};
