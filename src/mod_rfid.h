/**
 * @file    src/mod_rfid.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_RFID_H_
#define _MOD_RFID_H_

#include "hal.h"
#include "module.h"
#include "mod_led.h"
#include "mfrc522.h"

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/
#ifndef MOD_RFID_THREADSIZE
#define MOD_RFID_THREADSIZE 320
#endif

#ifndef MOD_RFID_THREADPRIO
#define MOD_RFID_THREADPRIO LOWPRIO
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

class ModuleRFID : public Module<MOD_RFID_THREADSIZE>
{
public:
    enum EventsFlags
    {
        CardDetected = 1 << 0,
        CardLost = 1 << 1,
    };

    ModuleRFID();

    virtual void Start();
    virtual void Shutdown();

    void SetDriver(MFRC522Driver* driver);
    void SetLed(Led* led);

    void RegisterListener(chibios_rt::EvtListener* listener, eventmask_t mask);
    void UnregisterListener(chibios_rt::EvtListener* listener);

    bool GetCurrentCardId(MifareUID& id);

protected:
    typedef Module<MOD_RFID_THREADSIZE> BaseClass;

    virtual void ThreadMain();

    virtual tprio_t GetThreadPrio() const {return MOD_RFID_THREADPRIO;}

private:
    void SetCardDetectLed(bool on);

    bool m_detectedCard;
    MFRC522Driver* m_mfrcDriver;
    Led* m_detectLed;
    MifareUID m_cardID;

    chibios_rt::EvtSource m_evtSource;
    chibios_rt::Mutex m_mutex;
};

}


/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* _MOD_RFID_H_ */

/** @} */
