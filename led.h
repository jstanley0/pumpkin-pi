#pragma once

class LED
{
public:
    LED();
    ~LED();

    // brightness range is 0..511, because reasons.
    // 9 bits are better than 8, right? 
    void set_color(int r, int g, int b);

private:
    void allocate_dma_channel();
    int adjust_pwm(int gpio_mask, int from, int to);
    int m_ch, m_r, m_g, m_b;
};
