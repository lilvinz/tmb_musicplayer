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
#include "mmc.h"
#include "mod_led.h"
#include "mod_cardreader.h"

#include <stdlib.h>


ModLED LED_ORANGE;
ModLED LED_GREEN;
ModLED LED_BLUE;
ModLED LED_RED;

MMCDriver MMCD1;

static CardReaderConfig cardReaderCfg =
{
        .mmc = &MMCD1,
        .ledCardDetect = &LED_GREEN,

        /*Card detection pin*/
        .gpio = GPIOC,
        .pin = GPIOC_PIN5,
        .setOn = false
};



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


static const ShellCommand commands[] = {
    {"mem", cmd_mem},
    {"threads", cmd_threads},
    {"mount", cmd_mount},
    {"umount", cmd_unmount},
    {"tree", cmd_tree},
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
    mod_cardreader_init(&cardReaderCfg);


    /*start modules*/
    mod_cardreader_start();

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
