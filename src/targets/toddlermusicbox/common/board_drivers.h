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

#ifndef BOARD_DRIVERS_H_
#define BOARD_DRIVERS_H_

#include "qhal.h"

#include "mfrc522.h"
#include "ws281x.h"
#include "vs1053.h"

/*
 * @brief   This file is used to expose drivers to the application.
 *          It should only contain the highest required level of drivers.
 *          Lower level drivers should remain hidden from application.
 */


#if HAL_USE_MFRC522
extern MFRC522Driver RFID1;
#endif /* HAL_USE_MFRC522 */

#if HAL_USE_VS1053
extern VS1053Driver VS1053D1;
#endif /* HAL_USE_VS1053 */

#if HAL_USE_WS281X
extern ws281xDriver ws281x;
#endif /* HAL_USE_WS281X */

/* Status LEDs */
#if HAL_USE_LED
extern LedDriver LED1;
extern LedDriver LED2;
extern LedDriver LED3;
extern LedDriver LED4;
extern LedDriver LED5;
extern LedDriver LED6;
#endif /* HAL_USE_LED */

/* Internal flash */
#if HAL_USE_FLASH && HAL_USE_NVM_PARTITION
extern NVMPartitionDriver nvm_part_internal_flash_bl;
extern NVMPartitionDriver nvm_part_internal_flash_ee;
extern NVMPartitionDriver nvm_part_internal_flash_fw;
#if HAL_USE_NVM_FEE
extern NVMFeeDriver nvm_fee;
#endif /* HAL_USE_NVM_FEE */
#endif /* HAL_USE_FLASH && HAL_USE_NVM_PARTITION */

#if HAL_USE_NVM_MEMORY
#if defined(BL_BIN)
extern NVMMemoryDriver nvm_memory_bl_bin;
#endif /* defined(BL_BIN) */
#if STM32_BKPRAM_ENABLE == TRUE
extern NVMMemoryDriver nvm_memory_bkpsram;
#endif /* STM32_BKPRAM_ENABLE == TRUE */
#endif /* HAL_USE_NVM_MEMORY */

#endif /* BOARD_DRIVERS_H_ */
