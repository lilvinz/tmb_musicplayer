/**
 * @file    src/mod_input.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_input.h"

#if MOD_INPUT

#include "ch_tools.h"
#include "watchdog.h"
#include "module_init_cpp.h"

#include "qhal.h"
#include "board_buttons.h"

template <>
tmb_musicplayer::ModuleInput tmb_musicplayer::ModuleInputSingeton::instance = tmb_musicplayer::ModuleInput();

namespace tmb_musicplayer
{



ModuleInput::ModuleInput()
{

}

ModuleInput::~ModuleInput()
{

}


void ModuleInput::Init()
{
    watchdog_register(WATCHDOG_MOD_INPUT);
    buttons = BoardButtons::Buttons;
    buttonCount = sizeof(BoardButtons::Buttons)/sizeof(BoardButtons::Buttons[0]);
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
        watchdog_reload(WATCHDOG_MOD_INPUT);
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
MODULE_INITCALL(2, qos::ModuleInit<tmb_musicplayer::ModuleInputSingeton>::Init,
        qos::ModuleInit<tmb_musicplayer::ModuleInputSingeton>::Start,
        qos::ModuleInit<tmb_musicplayer::ModuleInputSingeton>::Shutdown)

#endif /* MOD_INPUT */

/** @} */
