/**
 * @file    vs1053.c
 * @brief   VS1053 Driver code.
 *
 * @addtogroup VS1053
 * @{
 */

#include "hal.h"
#include "vs1053.h"
#include <string.h>

#if HAL_USE_VS1053 || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/
#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03

/*SCI Registers*/
#define SCI_MODE        	0x00
#define SCI_STATUS      	0x01
#define SCI_BASS        	0x02
#define SCI_CLOCKF      	0x03
#define SCI_DECODE_TIME 	0x04
#define SCI_AUDATA      	0x05
#define SCI_WRAM        	0x06
#define SCI_WRAMADDR    	0x07
#define SCI_HDAT0       	0x08
#define SCI_HDAT1       	0x09
#define SCI_AIADDR      	0x0a
#define SCI_VOL         	0x0b
#define SCI_AICTRL0     	0x0c
#define SCI_AICTRL1     	0x0d
#define SCI_AICTRL2     	0x0e
#define SCI_AICTRL3     	0x0f

/* SCI_MODE Register bits */
#define SM_DIFF         	0x0001
#define SM_LAYER12			0x0002
#define SM_RESET        	0x0004
#define SM_CANCEL           0x0008
#define SM_PDOWN        	0x0010
#define SM_TESTS        	0x0020
#define SM_STREAM       	0x0040
#define SM_PLUSV        	0x0080
#define SM_DACT         	0x0100
#define SM_SDIORD       	0x0200
#define SM_SDISHARE     	0x0400
#define SM_SDINEW       	0x0800
#define SM_ADPCM        	0x1000
#define SM_ADPCM_HP     	0x2000
#define SM_LINE1            0x4000

/*Common Parameter Addresses*/
#define PARA_endFillByte    0x1E06

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/
static bool ReadDREQ(VS1053Driver* VS1053p)
{
    return palReadPad(VS1053p->config->xDREQPort, VS1053p->config->xDREQPad);
}
static bool ResetChip(VS1053Driver* VS1053p)
{
    palClearPad(VS1053p->config->xResetPort, VS1053p->config->xResetPad);

    chThdSleep(MS2ST(10));

    palSetPad(VS1053p->config->xResetPort, VS1053p->config->xResetPad);

    chThdSleep(MS2ST(5));

    if (ReadDREQ(VS1053p) == false)
    {
        return false;
    }
    return true;
}

static void ActivateSCI(VS1053Driver* VS1053p)
{
    palClearPad(VS1053p->config->xCSPort, VS1053p->config->xCSPad);
}

static void DeactivateSCI(VS1053Driver* VS1053p)
{
    palSetPad(VS1053p->config->xCSPort, VS1053p->config->xCSPad);
}

static void ActivateSDI(VS1053Driver* VS1053p)
{
    palClearPad(VS1053p->config->xDCSPort, VS1053p->config->xDCSPad);
}

static void DeactivateSDI(VS1053Driver* VS1053p)
{
    palSetPad(VS1053p->config->xDCSPort, VS1053p->config->xDCSPad);
}


static void WriteRegister(VS1053Driver* VS1053p, uint8_t addressbyte, uint16_t highbyte, uint8_t lowbyte)
{
    ActivateSCI(VS1053p);
    spiSelect(VS1053p->config->spid);
    VS1053p->txBuffer[0] = VS_WRITE_COMMAND;
    VS1053p->txBuffer[1] = addressbyte;
    VS1053p->txBuffer[2] = highbyte;
    VS1053p->txBuffer[3] = lowbyte;
    spiSend(VS1053p->config->spid, 4, VS1053p->txBuffer);
    spiUnselect(VS1053p->config->spid);
    DeactivateSCI(VS1053p);

    while (ReadDREQ(VS1053p) == false)
    {
    }
}

static uint16_t ReadRegister(VS1053Driver* VS1053p, uint8_t addressbyte)
{
    ActivateSCI(VS1053p);

    spiSelect(VS1053p->config->spid);
    VS1053p->txBuffer[0] = VS_READ_COMMAND;
    VS1053p->txBuffer[1] = addressbyte;
    spiExchange(VS1053p->config->spid, 4, VS1053p->txBuffer, VS1053p->rxBuffer);
    spiUnselect(VS1053p->config->spid);

    DeactivateSCI(VS1053p);

    uint16_t result = VS1053p->rxBuffer[2] << 8;
    result |= VS1053p->rxBuffer[3];

    return result;
}

static void SoftReset(VS1053Driver* VS1053p)
{
    WriteRegister(VS1053p, SCI_MODE, SM_SDINEW>>8, SM_RESET);

}

static void SetVolume(VS1053Driver* VS1053p, uint8_t left, uint8_t right)
{
    WriteRegister(VS1053p, SCI_VOL, left,  right);
}

static bool InitChip(VS1053Driver* VS1053p)
{
    /*Set clock frequency*/
    WriteRegister(VS1053p, SCI_CLOCKF,0x60 | 0x08, 0x00);

    /*Set mode*/
    SoftReset(VS1053p);

    SetVolume(VS1053p, 50, 50);


    return true;
}
static uint8_t ReadEndFillByte(VS1053Driver* VS1053p)
{
    WriteRegister(VS1053p, SCI_WRAMADDR,(PARA_endFillByte & 0xFF00) >> 8, (PARA_endFillByte & 0x00FF));
    return (uint8_t)ReadRegister(VS1053p, SCI_WRAM) & 0xFF;
}
/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   VS1053 Driver initialization.
 * @note    This function is implicitly invoked by @p halInit(), there is
 *          no need to explicitly initialize the driver.
 *
 * @init
 */
void VS1053Init(void) {

}

/**
 * @brief   Initializes the standard part of a @p VS1053Driver structure.
 *
 * @param[out] VS1053p     pointer to the @p VS1053Driver object
 *
 * @init
 */
void VS1053ObjectInit(VS1053Driver* VS1053p) {

	VS1053p->state = VS1053_STOP;
	VS1053p->config = NULL;
}

/**
 * @brief   Configures and activates the VS1053 peripheral.
 *
 * @param[in] VS1053p      pointer to the @p VS1053Driver object
 * @param[in] config    pointer to the @p VS1053Config object
 *
 * @api
 */

void VS1053Start(VS1053Driver* VS1053p, const VS1053Config* config) {

	osalDbgCheck((VS1053p != NULL) && (config != NULL));

	osalSysLock();
	osalDbgAssert((VS1053p->state == VS1053_STOP) || (VS1053p->state == VS1053_READY),
			"invalid state");
	VS1053p->config = config;

	osalSysUnlock();

	if (ResetChip(VS1053p) == false)
	{
	    return;
	}

	DeactivateSCI(VS1053p);
	DeactivateSDI(VS1053p);

	spiStart(config->spid, config->spiCfg);

	spiUnselect(VS1053p->config->spid);

	InitChip( VS1053p);

	osalSysLock();
	VS1053p->state = VS1053_ACTIVE;
	osalSysUnlock();
}

/**
 * @brief   Deactivates the VS1053 peripheral.
 *
 * @param[in] VS1053p      pointer to the @p VS1053Driver object
 *
 * @api
 */
void VS1053Stop(VS1053Driver* VS1053p) {

	osalDbgCheck(VS1053p != NULL);

	osalSysLock();
	osalDbgAssert(VS1053p->state == VS1053_ACTIVE, "invalid state");
	osalSysUnlock();

	palClearPad(VS1053p->config->xResetPort, VS1053p->config->xResetPad);

	spiStop(VS1053p->config->spid);

	osalSysLock();
	VS1053p->state = VS1053_STOP;
	osalSysUnlock();
}

void VS1053SineTest(VS1053Driver* VS1053p, uint16_t freq, uint8_t leftVol, uint8_t rightVol)
{
    osalDbgCheck(VS1053p != NULL);

    osalSysLock();
    osalDbgAssert(VS1053p->state == VS1053_ACTIVE, "invalid state");
    osalSysUnlock();

    /*Set sample rate*/
    WriteRegister(VS1053p, SCI_AUDATA, 0x45, 0xAC);

    /*Set volume*/
    WriteRegister(VS1053p, SCI_VOL, leftVol, rightVol);

    /*Set frequency*/
    WriteRegister(VS1053p, SCI_AICTRL0, 0x00ff & (freq >> 8), 0x00ff & freq);
    WriteRegister(VS1053p, SCI_AICTRL1, 0x00ff & (freq >> 8), 0x00ff & freq);

    /*Start test*/
    WriteRegister(VS1053p, SCI_AIADDR, 0x40, 0x20);
}

/*
 * Volume Control. 0 = Max, 254 is Silence, 255 Analog Power Down
 */
void VS1053SetVolume(VS1053Driver* VS1053p, uint8_t leftVol, uint8_t rightVol)
{
    osalDbgCheck(VS1053p != NULL);

    osalSysLock();
    osalDbgAssert(VS1053p->state == VS1053_ACTIVE, "invalid state");
    osalSysUnlock();

    /*Set volume*/
    SetVolume(VS1053p, leftVol, rightVol);
}

uint8_t VS1053SendData(VS1053Driver* VS1053p, const char* data, uint8_t bytes)
{
    while (ReadDREQ(VS1053p) == false)
    {
        chThdSleep(MS2ST(1));
    }

    DeactivateSCI(VS1053p);
    ActivateSDI(VS1053p);

    spiSelect(VS1053p->config->spid);
    spiSend(VS1053p->config->spid, bytes, data);
    spiUnselect(VS1053p->config->spid);

    DeactivateSDI(VS1053p);

    return bytes;
}

void VS1053StopPlaying(VS1053Driver* VS1053p)
{
    uint8_t endFillByte = ReadEndFillByte(VS1053p);
    char buf[32];
    memset(buf, endFillByte, sizeof(buf));

    int8_t y;
    for (y = 0; y < 66; y++)
        VS1053SendData(VS1053p, buf, sizeof(buf)); // 66*32 > 2052

    WriteRegister(VS1053p, SCI_MODE, SM_SDINEW>>8, SM_CANCEL);

    uint16_t byteCount = 0;
    while (true)
    {
        VS1053SendData(VS1053p, buf, sizeof(buf));
        byteCount = byteCount+ sizeof(buf);
        uint16_t mode = ReadRegister(VS1053p, SCI_MODE);
        if ((mode & SM_CANCEL) == 0)
        {
            WriteRegister(VS1053p, SCI_MODE, SM_SDINEW>>8, SM_LAYER12);
            return;
        }
        if(byteCount > 2048)
        {
            break;
        }
    }

    SoftReset(VS1053p);
}

void VS1053ReadHeaderData(VS1053Driver* VS1053p, uint16_t* headerData0, uint16_t* headerData1)
{
    while (ReadDREQ(VS1053p) == false)
    {
        chThdSleep(MS2ST(1));
    }
    *headerData0 = ReadRegister(VS1053p, SCI_HDAT0);
    *headerData1 = ReadRegister(VS1053p, SCI_HDAT1);
}

uint16_t VS1053ReadStatus(VS1053Driver* VS1053p)
{
    while (ReadDREQ(VS1053p) == false)
    {
        chThdSleep(MS2ST(1));
    }
    return ReadRegister(VS1053p, SCI_STATUS);
}

uint16_t VS1053ReadSampleRate(VS1053Driver* VS1053p)
{
    while (ReadDREQ(VS1053p) == false)
    {
        chThdSleep(MS2ST(1));
    }
    return ReadRegister(VS1053p, SCI_AUDATA);
}

void VS1053GiveBus(VS1053Driver* VS1053p)
{
    ActivateSDI(VS1053p);
    ActivateSCI(VS1053p);
}

#endif /* HAL_USE_VS1053 */

/** @} */
