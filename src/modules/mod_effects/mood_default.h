/**
 * @file    src/mood_default.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOOD_DEFAULT_H_
#define _MOOD_DEFAULT_H_

#include "effect_buttons.h"
#include "mood.h"

namespace tmb_musicplayer
{
/**
 * @brief
 */
class MoodDefault : public Mood
{
public:
    MoodDefault();
    ~MoodDefault();

    virtual void Draw(systime_t sysTime, DisplayBuffer* display);
    virtual void SwitchMode(uint8_t mode);

private:
    EffectButtonsCfg effButtons_cfg =
    {
        .play = {
            .x = 2,
            .y = 0,
            .color = {0x51, 0xBD, 0x1f},
        },
        .vol_up = {
            .x = 4,
            .y = 0,
            .color = {0x00, 0xFF, 0xBF},
        },
        .vol_down = {
            .x = 0,
            .y = 0,
            .color = {0x00, 0x8D, 0x6A},
        },
        .next = {
            .x = 3,
            .y = 0,
            .color = {0xFF, 0x00, 0x2D},
        },
        .prev = {
            .x = 1,
            .y = 0,
            .color = {0x9D, 0x00, 0x1C},
        },
        .special = {
            .x = 5,
            .y = 0,
            .color = {0xFF, 0x5F, 0x00},
        },

        .playMode = EFFECT_BUTTON_MODE_EMPTYPLAYLIST,
        .colorModeEmptyPlayList = {0x29, 0x00, 0x02},
        .colorModePause = {0x28, 0x5F, 0x0F},
        .colorModeStop = {0xE4, 0x24, 0x2E},

        .blendperiod = MS2ST(500),
    };

    EffectButtonsData effButtons_data =
    {
        .lastPlayMode = EFFECT_BUTTON_MODE_EMPTYPLAYLIST,
        .lastBlendStep = 1.0f,
        .lastPlayModeColor = {0x29, 0x00, 0x02},
        .lastUpdate = 0,
    };

    Effect effButtons =
    {
        .effectcfg = &effButtons_cfg,
        .effectdata = &effButtons_data,
        .update = &EffectButtonsUpdate,
        .reset = &EffectButtonsReset,
        .p_next = NULL,
    };
};

}
#endif /* _MOOD_DEFAULT_H_ */

/** @} */
