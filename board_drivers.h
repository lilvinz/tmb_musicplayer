/**
 * @file    board_drivers.h
 * @brief
 *
 * @{
 */

#ifndef _BOARD_DRIVERS_H_
#define _BOARD_DRIVERS_H_

#include "hal.h"
#include "mfrc522.h"

/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/
#define TMB_LED_SDCARDREAD 1
#define TMB_LED_CODECDECODE 2
#define TMB_LED_RFIDDETECT 3
#define TMB_LED_SDCARDDETECT 4
/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    stm32_gpio_t *gpio;
    uint32_t pin;
    bool clearOn;
} LEDPinConfig;

void BoardDriverInit(void);
void BoardDriverStart(void);
void BoardDriverShutdown(void);

void GetLedConfig(int16_t ledid, LEDPinConfig* pconfig);
MFRC522Driver* GetRFIDDriver(void);

#ifdef __cplusplus
}
#endif

#endif /* _LEDCONF_H_ */

/** @} */
