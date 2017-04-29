/**
 * @file    src/mod_musicbox.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_MUSICBOX_H_
#define _MOD_MUSICBOX_H_

#include "hal.h"
#include "button.h"
#include "module.h"

#include "mfrc522.h"

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/
#ifndef MOD_MUSICBOX_THREADSIZE
#define MOD_MUSICBOX_THREADSIZE 640
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

class ModuleMusicbox : public Module<MOD_MUSICBOX_THREADSIZE>
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

    virtual void Start();
    virtual void Shutdown();

    void SetButton(ButtonType type, Button* button);
    void SetRFIDModule(class ModuleRFID* module);
    void SetPlayerModule(class ModulePlayer* module);
    void SetCardreaderModule(class ModuleCardreader* module, class Led* led);

protected:
    typedef Module<MOD_MUSICBOX_THREADSIZE> BaseClass;

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
    void ProcessMifareUID(const MifareUID& uid);

    static size_t MifareUIDToString(const MifareUID& uid, char* psz);

    bool hasRFIDCard;
    int16_t volume;

    ButtonData buttons[ButtonTypeCount];
    MifareUID uid;

    chibios_rt::EvtListener rfidEvtListener;
    chibios_rt::EvtListener cardreaderEvtListener;

    char absoluteFileNameBuffer[1024];
    char fileNameBuffer[512];

    class ModuleRFID* m_modRFID = NULL;
    class ModuleCardreader* m_modCardreader = NULL;
    class ModulePlayer* m_modPlayer = NULL;

    class Led* m_cardDetectLED;
};

}

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#endif /* _MOD_MUSICBOX_H_ */

/** @} */
