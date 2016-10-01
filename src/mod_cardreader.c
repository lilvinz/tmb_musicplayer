/**
 * @file    src/mod_cardreader.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_cardreader.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "ch.h"
#include "targetconf.h"
#include "chprintf.h"

#include "ff.h"


typedef struct {
    CardReaderConfig* cfgp;
    thread_t* cardDetectionThreadp;
    bool cardDetected;
    FATFS* fsp;
} ModCardReaderData;

static ModCardReaderData modCardReaderData;
static THD_WORKING_AREA(waCardReader, MOD_CARDREADER_THREADSIZE);
static FATFS SDC_FS;

/* Generic large buffer.*/
static char fbuff[1024];

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

/*
 * Scan Files in a path and print them to the character stream.
 */
static FRESULT listFiles(BaseSequentialStream *chp, char *path) {
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int fyear,fmonth,fday,fhour,fminute,fsecond;

    int i;
    char *fn;

#if _USE_LFN
    fno.lfname = 0;
    fno.lfsize = 0;
#endif
    /*
     * Open the Directory.
     */
    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        /*
         * If the path opened successfully.
         */
        i = strlen(path);
        while (true) {
            /*
             * Read the Directory.
             */
            res = f_readdir(&dir, &fno);
            /*
             * If the directory read failed or the
             */
            if (res != FR_OK || fno.fname[0] == 0) {
                break;
            }
            /*
             * If the directory or file begins with a '.' (hidden), continue
             */
            if (fno.fname[0] == '.') {
                continue;
            }
            fn = fno.fname;
            /*
             * Extract the date.
             */
            fyear = ((0b1111111000000000&fno.fdate) >> 9)+1980;
            fmonth= (0b0000000111100000&fno.fdate) >> 5;
            fday  = (0b0000000000011111&fno.fdate);
            /*
             * Extract the time.
             */
            fhour   = (0b1111100000000000&fno.ftime) >> 11;
            fminute = (0b0000011111100000&fno.ftime) >> 5;
            fsecond = (0b0000000000011111&fno.ftime)*2;
            /*
             * Print date and time of the file.
             */
            chprintf(chp, "%4d-%02d-%02d %02d:%02d:%02d ", fyear, fmonth, fday, fhour, fminute, fsecond);
            /*
             * If the 'file' is a directory.
             */
            if (fno.fattrib & AM_DIR) {
                /*
                 * Add a slash to the end of the path
                 */
                path[i++] = '/';
                strcpy(&path[i], fn);
                /*
                 * Print that it is a directory and the path.
                 */
                chprintf(chp, "<DIR> %s/\r\n", path);
                /*
                 * Recursive call to scan the files.
                 */
                res = listFiles(chp, path);
                if (res != FR_OK) {
                    break;
                }
                path[--i] = 0;
            } else {
                /*
                 * Otherwise print the path as a file.
                 */
                chprintf(chp, "      %s/%s\r\n", path, fn);
            }
        }
    }
    else
    {
        chprintf(chp, "FS: f_opendir() failed\r\n");
    }
    return res;
}

static bool mountFS(ModCardReaderData* datap)
{
    if (sdcConnect(datap->cfgp->sdc) == HAL_SUCCESS)
    {
        FRESULT err;
        err = f_mount(datap->fsp, "/mount/", 1);
        if (err != FR_OK) {
            chprintf(DEBUG_CANNEL, "FS: f_mount() failed. Is the SD card inserted?\r\n");
            printError(DEBUG_CANNEL, err);
            return false;
        }
        chprintf(DEBUG_CANNEL, "FS: f_mount() succeeded\r\n");

        return true;
    }
    chprintf(DEBUG_CANNEL, "Failed to connect sdc card.\r\n");

    return false;
}

static bool unmountFS(ModCardReaderData* datap)
{
    FRESULT err;

    err = f_mount(NULL, "/mount/", 0);

    sdcDisconnect(datap->cfgp->sdc);
    if (err != FR_OK) {
        chprintf(DEBUG_CANNEL, "FS: f_mount() unmount failed\r\n");
        printError(DEBUG_CANNEL, err);
        return false;
    }

    chprintf(DEBUG_CANNEL, "FS: f_mount() unmount succeeded\r\n");
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
    modCardReaderData.fsp = &SDC_FS;
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

void mod_cardreader_ls(BaseSequentialStream *chp)
{
    /*
     * Set the file path buffer to 0
     */

    memset(fbuff,0,sizeof(fbuff));
    listFiles(chp, fbuff);
}

/** @} */
