/**
 * @file    src/mod_cardreader.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_CARDREADER_H_
#define _MOD_CARDREADER_H_

#include "target_cfg.h"
#include "threadedmodule.h"
#include "singleton.h"

#if MOD_CARDREADER

#include "ff.h"

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/
#ifndef MOD_CARDREADER_THREADSIZE
#define MOD_CARDREADER_THREADSIZE 512
#endif

#ifndef MOD_CARDREADER_THREADPRIO
#define MOD_CARDREADER_THREADPRIO LOWPRIO
#endif

namespace tmb_musicplayer
{

class ModuleCardreader : public qos::ThreadedModule<MOD_CARDREADER_THREADSIZE>
{
public:

    enum EventsFlags
    {
        FilesystemMounted = 1 << 0,
        FilesystemUnmounted = 1 << 1,
    };

    ModuleCardreader();
    ~ModuleCardreader();

    virtual void Init();
    virtual void Start();
    virtual void Shutdown();

    void RegisterListener(chibios_rt::EvtListener* listener, eventmask_t mask);
    void UnregisterListener(chibios_rt::EvtListener* listener);

    //Filesystem commands
    bool CommandCD(const char* path);
    bool CommandFind(DIR* dp, FILINFO* fno, const char* path, const char* pattern);

protected:
    typedef qos::ThreadedModule<MOD_CARDREADER_THREADSIZE> BaseClass;

    virtual void ThreadMain();

    virtual tprio_t GetThreadPrio() const {return MOD_CARDREADER_THREADPRIO;}

private:

    void OnCardRemoved();
    void OnCardInserted();

    bool UnmountFilesystem();
    bool MountFilesystem();

    void SetCardDetectLed(bool on);

    static void PrintFilesystemError(BaseSequentialStream *chp, FRESULT err);
    static const char* FilesystemResultToString(FRESULT stat);

    chibios_rt::EvtSource m_evtSource;
    FATFS m_filesystem;
};
typedef qos::Singleton<ModuleCardreader> ModuleCardreaderSingelton;
}

#endif /* MOD_CARDREADER */
#endif /* _MOD_CARDREADER_H_ */

/** @} */
