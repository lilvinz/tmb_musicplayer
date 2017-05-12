/**
 * @file    src/mod_rfid.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_rfid.h"

#if MOD_RFID

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "ch.h"
#include "targetconf.h"
#include "chprintf.h"


namespace tmb_musicplayer
{
/**
 * @brief
 */



ModuleRFID::ModuleRFID() :
        m_detectedCard(false),
        m_mfrcDriver(NULL),
        m_detectLed(NULL)
{

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
    m_detectLed = NULL;
}

void ModuleRFID::SetDriver(MFRC522Driver* driver)
{
    m_mfrcDriver = driver;
}

void ModuleRFID::SetLed(Led* led)
{
    m_detectLed = led;
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
    if (m_detectLed != NULL)
    {
        if (on == true)
        {
            m_detectLed->On();
        }
        else
        {
            m_detectLed->Off();
        }
    }
}

}

#endif
/** @} */
