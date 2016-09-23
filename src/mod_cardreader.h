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

/**
 * @brief   Cardreader descriptor type.
 */
typedef struct {
    MMCDriver* mmc;
    ModLED* ledCardDetect;

    /*Card detection pin*/
    stm32_gpio_t *gpio;
    uint32_t pin;
    bool setOn;
} CardReaderConfig;

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void mod_cardreader_init(CardReaderConfig* cfgp);
  bool mod_cardreader_start(void);
  void mod_cardreader_stop(void);
#ifdef __cplusplus
}
#endif

#endif /* _MOD_CARDREADER_H_ */

/** @} */
