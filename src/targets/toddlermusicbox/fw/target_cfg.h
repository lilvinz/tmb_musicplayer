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

#ifndef TARGET_CFG_H
#define TARGET_CFG_H

#include "board_drivers.h"

#define LED_HEARTBEAT           &LED5
#define LED_STATUS              &LED6
#define LED_CARDDETECT          &LED3
#define LED_READ                &LED1
#define LED_DECODE              &LED2
#define LED_RFID                &LED4

#define CODEC                   &VS1053D1

#define PARTITION_BL            ((BaseNVMDevice*)&nvm_part_internal_flash_bl)
#define PARTITION_FW            ((BaseNVMDevice*)&nvm_part_internal_flash_fw)
#define PARTITION_BL_UPDATE     ((BaseNVMDevice*)&nvm_memory_bl_bin)

/* List modules here. */
#define MOD_TEST_CPP                TRUE
#define MOD_MUSICBOX                TRUE
#define MOD_PLAYER                  TRUE
#define MOD_RFID                    TRUE
#define MOD_INPUT                   TRUE
#define MOD_EFFECTS                 TRUE
#define MOD_CARDREADER              TRUE

#define DISPLAY_WIDTH 5
#define DISPLAY_HEIGHT 1
#define LEDCOUNT 5


#define DEBUG_CANNEL (BaseSequentialStream *)&SD6

#endif /* TARGET_CFG_H */
