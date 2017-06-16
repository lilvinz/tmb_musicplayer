/**
 * @file    src/mod_player.c
 * @brief
 *
 * @addtogroup
 * @{
 */
#include "mod_player.h"

#if MOD_PLAYER

#include "ch_tools.h"
#include "watchdog.h"
#include "module_init_cpp.h"

#include "qhal.h"
#include "chprintf.h"

#include "ff.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mod_effects.h"

template <>
tmb_musicplayer::ModulePlayer tmb_musicplayer::ModulePlayerSingelton::instance = tmb_musicplayer::ModulePlayer();

#define EVENTMASK_PUMPTHREAD_STOP EVENT_MASK(0)
#define EVENTMASK_PUMPTHREAD_START EVENT_MASK(1)
#define EVENTMASK_PUMPTHREAD_ABORT EVENT_MASK(2)
#define EVENTMASK_COMMAND_PLAY EVENT_MASK(3)
#define EVENTMASK_COMMAND_STOP EVENT_MASK(4)
#define EVENTMASK_COMMAND_VOLUME EVENT_MASK(5)
#define EVENTMASK_COMMAND_PAUSE EVENT_MASK(6)
#define EVENTMASK_MAIL EVENT_MASK(7)

namespace tmb_musicplayer
{

ModulePlayer::ModulePlayer()
{

}

ModulePlayer::~ModulePlayer()
{

}

void ModulePlayer::Init()
{
    watchdog_register(WATCHDOG_MOD_PLAYER_PUMP);
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

    State state = StateIdle;
    bool hasNewTitle = false;
    char m_pathbuffer[512];
    while (chThdShouldTerminateX() == false)
    {
        eventmask_t evt = chEvtWaitAny(ALL_EVENTS);
        if (evt & EVENTMASK_PUMPTHREAD_START)
        {
            state = StatePlay;
            m_evtSource.broadcastFlags(StatePlay);
        }
        else if (evt & EVENTMASK_PUMPTHREAD_STOP)
        {
            state = StateIdle;
            m_evtSource.broadcastFlags(EventStop);
        }
        else if (evt & EVENTMASK_PUMPTHREAD_ABORT)
        {
            m_evtSource.broadcastFlags(EventAbort);
            if (hasNewTitle) {
                m_pumpThread.SetBasePath(m_pathbuffer);
                chprintf(DEBUG_CANNEL, "ModulePlayer: play file %s.\r\n", m_pumpThread.AccessPathBuffer());
                m_pumpThread.StartTransfer();
            }
            else
            {
                state = StateIdle;
            }
            hasNewTitle = false;
        }
        else if (evt & EVENTMASK_MAIL)
        {
            /* Processing the event.*/
            Message* msg = NULL;
            while (m_Mailbox.fetch(&msg, TIME_IMMEDIATE) == MSG_OK)
            {
                if (msg->evtMask & EVENTMASK_COMMAND_PLAY)
                {
                   if (state != StatePlay) {
                       char* basePath = m_pumpThread.AccessPathBuffer();
                       strcpy(basePath, msg->fileName);
                       m_pumpThread.SetBasePath(msg->fileName);
                       chprintf(DEBUG_CANNEL, "ModulePlayer: play file %s.\r\n", m_pumpThread.AccessPathBuffer());
                       m_pumpThread.StartTransfer();
                   } else {
                       memset(m_pathbuffer, 0, sizeof(m_pathbuffer));
                       strcpy(m_pathbuffer, msg->fileName);
                       hasNewTitle = true;
                       m_pumpThread.StopTransfer();
                   }

                }
                else if (msg->evtMask & EVENTMASK_COMMAND_VOLUME)
                {
                    chprintf(DEBUG_CANNEL, "ModulePlayer: set volume %d.\r\n", msg->volume);
                    m_pumpThread.SetVolume(msg->volume);
                }

                m_MsgObjectPool.free(msg);


            }
        }
        else if (evt & EVENTMASK_COMMAND_PLAY)
        {
        }
        else if (evt & EVENTMASK_COMMAND_STOP)
        {
            state = StateIdle;
            m_pumpThread.StopTransfer();
        }
        else if (evt & EVENTMASK_COMMAND_PAUSE)
        {
            if (state == StatePause)
            {
                char* basePath = m_pumpThread.AccessPathBuffer();
                if (*basePath != 0)
                {
                    state = StatePlay;
                    m_pumpThread.StartTransfer();
                    m_evtSource.broadcastFlags(EventPlay);
                }
            }
            else if (state == StatePlay)
            {
                state = StatePause;
                m_pumpThread.PauseTransfer();
                m_evtSource.broadcastFlags(EventPause);
            }
            else if (state == StateIdle)
            {
                char* basePath = m_pumpThread.AccessPathBuffer();
                chprintf(DEBUG_CANNEL, "ModulePlayer: play file %s.\r\n", basePath);
                m_pumpThread.StartTransfer();
            }
        }
    }
}

void ModulePlayer::RegisterListener(chibios_rt::EvtListener* listener, eventmask_t mask)
{
    m_evtSource.registerMask(listener, mask);
}

void ModulePlayer::UnregisterListener(chibios_rt::EvtListener* listener)
{
    m_evtSource.unregister(listener);
}

void ModulePlayer::Play(const char* path)
{
    if (path == NULL)
    {
        m_moduleThread.signalEvents(EVENTMASK_COMMAND_STOP);
    }
    else
    {
        Message* msg = (Message*)m_MsgObjectPool.alloc();
        if (msg != NULL)
        {
            msg->evtMask = EVENTMASK_COMMAND_PLAY;
            strcpy(msg->fileName, path);
            if (m_Mailbox.post(msg, MS2ST(1)) == MSG_OK)
            {
                m_moduleThread.signalEvents(EVENTMASK_MAIL);
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
    m_moduleThread.signalEvents(EVENTMASK_COMMAND_PAUSE);
}

void ModulePlayer::Stop(void)
{
    m_moduleThread.signalEvents(EVENTMASK_COMMAND_STOP);
}

void ModulePlayer::Volume(uint8_t volume)
{
//    m_codecMutex.lock();
//    VS1053SetVolume(m_codec, volume, volume);
//    m_codecMutex.lock();
    Message* msg = (Message*)m_MsgObjectPool.alloc();
    if (msg != NULL)
    {
        msg->evtMask = EVENTMASK_COMMAND_VOLUME;
        msg->volume = volume;
        if (m_Mailbox.post(msg, MS2ST(1)) == MSG_OK)
        {
            m_moduleThread.signalEvents(EVENTMASK_MAIL);
        }
        else
        {
            m_MsgObjectPool.free(msg);
        }
    }
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

void ModulePlayer::PumpThread::SetVolume(uint8_t volume)
{
    chibios_rt::System::lock();
    m_volume = volume;
    chibios_rt::System::unlock();

    m_codecMutex.lock();
    {
        VS1053SetVolume(CODEC, volume, volume);
    }
    m_codecMutex.unlock();
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

void ModulePlayer::PumpThread::SignalReadActionOn()
{
#if HAL_USE_LED
    ledOn(LED_READ);
#endif
}

void ModulePlayer::PumpThread::SignalReadActionOff()
{
#if HAL_USE_LED
    ledOff(LED_READ);
#endif
}

void ModulePlayer::PumpThread::SignalDecodeActionOn()
{
#if HAL_USE_LED
    ledOn(LED_DECODE);
#endif
}

void ModulePlayer::PumpThread::SignalDecodeActionOff()
{
#if HAL_USE_LED
    ledOff(LED_DECODE);
#endif
}


void ModulePlayer::PumpThread::main()
{
    chRegSetThreadName("playerPump");

    char Buffer[32];
    UINT ByteToRead = sizeof(Buffer);
    UINT ByteRead;

    FIL fsrc;
    bool pumpData = false;
    while (chThdShouldTerminateX() == false)
    {
        watchdog_reload(WATCHDOG_MOD_PLAYER_PUMP);
        chibios_rt::System::lock();
        pumpData = m_pump;
        chibios_rt::System::unlock();
        bool aborted = true;
        if (pumpData == true)
        {
            FRESULT err = f_open(&fsrc, m_pathbuffer, FA_READ);
            if (err == FR_OK)
            {
                bool bReadStreamHeader = true;
                uint16_t headerDater[2];
                uint16_t codecStatus;
                uint32_t byteTransferred = 0;

                m_playerThread->signalEvents(EVENTMASK_PUMPTHREAD_START);
                /*
                 * Do while the number of bytes read is equal to the number of bytes to read
                 * (the buffer is filled)
                 */
                do
                {
                    watchdog_reload(WATCHDOG_MOD_PLAYER_PUMP);

                    chibios_rt::System::lock();
                    pumpData = m_pump;
                    bool pausePump = m_pausePump;
                    chibios_rt::System::unlock();

                    aborted = true;
                    if (pumpData == false)
                    {
                        if (pausePump ==  true)
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
                        aborted = false;
                        /*
                         * Clear the buffer.
                         */
                        memset(Buffer, 0, sizeof(Buffer));
                        /*
                         * Read the file.
                         */
                        SignalReadActionOn();
                        err = f_read(&fsrc, Buffer, ByteToRead, &ByteRead);
                        SignalReadActionOff();
                        if (err == FR_OK && ByteRead > 0)
                        {
                            SignalDecodeActionOn();

                            m_codecMutex.lock();
                            {
                                if (VS1053SendData(CODEC, Buffer, ByteRead) != ByteRead)
                                {
                                    ByteRead = 0;
                                }
                                codecStatus = VS1053ReadStatus(CODEC);
                            }
                            m_codecMutex.unlock();

                            SignalDecodeActionOff();

                            byteTransferred = byteTransferred + ByteRead;

                            if (bReadStreamHeader == true)
                            {
                                m_codecMutex.lock();
                                {
                                    VS1053ReadHeaderData(CODEC, headerDater, headerDater + 1);
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
                    VS1053StopPlaying(CODEC);
                }
                m_codecMutex.unlock();
            }

            chibios_rt::System::lock();
            m_pump = false;
            chibios_rt::System::unlock();

            if (aborted) {
                m_playerThread->signalEvents(EVENTMASK_PUMPTHREAD_ABORT);
            } else {
                m_playerThread->signalEvents(EVENTMASK_PUMPTHREAD_STOP);
            }
        }
        chThdSleep(MS2ST(100));
    }
}

}

MODULE_INITCALL(4, qos::ModuleInit<tmb_musicplayer::ModulePlayerSingelton>::Init,
        qos::ModuleInit<tmb_musicplayer::ModulePlayerSingelton>::Start,
        qos::ModuleInit<tmb_musicplayer::ModulePlayerSingelton>::Shutdown)

#endif /* MOD_PLAYER */
/** @} */
