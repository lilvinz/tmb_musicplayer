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

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/
#define BUTTON_DOWN 1
#define BUTTON_UP 2
#define BUTTON_PRESSED 4

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

typedef struct {
    ioportid_t port;
    uint8_t pad;
} ButtonConfig;

typedef struct {
    ButtonConfig cfg;
    event_source_t eventSource;
    bool lastState;
    bool state;
    systime_t lastDebounceTime;
    systime_t lastDownTime;
} Button;


/**
 * @brief
 */
typedef struct {
    Button* pButtons;
    uint32_t buttonCount;
} ModInputConfig;



/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void mod_input_init(ModInputConfig* config);
  bool mod_input_start(void);
  void mod_input_shutdown(void);
#ifdef __cplusplus
}
#endif

#endif /* _MOD_LED_H_ */

/** @} */
