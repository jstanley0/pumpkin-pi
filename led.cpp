// GPIO  9 = red
// GPIO 10 = blue
// GPIO 11 = green
// TODO FIXME hack this to use one DMA channel

#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "pwm/pwm.h"
}

#include "led.h"

LED::LED()
{
    set_loglevel(LOG_LEVEL_ERRORS);
    setup(PULSE_WIDTH_INCREMENT_GRANULARITY_US_DEFAULT, DELAY_VIA_PCM);
    allocate_dma_channels();
    init_channel(m_dma_channels[0], 5120);
    init_channel(m_dma_channels[1], 5120);
    init_channel(m_dma_channels[2], 5120);
}

LED::~LED()
{
    shutdown();
}

void LED::allocate_dma_channels()
{
    FILE *s = popen("cat /sys/module/dma/parameters/dmachans", "r");
    if (s) {
        int bitmask = -1;
        fscanf(s, "%d", &bitmask);
        pclose(s);
        if (bitmask == -1) {
            fprintf(stderr, "LED: failed to read list of DMA channels\n");
            exit(1);
        }
        int channels = 0;
        for(int ch = 14; ch >= 0; --ch) {
            if (0 == (bitmask & (1 << ch))) {
                m_dma_channels[channels++] = ch;
                if (channels == 3)
                    break;
            }
        }
        if (channels < 3) {
            fprintf(stderr, "LED: insufficient available DMA channels");
            exit(1);
        }
    } else {
        perror("LED: failed to get available DMA channels");
        exit(1);
    }
}

void LED::set_color(int r, int g, int b)
{
    add_channel_pulse(m_dma_channels[0],  9, 0, r);
    add_channel_pulse(m_dma_channels[1], 10, 0, g);
    add_channel_pulse(m_dma_channels[2], 11, 0, b);
}
