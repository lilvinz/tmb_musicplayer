/**
 * @file    src/mod_musicbox.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_MUSICBOX_H_
#define _MOD_MUSICBOX_H_

#include "target_cfg.h"
#include "threadedmodule.h"
#include "singleton.h"

#if MOD_MUSICBOX

#include "button.h"
#include "mfrc522.h"

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/
#ifndef MOD_MUSICBOX_THREADSIZE
#define MOD_MUSICBOX_THREADSIZE 2024
#endif

#ifndef MOD_MUSICBOX_THREADPRIO
#define MOD_MUSICBOX_THREADPRIO LOWPRIO
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/
namespace tmb_musicplayer
{
/**
 * @brief
 */

class ModuleMusicbox : public qos::ThreadedModule<MOD_MUSICBOX_THREADSIZE>
{
public:
    enum ButtonType
    {
        Play = 0,
        Next,
        Prev,
        VolUp,
        VolDown,
        ButtonTypeCount
    };

    ModuleMusicbox();
    ~ModuleMusicbox();

    virtual void Init();
    virtual void Start();
    virtual void Shutdown();

protected:
    typedef qos::ThreadedModule<MOD_MUSICBOX_THREADSIZE> BaseClass;

    virtual void ThreadMain();

    virtual tprio_t GetThreadPrio() const {return MOD_MUSICBOX_THREADPRIO;}

private:
    typedef void (ModuleMusicbox::*ButtonEventHandler)(Button*, eventflags_t);
    struct ButtonData
    {
        Button* button;
        chibios_rt::EvtListener evtListener;
        eventmask_t evtMask;
        ButtonEventHandler handler;
    };

    void OnPlayButton(Button* btn, eventflags_t flags);
    void OnNextButton(Button* btn, eventflags_t flags);
    void OnPrevButton(Button* btn, eventflags_t flags);
    void OnVolUpButton(Button* btn, eventflags_t flags);
    void OnVolDownButton(Button* btn, eventflags_t flags);
    void OnRFIDEvent(eventflags_t flags);
    void OnCardReaderEvent(eventflags_t flags);

    void RegisterButtonEvents();
    void UnregisterButtonEvents();

    void ChangeVolume(int16_t diff);
    void ProcessMifareUID(const char* pszUID);

    static size_t MifareUIDToString(const MifareUID& uid, char* psz);

    bool hasRFIDCard = false;
    int16_t volume = 50;

    ButtonData buttons[ButtonTypeCount];
    MifareUID uid;

    char absoluteFileNameBuffer[1024];
    char fileNameBuffer[512];

    class ModuleRFID* m_modRFID = NULL;
    class ModuleCardreader* m_modCardreader = NULL;
    class ModulePlayer* m_modPlayer = NULL;
    class ModuleEffects* m_modEffects = NULL;
};
typedef qos::Singleton<ModuleMusicbox> ModuleMusicboxSingelton;

}

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* MOD_MUSICBOX */
#endif /* _MOD_MUSICBOX_H_ */

/** @} */
