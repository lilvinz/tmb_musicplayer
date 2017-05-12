/**
 * @file    src/mod_effects.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_effects.h"

#if MOD_TEST
/**
 * @brief
 */
typedef struct
{
    const ModEffectsConfig *config;
    thread_t* pThread;
} ModEffectsData;
static ModEffectsData modData;

static THD_WORKING_AREA(waModuleThread, MOD_EFFECTS_THREADSIZE);

static THD_FUNCTION(moduleThread, arg)
{
    (void) arg;
    chRegSetThreadName("effects");

    ModEffectsData* datap = &modData;

    while (!chThdShouldTerminateX())
    {
        systime_t now = chVTGetSystemTimeX();

        chThdSleep(MS2ST(10));
    }
}

void mod_effects_init(ModEffectsConfig* config)
{
    osalDbgCheck(config != NULL);

    modData.config = config;
}

bool mod_effects_start(void)
{
    if (modData.pThread == NULL)
    {
        modData.pThread = chThdCreateStatic(waModuleThread,
                sizeof(waModuleThread),
                MOD_EFFECTS_THREADPRIO, moduleThread, NULL);

        return true;
    }
    return false;
}

void mod_effects_shutdown(void)
{
    if (modData.pThread != NULL)
    {
        chThdTerminate(modData.pThread);
        modData.pThread = NULL;
    }
}

#endif

/** @} */
