/**
 * @file    src/mod_input.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_input.h"

namespace tmb_musicplayer
{
ModuleInput::ModuleInput(Button** btns, size_t count) :
    buttons(btns),
    buttonCount(count)
{

}

void ModuleInput::Start()
{
    Button** btn = buttons;
    systime_t now = chVTGetSystemTimeX();
    for (size_t i = 0; i < buttonCount; i++)
    {
        (*btn)->InitState(now);
        ++btn;
    }

    BaseClass::Start();
}
void ModuleInput::Shutdown()
{
    BaseClass::Shutdown();
}

void ModuleInput::ThreadMain()
{
    chRegSetThreadName("input");

    while (!chibios_rt::BaseThread::shouldTerminate())
    {
        Button** btn = buttons;
        systime_t now = chVTGetSystemTimeX();

        for (size_t i = 0; i < buttonCount; i++)
        {
            (*btn)->Process(now);
            ++btn;
        }
        chibios_rt::BaseThread::sleep(MS2ST(10));
    }
}

}

/** @} */
