/**
 * @file    src/mod_rfid.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_RFID_H_
#define _MOD_RFID_H_

#include "hal.h"
#include "mod_led.h"
#include "mfrc522.h"

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/
#ifndef MOD_RFID_THREADSIZE
#define MOD_RFID_THREADSIZE 256
#endif

#ifndef MOD_RFID_THREADPRIO
#define MOD_RFID_THREADPRIO LOWPRIO
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   RFID descriptor type.
 */
typedef struct {
    MFRC522Driver* mfrcd;
    ModLED* ledCardDetect;
} RFIDConfig;

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void mod_rfid_init(RFIDConfig* cfgp);
  bool mod_rfid_start(void);
  void mod_rfid_stop(void);
#ifdef __cplusplus
}
#endif

#endif /* _MOD_RFID_H_ */

/** @} */
