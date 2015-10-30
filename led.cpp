// GPIO  9 = red
// GPIO 10 = blue
// GPIO 11 = green
// brightness levels 0..511 (because reasons. 9 bits is better than 8, right?)
// TODO FIXME hack pwm.c to make it possible to use one channel
// TODO FIXME SRS FRD actually pick a DMA channel that isn't in use

extern "C" {
#include "pwm/pwm.h"
}

#include "led.h"

LED::LED()
{
    set_loglevel(LOG_LEVEL_ERRORS);
    setup(PULSE_WIDTH_INCREMENT_GRANULARITY_US_DEFAULT, DELAY_VIA_PCM);
    init_channel(12, 5120);
    init_channel(13, 5120);
    init_channel(14, 5120);
}

LED::~LED()
{
    shutdown();
}

void LED::set_color(int r, int g, int b)
{
    add_channel_pulse(12, 9,  0, r);
    add_channel_pulse(13, 10, 0, b);
    add_channel_pulse(14, 11, 0, g);
}
