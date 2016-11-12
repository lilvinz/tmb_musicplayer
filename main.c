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

#include "usbcfg.h"
#include "mod_led.h"
#include "mod_cardreader.h"
#include "mod_rfid.h"
#include "mod_musicplayer.h"
#include "mod_input.h"
#include "mod_musicbox.h"

#include "mfrc522.h"
#include "vs1053.h"
#include "ws281x.h"

#include <stdlib.h>

ModLED LED_ORANGE;
ModLED LED_GREEN;
ModLED LED_BLUE;
ModLED LED_RED;

static CardReaderConfig cardReaderCfg =
{
        .sdc = &SDCD1,
        .ledCardDetect = &LED_GREEN,

        /*Card detection pin*/
        .gpio = GPIOA,
        .pin = 15U,
        .setOn = false
};

MFRC522Driver RFID1;
VS1053Driver VS1053D1;
ws281xDriver ws281x;

static RFIDConfig rfidCfg =
{
    .mfrcd = &RFID1,
    .ledCardDetect = &LED_ORANGE,
};

static MusicPlayerConfig playerCfg =
{
    .codecp = &VS1053D1,
    .ledReadData = &LED_RED,
    .ledSendData = &LED_BLUE,
};

static Button buttons[] =
{
        /*play*/
        {
            .cfg = {.port = GPIOE, .pad = 3},
        },
        /*next*/
        {
            .cfg = {.port = GPIOE, .pad = 2},
        },
        /*prev*/
        {
            .cfg = {.port = GPIOE, .pad = 4},
        },
        /*volup*/
        {
            .cfg = {.port = GPIOE, .pad = 1},
        },
        /*voldown*/
        {
            .cfg = {.port = GPIOE, .pad = 5},
        }
};

static ModInputConfig modInputCfg =
{
        .pButtons = buttons,
        .buttonCount = 5,
};

static MusicBoxConfig modMusicbocCfg =
{
        .btnPlay = &buttons[0],
        .btnNext = &buttons[1],
        .btnPrev = &buttons[2],
        .btnVolUp = &buttons[3],
        .btnVolDown = &buttons[4],
};

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



/* Virtual serial port over USB.*/

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
  size_t n, size;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: mem\r\n");
    return;
  }
  n = chHeapStatus(NULL, &size);
  chprintf(chp, "core free memory : %u bytes\r\n", chCoreGetStatusX());
  chprintf(chp, "heap fragments   : %u\r\n", n);
  chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]) {
  static const char *states[] = {CH_STATE_NAMES};
  thread_t *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: threads\r\n");
    return;
  }
  chprintf(chp, "    addr    stack prio refs     state\r\n");
  tp = chRegFirstThread();
  do {
    chprintf(chp, "%08lx %08lx %4lu %4lu %9s\r\n",
             (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
             (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
             states[tp->p_state]);
    tp = chRegNextThread(tp);
  } while (tp != NULL);
}

static void cmd_ls(BaseSequentialStream *chp, int argc, char *argv[]) {

  char* path = 0;
  if (argc > 1) {
    chprintf(chp, "Usage: ls <path>\r\n");
    return;
  }

  if (argc == 1)
  {
      path = argv[0];
  }

  mod_cardreader_ls(chp, path);

}

static void cmd_cd(BaseSequentialStream *chp, int argc, char *argv[]) {

  if (argc != 1) {
    chprintf(chp, "Usage: cd <path>\r\n");
    return;
  }

  if (mod_cardreader_cd(argv[0]))
  {
      chprintf(chp, "changed to dir %s\r\n", argv[0]);
  }

}

static void cmd_find(BaseSequentialStream *chp, int argc, char *argv[]) {

  if (argc != 2) {
    chprintf(chp, "Usage: find <path>\r\n");
    return;
  }

  char lfNamebuffer[128];

  DIR dp;
  FILINFO fno;
  fno.lfname = lfNamebuffer;
  fno.lfsize = sizeof(lfNamebuffer);

  if (mod_cardreader_find(&dp, &fno, argv[0], argv[1]) && fno.fname[0])
  {
      if (fno.fattrib & AM_DIR)
      {
          if (fno.fattrib & AM_LFN)
          {
              chprintf(chp, "found dir %s\r\n", fno.fname);
          }
          else
          {
              chprintf(chp, "found dir %s\r\n", fno.lfname);
          }
      }
      else
      {
          if (fno.fattrib & AM_LFN)
          {
              chprintf(chp, "found file %s\r\n", fno.fname);
          }
          else
          {
              chprintf(chp, "found file %s\r\n", fno.lfname);
          }
      }

      f_closedir(&dp);
  }
  else
      chprintf(chp, "%s not found\r\n", argv[1]);

}

static void cmd_play(BaseSequentialStream *chp, int argc, char *argv[])
{
   (void) argv;
  if (argc != 1) {
    chprintf(chp, "Usage: play <path>\r\n");
    return;
  }


}

static void cmd_sine(BaseSequentialStream *chp, int argc, char *argv[])
{
  (void)argv;
  if (argc != 1) {
    chprintf(chp, "Usage: sine <freq>\r\n");
    return;
  }

  uint16_t freq = (uint16_t)atoi(argv[0]);
  VS1053SineTest(&VS1053D1, freq, 0x24, 0x24);
}

static void cmd_vol(BaseSequentialStream *chp, int argc, char *argv[])
{
  (void)argv;
  if (argc != 2) {
    chprintf(chp, "Usage: vol <left> <right>\r\n");
    return;
  }

  uint8_t left = (uint8_t)atoi(argv[0]);
  uint8_t right = (uint8_t)atoi(argv[1]);
  VS1053SetVolume(&VS1053D1, left, right);
}


static const ShellCommand commands[] = {
    {"mem", cmd_mem},
    {"threads", cmd_threads},
    {"ls", cmd_ls},
    {"cd", cmd_cd},
    {"find", cmd_find},
    {"play", cmd_play},
    {"sine", cmd_sine},
    {"vol", cmd_vol},
    {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SDU1,
  commands
};

/*
 * Application entry point.
 */

int main(void)
{
    thread_t *shelltp = NULL;

    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();

    chSysInit();

    /*
     * Shell manager initialization.
     */
    shellInit();



    BoardDriverInit();

    BoardDriverStart();

    /*init modules*/
    mod_input_init(&modInputCfg);
    mod_musicplayer_init(&playerCfg);
    mod_cardreader_init(&cardReaderCfg);
    mod_rfid_init(&rfidCfg);
    mod_musicbox_init(&modMusicbocCfg);


    /*start modules*/
    mod_cardreader_start();
    mod_rfid_start();
    mod_musicplayer_start();
    mod_input_start();
    mod_musicbox_start();

    /*
     * Activates the USB driver and then the USB bus pull-up on D+.
     * Note, a delay is inserted in order to not have to disconnect the cable
     * after a reset.
     */
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1000);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);


    /*play file from start*/
    mod_musicplayer_cmdPlay("/music/rock_pop");


    /*
     * Normal main() thread activity, in this demo it just performs
     * a shell respawn upon its termination.
     */
    while (TRUE)
    {
        if (!shelltp) {
          if (SDU1.config->usbp->state == USB_ACTIVE) {
            /* Spawns a new shell.*/
            shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
          }
        }
        else {
          /* If the previous shell exited.*/
          if (chThdTerminatedX(shelltp)) {
            /* Recovers memory of the previous shell.*/
            chThdRelease(shelltp);
            shelltp = NULL;
          }
        }
        chThdSleepMilliseconds(500);
    }
}
