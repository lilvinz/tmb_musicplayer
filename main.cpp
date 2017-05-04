/*
 ChibiOS - Copyright (C) 2006-2014 Giovanni Di Sirio

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

#include "targetconf.h"

/* addition driver */
#include "chprintf.h"
#include "shell.h"

#include "float.h"
#include "math.h"
#include <string.h>
#include <stdlib.h>

#include "button.h"
#include "mod_led.h"
#include "mod_input.h"
#include "mod_rfid.h"
#include "mod_cardreader.h"
#include "mod_player.h"
#include "mod_musicbox.h"

#include "mfrc522.h"

using namespace tmb_musicplayer;

static Button playBtn(GPIOE, 7);
static Button nextBtn(GPIOE, 2);
static Button prevBtn(GPIOE, 4);
static Button volupBtn(GPIOE, 1);
static Button voldownBtn(GPIOE, 5);
static Button carddetectBtn(GPIOA, 15U);

static Button* buttons[] = {&playBtn, &nextBtn, &prevBtn, &volupBtn, &voldownBtn, &carddetectBtn};

static ModuleInput modInput(buttons, 6);
static ModuleRFID modRFID;
static ModuleCardreader modCardreader;
static ModulePlayer modPlayer;
static ModuleMusicbox modMusicbox;

static Led SDCardReadLed;
static Led SDCardDetect;
static Led CodecDecodeLed;
static Led RFIDCardDetect;
static Led HeartBeat;
static Led LEDSpare;

void InitLED(int16_t ledId, Led& led)
{
    LEDPinConfig ledConfig;
    GetLedConfig(ledId, &ledConfig);

    led.Initialize(ledConfig.gpio, ledConfig.pin, ledConfig.clearOn);

    //switch off by default
    led.Off();
}

/*
 * Application entry point.
 */

int main(void)
{
    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chibios_rt::System::init();

    /*
     * Shell manager initialization.
     */
//    shellInit();

    //Setup LED
    InitLED(TMB_LED_SDCARDREAD, SDCardReadLed);
    InitLED(TMB_LED_SDCARDDETECT, SDCardDetect);
    InitLED(TMB_LED_CODECDECODE, CodecDecodeLed);
    InitLED(TMB_LED_RFIDDETECT, RFIDCardDetect);
    InitLED(TMB_LED_HEARTBEAT, HeartBeat);
    InitLED(TMB_LED_SPARE, LEDSpare);

    BoardDriverInit();

    BoardDriverStart();

    /*init modules*/
    modMusicbox.SetButton(ModuleMusicbox::Play, &playBtn);
    modMusicbox.SetButton(ModuleMusicbox::Next, &nextBtn);
    modMusicbox.SetButton(ModuleMusicbox::Prev, &prevBtn);
    modMusicbox.SetButton(ModuleMusicbox::VolUp, &volupBtn);
    modMusicbox.SetButton(ModuleMusicbox::VolDown, &voldownBtn);
    modMusicbox.SetRFIDModule(&modRFID);
    modMusicbox.SetPlayerModule(&modPlayer);
    modMusicbox.SetCardreaderModule(&modCardreader, &SDCardDetect);

    modRFID.SetDriver(GetRFIDDriver());
    modRFID.SetLed(&RFIDCardDetect);

    modCardreader.SetDriver(GetCardDriver());
    modCardreader.SetCDButton(&carddetectBtn);

    modPlayer.SetDecoder(GetCodecDriver());
    modPlayer.SetLED(&SDCardReadLed, &CodecDecodeLed);


    /*start modules*/
    modMusicbox.Start();
    modInput.Start();
    modRFID.Start();
    modCardreader.Start();
    modPlayer.Start();

    /*
     * Activates the USB driver and then the USB bus pull-up on D+.
     * Note, a delay is inserted in order to not have to disconnect the cable
     * after a reset.
     */
//    usbDisconnectBus(serusbcfg.usbp);
//    chThdSleepMilliseconds(1000);
//    usbStart(serusbcfg.usbp, &usbcfg);
//    usbConnectBus(serusbcfg.usbp);


    /*play file from start*/
    //modPlayer.Play("/music/rock_pop");


    /*
     * Normal main() thread activity, in this demo it just performs
     * a shell respawn upon its termination.
     */
    while (TRUE)
    {
        HeartBeat.Toggle();
        chibios_rt::BaseThread::sleep(MS2ST(500));
    }
}
