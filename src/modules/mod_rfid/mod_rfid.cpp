/**
 * @file    src/mod_rfid.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_rfid.h"

#if MOD_RFID

#include "ch_tools.h"
#include "watchdog.h"
#include "module_init_cpp.h"

#include "qhal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


namespace tmb_musicplayer
{

ModuleRFID ModuleRFID::modInstance = ModuleRFID();
/**
 * @brief
 */



ModuleRFID::ModuleRFID() :
        m_detectedCard(false),
        m_mfrcDriver(NULL)
{

}

ModuleRFID::~ModuleRFID()
{

}

void ModuleRFID::Init()
{
    m_mfrcDriver = &RFID1;
    watchdog_register(WATCHDOG_MOD_RFID);
}

void ModuleRFID::Start()
{
    chDbgAssert(m_mfrcDriver != NULL, "No driver set!!");

    m_detectedCard = false;

    BaseClass::Start();
}

void ModuleRFID::Shutdown()
{
    BaseClass::Shutdown();

    m_mfrcDriver = NULL;
}

void ModuleRFID::RegisterListener(chibios_rt::EvtListener* listener, eventmask_t mask)
{
    m_evtSource.registerMask(listener, mask);
}

void ModuleRFID::UnregisterListener(chibios_rt::EvtListener* listener)
{
    m_evtSource.unregister(listener);
}

bool ModuleRFID::GetCurrentCardId(MifareUID& id)
{
    bool detected = false;
    m_mutex.lock();
    if (m_detectedCard == true)
    {
        memcpy(&id, &m_cardID, sizeof(m_cardID));
        detected = true;
    }
    m_mutex.unlock();
    return detected;
}



void ModuleRFID::ThreadMain()
{
    chRegSetThreadName("rfidreader");

    m_detectedCard = false;

    SetCardDetectLed(false);
    while (!chThdShouldTerminateX())
    {
        watchdog_reload(WATCHDOG_MOD_RFID);
        bool lastDetectState = m_detectedCard;
        m_mutex.lock();
        MIFARE_Status_t status = MifareCheck(m_mfrcDriver, &m_cardID);
        m_detectedCard = status == MIFARE_OK;
        if (m_detectedCard == false)
        {
            /*reset cardid*/
            m_cardID.size = 0;
        }
        m_mutex.unlock();

        if (m_detectedCard == true)
        {
            if (lastDetectState == false)
            {
                SetCardDetectLed(true);
                m_evtSource.broadcastFlags(CardDetected);
            }
        }
        else
        {
          if (lastDetectState == true)
          {
              SetCardDetectLed(false);
              m_evtSource.broadcastFlags(CardLost);
          }
        }

        chibios_rt::BaseThread::sleep(MS2ST(100));
    }
}

void ModuleRFID::SetCardDetectLed(bool on)
{
#if HAL_USE_LED
    if (on == true)
    {
        ledOn(LED_CARDDETECT);
    }
    else
    {
        ledOff(LED_CARDDETECT);
    }
#endif /* HAL_USE_LED */

}

}

MODULE_INITCALL(1, qos::ModuleInit<tmb_musicplayer::ModuleRFID>::Init, qos::ModuleInit<tmb_musicplayer::ModuleRFID>::Start, qos::ModuleInit<tmb_musicplayer::ModuleRFID>::Shutdown)

#endif
/** @} */
