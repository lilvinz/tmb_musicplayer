/**
 * @file    src/mod_cardreader.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_CARDREADER_H_
#define _MOD_CARDREADER_H_

#include "hal.h"
#include "mod_led.h"
#include "ff.h"
#include "module.h"

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/
#ifndef MOD_CARDREADER_THREADSIZE
#define MOD_CARDREADER_THREADSIZE 512
#endif

#ifndef MOD_CARDREADER_THREADPRIO
#define MOD_CARDREADER_THREADPRIO LOWPRIO
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/
namespace tmb_musicplayer
{

class ModuleCardreader : public Module<MOD_CARDREADER_THREADSIZE>
{
public:
    enum EventsFlags
    {
        FilesystemMounted = 1 << 0,
        FilesystemUnmounted = 1 << 1,
    };

    ModuleCardreader();

    virtual void Start();
    virtual void Shutdown();

    void SetDriver(SDCDriver* driver);
    void SetCDButton(class Button* btn);

    void RegisterListener(chibios_rt::EvtListener* listener, eventmask_t mask);
    void UnregisterListener(chibios_rt::EvtListener* listener);

    //Filesystem commands
    bool CommandCD(const char* path);
    bool CommandFind(DIR* dp, FILINFO* fno, const char* path, const char* pattern);

protected:
    typedef Module<MOD_CARDREADER_THREADSIZE> BaseClass;

    virtual void ThreadMain();

    virtual tprio_t GetThreadPrio() const {return MOD_CARDREADER_THREADPRIO;}

private:

    void OnCardRemoved();
    void OnCardInserted();

    bool UnmountFilesystem();
    bool MountFilesystem();

    static void PrintFilesystemError(BaseSequentialStream *chp, FRESULT err);
    static const char* FilesystemResultToString(FRESULT stat);

    class Button* m_carddetectButton;
    SDCDriver* m_sdc;
    chibios_rt::EvtSource m_evtSource;
    FATFS m_filesystem;
};
}
/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* _MOD_CARDREADER_H_ */

/** @} */
