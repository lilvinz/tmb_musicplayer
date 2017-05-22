/**
 * @file    src/mod_effects.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_EFFECTS_H_
#define _MOD_EFFECTS_H_

#include "target_cfg.h"
#include "threadedmodule.h"
#include "singleton.h"

#include "color.h"
#include "display.h"

#include "mood.h"
#include "mood_default.h"

#if MOD_EFFECTS

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/
#ifndef MOD_EFFECTS_THREADSIZE
#define MOD_EFFECTS_THREADSIZE 256
#endif

#ifndef MOD_EFFECTS_THREADPRIO
#define MOD_EFFECTS_THREADPRIO LOWPRIO
#endif

#ifndef LEDCOUNT
#error "LEDCOUNT driver must be specified for this target"
#endif

#ifndef DISPLAY_WIDTH
#error "DISPLAY_WIDTH driver must be specified for this target"
#endif

#ifndef DISPLAY_HEIGHT
#error "DISPLAY_HEIGHT driver must be specified for this target"
#endif

namespace tmb_musicplayer
{
/**
 * @brief
 */

class ModuleEffects : public qos::ThreadedModule<MOD_EFFECTS_THREADSIZE>
{
public:

    enum PlayModes
    {
        ModePlay = 0,
        ModePause,
        ModeStop,
        ModeEmptyPlaylist,
    };

    ModuleEffects();
    ~ModuleEffects();

    virtual void Init();
    virtual void Start();
    virtual void Shutdown();

    void SetMode(PlayModes mode);

protected:
    typedef qos::ThreadedModule<MOD_EFFECTS_THREADSIZE> BaseClass;

    virtual void ThreadMain();
    virtual tprio_t GetThreadPrio() const {return MOD_EFFECTS_THREADPRIO;}

private:
    void DrawCurrentMood();

    Color displayPixel[LEDCOUNT];
    DisplayBuffer display =
    {
        .width = DISPLAY_WIDTH,
        .height = DISPLAY_HEIGHT,
        .pixels = displayPixel,
    };

    Mood* currentMood = NULL;

    class Msg
    {
    public:
        PlayModes mode;
        uint8_t spare1;
        uint8_t spare2;
        uint8_t spare3;
    };


    chibios_rt::ObjectsPool<Msg, 2> m_MsgObjectPool;
    chibios_rt::Mailbox<Msg*, 2> m_Mailbox;

    static MoodDefault defaultMood;

};
typedef qos::Singleton<ModuleEffects> ModuleEffectsSingelton;

}
#endif /* MOD_EFFECTS */
#endif /* _MOD_EFFECTS_H_ */

/** @} */
