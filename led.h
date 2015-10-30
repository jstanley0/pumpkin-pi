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
    int m_dma_channels[3];
    void allocate_dma_channels();
};
