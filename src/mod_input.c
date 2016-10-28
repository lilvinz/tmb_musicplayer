/**
 * @file    src/mod_input.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_input.h"

static bool ReadButton(const ButtonConfig* btnCfg)
{
    return palReadPad(btnCfg->port, btnCfg->pad);
}

/**
 * @brief
 */
typedef struct
{
    const ModInputConfig *config;
    thread_t* pThread;
} ModInputData;
static ModInputData modInputeData;

static THD_WORKING_AREA(waInputThread, MOD_INPUT_THREADSIZE);

static THD_FUNCTION(inputThread, arg)
{
    (void) arg;
    chRegSetThreadName("input");

    ModInputData* datap = &modInputeData;

    while (!chThdShouldTerminateX())
    {
        systime_t now = chVTGetSystemTimeX();

        uint32_t i;
        for (i = 0; i < datap->config->buttonCount; i++)
        {
            Button* btn = &datap->config->pButtons[i];

            bool state = ReadButton(&btn->cfg);

            if (state != btn->lastState)
            {
                btn->lastDebounceTime = now;
            }

            if ((now - btn->lastDebounceTime) >= MS2ST(10))
            {
                if (state != btn->state)
                {
                    if (state)
                    {
                        chEvtBroadcastFlags(&btn->eventSource, BUTTON_UP | BUTTON_PRESSED);
                    }
                    else
                    {
                        chEvtBroadcastFlags(&btn->eventSource, BUTTON_DOWN);
                        btn->lastDownTime = now;
                    }
                    btn->state = state;
                }
            }

            if (btn->state == false && ((now - btn->lastDownTime) > MS2ST(2000)))
            {
                chEvtBroadcastFlags(&btn->eventSource, BUTTON_PRESSED);
            }

            btn->lastState = state;
        }
        chThdSleep(MS2ST(10));
    }
}

void mod_input_init(ModInputConfig* config)
{
    osalDbgCheck(config != NULL);

    modInputeData.config = config;

    uint32_t i;
    for (i = 0; i < config->buttonCount; i++)
    {
        Button* btn = &config->pButtons[i];

        osalDbgCheck(btn != NULL);
        chEvtObjectInit(&btn->eventSource);
    }
}

bool mod_input_start(void)
{
    if (modInputeData.pThread == NULL)
    {
        uint32_t i;
        for (i = 0; i < modInputeData.config->buttonCount; i++)
        {
            Button* btn = &modInputeData.config->pButtons[i];

            bool state = ReadButton(&btn->cfg);
            btn->state = state;
            btn->lastState = state;
        }

        modInputeData.pThread = chThdCreateStatic(waInputThread,
                sizeof(waInputThread),
                MOD_INPUT_THREADPRIO, inputThread, NULL);

        return true;
    }
    return false;
}

void mod_input_shutdown(void)
{
    if (modInputeData.pThread != NULL)
    {
        chThdTerminate(modInputeData.pThread);
        modInputeData.pThread = NULL;
    }
}

/** @} */
