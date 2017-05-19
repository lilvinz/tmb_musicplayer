/**
 * @file    src/button.cpp
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "button.h"

namespace tmb_musicplayer
{

Button::Button(ioportid_t ioportid, uint8_t p) :
        port(ioportid),
        pad(p),
        lastState(false),
        state(false),
        lastDebounceTime(0),
        lastDownTime(0)
{

}

void Button::RegisterListener(chibios_rt::EvtListener* listener, eventmask_t mask)
{
    evtSource.registerMask(listener, mask);
}

void Button::UnregisterListener(chibios_rt::EvtListener* listener)
{
    evtSource.unregister(listener);
}

void Button::InitState(systime_t now)
{
    bool currState = ReadPal(port, pad);
    state = currState;
    lastState = currState;
}

void Button::Process(systime_t now)
{
    bool currState = ReadPal(port, pad);

    if (currState != lastState)
    {
        lastDebounceTime = now;
    }

    if ((now - lastDebounceTime) >= MS2ST(10))
    {
        if (currState != state)
        {
            if (currState)
            {
                evtSource.broadcastFlags(Up | Pressed);
            }
            else
            {
                evtSource.broadcastFlags(Down);
                lastDownTime = now;
            }
            state = currState;
        }
    }

    if (state == false && ((now - lastDownTime) > MS2ST(2000)))
    {
        evtSource.broadcastFlags(Pressed);
        lastDownTime = lastDownTime + MS2ST(200);
    }

    lastState = currState;
}

bool Button::ReadPal(ioportid_t port, uint8_t pad)
{
    return palReadPad(port, pad);
}

bool Button::GetState() const
{
    return state;
}
}

/** @} */
