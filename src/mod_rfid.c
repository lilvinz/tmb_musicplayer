/**
 * @file    src/mod_rfid.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_rfid.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "ch.h"
#include "targetconf.h"
#include "chprintf.h"


typedef struct {
    RFIDConfig* cfgp;
    thread_t* threadp;
    bool detectedCard;
    struct MifareUID cardID;
    event_source_t eventSource;
} ModRFIDData;

static MUTEX_DECL(CardIDMutex);

static ModRFIDData modRFIDData;
static THD_WORKING_AREA(waRFIDReader, MOD_RFID_THREADSIZE);

/*
 * This is a periodic thread that reads uid from rfid periphal
 */
static THD_FUNCTION(rfidReader, arg)
{
    (void)arg;
    chRegSetThreadName("rfidreader");

    ModRFIDData* datap = &modRFIDData;

    mod_led_off(datap->cfgp->ledCardDetect);
    datap->detectedCard = false;
    while (!chThdShouldTerminateX())
    {
        chMtxLock(&CardIDMutex);
        if (MifareCheck(datap->cfgp->mfrcd, &datap->cardID) == MIFARE_OK)
        {
            if (datap->detectedCard == false)
            {
                mod_led_on(datap->cfgp->ledCardDetect);
                datap->detectedCard = true;
                chEvtBroadcastFlags(&datap->eventSource, RFID_DETECTED);
            }
        }
        else
        {
          if (datap->detectedCard == true)
          {
              mod_led_off(datap->cfgp->ledCardDetect);
              datap->cardID.size = 0;
              datap->detectedCard = false;
              chEvtBroadcastFlags(&datap->eventSource, RFID_LOST);
          }
        }
        chMtxUnlock(&CardIDMutex);

        chThdSleep(MS2ST(100));
    }
}

void mod_rfid_init(RFIDConfig* cfgp)
{
    modRFIDData.cfgp = cfgp;
    modRFIDData.detectedCard = false;
    chEvtObjectInit(&modRFIDData.eventSource);
}

bool mod_rfid_start(void)
{
    if (modRFIDData.threadp == NULL)
    {
        modRFIDData.threadp = chThdCreateStatic(waRFIDReader, sizeof(waRFIDReader),
                MOD_RFID_THREADPRIO, rfidReader, NULL);
        return true;
    }
    return false;
}

void mod_rfid_stop(void)
{
    if (modRFIDData.threadp != NULL)
    {
        chThdTerminate(modRFIDData.threadp);
        modRFIDData.threadp = NULL;
    }
}

event_source_t* mod_rfid_eventscource(void)
{
    return &modRFIDData.eventSource;
}

bool mod_rfid_cardid(struct MifareUID* id)
{
    if (modRFIDData.detectedCard)
    {
        memcpy(id, &modRFIDData.cardID, sizeof(modRFIDData.cardID));
        return true;
    }
    return false;
}

/** @} */
