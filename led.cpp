extern "C" {
#include "pwm/pwm.h"
}

#include "led.h"

LED::LED()
{
    set_loglevel(LOG_LEVEL_ERRORS);
    setup(PULSE_WIDTH_INCREMENT_GRANULARITY_US_DEFAULT, DELAY_VIA_PCM);
    init_channel(0, 5120);
}

LED::~LED()
{
    shutdown();
}

void LED::set_color(int r, int g, int b)
{
    clear_channel(0);
    if (r) add_channel_pulse(0, 9,  0, r);
    if (b) add_channel_pulse(0, 10, 0, b);
    if (g) add_channel_pulse(0, 11, 0, g);
}
