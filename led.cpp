#include "pwm/pwm.h"
#include "led.h"

LED::LED()
{
    setup(100, DELAY_VIA_PWM);
    init_channel(0, 25600);
}

LED::~LED()
{
    shutdown();
}
 
void LED::set_color(int r, int g, int b)
{
    clear_channel(0);
    add_channel_pulse(0, 9,  0, g);
    add_channel_pulse(0, 10, 0, b);
    add_channel_pulse(0, 11, 0, r);    
}
