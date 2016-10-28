/**
 * @file    src/mod_musicbox.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_musicbox.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "ch.h"
#include "targetconf.h"
#include "chprintf.h"

#include "ff.h"
#include "mod_rfid.h"

#define EVENTMASK_RFID EVENT_MASK(0)


typedef struct {
    MusicBoxConfig* cfgp;
    thread_t* pThread;
} ModMusicBoxData;

static ModMusicBoxData modMusicBoxData;
static THD_WORKING_AREA(waMusicBoxThread, MOD_MUSICBOX_THREADSIZE);
/*
 *
 */
static THD_FUNCTION(musicbox, arg)
{
  (void)arg;
  chRegSetThreadName("musicbox");

  event_listener_t rfidEvtListener;
  chEvtRegisterMaskWithFlags(mod_rfid_eventscource(),
                               &rfidEvtListener,
                               EVENTMASK_RFID,
                               RFID_DETECTED | RFID_LOST);

  while (!chThdShouldTerminateX())
  {
      eventmask_t evt = chEvtWaitAny(ALL_EVENTS);
        if (evt & EVENTMASK_RFID)
        {
            eventflags_t flags = chEvtGetAndClearFlags(&rfidEvtListener);

            if (flags & RFID_DETECTED)
            {

            }
            if (flags & RFID_LOST)
            {

            }
        }
  }
  chEvtUnregister(mod_rfid_eventscource(), &rfidEvtListener);
}

void mod_musicbox_init(MusicBoxConfig* cfgp)
{
    modMusicBoxData.cfgp = cfgp;
}

bool mod_musicbox_start(void)
{
    if (modMusicBoxData.pThread == NULL)
    {
        modMusicBoxData.pThread = chThdCreateStatic(waMusicBoxThread, sizeof(waMusicBoxThread),
                MOD_MUSICBOX_THREADPRIO, musicbox, NULL);
        return true;
    }
    return false;
}

void mod_musicbox_stop(void)
{
    if (modMusicBoxData.pThread != NULL)
    {
        chThdTerminate(modMusicBoxData.pThread);
        modMusicBoxData.pThread = NULL;
    }
}


/** @} */
