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

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/
#define CARDREADER_EVENT_MOUNTED 1
#define CARDREADER_EVENT_UNMOUNTED 2

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
    SDCDriver* sdc;
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

  event_source_t* mod_cardreader_eventscource(void);

  void mod_cardreader_ls(BaseSequentialStream *chp, char* path);
  bool mod_cardreader_cd(const char* path);
  bool mod_cardreader_find(DIR* dp, FILINFO* fno, const char* path, const char* pattern);
#ifdef __cplusplus
}
#endif

#endif /* _MOD_CARDREADER_H_ */

/** @} */
