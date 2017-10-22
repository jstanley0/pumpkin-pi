#include <stdio.h>
#include <stdlib.h>
#include <pigpiod_if2.h>
#include "led.h"
#include "error.h"

// GPIOs for color/position
#define RL  8
#define GL  7
#define BL 25
#define RR 10
#define GR 11
#define BR  9

LED::LED()
{
    m_pi = pigpio_start(NULL, NULL);
    if (m_pi < 0) {
        perror("failed to connect to pigpio daemon");
        exit(1);
    }
    init_pin(RL);
    init_pin(GL);
    init_pin(BL);
    init_pin(RR);
    init_pin(GR);
    init_pin(BR);
}

LED::~LED()
{
    set_color(0, 0, 0);
    pigpio_stop(m_pi);
}

void LED::init_pin(int gpio)
{
    set_mode(m_pi, gpio, PI_OUTPUT);
    set_PWM_range(m_pi, gpio, 511);
}

void LED::set_color(int r, int g, int b)
{
    set_PWM_dutycycle(m_pi, RL, r);
    set_PWM_dutycycle(m_pi, RR, r);
    set_PWM_dutycycle(m_pi, GL, g);
    set_PWM_dutycycle(m_pi, GR, g);
    set_PWM_dutycycle(m_pi, BL, b);
    set_PWM_dutycycle(m_pi, BR, b);
}

void LED::set_color(int rL, int gL, int bL, int rR, int gR, int bR)
{
    set_PWM_dutycycle(m_pi, RL, rL);
    set_PWM_dutycycle(m_pi, RR, rR);
    set_PWM_dutycycle(m_pi, GL, gL);
    set_PWM_dutycycle(m_pi, GR, gR);
    set_PWM_dutycycle(m_pi, BL, bL);
    set_PWM_dutycycle(m_pi, BR, bR);
}
