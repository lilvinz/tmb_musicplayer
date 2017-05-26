/**
 * @file    src/mod_player.h
 *
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_PLAYER_H_
#define _MOD_PLAYER_H_

#include "target_cfg.h"
#include "threadedmodule.h"
#include "singleton.h"


#if MOD_PLAYER


/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/
#ifndef MOD_MUSICPLAYER_DATAPUMP_THREADSIZE
#define MOD_MUSICPLAYER_DATAPUMP_THREADSIZE 2028
#endif

#ifndef MOD_MUSICPLAYER_DATAPUMP_THREADPRIO
#define MOD_MUSICPLAYER_DATAPUMP_THREADPRIO NORMALPRIO
#endif

#ifndef MOD_PLAYER_THREADSIZE
#define MOD_PLAYER_THREADSIZE 1540
#endif

#ifndef MOD_PLAYER_THREADPRIO
#define MOD_PLAYER_THREADPRIO LOWPRIO
#endif

#ifndef MOD_PLAYER_CMD_QUEUE_SIZE
#define MOD_PLAYER_CMD_QUEUE_SIZE 2
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

namespace tmb_musicplayer
{

class ModulePlayer : public qos::ThreadedModule<MOD_PLAYER_THREADSIZE>
{
public:

    enum EventsFlags
    {
        EventPlay = 1 << 0,
        EventPause = 1 << 1,
        EventStop = 1 << 2,
        EventNext = 1 << 3,
        EventPrev = 1 << 4
    };

    ModulePlayer();
    ~ModulePlayer();

    virtual void Init();
    virtual void Start();
    virtual void Shutdown();

    void Play(const char* path);
    void Toggle(void);
    void Stop(void);
    void Next(void);
    void Prev(void);
    void Volume(uint8_t volume);

    void RegisterListener(chibios_rt::EvtListener* listener, eventmask_t mask);
    void UnregisterListener(chibios_rt::EvtListener* listener);


protected:
    typedef qos::ThreadedModule<MOD_PLAYER_THREADSIZE> BaseClass;

    enum CommandEventFlags
    {
        PLAY_FILE = 1,
        STOP = 1 << 1,
        PAUSE = 1 << 2,
        NEXT = 1 << 3,
        PREV = 1 << 4,
    };

    virtual void ThreadMain();
    virtual tprio_t GetThreadPrio() const {return MOD_PLAYER_THREADPRIO;}

private:

    static bool QueryCurrentFilename(uint16_t wantedFileId, char* pszFileNameBuffer);
    static bool FindFileWithID(uint16_t wantedFileId, uint16_t& folderStartId, char* pszFileNameBuffer);

    enum State
    {
        StateIdle = 0,
        StatePlay,
        StatePause,
        StateNext,
        StatePrev,
    };

    class PumpThread : public chibios_rt::BaseStaticThread<MOD_MUSICPLAYER_DATAPUMP_THREADSIZE>
    {
    public:
        PumpThread();

        void StartTransfer();
        void PauseTransfer();
        void StopTransfer();
        void SetVolume(uint8_t volume);

        void SetPlayerThread(chibios_rt::BaseThread* thread)
        {
            m_playerThread = thread;
        }

       char* AccessPathBuffer() {return m_pathbuffer;}

       void SetBasePath(const char* path);
       void ResetPathtoBase();
       void ResetPath();

    protected:
        virtual void main();

    private:

        void SignalReadActionOn();
        void SignalReadActionOff();

        void SignalDecodeActionOn();
        void SignalDecodeActionOff();

        char m_pathbuffer[512];
        uint16_t basePathEndIdx = 0;

        bool m_pump = false;
        bool m_pausePump = false;
        uint8_t m_volume = 0;
        chibios_rt::Mutex m_codecMutex;
        chibios_rt::BaseThread* m_playerThread;
    };

    class Message
    {
    public:
        eventmask_t evtMask;
        char fileName[128];
        uint8_t volume;
    };

    PumpThread m_pumpThread;

    chibios_rt::EvtSource m_evtSource;
    chibios_rt::ObjectsPool<Message, MOD_PLAYER_CMD_QUEUE_SIZE> m_MsgObjectPool;
    chibios_rt::Mailbox<Message*, MOD_PLAYER_CMD_QUEUE_SIZE> m_Mailbox;
};

typedef qos::Singleton<ModulePlayer> ModulePlayerSingelton;

}

#endif /* MOD_PLAYER */

#endif /* _MOD_PLAYER_H_ */

/** @} */
