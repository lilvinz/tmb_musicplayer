/**
 * @file    src/mod_cardreader.cpp
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_cardreader.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "ch.h"
#include "targetconf.h"
#include "chprintf.h"

#include "ff.h"

#include "button.h"

namespace tmb_musicplayer
{

ModuleCardreader::ModuleCardreader()
{

}

void ModuleCardreader::Start()
{
    chDbgAssert(m_sdc != NULL, "No driver set!!");
    BaseClass::Start();
}

void ModuleCardreader::Shutdown()
{
    BaseClass::Shutdown();

    m_carddetectButton = NULL;
    m_sdc = NULL;
}

void ModuleCardreader::RegisterListener(chibios_rt::EvtListener* listener, eventmask_t mask)
{
    m_evtSource.registerMask(listener, mask);
}

void ModuleCardreader::UnregisterListener(chibios_rt::EvtListener* listener)
{
    m_evtSource.unregister(listener);
}

void ModuleCardreader::SetDriver(SDCDriver* driver)
{
    m_sdc = driver;
}

void ModuleCardreader::SetCDButton(Button* btn)
{
    m_carddetectButton = btn;
}

void ModuleCardreader::ThreadMain()
{
    chRegSetThreadName("cardReader");

    chibios_rt::EvtListener carddetectEvtListener;
    m_carddetectButton->RegisterListener(&carddetectEvtListener, EVENT_MASK(0));

    if (m_carddetectButton->GetState() == false)
    {
        OnCardInserted();
    }

    while (!chThdShouldTerminateX())
    {
        eventmask_t evt = chEvtWaitAny(ALL_EVENTS);
        if (evt & EVENT_MASK(0))
        {
            eventflags_t flags = carddetectEvtListener.getAndClearFlags();
            if (flags & Button::Up)
            {
                OnCardInserted();
            }
            else if (flags & Button::Down)
            {
                OnCardRemoved();
            }
        }
    }

    OnCardRemoved();
}

void ModuleCardreader::OnCardRemoved()
{
    chprintf(DEBUG_CANNEL, "Memory card removed.\r\n");

    UnmountFilesystem();

    m_evtSource.broadcastFlags(FilesystemUnmounted);
}

void ModuleCardreader::OnCardInserted()
{
    chprintf(DEBUG_CANNEL, "Memory card inserted.\r\n");

    if (MountFilesystem() == true)
    {
        m_evtSource.broadcastFlags(FilesystemMounted);
    }
}

bool ModuleCardreader::MountFilesystem()
{
    if (sdcConnect(m_sdc) == HAL_SUCCESS)
    {
        FRESULT err;
        err = f_mount(&m_filesystem, "/mount/", 1);
        if (err != FR_OK) {
            chprintf(DEBUG_CANNEL, "FS: f_mount() failed. Is the SD card inserted?\r\n");
            PrintFilesystemError(DEBUG_CANNEL, err);
            return false;
        }
        chprintf(DEBUG_CANNEL, "FS: f_mount() succeeded\r\n");

        return true;
    }
    chprintf(DEBUG_CANNEL, "Failed to connect sdc card.\r\n");

    return false;
}

bool ModuleCardreader::UnmountFilesystem()
{
    FRESULT err;

    err = f_mount(NULL, "/mount/", 0);

    sdcDisconnect(m_sdc);
    if (err != FR_OK) {
        chprintf(DEBUG_CANNEL, "FS: f_mount() unmount failed\r\n");
        PrintFilesystemError(DEBUG_CANNEL, err);
        return false;
    }

    chprintf(DEBUG_CANNEL, "FS: f_mount() unmount succeeded\r\n");
    return true;
}


bool ModuleCardreader::CommandCD(const char* path)
{
    DIR dir;
    FRESULT res = f_opendir(&dir, path);
    if (res == FR_OK)
    {
        return true;
    }
    chprintf(DEBUG_CANNEL, "FS: f_opendir \"%s\" failed\r\n", path);
    PrintFilesystemError(DEBUG_CANNEL, res);
    return false;
}

bool ModuleCardreader::CommandFind(DIR* dp, FILINFO* fno, const char* path, const char* pattern)
{
    FRESULT res = f_findfirst(dp, fno, path, pattern);
    if (res == FR_OK)
    {
        return true;
    }

    chprintf(DEBUG_CANNEL, "FS: f_findfirst \"%s\" in path \"%s\" failed\r\n", pattern, path);
    PrintFilesystemError(DEBUG_CANNEL, res);
    return false;
}

void ModuleCardreader::PrintFilesystemError(BaseSequentialStream* chp, FRESULT err)
{
    chprintf(chp, "\t%s.\r\n", FilesystemResultToString(err));
}

const char* ModuleCardreader::FilesystemResultToString(FRESULT stat)
{
    static const char* ErrorStrings[] = {
        "Succeeded",
        "A hard error occurred in the low level disk I/O layer",
        "Assertion failed",
        "The physical drive cannot work",
        "Could not find the file",
        "Could not find the path",
        "The path name format is invalid",
        "Access denied due to prohibited access or directory full",
        "Access denied due to prohibited access",
        "The file/directory object is invalid",
        "The physical drive is write protected",
        "The logical drive number is invalid",
        "The volume has no work area",
        "There is no valid FAT volume",
        "The f_mkfs() aborted due to any parameter error",
        "Could not get a grant to access the volume within defined period",
        "The operation is rejected according to the file sharing policy",
        "LFN working buffer could not be allocated",
        "Number of open files > _FS_SHARE",
        "Given parameter is invalid",
        "Unknown"
    };

    if (stat > FR_INVALID_PARAMETER)
    {
        return ErrorStrings[20];
    }

    return ErrorStrings[stat];
}

}


/** @} */
