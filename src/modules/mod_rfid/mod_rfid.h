/**
 * @file    src/mod_rfid.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_RFID_H_
#define _MOD_RFID_H_

#include "target_cfg.h"
#include "threadedmodule.h"

#if MOD_RFID

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

class ModuleRFID : public qos::ThreadedModule<MOD_RFID_THREADSIZE>
{
public:
    enum EventsFlags
    {
        CardDetected = 1 << 0,
        CardLost = 1 << 1,
    };

    ModuleRFID();
    ~ModuleRFID();

    virtual void Init();
    virtual void Start();
    virtual void Shutdown();

    static ModuleRFID* GetInstance()
    {
        return &modInstance;
    }

    void RegisterListener(chibios_rt::EvtListener* listener, eventmask_t mask);
    void UnregisterListener(chibios_rt::EvtListener* listener);

    bool GetCurrentCardId(MifareUID& id);

protected:
    typedef qos::ThreadedModule<MOD_RFID_THREADSIZE> BaseClass;

    virtual tprio_t GetThreadPrio() const {return MOD_RFID_THREADPRIO;}
    virtual void ThreadMain();


private:
    void SetCardDetectLed(bool on);

    bool m_detectedCard;
    MFRC522Driver* m_mfrcDriver;
    MifareUID m_cardID;

    chibios_rt::EvtSource m_evtSource;
    chibios_rt::Mutex m_mutex;

    static ModuleRFID modInstance;
};

}

#endif

#endif /* _MOD_RFID_H_ */

/** @} */
