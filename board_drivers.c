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
#include "mfrc522.h"
#include "vs1053.h"
#include "mod_led.h"

extern SerialUSBDriver SDU1;
extern SDCDriver SDCD1;
extern MFRC522Driver RFID1;
extern VS1053Driver VS1053D1;

extern ModLED LED_ORANGE;
extern ModLED LED_GREEN;
extern ModLED LED_BLUE;
extern ModLED LED_RED;

static ModLEDConfig ledCfg1 = {GPIOD, GPIOD_LED3, false};
static ModLEDConfig ledCfg2 = {GPIOD, GPIOD_LED4, false};
static ModLEDConfig ledCfg3 = {GPIOD, GPIOD_LED5, false};
static ModLEDConfig ledCfg4 = {GPIOD, GPIOD_LED6, false};

/*
 * Working area for driver.
 */
static uint8_t sd_scratchpad[512];

/*
 * SDIO configuration.
 */
static const SDCConfig sdccfg = {
  sd_scratchpad,
  SDC_MODE_4BIT
};

/*
 * SPI1 configuration structure.
 * Speed 5.25MHz, CPHA=1, CPOL=1, 8bits frames, MSb transmitted first.
 * The slave select line is the pin GPIOA_LRCK on the port GPIOA.
 */
static const SPIConfig SPI1cfg = {
  NULL,
  /* HW dependent part.*/
  GPIOC,
  GPIOC_PIN4,
  SPI_CR1_BR_0 | SPI_CR1_BR_1
};

static const SPIConfig SPI2cfg = {
  NULL,
  /* HW dependent part.*/
  GPIOD,
  12U,
  SPI_CR1_BR_0 | SPI_CR1_BR_1
};


/*
 * VSConfiguration
 */

static VS1053Config VS1053D1_cfg =
{
    .spid = &SPID2,
    .spiCfg = &SPI2cfg,
    .xResetPort = GPIOD,
    .xResetPad = 10U,
    .xCSPort = GPIOD,
    .xCSPad = 11U,
    .xDCSPort = GPIOD,
    .xDCSPad = 8U,
    .xDREQPort = GPIOD,
    .xDREQPad = 9U,
};

/*
 * RFID configuration.
 */
static MFRC522Config RFID1_cfg =
{

};

void BoardDriverInit(void)
{
    mod_led_init(&LED_ORANGE, &ledCfg1);
    mod_led_init(&LED_GREEN, &ledCfg2);
    mod_led_init(&LED_RED, &ledCfg3);
    mod_led_init(&LED_BLUE, &ledCfg4);

    /*vs1053 pin configuratio*/
    palSetPadMode(GPIOB, GPIOB_PIN13, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST );
    palSetPadMode(GPIOB, GPIOB_PIN14, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(GPIOB, GPIOB_PIN15, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(GPIOD, 8U, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOD, 10U, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOD, 11U, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOD, 12U, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOD, 9U, PAL_MODE_INPUT_PULLUP);

    /*SDIO Card detection pin*/
    palSetPadMode(GPIOA, 15U, PAL_MODE_INPUT_PULLUP);
    /*SDIO Configuration*/
    palSetPadMode(GPIOC, 8U, PAL_MODE_ALTERNATE(12));
    palSetPadMode(GPIOC, 9U, PAL_MODE_ALTERNATE(12));
    palSetPadMode(GPIOC, 10U, PAL_MODE_ALTERNATE(12));
    palSetPadMode(GPIOC, 11U, PAL_MODE_ALTERNATE(12));
    palSetPadMode(GPIOC, 12U, PAL_MODE_ALTERNATE(12));
    palSetPadMode(GPIOD, 2U, PAL_MODE_ALTERNATE(12));

    /*mfrc522 cs pins*/
    palSetPadMode(GPIOC, GPIOC_PIN4, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_FLOATING);

    sduObjectInit(&SDU1);
    sdcObjectInit (&SDCD1);
    MFRC522ObjectInit(&RFID1);
    VS1053ObjectInit(&VS1053D1);
}

void BoardDriverStart(void)
{
    sdcStart(&SDCD1, &sdccfg);
    /*
     * Initializes a serial-over-USB CDC driver.
     */
    sduStart(&SDU1, &serusbcfg);

    spiStart(&SPID1, &SPI1cfg);

    MFRC522Start(&RFID1, &RFID1_cfg);
    VS1053Start(&VS1053D1, &VS1053D1_cfg);
}

void BoardDriverShutdown(void)
{
    VS1053Stop(&VS1053D1);
    MFRC522Stop(&RFID1);
    spiStop(&SPID1);


    sduStop(&SDU1);
    sdcStop(&SDCD1);
}

/** @} */
