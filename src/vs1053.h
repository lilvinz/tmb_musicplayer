
/**
 * @file    vs1053.h
 * @brief   VS1053 Driver macros and structures.
 *
 * @addtogroup vs1053
 * @{
 */

#ifndef _VS1053_H_
#define _VS1053_H_

#if HAL_USE_VS1053 || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/


/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/


/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  VS1053_UNINIT = 0,                   /**< Not initialized.                   */
  VS1053_STOP = 1,                     /**< Stopped.                           */
  VS1053_READY = 2,                    /**< Ready.                             */
  VS1053_ACTIVE = 3,                   /**< Active.                            */
} VS1053state_t;
/**
 * @brief   Type of a structure representing an VS1053Driver driver.
 */
typedef struct VS1053Driver VS1053Driver;
/**
 * @brief   Driver configuration structure.
 * @note    It could be empty on some architectures.
 */
typedef struct
{
    SPIDriver* spid;
    SPIConfig* spiCfg;

    /**
     * @brief Port used of X_RESET
     */
    ioportid_t            xResetPort;
    /**
     * @brief Pad used of X_RESET
     */
    uint8_t               xResetPad;

    /**
     * @brief Port used of X_CS
     */
    ioportid_t            xCSPort;
    /**
     * @brief Pad used of X_CS
     */
    uint8_t               xCSPad;

    /**
     * @brief Port used of X_DCS
     */
    ioportid_t            xDCSPort;
    /**
     * @brief Pad used of X_DCS
     */
    uint8_t               xDCSPad;

    /**
     * @brief Port used of X_DCS
     */
    ioportid_t            xDREQPort;
    /**
     * @brief Pad used of X_DCS
     */
    uint8_t               xDREQPad;

} VS1053Config;


/**
 * @brief   Structure representing an VS1053 driver.
 */
struct VS1053Driver {
  /**
   * @brief   Driver state.
   */
	VS1053state_t                state;
  /**
   * @brief   Current configuration data.
   */
  const VS1053Config           *config;
  uint8_t                  txBuffer[4];
  uint8_t                  rxBuffer[4];
  /* End of the mandatory fields.*/
};
/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/
#define VS1053CanJump(status) (status && (1 << 15) == 0)
/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void VS1053Init(void);
  void VS1053ObjectInit(VS1053Driver* VS1053p);
  void VS1053Start(VS1053Driver* VS1053p, const VS1053Config *config);
  void VS1053Stop(VS1053Driver* VS1053p);

  void VS1053SineTest(VS1053Driver* VS1053p, uint16_t freq, uint8_t leftVol, uint8_t rightVol);
  void VS1053SetVolume(VS1053Driver* VS1053p, uint8_t leftVol, uint8_t rightVol);
  uint8_t VS1053SendData(VS1053Driver* VS1053p, const char* data, uint8_t bytes);
  void VS1053StopPlaying(VS1053Driver* VS1053p);
  void VS1053ReadHeaderData(VS1053Driver* VS1053p, uint16_t* headerData0, uint16_t* headerData1);
  uint16_t VS1053ReadStatus(VS1053Driver* VS1053p);
  uint16_t VS1053ReadSampleRate(VS1053Driver* VS1053p);
  void VS1053GiveBus(VS1053Driver* VS1053p);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_VS1053 */

#endif /* _VS1053_H_ */

/** @} */
