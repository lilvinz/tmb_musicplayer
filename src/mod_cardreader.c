/**
 * @file    src/mod_cardreader.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_cardreader.h"

#include "ch.h"
#include "targetconf.h"
#include "chprintf.h"

#include "ff.h"


typedef struct {
    CardReaderConfig* cfgp;
    thread_t* cardDetectionThreadp;
    bool cardDetected;
    FATFS* mmcFSp;
} ModCardReaderData;

static ModCardReaderData modCardReaderData;
static THD_WORKING_AREA(waCardReader, MOD_CARDREADER_THREADSIZE);
static FATFS MMC_FS;

static char* fresult_str(FRESULT stat)
{
    switch (stat) {
        case FR_OK:
            return "Succeeded";
        case FR_DISK_ERR:
            return "A hard error occurred in the low level disk I/O layer";
        case FR_INT_ERR:
            return "Assertion failed";
        case FR_NOT_READY:
            return "The physical drive cannot work";
        case FR_NO_FILE:
            return "Could not find the file";
        case FR_NO_PATH:
            return "Could not find the path";
        case FR_INVALID_NAME:
            return "The path name format is invalid";
        case FR_DENIED:
            return "Access denied due to prohibited access or directory full";
        case FR_EXIST:
            return "Access denied due to prohibited access";
        case FR_INVALID_OBJECT:
            return "The file/directory object is invalid";
        case FR_WRITE_PROTECTED:
            return "The physical drive is write protected";
        case FR_INVALID_DRIVE:
            return "The logical drive number is invalid";
        case FR_NOT_ENABLED:
            return "The volume has no work area";
        case FR_NO_FILESYSTEM:
            return "There is no valid FAT volume";
        case FR_MKFS_ABORTED:
            return "The f_mkfs() aborted due to any parameter error";
        case FR_TIMEOUT:
            return "Could not get a grant to access the volume within defined period";
        case FR_LOCKED:
            return "The operation is rejected according to the file sharing policy";
        case FR_NOT_ENOUGH_CORE:
            return "LFN working buffer could not be allocated";
        case FR_TOO_MANY_OPEN_FILES:
            return "Number of open files > _FS_SHARE";
        case FR_INVALID_PARAMETER:
            return "Given parameter is invalid";
        default:
            return "Unknown";
    }
    return "";
}

static void printError(BaseSequentialStream *chp, FRESULT err)
{
    chprintf(chp, "\t%s.\r\n",fresult_str(err));
}
static bool mountFS(ModCardReaderData* datap)
{
    if (mmcConnect(datap->cfgp->mmc) == HAL_SUCCESS)
    {
        FRESULT err;
        err = f_mount(datap->mmcFSp, "/mount/", 1);
        if (err != FR_OK) {
            chprintf(DEBUG_CANNEL, "FS: f_mount() failed. Is the SD card inserted?\r\n");
            printError(DEBUG_CANNEL, err);
            return false;
        }
        chprintf(DEBUG_CANNEL, "FS: f_mount() succeeded\r\n");

        return true;
    }

    return false;
}

static bool unmountFS(ModCardReaderData* datap)
{
    FRESULT err;

    err = f_mount(NULL, "/mount/", 0);

    mmcDisconnect(datap->cfgp->mmc);
    if (err != FR_OK) {
        chprintf(DEBUG_CANNEL, "FS: f_mount() unmount failed\r\n");
        printError(DEBUG_CANNEL, err);
        return false;
    }

    return true;
}


static void cardReaderOnCardInserted(ModCardReaderData* datap)
{
    chprintf(DEBUG_CANNEL, "Memory card inserted.\r\n");

    mod_led_on(datap->cfgp->ledCardDetect);
    datap->cardDetected = true;

    if (mountFS(datap) == true)
    {

    }
}

static void cardReaderOnCardRemoved(ModCardReaderData* datap)
{
    chprintf(DEBUG_CANNEL, "Memory card removed.\r\n");

    mod_led_off(datap->cfgp->ledCardDetect);
    datap->cardDetected = false;

    unmountFS(datap);
}
/*
 * This is a periodic thread that reads uid from rfid periphal
 */
static THD_FUNCTION(cardReader, arg)
{
  (void)arg;
  chRegSetThreadName("cardReader");


  ModCardReaderData* datap = &modCardReaderData;

  bool lastDetectedCard = false;
  while (!chThdShouldTerminateX())
  {
      bool detectedCard = false;
      /*check card detection pin*/
      if (palReadPad(datap->cfgp->gpio, datap->cfgp->pin) == datap->cfgp->setOn)
      {
          detectedCard = true;
      }

      /*check if card was inserted or removed*/
      if (detectedCard != lastDetectedCard)
      {
          if (detectedCard == true)
          {
              cardReaderOnCardInserted(datap);
          }
          else
          {
              cardReaderOnCardRemoved(datap);
          }
      }

      lastDetectedCard = detectedCard;

      chThdSleep(MS2ST(100));
  }

  cardReaderOnCardRemoved(datap);
}

void mod_cardreader_init(CardReaderConfig* cfgp)
{
    modCardReaderData.cfgp = cfgp;
    modCardReaderData.mmcFSp = &MMC_FS;
}

bool mod_cardreader_start(void)
{
    if (modCardReaderData.cardDetectionThreadp == NULL)
    {
        modCardReaderData.cardDetectionThreadp = chThdCreateStatic(waCardReader, sizeof(waCardReader),
                MOD_CARDREADER_THREADPRIO, cardReader, NULL);
        return true;
    }
    return false;
}

void mod_cardreader_stop(void)
{
    if (modCardReaderData.cardDetectionThreadp != NULL)
    {
        chThdTerminate(modCardReaderData.cardDetectionThreadp);
        modCardReaderData.cardDetectionThreadp = NULL;
    }
}


/** @} */
