/**
 * @file    src/mod_musicbox.cpp
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_musicbox.h"


#if MOD_MUSICBOX

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ch.h"
#include "targetconf.h"
#include "chprintf.h"

#include "ff.h"
#include "mod_rfid.h"
#include "mod_cardreader.h"
#include "mod_player.h"

#define EVENTMASK_RFID EVENT_MASK(0)
#define EVENTMASK_BTN_PLAY EVENT_MASK(1)
#define EVENTMASK_BTN_NEXT EVENT_MASK(2)
#define EVENTMASK_BTN_PREV EVENT_MASK(3)
#define EVENTMASK_BTN_VOLUP EVENT_MASK(4)
#define EVENTMASK_BTN_VOLDOWN EVENT_MASK(5)
#define EVENTMASK_CARDREADER EVENT_MASK(6)

namespace tmb_musicplayer
{

ModuleMusicbox::ModuleMusicbox()
{
    //buttons[0].button = cfgp->btnPlay;
    buttons[Play].handler = &ModuleMusicbox::OnPlayButton;
    buttons[Play].evtMask = EVENTMASK_BTN_PLAY;

    //buttons[1].btn = cfgp->btnNext;
    buttons[Next].handler = &ModuleMusicbox::OnNextButton;
    buttons[Next].evtMask = EVENTMASK_BTN_NEXT;

    //buttons[Prev].btn = cfgp->btnPrev;
    buttons[Prev].handler = &ModuleMusicbox::OnPrevButton;
    buttons[Prev].evtMask = EVENTMASK_BTN_PREV;

    //buttons[VolUp].btn = cfgp->btnVolUp;
    buttons[VolUp].handler = &ModuleMusicbox::OnVolUpButton;
    buttons[VolUp].evtMask = EVENTMASK_BTN_VOLUP;

    //buttons[VolDown].btn = cfgp->btnVolDown;
    buttons[VolDown].handler = &ModuleMusicbox::OnVolDownButton;
    buttons[VolDown].evtMask = EVENTMASK_BTN_VOLDOWN;

    volume = 50;

    hasRFIDCard = false;

}

void ModuleMusicbox::Start()
{
    BaseClass::Start();
}

void ModuleMusicbox::Shutdown()
{
    BaseClass::Shutdown();
}

void ModuleMusicbox::ThreadMain()
{
    chRegSetThreadName("musicbox");

    chibios_rt::EvtListener rfidEvtListener;
    chibios_rt::EvtListener cardreaderEvtListener;

    if (m_modRFID != NULL)
    {
        m_modRFID->RegisterListener(&rfidEvtListener, EVENTMASK_RFID);
    }

    if (m_modCardreader != NULL)
    {
        m_modCardreader->RegisterListener(&cardreaderEvtListener, EVENTMASK_CARDREADER);
    }

    RegisterButtonEvents();


    while (!chThdShouldTerminateX())
    {
        eventmask_t evt = chEvtWaitAny(ALL_EVENTS);
        if (evt & EVENTMASK_RFID)
        {
            eventflags_t flags = rfidEvtListener.getAndClearFlags();
            OnRFIDEvent(flags);
        }

        if (evt & EVENTMASK_CARDREADER)
        {
            eventflags_t flags = cardreaderEvtListener.getAndClearFlags();
            OnCardReaderEvent(flags);
        }

        /* process buttons */
        int i;
        for (i = 0; i < 5; i++)
        {
            ButtonData& btnData = buttons[i];
            if (evt & btnData.evtMask)
            {
                eventflags_t flags = btnData.evtListener.getAndClearFlags();
                (this->*btnData.handler)(btnData.button, flags);
            }
        }
    }

    UnregisterButtonEvents();

    if (m_modCardreader != NULL)
    {
        m_modCardreader->UnregisterListener(&cardreaderEvtListener);
    }

    if (m_modRFID != NULL)
    {
        m_modRFID->UnregisterListener(&rfidEvtListener);
    }

    //chEvtUnregister(mod_rfid_eventscource(), &rfidEvtListener);
}

void ModuleMusicbox::SetButton(ButtonType type, Button* button)
{
    buttons[type].button = button;
}

void ModuleMusicbox::RegisterButtonEvents()
{
    for (int i = 0; i < ButtonTypeCount; i++)
    {
        ButtonData& btnData = buttons[i];
        btnData.button->RegisterListener(&btnData.evtListener, btnData.evtMask);
    }
}

void ModuleMusicbox::UnregisterButtonEvents()
{
    for (int i = 0; i < ButtonTypeCount; i++)
    {
        ButtonData& btnData = buttons[i];
        btnData.button->UnregisterListener(&btnData.evtListener);
    }
}

void ModuleMusicbox::OnPlayButton(Button* btn, eventflags_t flags)
{
    (void)btn;
    if (flags & Button::Up)
    {
        m_modPlayer->Toggle();
    }
}
void ModuleMusicbox::OnNextButton(Button* btn, eventflags_t flags)
{
    (void)btn;
    if (flags & Button::Pressed)
    {
        m_modPlayer->Next();
    }
}

void ModuleMusicbox::OnPrevButton(Button* btn, eventflags_t flags)
{
    (void)btn;
    if (flags & Button::Pressed)
    {
        m_modPlayer->Prev();
    }
}

void ModuleMusicbox::OnVolUpButton(Button* btn, eventflags_t flags)
{
    (void)btn;
    if (flags & Button::Pressed)
    {
        ChangeVolume(-10);
    }
}

void ModuleMusicbox::OnVolDownButton(Button* btn, eventflags_t flags)
{
    (void)btn;
    if (flags & Button::Pressed)
    {
        ChangeVolume(10);
    }
}

void ModuleMusicbox::OnRFIDEvent(eventflags_t flags)
{
    if (flags & ModuleRFID::CardDetected)
    {
        if (m_modRFID->GetCurrentCardId(uid) == true)
        {
            hasRFIDCard = true;
            ProcessMifareUID(uid);
        }
    }

    if (flags & ModuleRFID::CardLost)
    {
        hasRFIDCard = false;
        m_modPlayer->Stop();
    }
}

void ModuleMusicbox::OnCardReaderEvent(eventflags_t flags)
{
    if (flags & ModuleCardreader::FilesystemMounted)
    {
        if (m_cardDetectLED != NULL)
        {
            m_cardDetectLED->On();
        }
    }

    if (flags & ModuleCardreader::FilesystemUnmounted)
    {
        if (m_cardDetectLED != NULL)
        {
            m_cardDetectLED->Off();
        }
    }
}

void ModuleMusicbox::ChangeVolume(int16_t diff)
{
    volume = volume + diff;
    if (volume < 0)
    {
        volume = 0;
    }
    else if (volume > 254)
    {
        volume = 254;
    }

    //mod_musicplayer_cmdVolume((uint8_t)volume);
}

void ModuleMusicbox::ProcessMifareUID(const MifareUID& uid)
{
    DIR directory;

    FILINFO fileInfo;
    fileInfo.lfname = fileNameBuffer;
    fileInfo.lfsize = sizeof(fileNameBuffer);

    char pszUID[32];

    if (MifareUIDToString(uid, pszUID) > 0)
    {
        /*search for folder*/
        if (m_modCardreader != NULL)
        {
            if (m_modCardreader->CommandFind(&directory, &fileInfo, "/music", pszUID) == true)
            {
                memset(absoluteFileNameBuffer, 0, sizeof(absoluteFileNameBuffer));
                strcat(absoluteFileNameBuffer, "/music/");
                strcat(absoluteFileNameBuffer, fileInfo.lfname);

                m_modPlayer->Play(absoluteFileNameBuffer);
            }
        }

    }
}

size_t ModuleMusicbox::MifareUIDToString(const MifareUID& uid, char* psz)
{
    int i;
    size_t charCount = 0;
    char* pszTarget = psz;
    for (i = 0; i < uid.size; i++)
    {
        char c = 0x0f & (uid.bytes[i] >> 4);
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

        c = 0x0f & uid.bytes[i];
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

void ModuleMusicbox::SetRFIDModule(ModuleRFID* module)
{
    m_modRFID = module;
}

void ModuleMusicbox::SetPlayerModule(class ModulePlayer* module)
{
    m_modPlayer = module;
}

void ModuleMusicbox::SetCardreaderModule(ModuleCardreader* module, Led* led)
{
    m_modCardreader = module;

    m_cardDetectLED = led;
}

}


#endif /* MOD_MUSICBOX */
/** @} */
