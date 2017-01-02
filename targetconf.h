/**
 * @file    targetconf.h
 * @brief
 *
 * @{
 */

#ifndef _TARGETCONF_H_
#define _TARGETCONF_H_

#ifdef __cplusplus
#include "ch.hpp"
#else
#include "ch.h"
#endif

#include "hal.h"
#include "board_drivers.h"
#include "usbcfg.h"

#define DEBUG_CANNEL (BaseSequentialStream *)&SDU1

#endif /* _TARGETCONF_H_ */

/** @} */
