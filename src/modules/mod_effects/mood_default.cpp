/**
 * @file    src/mood_default.cpp
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mood_default.h"

namespace tmb_musicplayer {

MoodDefault::MoodDefault() {
}

MoodDefault::~MoodDefault() {
}

void MoodDefault::SwitchMode(uint8_t mode) {
    if (mode <= EFFECT_BUTTON_MODE_EMPTYPLAYLIST) {
        m_newMode = mode;
    }
}

void MoodDefault::Draw(systime_t sysTime, DisplayBuffer* display) {
    if (m_newMode != m_currentMode) {
        m_currentMode = m_newMode;
        effButtons_cfg.playMode = m_currentMode;
        m_modeChangedTime = sysTime;
        m_showButtons = true;
    }

    if (m_showButtons) {
        EffectUpdate(&effButtons, 0, 0, sysTime, display);

        if ((sysTime - m_modeChangedTime) >= MS2ST(30000)) {
            m_showButtons = false;
        }
    } else {
        if (m_currentMode == EFFECT_BUTTON_MODE_PLAY) {
            EffectUpdate(&effRandomPixel, 0, 0, sysTime, display);
        } else {
            EffectUpdate(&effButtons, 0, 0, sysTime, display);
        }
    }
}

}  // namespace tmb_musicplayer

/** @} */
