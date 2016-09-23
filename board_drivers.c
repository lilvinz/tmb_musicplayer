/**
 * @file    board_drivers.c
 * @brief
 *
 * @{
 */

#include "board_drivers.h"

#include "ch.h"
#include "hal.h"

#include "usbcfg.h"
#include "mmc.h"
#include "mod_led.h"

extern SerialUSBDriver SDU1;
extern MMCDriver MMCD1;

extern ModLED LED_ORANGE;
extern ModLED LED_GREEN;
extern ModLED LED_BLUE;
extern ModLED LED_RED;

static ModLEDConfig ledCfg1 = {GPIOD, GPIOD_LED3, false};
static ModLEDConfig ledCfg2 = {GPIOD, GPIOD_LED4, false};
static ModLEDConfig ledCfg3 = {GPIOD, GPIOD_LED5, false};
static ModLEDConfig ledCfg4 = {GPIOD, GPIOD_LED6, false};


/* Maximum speed SPI configuration (18MHz, CPHA=0, CPOL=0, MSb first).*/
static SPIConfig hs_spicfg = {NULL, GPIOC, GPIOC_PIN4, SPI_CR1_BR_1 | SPI_CR1_BR_0};

/* Low speed SPI configuration (281.250kHz, CPHA=0, CPOL=0, MSb first).*/
static SPIConfig ls_spicfg = {NULL, GPIOC, GPIOC_PIN4, SPI_CR1_BR_2 | SPI_CR1_BR_1};

/* MMC/SD over SPI driver configuration.*/
static MMCConfig MMCD1cfg = {&SPID2, &ls_spicfg, &hs_spicfg};


void BoardDriverInit(void)
{
    mod_led_init(&LED_ORANGE, &ledCfg1);
    mod_led_init(&LED_GREEN, &ledCfg2);
    mod_led_init(&LED_RED, &ledCfg3);
    mod_led_init(&LED_BLUE, &ledCfg4);

    /*mmc pin configuratio*/
    palSetPadMode(GPIOC, GPIOC_PIN4, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOC, GPIOC_PIN5, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOB, GPIOB_PIN13, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST );
    palSetPadMode(GPIOB, GPIOB_PIN14, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(GPIOB, GPIOB_PIN15, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);

    sduObjectInit(&SDU1);
    mmcObjectInit(&MMCD1);
}

void BoardDriverStart(void)
{
    mmcStart(&MMCD1, &MMCD1cfg);
    /*
     * Initializes a serial-over-USB CDC driver.
     */
    sduStart(&SDU1, &serusbcfg);

    /*
     * Activates the USB driver and then the USB bus pull-up on D+.
     * Note, a delay is inserted in order to not have to disconnect the cable
     * after a reset.
     */
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1000);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);
}

void BoardDriverShutdown(void)
{
    sduStop(&SDU1);
    mmcStop(&MMCD1);
}

/** @} */
