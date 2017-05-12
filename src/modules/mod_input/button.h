/**
 * @file    src/button.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "ch.hpp"
#include "hal.h"


namespace tmb_musicplayer
{

class Button
{
public:

    enum EventsFlags
    {
        Down = 1 << 0,
        Up = 1 << 1,
        Pressed = 1 << 2,
    };

    Button(ioportid_t ioportid, uint8_t p);

    void RegisterListener(chibios_rt::EvtListener* listener, eventmask_t mask);
    void UnregisterListener(chibios_rt::EvtListener* listener);

    void InitState(systime_t now);
    void Process(systime_t now);

    bool GetState() const;

private:

    static bool ReadPal(ioportid_t port, uint8_t pad);

   ioportid_t port;
   uint8_t pad;

   chibios_rt::EvtSource evtSource;

   bool lastState;
   bool state;
   systime_t lastDebounceTime;
   systime_t lastDownTime;
};

}


/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* _BUTTON_H_ */

/** @} */
