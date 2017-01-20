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

#include "ws281x.h"
#include "ledconf.h"



extern SerialUSBDriver SDU1;
extern SDCDriver SDCD1;
static MFRC522Driver RFID1;
static VS1053Driver VS1053D1;
static ws281xDriver ws281x;

static ws281xConfig ws281x_cfg =
{
    LEDCOUNT,
    LED1,
    {
        12000000,
        WS2811_BIT_PWM_WIDTH,
        NULL,
        {
            { PWM_OUTPUT_DISABLED, NULL },
            { PWM_OUTPUT_DISABLED, NULL },
            { PWM_OUTPUT_ACTIVE_HIGH, NULL },
            { PWM_OUTPUT_DISABLED, NULL }
        },
        0,
        TIM_DIER_UDE | TIM_DIER_CC3DE,
    },
    &PWMD4,
    2,
    WS2811_ZERO_PWM_WIDTH,
    WS2811_ONE_PWM_WIDTH,
    STM32_DMA1_STREAM7,
    2,
};
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
  GPIOC,
  14U,
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
    /*vs1053 pin configuratio*/
    palSetPadMode(GPIOB, GPIOB_PIN13, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_LOWEST );
    palSetPadMode(GPIOB, GPIOB_PIN14, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_LOWEST);
    palSetPadMode(GPIOB, GPIOB_PIN15, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_LOWEST);
    palSetPadMode(GPIOD, 8U, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_LOWEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOD, 10U, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_LOWEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOD, 11U, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_LOWEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOC, 14U, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_LOWEST | PAL_STM32_PUPDR_FLOATING);
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
    palSetPadMode(GPIOC, GPIOC_PIN4, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_LOWEST | PAL_STM32_PUPDR_FLOATING);

    /*buttons*/
    palSetPadMode(GPIOE, 5U, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOE, 4U, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOE, 3U, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOE, 2U, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOE, 1U, PAL_MODE_INPUT_PULLUP);

    /*ws2811 led pin*/
    palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(2));

    ws281xObjectInit(&ws281x);
    sduObjectInit(&SDU1);
    sdcObjectInit (&SDCD1);
    MFRC522ObjectInit(&RFID1);
    VS1053ObjectInit(&VS1053D1);
}

void BoardDriverStart(void)
{
    ws281xStart(&ws281x, &ws281x_cfg);

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
    ws281xStop(&ws281x);
}

void GetLedConfig(int16_t ledid, LEDPinConfig* pconfig)
{
    switch (ledid)
    {
    case TMB_LED_SDCARDREAD:
        pconfig->gpio = GPIOD;
        pconfig->pin = GPIOD_LED5;
        pconfig->clearOn = false;
        break;
    case TMB_LED_CODECDECODE:
        pconfig->gpio = GPIOD;
        pconfig->pin = GPIOD_LED6;
        pconfig->clearOn = false;
        break;
    case TMB_LED_RFIDDETECT:
        pconfig->gpio = GPIOD;
        pconfig->pin = GPIOD_LED3;
        pconfig->clearOn = false;
        break;
    case TMB_LED_SDCARDDETECT:
        pconfig->gpio = GPIOD;
        pconfig->pin = GPIOD_LED4;
        pconfig->clearOn = false;
        break;
    }
}

static uint8_t txbuf[2];
static uint8_t rxbuf[2];

void MFRC522WriteRegister(MFRC522Driver* mfrc522p, uint8_t addr, uint8_t val)
{
    (void)mfrc522p;
    spiSelect(&SPID1);
    txbuf[0] = (addr << 1) & 0x7E;
    txbuf[1] = val;
    spiSend(&SPID1, 2, txbuf);
    spiUnselect(&SPID1);
}

uint8_t MFRC522ReadRegister(MFRC522Driver* mfrc522p, uint8_t addr)
{
    (void)mfrc522p;
    spiSelect(&SPID1);
    txbuf[0] = ((addr << 1) & 0x7E) | 0x80;
    txbuf[1] = 0xff;
    spiExchange(&SPID1, 2, txbuf, rxbuf);
    spiUnselect(&SPID1);
    return rxbuf[1];
}

MFRC522Driver* GetRFIDDriver(void)
{
    return &RFID1;
}

SDCDriver* GetCardDriver(void)
{
    return &SDCD1;
}

/** @} */
