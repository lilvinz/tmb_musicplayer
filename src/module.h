/**
 * @file    src/module.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MODULE_H_
#define _MODULE_H_

#include "ch.hpp"

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/
namespace tmb_musicplayer
{

template <int N>
class Module
{
public:
    Module()
    {

    }

    virtual void Start()
    {
        thread.SetModule(this);
        thread.start(GetThreadPrio());
    }

    virtual void Shutdown()
    {
        thread.requestTerminate();
    }

protected:
    virtual tprio_t GetThreadPrio() const = 0;

    virtual void ThreadMain() = 0;

    class ModuleThread : public chibios_rt::BaseStaticThread<N>
    {
    public:
        ModuleThread() :
            module(NULL)
        {

        }

        void SetModule(Module* mod)
        {
            module = mod;
        }

    protected:
        virtual void main()
        {
            module->ThreadMain();
        }

    private:
        Module* module;
    };

private:
    ModuleThread thread;
};

}

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#endif /* _MODULE_H_ */

/** @} */
