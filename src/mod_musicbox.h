/**
 * @file    src/mod_musicbox.h
 * @brief
 *
 * @addtogroup
 * @{
 */

#ifndef _MOD_MUSICBOX_H_
#define _MOD_MUSICBOX_H_

#include "hal.h"
#include "mod_input.h"

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/
#ifndef MOD_MUSICBOX_THREADSIZE
#define MOD_MUSICBOX_THREADSIZE 640
#endif

#ifndef MOD_MUSICBOX_THREADPRIO
#define MOD_MUSICBOX_THREADPRIO LOWPRIO
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
    Button* btnPlay;
    Button* btnVolUp;
    Button* btnVolDown;
    Button* btnNext;
    Button* btnPrev;
} MusicBoxConfig;

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void mod_musicbox_init(MusicBoxConfig* cfgp);
  bool mod_musicbox_start(void);
  void mod_musicbox_stop(void);
#ifdef __cplusplus
}
#endif

#endif /* _MOD_MUSICBOX_H_ */

/** @} */
