/**
 * @file    src/mod_musicplayer.c
 * @brief
 *
 * @addtogroup
 * @{
 */

#include "mod_musicplayer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "ch.h"
#include "targetconf.h"
#include "chprintf.h"

#include "ff.h"

#define EVENTMASK_COMMANDS EVENT_MASK(0)
#define EVENTMASK_CODEC EVENT_MASK(1)
#define EVENTMASK_PUMPTHREAD_STOP EVENT_MASK(2)

/**
 * @brief   Player EVENT Flags
 */

typedef enum {
  PLAY_FILE = 1,
  STOP = 1 << 1,
  PAUSE = 1 << 2,
  NEXT = 1 << 3,
  PREV = 1 << 4,
} PlayerEventFlags;

typedef struct {
    char fileName[128];
} PlayerCmdMsg;

//Declare Messagepool for at least 5 msg
static PlayerCmdMsg PlayerCmdMsgBuffer[MOD_MUSICPLAYER_CMD_QUEUE_SIZE];
static MEMORYPOOL_DECL(PlayerCmdMsgPool, sizeof(PlayerCmdMsg), NULL);

//set up Mailbox
static msg_t PlayerCmdQueue[MOD_MUSICPLAYER_CMD_QUEUE_SIZE];
static MAILBOX_DECL(playerCommands, PlayerCmdQueue, MOD_MUSICPLAYER_CMD_QUEUE_SIZE);


typedef struct {
    MusicPlayerConfig* cfgp;
    thread_t* playerThread;
    event_source_t commandEventSource;

    bool transferFile;
    char fileNameBuff[512];

    char currentPath[512];
    DIR openDirectory;
} ModMusicPlayerData;

static ModMusicPlayerData modMusicPlayerData;
static THD_WORKING_AREA(waMusicplayerThread, 512);
static THD_WORKING_AREA(waMusicplayerDatapumpThread, MOD_MUSICPLAYER_DATAPUMP_THREADSIZE);

/* Generic large buffer.*/
static char fnameBuff[512];

static void PlayNextFileFromDirectory(ModMusicPlayerData* datap)
{
    FILINFO fno;
    fno.lfname = fnameBuff;
    fno.lfsize = sizeof(fnameBuff);

    char *fn;
    int i = strlen(datap->currentPath);
    while (true)
    {
        FRESULT res = f_readdir(&datap->openDirectory, &fno);
        /*
         * If the directory read failed or the
         */
        if (res != FR_OK || (fno.lfname[0] == 0 && fno.fname[0] == 0))
        {
            f_closedir(&datap->openDirectory);
            memset(datap->currentPath, 0, sizeof(datap->currentPath));
            break;
        }

        fn = fno.lfname;
        if (fno.lfname[0] == 0)
        {
            fn = fno.fname;
        }
        /*
         * If the directory or file begins with a '.' (hidden), continue
         */
        if (fn[0] == '.') {
            continue;
        }
        /*
         * If the 'file' is a directory.
         */
        if (fno.fattrib & AM_DIR)
        {
            // skip directories for now
        }
        else
        {
            memset(datap->fileNameBuff, 0, sizeof(datap->fileNameBuff));
            strcpy(datap->fileNameBuff, datap->currentPath);
            datap->fileNameBuff[i++] = '/';
            strcpy(&datap->fileNameBuff[i], fn);
            datap->transferFile = true;
            break;
        }
    }

}

static void HandlePlayCommand(ModMusicPlayerData* datap)
{
    /*try to open a directory*/
    FRESULT res = f_opendir(&datap->openDirectory, datap->currentPath);
    if (res == FR_OK)
    {
        PlayNextFileFromDirectory(datap);
    }
}
/*
 *
 */
static THD_FUNCTION(dataPump, arg)
{
  (void)arg;
  chRegSetThreadName("dataPump");

  ModMusicPlayerData* datap = &modMusicPlayerData;



  char Buffer[32];
  UINT ByteToRead = sizeof(Buffer);
  UINT ByteRead;

  FIL fsrc;

  while (!chThdShouldTerminateX())
  {
      if (datap->transferFile == true)
      {
          FRESULT err = f_open(&fsrc, datap->fileNameBuff, FA_READ);
          if (err == FR_OK)
          {
              //startTransfer = false;

              /*
               * Do while the number of bytes read is equal to the number of bytes to read
               * (the buffer is filled)
               */
              do {
                  /*
                   * Clear the buffer.
                   */
                  memset(Buffer,0,sizeof(Buffer));
                  /*
                   * Read the file.
                   */
                  mod_led_on(datap->cfgp->ledReadData);
                  err = f_read(&fsrc, Buffer, ByteToRead, &ByteRead);
                  mod_led_off(datap->cfgp->ledReadData);
                  if (err == FR_OK)
                  {
                      mod_led_on(datap->cfgp->ledSendData);
                      if (VS1053SendData(datap->cfgp->codecp, Buffer, ByteRead) != ByteRead)
                      {
                          ByteRead = 0;
                      }
                      mod_led_off(datap->cfgp->ledSendData);
                  }
                  else
                  {
                      break;
                  }

                  if (datap->transferFile == false)
                  {
                      break;
                  }
              } while (ByteRead >= ByteToRead);

              f_close(&fsrc);

              VS1053StopPlaying(datap->cfgp->codecp);

              datap->transferFile = false;
              chEvtSignal(datap->playerThread, EVENTMASK_PUMPTHREAD_STOP);
          }
      }
      chThdSleep(MS2ST(100));
  }
}

static THD_FUNCTION(musicplayer, arg)
{
  (void)arg;
  chRegSetThreadName("musicplayer");

  ModMusicPlayerData* datap = &modMusicPlayerData;

  event_listener_t commandListener;
  chEvtRegisterMaskWithFlags(&modMusicPlayerData.commandEventSource,
                               &commandListener,
                               EVENTMASK_COMMANDS,
                               PLAY_FILE | STOP | PAUSE | NEXT | PREV);

  while (!chThdShouldTerminateX())
  {
      eventmask_t evt = chEvtWaitAny(ALL_EVENTS);
      if (evt & EVENTMASK_PUMPTHREAD_STOP)
      {
          PlayNextFileFromDirectory(datap);
      }
      else if (evt & EVENTMASK_COMMANDS)
      {
          eventflags_t flags = chEvtGetAndClearFlags(&commandListener);

          if (flags & PLAY_FILE)
          {
              msg_t p;

              /* Processing the event.*/
              while (chMBFetch(&playerCommands, (msg_t*)&p, TIME_IMMEDIATE) == MSG_OK)
              {
                  memset(datap->currentPath, 0, sizeof(datap->currentPath));

                  PlayerCmdMsg* pCommandMsg = (PlayerCmdMsg*)p;
                  strcpy(datap->currentPath, pCommandMsg->fileName);
                  chPoolFree(&PlayerCmdMsgPool, pCommandMsg);

                  HandlePlayCommand(datap);
              }
          }
          if (flags & STOP)
          {
              datap->transferFile = false;
          }
          if (flags & PAUSE)
          {

          }
          if (flags & NEXT)
          {

          }
          if (flags & PREV)
          {

          }
      }
  }

  chEvtUnregister(&datap->commandEventSource, &commandListener);
}

void mod_musicplayer_init(MusicPlayerConfig* cfgp)
{
    modMusicPlayerData.cfgp = cfgp;

    chPoolObjectInit(&PlayerCmdMsgPool, sizeof(PlayerCmdMsg), NULL);
    chPoolLoadArray(&PlayerCmdMsgPool, PlayerCmdMsgBuffer, MOD_MUSICPLAYER_CMD_QUEUE_SIZE);
    chMBObjectInit(&playerCommands, PlayerCmdQueue, MOD_MUSICPLAYER_CMD_QUEUE_SIZE);

    chEvtObjectInit(&modMusicPlayerData.commandEventSource);
}

bool mod_musicplayer_start(void)
{
    if (modMusicPlayerData.playerThread == NULL)
    {
        modMusicPlayerData.playerThread = chThdCreateStatic(waMusicplayerThread, sizeof(waMusicplayerThread),
                MOD_MUSICPLAYER_DATAPUMP_THREADPRIO, musicplayer, NULL);

        chThdCreateStatic(waMusicplayerDatapumpThread, sizeof(waMusicplayerDatapumpThread),
                        MOD_MUSICPLAYER_DATAPUMP_THREADPRIO - 1, dataPump, NULL);
        return true;
    }
    return false;
}

void mod_musicplayer_stop(void)
{
    if (modMusicPlayerData.playerThread != NULL)
    {
        chThdTerminate(modMusicPlayerData.playerThread);

        chEvtBroadcastFlags(&modMusicPlayerData.commandEventSource, STOP);

        modMusicPlayerData.playerThread = NULL;
    }
}

void mod_musicplayer_cmdPlay(const char* path)
{
    PlayerCmdMsg* pCmd = (PlayerCmdMsg*)chPoolAlloc(&PlayerCmdMsgPool);
    if (pCmd != NULL)
    {
        strcpy(pCmd->fileName, path);
        if (chMBPost(&playerCommands, (msg_t)pCmd, MS2ST(1)) == MSG_OK)
        {
            chEvtBroadcastFlags(&modMusicPlayerData.commandEventSource, PLAY_FILE);
        }
        else
        {
            chPoolFree(&PlayerCmdMsgPool, pCmd);
        }
    }


}

void mod_musicplayer_cmdStop(void)
{
    chEvtBroadcastFlags(&modMusicPlayerData.commandEventSource, STOP);

}


/** @} */
