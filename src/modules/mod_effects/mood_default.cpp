/**
 * @file    src/mood_default.cpp
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mood_default.h"

namespace tmb_musicplayer
{

MoodDefault::MoodDefault()
{

}

MoodDefault::~MoodDefault()
{

}

void MoodDefault::SwitchMode(uint8_t mode)
{
    if (mode <= EFFECT_BUTTON_MODE_EMPTYPLAYLIST)
    {
        effButtons_cfg.playMode = mode;
    }
}

void MoodDefault::Draw(systime_t sysTime, DisplayBuffer* display)
{
    EffectUpdate(&effButtons, 0, 0, sysTime, display);
}

}

/** @} */
