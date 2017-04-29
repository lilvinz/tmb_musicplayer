/**
 * @file    src/mod_player.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ch.h"
#include "targetconf.h"
#include "chprintf.h"

#include "ff.h"
#include "mod_player.h"
#include "mod_led.h"

#define EVENTMASK_COMMANDS EVENT_MASK(0)
#define EVENTMASK_CODEC EVENT_MASK(1)
#define EVENTMASK_PUMPTHREAD_STOP EVENT_MASK(2)

namespace tmb_musicplayer
{

ModulePlayer::ModulePlayer()
{

}

void ModulePlayer::Start()
{
    BaseClass::Start();

    m_pumpThread.SetPlayerThread(&m_moduleThread);
    m_pumpThread.start(MOD_MUSICPLAYER_DATAPUMP_THREADPRIO);
}

void ModulePlayer::Shutdown()
{
    m_pumpThread.requestTerminate();
    BaseClass::Shutdown();
}

void ModulePlayer::ThreadMain()
{
    chRegSetThreadName("player");

    chibios_rt::EvtListener commandListener;
    m_eventSource.registerMask(&commandListener, EVENTMASK_COMMANDS);

    State state = StateIdle;

    uint16_t currentFileInDirectory = 0;

    while (chThdShouldTerminateX() == false)
    {
        eventmask_t evt = chEvtWaitAny(ALL_EVENTS);
        if (evt & EVENTMASK_PUMPTHREAD_STOP)
        {
            bool startPlaying = false;
            if (state == StateNext || state == StatePlay)
            {
                ++currentFileInDirectory;
                startPlaying = true;
            }
            else if (state == StatePrev && currentFileInDirectory > 0)
            {
                --currentFileInDirectory;
                startPlaying = true;
            }

            if (startPlaying == true)
            {
                m_pumpThread.ResetPathtoBase();
                char* basePath = m_pumpThread.AccessPathBuffer();
                if (QueryCurrentFilename(currentFileInDirectory, basePath) == true)
                {
                    state = StatePlay;
                    m_pumpThread.StartTransfer();
                }
                else
                {
                    state = StateIdle;
                }
            }
            else
            {
                state = StateIdle;
            }
        }
        else if (evt & EVENTMASK_COMMANDS)
        {
            eventflags_t flags = commandListener.getAndClearFlags();
            if (flags & PLAY_FILE)
            {
                /* Processing the event.*/
                FileNameMsg* msg = NULL;

                while (m_Mailbox.fetch(&msg, TIME_IMMEDIATE) == MSG_OK)
                {
                    m_pumpThread.SetBasePath(msg->fileName);
                    m_MsgObjectPool.free(msg);

                    currentFileInDirectory = 0;

                    char* basePath = m_pumpThread.AccessPathBuffer();
                    if (QueryCurrentFilename(currentFileInDirectory, basePath) == true)
                    {
                        state = StatePlay;
                        m_pumpThread.StartTransfer();
                    }
                }
            }
            if (flags & STOP)
            {
                state = StateIdle;
                m_pumpThread.StopTransfer();
            }
            if (flags & PAUSE)
            {
                if (state == StatePause)
                {
                    char* basePath = m_pumpThread.AccessPathBuffer();
                    if (*basePath != 0)
                    {
                        state = StatePlay;
                        m_pumpThread.StartTransfer();
                    }
                }
                else if (state == StatePlay)
                {
                    state = StatePause;
                    m_pumpThread.StopTransfer();
                }
                else if (state == StateIdle)
                {
                    currentFileInDirectory = 0;
                    m_pumpThread.ResetPathtoBase();
                    char* basePath = m_pumpThread.AccessPathBuffer();
                    if (QueryCurrentFilename(currentFileInDirectory, basePath) == true)
                    {
                        state = StatePlay;
                        m_pumpThread.StartTransfer();
                    }
                }
            }
            if (flags & NEXT)
            {
                if (state == StatePlay)
                {
                    state = StateNext;
                    m_pumpThread.StopTransfer();
                }
            }
            if (flags & PREV)
            {
                if (state == StatePlay)
                {
                    state = StatePrev;
                    m_pumpThread.StopTransfer();
                }
            }
        }
    }

    m_eventSource.unregister(&commandListener);
}

void ModulePlayer::Play(const char* path)
{
    if (path == NULL)
    {
        m_eventSource.broadcastFlags(PLAY_FILE);
    }
    else
    {
        FileNameMsg* msg = (FileNameMsg*)m_MsgObjectPool.alloc();
        if (msg != NULL)
        {
            strcpy(msg->fileName, path);
            if (m_Mailbox.post(msg, MS2ST(1)) == MSG_OK)
            {
                m_eventSource.broadcastFlags(PLAY_FILE);
            }
            else
            {
                m_MsgObjectPool.free(msg);
            }
        }
    }
}

void ModulePlayer::Toggle(void)
{
    m_eventSource.broadcastFlags(PAUSE);
}

void ModulePlayer::Stop(void)
{
    m_eventSource.broadcastFlags(STOP);
}

void ModulePlayer::Next(void)
{
    m_eventSource.broadcastFlags(NEXT);
}

void ModulePlayer::Prev(void)
{
    m_eventSource.broadcastFlags(PREV);
}

void ModulePlayer::Volume(uint8_t volume)
{
//    m_codecMutex.lock();
//    VS1053SetVolume(m_codec, volume, volume);
//    m_codecMutex.lock();
}

bool ModulePlayer::QueryCurrentFilename(uint16_t wantedFileId, char* path)
{
    uint16_t startId = 0;
    return FindFileWithID(wantedFileId, startId, path);
}

bool ModulePlayer::FindFileWithID(uint16_t wantedFileId, uint16_t& folderStartId, char* pszFileNameBuffer)
{
    char fileNameBuffer[80];
    FILINFO fno;
    DIR dir;

    fno.lfname = fileNameBuffer;
    fno.lfsize = sizeof(fileNameBuffer);

    char *fn;
    int i = strlen(pszFileNameBuffer);
    uint16_t fileId = folderStartId;
    FRESULT res = f_opendir(&dir, pszFileNameBuffer);
    if (res == FR_OK)
    {
        while (true)
        {
            FRESULT res = f_readdir(&dir, &fno);
            /*
             * If the directory read failed or the
             */
            if (res != FR_OK || (fno.lfname[0] == 0 && fno.fname[0] == 0))
            {
                f_closedir(&dir);
                folderStartId = fileId;
                return false;
            }

            fn = fno.lfname;
            if (fno.lfname[0] == 0)
            {
                fn = fno.fname;
            }
            /*
             * If the directory or file begins with a '.' (hidden), continue
             */
            if (fn[0] == '.')
            {
                continue;
            }
            /*
             * If the 'file' is a directory.
             */
            if (fno.fattrib & AM_DIR)
            {
                folderStartId = fileId;
                pszFileNameBuffer[i++] = '/';
                strcpy(&pszFileNameBuffer[i], fn);
                if (FindFileWithID(wantedFileId, folderStartId, pszFileNameBuffer) == true)
                {
                    f_closedir(&dir);
                    fileId = folderStartId;
                    return true;
                }
                fileId = folderStartId;
            }
            else
            {
                if (fileId == wantedFileId)
                {
                    pszFileNameBuffer[i++] = '/';
                    strcpy(&pszFileNameBuffer[i], fn);
                    f_closedir(&dir);
                    folderStartId = fileId;
                    return true;
                }
                ++fileId;
            }
        }
    }
    folderStartId = fileId;
    return false;
}

ModulePlayer::PumpThread::PumpThread()
{

}

void ModulePlayer::PumpThread::StartTransfer()
{
    chibios_rt::System::lock();
    m_pump = true;
    m_pausePump = false;
    chibios_rt::System::unlock();
}

void ModulePlayer::PumpThread::PauseTransfer()
{
    chibios_rt::System::lock();
    m_pump = false;
    m_pausePump = true;
    chibios_rt::System::unlock();
}

void ModulePlayer::PumpThread::StopTransfer()
{
    chibios_rt::System::lock();
    m_pump = false;
    m_pausePump = false;
    chibios_rt::System::unlock();
}

void ModulePlayer::PumpThread::SetBasePath(const char* path)
{
    memset(m_pathbuffer, 0, sizeof(m_pathbuffer));
    strcpy(m_pathbuffer, path);
    basePathEndIdx = strlen(path);
}

void ModulePlayer::PumpThread::ResetPathtoBase()
{
    memset(m_pathbuffer + basePathEndIdx, 0, sizeof(m_pathbuffer) - basePathEndIdx);
}

void ModulePlayer::PumpThread::ResetPath()
{
    memset(m_pathbuffer, 0, sizeof(m_pathbuffer));
    basePathEndIdx = 0;
}

void ModulePlayer::PumpThread::main()
{
    chDbgAssert(m_codec != NULL, "No codec set!!");
    chRegSetThreadName("playerPump");

    char Buffer[32];
    UINT ByteToRead = sizeof(Buffer);
    UINT ByteRead;

    FIL fsrc;
    bool pumpData = false;
    while (chThdShouldTerminateX() == false)
    {
        chibios_rt::System::lock();
        pumpData = m_pump;
        chibios_rt::System::unlock();
        if (pumpData == true)
        {
            FRESULT err = f_open(&fsrc, m_pathbuffer, FA_READ);
            if (err == FR_OK)
            {
                bool bReadStreamHeader = true;
                uint16_t headerDater[2];
                uint16_t codecStatus;
                uint32_t byteTransferred = 0;
                /*
                 * Do while the number of bytes read is equal to the number of bytes to read
                 * (the buffer is filled)
                 */
                do
                {
                    if (m_pump == false)
                    {
                        if (m_pausePump ==  true)
                        {
                            /*pause*/
                            chThdSleep(MS2ST(1));
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        /*
                         * Clear the buffer.
                         */
                        memset(Buffer, 0, sizeof(Buffer));
                        /*
                         * Read the file.
                         */
                        m_readLED->On();
                        err = f_read(&fsrc, Buffer, ByteToRead, &ByteRead);
                        m_readLED->Off();
                        if (err == FR_OK && ByteRead > 0)
                        {
                            m_decodeLED->On();

                            m_codecMutex.lock();
                            {
                                if (VS1053SendData(m_codec, Buffer, ByteRead) != ByteRead)
                                {
                                    ByteRead = 0;
                                }
                                codecStatus = VS1053ReadStatus(m_codec);
                            }
                            m_codecMutex.unlock();

                            m_decodeLED->Off();

                            byteTransferred = byteTransferred + ByteRead;

                            if (bReadStreamHeader == true)
                            {
                                m_codecMutex.lock();
                                {
                                    VS1053ReadHeaderData(m_codec, headerDater, headerDater + 1);
                                }
                                m_codecMutex.unlock();

                                bool formatUnknown = false;
                                if (headerDater[1] > 0xFFE0)
                                {
                                    //mp3 file
                                }
                                else if (headerDater[1] > 0x7665) // "ve"
                                {
                                    //wav file
                                }
                                else if (headerDater[1] > 0x4154) // "AT"
                                {
                                    //AAC ADTSF file
                                }
                                else if (headerDater[1] > 0x4144) // "AD"
                                {
                                    //AAC .ADIF file
                                }
                                else if (headerDater[1] > 0x4D34) // "M4"
                                {
                                    //AAC .mp4 file
                                }
                                else if (headerDater[1] > 0x574D) // "WM"
                                {
                                    //WMA file
                                }
                                else if (headerDater[1] > 0x4D54) // "MT"
                                {
                                    //Midi file
                                }
                                else if (headerDater[1] > 0x4F67) // "Og"
                                {
                                    //Ogg Vorbis file
                                }
                                else
                                {
                                    //unknow
                                    formatUnknown = true;
                                }
                                bReadStreamHeader = VS1053CanJump(codecStatus)
                                        && !formatUnknown;
                            }

                        }
                    }
                } while (ByteRead >= ByteToRead);

                f_close(&fsrc);

                m_codecMutex.lock();
                {
                    VS1053StopPlaying(m_codec);
                }
                m_codecMutex.unlock();

                if (err == FR_OK)
                {
                    chibios_rt::System::lock();
                    m_pump = false;
                    chibios_rt::System::unlock();
                    m_playerThread->signalEvents(EVENTMASK_PUMPTHREAD_STOP);
                }
            }
        }
        chThdSleep(MS2ST(100));
    }
}

}


/** @} */
