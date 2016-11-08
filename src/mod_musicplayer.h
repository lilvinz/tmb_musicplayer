/**
 * @file    src/mod_musicplayer.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_MUSICPLAYER_H_
#define _MOD_MUSICPLAYER_H_

#include "hal.h"
#include "mod_led.h"
#include "vs1053.h"

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/
#ifndef MOD_MUSICPLAYER_DATAPUMP_THREADSIZE
#define MOD_MUSICPLAYER_DATAPUMP_THREADSIZE 1028
#endif

#ifndef MOD_MUSICPLAYER_DATAPUMP_THREADPRIO
#define MOD_MUSICPLAYER_DATAPUMP_THREADPRIO NORMALPRIO
#endif

#ifndef MOD_MUSICPLAYER_THREADSIZE
#define MOD_MUSICPLAYER_THREADSIZE 1028
#endif

#ifndef MOD_MUSICPLAYER_THREADPRIO
#define MOD_MUSICPLAYER_THREADPRIO LOWPRIO
#endif

#ifndef MOD_MUSICPLAYER_CMD_QUEUE_SIZE
#define MOD_MUSICPLAYER_CMD_QUEUE_SIZE 2
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Cardreader descriptor type.
 */
typedef struct {
    VS1053Driver* codecp;
    ModLED* ledReadData;
    ModLED* ledSendData;
} MusicPlayerConfig;

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void mod_musicplayer_init(MusicPlayerConfig* cfgp);
  bool mod_musicplayer_start(void);
  void mod_musicplayer_stop(void);

  void mod_musicplayer_cmdPlay(const char* path);
  void mod_musicplayer_cmdToggle(void);
  void mod_musicplayer_cmdStop(void);
  void mod_musicplayer_cmdNext(void);
  void mod_musicplayer_cmdPrev(void);
  void mod_musicplayer_cmdVolume(uint8_t volume);

#ifdef __cplusplus
}
#endif

#endif /* _MOD_MUSICPLAYER_H_ */

/** @} */
