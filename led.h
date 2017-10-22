#pragma once

class LED
{
    int m_pi;
    void init_pin(int gpio);
    
public:
    LED();
    ~LED();

    // brightness range is 0..511, because (historical) reasons.
    // 9 bits are better than 8, right?
    void set_color(int r, int g, int b);
    void set_color(int l[3], int r[3]);
};
