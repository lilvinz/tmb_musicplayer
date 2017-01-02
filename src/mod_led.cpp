/**
 * @file    src/mod_led.cpp
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_led.h"


namespace tmb_musicplayer
{
/**
 * @brief
 */
Led::Led()
{

}

void Led::Initialize(stm32_gpio_t* port, uint32_t pad, bool clearOn)
{
    m_port = port;
    m_pad = pad;
    m_clearOn = clearOn;
}

void Led::On()
{
    if (m_clearOn == true)
    {
        palClearPad(m_port, m_pad);
    }
    else
    {
        palSetPad(m_port, m_pad);
    }
}

void Led::Off()
{
    if (m_clearOn == true)
    {
        palSetPad(m_port, m_pad);
    }
    else
    {
        palClearPad(m_port, m_pad);
    }
}

void Led::Toggle()
{
    bool padState = palReadPad(m_port, m_pad);

    if (padState == true)
    {
        palClearPad(m_port, m_pad);
    }
    else
    {
        palSetPad(m_port, m_pad);
    }
}

bool Led::IsOn()
{
    bool padState = palReadPad(m_port, m_pad);
    return padState != m_clearOn;
}

}


/** @} */
