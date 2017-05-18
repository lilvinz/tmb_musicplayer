/*
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "target_cfg.h"

#include "nelems.h"

/*===========================================================================*/
/* Module local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Module exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Module local types.                                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module local variables.                                                   */
/*===========================================================================*/

/* MAY ONLY BE INCLUDED ONCE! */
#include "board_cfg.h"

static uint8_t txbuf[2];
static uint8_t rxbuf[2];

/*===========================================================================*/
/* Module local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Module exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Early initialization code.
 * @details This initialization must be performed just after stack setup
 *          and before any other initialization.
 */
void __early_init(void)
{
    stm32_clock_init();
}

#if HAL_USE_SDC
bool sdc_lld_is_card_inserted(SDCDriver *sdcp) {

    (void)sdcp;
    return TRUE;
}

bool sdc_lld_is_write_protected(SDCDriver *sdcp) {

  (void)sdcp;
  return TRUE;
}
#endif

/**
 * @brief   Prepare all driver configurations
 */
void boardInit(void)
{
    /**
     * Initialize custom drivers as boardInit() is being called at the end of halInit()
     */
    qhalInit();

    /**
     * call *ObjectInit() for all device instances which are created in here.
     */

    /* Start status LED driver */
#if HAL_USE_LED
    ledObjectInit(&LED1);
    ledObjectInit(&LED2);
    ledObjectInit(&LED3);
    ledObjectInit(&LED4);
    ledObjectInit(&LED5);
    ledObjectInit(&LED6);
#endif /* HAL_USE_LED */

    /* Internal flash */
#if HAL_USE_FLASH
#if HAL_USE_NVM_PARTITION
    nvmpartObjectInit(&nvm_part_internal_flash_bl);
    nvmpartObjectInit(&nvm_part_internal_flash_ee);
    nvmpartObjectInit(&nvm_part_internal_flash_fw);
#endif /* HAL_USE_NVM_PARTITION */
#if HAL_USE_NVM_FEE
    nvmfeeObjectInit(&nvm_fee);
#endif /* HAL_USE_NVM_FEE */
#endif /* HAL_USE_FLASH */

    /* nvm memory drivers */
#if HAL_USE_NVM_MEMORY
#if defined(BL_BIN)
    nvm_memory_bl_bin_cfg.sector_num = bl_bin_size;
    nvmmemoryObjectInit(&nvm_memory_bl_bin);
#endif /* defined(BL_BIN) */
#if STM32_BKPRAM_ENABLE
    nvmmemoryObjectInit(&nvm_memory_bkpsram);
#endif /* STM32_BKPRAM_ENABLE */
#endif /* HAL_USE_NVM_MEMORY */

#if HAL_USE_SERIAL
    sdInit();
#endif /* HAL_USE_SERIAL */

#if HAL_USE_WS281X
    ws281xObjectInit(&ws281x);
#endif /* HAL_USE_WS281X */

#if HAL_USE_SDC
    sdcObjectInit (&SDCD1);
#endif /* HAL_USE_SDC */

#if HAL_USE_MFRC522
    MFRC522ObjectInit(&RFID1);
#endif /* HAL_USE_MFRC522 */

#if HAL_USE_VS1053
    VS1053ObjectInit(&VS1053D1);
#endif /* HAL_USE_VS1053 */
}

/**
 * @brief   Start all drivers
 */
void boardStart(void)
{
    /* Start watchdog */
#if HAL_USE_WDG
    wdgStart(&WDGD1, &WDGD1_cfg);
#endif /* HAL_USE_WDG */

    /* Start status LED driver */
#if HAL_USE_LED
    ledStart(&LED1, &led_1_cfg);
    ledStart(&LED2, &led_2_cfg);
    ledStart(&LED3, &led_3_cfg);
    ledStart(&LED4, &led_4_cfg);
    ledStart(&LED5, &led_5_cfg);
    ledStart(&LED6, &led_6_cfg);
#endif /* HAL_USE_LED */

#if HAL_USE_SERIAL
    sdStart(&SD2,NULL);
#endif /* HAL_USE_SERIAL */

#if HAL_USE_WS281X
    ws281xStart(&ws281x, &ws281x_cfg);
#endif /* HAL_USE_WS281X */

#if HAL_USE_VS1053
    /*vs1053 pin configuratio*/
    palSetPadMode(GPIOB, GPIOB_PIN13, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_LOWEST );
    palSetPadMode(GPIOB, GPIOB_PIN14, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_LOWEST);
    palSetPadMode(GPIOB, GPIOB_PIN15, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_LOWEST);
    palSetPadMode(GPIOD, 8U, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_LOWEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOD, 10U, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_LOWEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOD, 11U, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_LOWEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOC, 14U, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_LOWEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOD, 9U, PAL_MODE_INPUT_PULLUP);
    VS1053Start(&VS1053D1, &VS1053D1_cfg);
#endif /* HAL_USE_VS1053 */

    /* Internal flash */
#if HAL_USE_FLASH
    flashStart(&FLASHD, &FLASHD_cfg);
#if HAL_USE_NVM_PARTITION
    nvmpartStart(&nvm_part_internal_flash_bl, &nvm_part_internal_flash_bl_cfg);
    nvmpartStart(&nvm_part_internal_flash_ee, &nvm_part_internal_flash_ee_cfg);
    nvmpartStart(&nvm_part_internal_flash_fw, &nvm_part_internal_flash_fw_cfg);
#endif /* HAL_USE_NVM_PARTITION */
#if HAL_USE_NVM_FEE
    nvmfeeStart(&nvm_fee, &nvm_fee_cfg);
#endif /* HAL_USE_NVM_FEE */
#endif /* HAL_USE_FLASH */

    /* nvm memory drivers */
#if HAL_USE_NVM_MEMORY
#if defined(BL_BIN)
    nvmmemoryStart(&nvm_memory_bl_bin, &nvm_memory_bl_bin_cfg);
#endif /* defined(BL_BIN) */
#if STM32_BKPRAM_ENABLE
    nvmmemoryStart(&nvm_memory_bkpsram, &nvm_memory_bkpsram_cfg);
#endif /* STM32_BKPRAM_ENABLE */
#endif /* HAL_USE_NVM_MEMORY */

#if HAL_USE_SDC
    sdcStart(&SDCD1, &sdccfg);
#endif /* HAL_USE_SDC */

#if HAL_USE_MFRC522
    spiStart(&SPID1, &SPI1cfg);
    MFRC522Start(&RFID1, &RFID1_cfg);
#endif /* HAL_USE_MFRC522 */



}

/**
 * @brief   Stop all drivers in the reverse order of their start.
 * @note    Pin settings must match the initial state in board.h
 */
void boardStop(void)
{
#if HAL_USE_MFRC522
    MFRC522Stop(&RFID1);
    spiStop(&SPID1);
#endif /* HAL_USE_MFRC522 */

#if HAL_USE_SDC
    sdcStop(&SDCD1);
#endif /* HAL_USE_SDC */

    /* nvm memory drivers */
#if HAL_USE_NVM_MEMORY
#if STM32_BKPRAM_ENABLE
    nvmmemorySync(&nvm_memory_bkpsram);
    nvmmemoryStop(&nvm_memory_bkpsram);
#endif /* STM32_BKPRAM_ENABLE */
#if defined(BL_BIN)
    nvmmemorySync(&nvm_memory_bl_bin);
    nvmmemoryStop(&nvm_memory_bl_bin);
#endif /* defined(BL_BIN) */
#endif /* HAL_USE_NVM_MEMORY */

    /* Internal flash */
#if HAL_USE_FLASH
#if HAL_USE_NVM_FEE
    nvmfeeSync(&nvm_fee);
    nvmfeeStop(&nvm_fee);
#endif /* HAL_USE_NVM_FEE */
#if HAL_USE_NVM_PARTITION
    nvmpartSync(&nvm_part_internal_flash_fw);
    nvmpartStop(&nvm_part_internal_flash_fw);
    nvmpartSync(&nvm_part_internal_flash_ee);
    nvmpartStop(&nvm_part_internal_flash_ee);
    nvmpartSync(&nvm_part_internal_flash_bl);
    nvmpartStop(&nvm_part_internal_flash_bl);
#endif /* HAL_USE_NVM_PARTITION */
    flashStop(&FLASHD);
#endif /* HAL_USE_FLASH */

#if HAL_USE_VS1053
    VS1053Stop(&VS1053D1);
#endif /* HAL_USE_VS1053 */

#if HAL_USE_WS281X
    ws281xStop(&ws281x);
#endif /* HAL_USE_WS281X */

#if HAL_USE_SERIAL
    sdStop(&SD2);
#endif /* HAL_USE_SERIAL */

    /* Stop status LED driver */
#if HAL_USE_LED
    ledOff(&LED6);
    ledStop(&LED6);
    ledOff(&LED5);
    ledStop(&LED5);
    ledOff(&LED4);
    ledStop(&LED4);
    ledOff(&LED3);
    ledStop(&LED3);
    ledOff(&LED2);
    ledStop(&LED2);
    ledOff(&LED1);
    ledStop(&LED1);
#endif /* HAL_USE_LED */

    /* Stop watchdog */
#if HAL_USE_WDG
    /*
     * The watchdog cannot be stopped again once started.
     * So the only thing we can do here is reload the counter.
     */
    wdgReset(&WDGD1);
#endif /* HAL_USE_WDG */
}

void boardReset(void)
{
    /* Execute reset */
    NVIC_SystemReset();
}

#if HAL_USE_MFRC522
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
#endif /* HAL_USE_MFRC522 */

