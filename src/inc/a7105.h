#pragma once
#ifndef A7105_H
#define A7105_H

#include <stdint.h>
#include "a7105_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

//Bit operations
#define A7105_WRITE(a)    ((a) & ~0x40)
#define A7105_READ(a)     ((a) | 0x40)
#define A7105_SET_BIT(byte, bit) ((byte) |= ((1) << (bit)))
#define A7105_CLEAR_BIT(byte, bit) ((byte) &= ~((1) << (bit)))
#define A7105_TOGGLE_BIT(byte, bit) ((byte) ^= ((1) << (bit)))
#define A7105_TEST_BIT(byte, bit) (((byte) >> (bit)) & (1))    //returns 1 or 0

/// TX/RX state
typedef enum TXRX_State {
    TXRX_OFF,
    TX_EN,
    RX_EN,
} TXRX_State;

#define A7105_0F_CHANNEL A7105_0F_PLL_I

typedef enum A7105_MASK {
    A7105_MASK_FBCF = 1 << 4,
    A7105_MASK_VBCF = 1 << 3,
} A7105_MASK;


/**
 * Resets the A7105, putting it into standby mode.
 * @todo add return description
 */
extern uint8_t A7105Reset(void);

/**
 * @brief Perform 3 calibrations as in chapter 15 of datasheet.
 * @details     Should be performed when everything is set up (to the point
 *              that a channel is selected).
 * @return 0 on success The ored combination of the following values:
 *    - 0x01  if VCO bank calibration took more than 1000us
 *    - 0x02  if VCO bank calibration was not successful
 *    - 0x04  if VCO current calibration took more than 1000us
 *    - 0x08  if VCO current calibration was not successful
 *    - 0x10  if IF filter bank calibration took more than 1000us
 *    - 0x20  if IF filter bank calibration was not successful
 */
extern uint8_t A7105Calib(void);

extern void A7105Init(void);

/**
 * @brief Send a packet of data to the A7105
 * @param data Byte array to send
 * @param len Length of the byte array
 * @param channel Channel to transmit data on
 */
extern void writeData(uint8_t *data, size_t len, uint8_t channel);

/**
 * @brief Read a packet of date from the A7105
 * @param buffer Byte array to hold the incoming data
 * @param len Length of the buffer, number of bytes to read in
 */
extern void readData(uint8_t *buffer, size_t len);

/**
 * @brief Set the A7105's ID
 * @param id 32-bit identifier
 */
extern void setId(uint32_t id);

/**
 * @brief A7105 supports programmable TX power from –20dBm ~1dBm by TX test register (28h).
 * @details User can configures PAC[1:0] and TBG[2:0] for different TX power level.
 *          The following tables show the typical TX power vs.
 *          current in different settings.
 *
 * | Address | Bit7   | Bit6   | Bit5   | Bit4   | Bit3   | Bit2   | Bit1   | Bit0   |
 * | :----:  | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
 * | 28h     | -      | -      | TXCS   | PAC1   | PAC0   | TBG2   | TBG1   | TBG0   |
 *
 * For 0 dBm TX output power, the register setting: PAC = 2 and TBG = 7 are recommended.
 * For -10 dBm TX output power (low current requirement), PAC = 1 and TBG = 3 is recommended.
 * @param PAC_reg PAC[1:0] Register
 * @param TBG_reg TBG[2:0] Register
 */
extern void A7105SetPowerReg(uint8_t PAC_reg, uint8_t TBG_reg);

/**
 * @brief A7105 supports programmable TX power from –20dBm ~1dBm by TX test register (28h).
 *
 * @see setPower(uint8_t PAC_reg, uint8_t TBG_reg)
 * @param power TxPower
 */
extern void A7105SetPower(TxPower power);

/**
 * @brief Sets the TxRx mode
 * @aparam mode TxRx mode
 */
extern void A7105SetTxRxMode(enum TXRX_State mode);

extern void A7105SetChannel(uint8_t chnl);

#ifdef __cplusplus
}
#endif

#endif //A7105_H
