/**
 * @file    src/mod_input.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_INPUT_H_
#define _MOD_INPUT_H_

#include "hal.h"
#include "module.h"
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

class ModuleInput : public Module<MOD_INPUT_THREADSIZE>
{
public:
    ModuleInput(Button** btns, size_t count);

    virtual void Start();
    virtual void Shutdown();

protected:
    typedef Module<MOD_INPUT_THREADSIZE> BaseClass;

    virtual void ThreadMain();

    virtual tprio_t GetThreadPrio() const {return MOD_INPUT_THREADPRIO;}
private:
    Button** buttons;
    size_t buttonCount;
};

}

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#endif /* _MOD_INPUT_H_ */

/** @} */
