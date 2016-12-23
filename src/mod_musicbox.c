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
#include "mod_musicplayer.h"
#include "mod_cardreader.h"

#define EVENTMASK_RFID EVENT_MASK(0)
#define EVENTMASK_BTN_PLAY EVENT_MASK(1)
#define EVENTMASK_BTN_NEXT EVENT_MASK(2)
#define EVENTMASK_BTN_PREV EVENT_MASK(3)
#define EVENTMASK_BTN_VOLUP EVENT_MASK(4)
#define EVENTMASK_BTN_VOLDOWN EVENT_MASK(5)
#define EVENTMASK_CARDREADER EVENT_MASK(6)

/* Generic large buffer.*/
static char absoluteFileNameBuffer[1024];
static char fileNameBuffer[512];

typedef void (*ButtonEventHandler)(Button*, eventflags_t);
typedef struct
{
    Button* btn;
    event_listener_t evtListener;
    eventmask_t evtMask;
    ButtonEventHandler handler;
} ButtonData;

typedef struct
{
    MusicBoxConfig* cfgp;
    thread_t* pThread;
    ButtonData buttons[5];
    int16_t volume;
    bool hasRFIDCard;
    struct MifareUID uid;
} ModMusicBoxData;
static ModMusicBoxData modMusicBoxData;

static void RegisterButtonEvents(void)
{
    int i;
    for (i = 0; i < 5; i++)
    {
        ButtonData* btnData = &modMusicBoxData.buttons[i];
        chEvtRegisterMaskWithFlags(&btnData->btn->eventSource, &btnData->evtListener, btnData->evtMask,
                    BUTTON_DOWN | BUTTON_UP | BUTTON_PRESSED);
    }
}

static void UnregisterButtonEvents(void)
{
    int i;
    for (i = 0; i < 5; i++)
    {
        ButtonData* btnData = &modMusicBoxData.buttons[i];
        chEvtUnregister(&btnData->btn->eventSource, &btnData->evtListener);
    }
}

static void ChangeVolume(int16_t diff)
{
    modMusicBoxData.volume = modMusicBoxData.volume + diff;
    if (modMusicBoxData.volume < 0)
    {
        modMusicBoxData.volume = 0;
    }
    else if (modMusicBoxData.volume > 254)
    {
        modMusicBoxData.volume = 254;
    }

    mod_musicplayer_cmdVolume((uint8_t)modMusicBoxData.volume);
}

static size_t MifareUIDToString(const struct MifareUID* uid, char* psz)
{
    int i;
    size_t charCount = 0;
    char* pszTarget = psz;
    for (i = 0; i < uid->size; i++)
    {
        char c = 0x0f & (uid->bytes[i] >> 4);
        if (c < 10)
        {
            *pszTarget = c + '0';
        }
        else
        {
            *pszTarget = c + ('A' - 10);
        }

        pszTarget++;
        charCount++;

        c = 0x0f & uid->bytes[i];
        if (c < 10)
        {
            *pszTarget = c + '0';
        }
        else
        {
            *pszTarget = c + ('A' - 10);
        }

        pszTarget++;
        charCount ++;
    }

    *pszTarget = 0;
    return charCount;
}

static void ProcessMifareUID(struct MifareUID* uid)
{
    DIR directory;

    FILINFO fileInfo;
    fileInfo.lfname = fileNameBuffer;
    fileInfo.lfsize = sizeof(fileNameBuffer);

    char pszUID[32];

    if (MifareUIDToString(uid, pszUID) > 0)
    {
        /*search for folder*/
        if (mod_cardreader_find(&directory, &fileInfo, "/music", pszUID) == true)
        {
            memset(absoluteFileNameBuffer, 0, sizeof(absoluteFileNameBuffer));
            strcat(absoluteFileNameBuffer, "/music/");
            strcat(absoluteFileNameBuffer, fileInfo.lfname);

            mod_musicplayer_cmdPlay(absoluteFileNameBuffer);
        }
    }
}


static THD_WORKING_AREA(waMusicBoxThread, MOD_MUSICBOX_THREADSIZE);
/*
 *
 */
static THD_FUNCTION(musicbox, arg)
{
    (void) arg;
    chRegSetThreadName("musicbox");

    event_listener_t rfidEvtListener;
    chEvtRegisterMaskWithFlags(mod_rfid_eventscource(), &rfidEvtListener,
                EVENTMASK_RFID,
                RFID_DETECTED | RFID_LOST);

    event_listener_t cardreaderEvtListener;
   chEvtRegisterMaskWithFlags(mod_cardreader_eventscource(), &cardreaderEvtListener,
               EVENTMASK_CARDREADER,
               CARDREADER_EVENT_MOUNTED | CARDREADER_EVENT_UNMOUNTED);

    RegisterButtonEvents();


    while (!chThdShouldTerminateX())
    {
        eventmask_t evt = chEvtWaitAny(ALL_EVENTS);
        if (evt & EVENTMASK_RFID)
        {
            eventflags_t flags = chEvtGetAndClearFlags(&rfidEvtListener);

            if (flags & RFID_DETECTED)
            {
                if (mod_rfid_cardid(&modMusicBoxData.uid) == true)
                {
                    modMusicBoxData.hasRFIDCard = true;
                    ProcessMifareUID(&modMusicBoxData.uid);
                }
            }

            if (flags & RFID_LOST)
            {
                modMusicBoxData.hasRFIDCard = false;
                mod_musicplayer_cmdStop();
            }
        }

        if (evt & EVENTMASK_CARDREADER)
        {
            eventflags_t flags = chEvtGetAndClearFlags(&cardreaderEvtListener);

            if (flags & CARDREADER_EVENT_MOUNTED)
            {
                if (modMusicBoxData.hasRFIDCard == true)
                {
                    ProcessMifareUID(&modMusicBoxData.uid);
                }
            }

            if (flags & CARDREADER_EVENT_UNMOUNTED)
            {
                mod_musicplayer_cmdStop();
            }
        }

        /* process buttons */
        int i;
        for (i = 0; i < 5; i++)
        {
            ButtonData* btnData = &modMusicBoxData.buttons[i];
            if (evt & btnData->evtMask)
            {
                eventflags_t flags = chEvtGetAndClearFlags(&btnData->evtListener);
                btnData->handler(btnData->btn, flags);
            }
        }
    }

    UnregisterButtonEvents();

    chEvtUnregister(mod_rfid_eventscource(), &rfidEvtListener);
}

static void OnPlayButton(Button* btn, eventflags_t flags)
{
    (void)btn;
    if (flags & BUTTON_UP)
    {
        mod_musicplayer_cmdToggle();
    }
}
static void OnNextButton(Button* btn, eventflags_t flags)
{
    (void)btn;
    if (flags & BUTTON_PRESSED)
    {
        mod_musicplayer_cmdNext();
    }
}

static void OnPrevButton(Button* btn, eventflags_t flags)
{
    (void)btn;
    if (flags & BUTTON_PRESSED)
    {
        mod_musicplayer_cmdPrev();
    }
}

static void OnVolUpButton(Button* btn, eventflags_t flags)
{
    (void)btn;
    if (flags & BUTTON_PRESSED)
    {
        ChangeVolume(-10);
    }
}

static void OnVolDownButton(Button* btn, eventflags_t flags)
{
    (void)btn;
    if (flags & BUTTON_PRESSED)
    {
        ChangeVolume(10);
    }
}


void mod_musicbox_init(MusicBoxConfig* cfgp)
{
    modMusicBoxData.cfgp = cfgp;

    modMusicBoxData.buttons[0].btn = cfgp->btnPlay;
    modMusicBoxData.buttons[0].handler = &OnPlayButton;
    modMusicBoxData.buttons[0].evtMask = EVENTMASK_BTN_PLAY;

    modMusicBoxData.buttons[1].btn = cfgp->btnNext;
    modMusicBoxData.buttons[1].handler = &OnNextButton;
    modMusicBoxData.buttons[1].evtMask = EVENTMASK_BTN_NEXT;

    modMusicBoxData.buttons[2].btn = cfgp->btnPrev;
    modMusicBoxData.buttons[2].handler = &OnPrevButton;
    modMusicBoxData.buttons[2].evtMask = EVENTMASK_BTN_PREV;

    modMusicBoxData.buttons[3].btn = cfgp->btnVolUp;
    modMusicBoxData.buttons[3].handler = &OnVolUpButton;
    modMusicBoxData.buttons[3].evtMask = EVENTMASK_BTN_VOLUP;

    modMusicBoxData.buttons[4].btn = cfgp->btnVolDown;
    modMusicBoxData.buttons[4].handler = &OnVolDownButton;
    modMusicBoxData.buttons[4].evtMask = EVENTMASK_BTN_VOLDOWN;

    modMusicBoxData.volume = 50;

    modMusicBoxData.hasRFIDCard = false;
}

bool mod_musicbox_start(void)
{
    if (modMusicBoxData.pThread == NULL)
    {
        modMusicBoxData.pThread = chThdCreateStatic(waMusicBoxThread,
                sizeof(waMusicBoxThread),
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
