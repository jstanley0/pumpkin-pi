// GPIO  9 = red
// GPIO 10 = blue
// GPIO 11 = green

#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "pwm/pwm.h"
}

#include "led.h"

void LED::allocate_dma_channel()
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
        for(int ch = 14; ch >= 0; --ch) {
            if (0 == (bitmask & (1 << ch))) {
                m_ch = ch;
                break;
            }
        }
        if (m_ch < 0) {
            fprintf(stderr, "no dma channel available");
            exit(1);
        }
    } else {
        perror("failed to get available DMA channels");
        exit(1);
    }
}

LED::LED() :
    m_ch(-1), m_r(0), m_g(0), m_b(0)
{
    set_loglevel(LOG_LEVEL_ERRORS);
    allocate_dma_channel();
    setup(PULSE_WIDTH_INCREMENT_GRANULARITY_US_DEFAULT, DELAY_VIA_PCM);
    init_channel(m_ch, 5120);
    frd_init_pwm(m_ch, (1 << 9) | (1 << 10) | (1 << 11));
}

LED::~LED()
{
    shutdown();
}

void LED::set_color(int r, int g, int b)
{
    m_r = adjust_pwm(1 << 9, m_r, r);
    m_b = adjust_pwm(1 << 10, m_b, b);
    m_g = adjust_pwm(1 << 11, m_g, g);
}

int LED::adjust_pwm(int gpio_mask, int from, int to)
{
    if (from != to) {
        if (to != 0) frd_set_high(m_ch, 512 - to, gpio_mask);
        if (from != 0) frd_clear(m_ch, 512 - from, gpio_mask);
    }
    return to;
}
