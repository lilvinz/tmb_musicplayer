/**
 * @file    src/mod_input.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_INPUT_H_
#define _MOD_INPUT_H_

#include "target_cfg.h"
#include "threadedmodule.h"
#include "singleton.h"

#if MOD_INPUT

#include "button.h"

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/
#ifndef MOD_INPUT_THREADSIZE
#define MOD_INPUT_THREADSIZE 128
#endif

#ifndef MOD_INPUT_THREADPRIO
#define MOD_INPUT_THREADPRIO LOWPRIO
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/
namespace tmb_musicplayer
{

class ModuleInput : public qos::ThreadedModule<MOD_INPUT_THREADSIZE>
{
public:
    ModuleInput();
    ~ModuleInput();

    virtual void Init();
    virtual void Start();
    virtual void Shutdown();

protected:
    typedef qos::ThreadedModule<MOD_INPUT_THREADSIZE> BaseClass;

    virtual tprio_t GetThreadPrio() const {return MOD_INPUT_THREADPRIO;}
    virtual void ThreadMain();

private:
    Button** buttons;
    size_t buttonCount;
};

typedef qos::Singleton<ModuleInput> ModuleInputSingeton;

}

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* MOD_INPUT */

#endif /* _MOD_INPUT_H_ */

/** @} */
