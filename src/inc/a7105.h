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

/**
 * TX/RX state
 */
enum TXRX_State {
    TXRX_OFF,
    TX_EN,
    RX_EN,
};


/**
 * A7105 states for strobe
 */
enum A7105_Strobe {
    A7105_STROBE_SLEEP = 0x80,
    A7105_STROBE_IDLE = 0x90,
    A7105_STROBE_STANDBY = 0xA0,
    A7105_STROBE_PLL = 0xB0,
    A7105_STROBE_RX = 0xC0,
    A7105_STROBE_TX = 0xD0,
    A7105_STROBE_RST_WRPTR = 0xE0,
    A7105_STROBE_RST_RDPTR = 0xF0,
};


/**
 * Register addresses
 */
enum {
    A7105_00_MODE = 0x00,
    A7105_01_MODE_CONTROL = 0x01,
    A7105_02_CALC = 0x02,
    A7105_03_FIFO_I = 0x03,
    A7105_04_FIFO_II = 0x04,
    A7105_05_FIFO_DATA = 0x05,
    A7105_06_ID_DATA = 0x06,
    A7105_07_RC_OSC_I = 0x07,
    A7105_08_RC_OSC_II = 0x08,
    A7105_09_RC_OSC_III = 0x09,
    A7105_0A_CK0_PIN = 0x0A,
    A7105_0B_GPIO1_PIN_I = 0x0B,
    A7105_0C_GPIO2_PIN_II = 0x0C,
    A7105_0D_CLOCK = 0x0D,
    A7105_0E_DATA_RATE = 0x0E,
    A7105_0F_PLL_I = 0x0F,
    A7105_10_PLL_II = 0x10,
    A7105_11_PLL_III = 0x11,
    A7105_12_PLL_IV = 0x12,
    A7105_13_PLL_V = 0x13,
    A7105_14_TX_I = 0x14,
    A7105_15_TX_II = 0x15,
    A7105_16_DELAY_I = 0x16,
    A7105_17_DELAY_II = 0x17,
    A7105_18_RX = 0x18,
    A7105_19_RX_GAIN_I = 0x19,
    A7105_1A_RX_GAIN_II = 0x1A,
    A7105_1B_RX_GAIN_III = 0x1B,
    A7105_1C_RX_GAIN_IV = 0x1C,
    A7105_1D_RSSI_THOLD = 0x1D,
    A7105_1E_ADC = 0x1E,
    A7105_1F_CODE_I = 0x1F,
    A7105_20_CODE_II = 0x20,
    A7105_21_CODE_III = 0x21,
    A7105_22_IF_CALIB_I = 0x22,
    A7105_23_IF_CALIB_II = 0x23,
    A7105_24_VCO_CURCAL = 0x24,
    A7105_25_VCO_SBCAL_I = 0x25,
    A7105_26_VCO_SBCAL_II = 0x26,
    A7105_27_BATTERY_DET = 0x27,
    A7105_28_TX_TEST = 0x28,
    A7105_29_RX_DEM_TEST_I = 0x29,
    A7105_2A_RX_DEM_TEST_II = 0x2A,
    A7105_2B_CPC = 0x2B,
    A7105_2C_XTAL_TEST = 0x2C,
    A7105_2D_PLL_TEST = 0x2D,
    A7105_2E_VCO_TEST_I = 0x2E,
    A7105_2F_VCO_TEST_II = 0x2F,
    A7105_30_IFAT = 0x30,
    A7105_31_RSCALE = 0x31,
    A7105_32_FILTER_TEST = 0x32,
};

#define A7105_0F_CHANNEL A7105_0F_PLL_I

enum A7105_MASK {
    A7105_MASK_FBCF = 1 << 4,
    A7105_MASK_VBCF = 1 << 3,
};


/**
 * Resets the A7105, putting it into standby mode.
 */
void A7105Reset(void);

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
uint8_t A7105Calib(void);

void A7105Init(void);

/**
* Writes a value to the given register
*
* @param regAddr Address of the register to write to
* @param value Value to write into the register
* @return Value returned from slave when writing the register
*/
uint8_t writeRegister(uint8_t regAddr, uint8_t value);

/**
* @brief Reads a value from the given register
*
* @param regAddr Address of the register to read
* @return The value of the register
*/
uint8_t readRegister(uint8_t regAddr);

/**
 * @brief Sends a strobe command to the A7105
 *
 * @param state Strobe command state
 */
uint8_t strobe(enum A7105_Strobe state);

/**
 * @brief Send a packet of data to the A7105
 *
 * @param data Byte array to send
 * @param len Length of the byte array
 * @param channel Channel to transmit data on
 */
void writeData(uint8_t *data, size_t len, uint8_t channel);

/**
 * @brief Read a packet of date from the A7105
 *
 * @param buffer Byte array to hold the incoming data
 * @param len Length of the buffer, number of bytes to read in
 */
void readData(uint8_t *buffer, size_t len);

/**
 * @brief Set the A7105's ID
 *
 * @param id 32-bit identifier
 */
void setId(uint32_t id);

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
 * @param PAC_reg PAC[1:0] Register
 * @param TBG_reg TBG[2:0] Register
 */
void setPower(uint8_t PAC_reg, uint8_t TBG_reg);

/**
 * @brief Sets the TxRx mode
 *
 * @aparam mode TxRx mode
 */
void setTxRxMode(enum TXRX_State mode);

#ifdef __cplusplus
}
#endif

#endif //A7105_H
