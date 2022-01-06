#ifndef A7105_H
#define A7105_H

#define _WRITE(a)    ((a) & ~0x40)
#define _READ(a)     ((a) | 0x40)

/**
 * TXRX state
 */
enum TXRX_State {
    TXRX_OFF,
    TX_EN,
    RX_EN,
};


/**
 * A7105 states for strobe
 */
enum A7105_State {
    A7105_SLEEP = 0x80,
    A7105_IDLE = 0x90,
    A7105_STANDBY = 0xA0,
    A7105_PLL = 0xB0,
    A7105_RX = 0xC0,
    A7105_TX = 0xD0,
    A7105_RST_WRPTR = 0xE0,
    A7105_RST_RDPTR = 0xF0,
};

/**
 * Register addresses
 */
enum {
    A7105_00_MODE = 0x00,
    A7105_01_MODE_CONTROL = 0x01,
    A7105_02_CALC = 0x02,
    A7105_03_FIFOI = 0x03,
    A7105_04_FIFOII = 0x04,
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
* Writes a value to the given register
*
* @param regAddr Address of the register to write to
* @param value Value to write into the register
* @return Value returned from slave when writing the register
*/
uint8_t writeRegister(uint8_t regAddr, uint8_t value);

/**
* Reads a value from the given register
*
* @param regAddr Address of the register to read
* @return The value of the register
*/
uint8_t readRegister(uint8_t regAddr);

/**
 * Sends a strobe command to the A7105
 *
 * @param state Strobe command state
 */
uint8_t strobe(enum A7105_State state);

/**
 * Send a packet of data to the A7105
 *
 * @param data Byte array to send
 * @param len Length of the byte array
 * @param channel Channel to transmit data on
 */
void writeData(uint8_t *data, size_t len, uint8_t channel);

/**
 * Read a packet of date from the A7105
 *
 * @param buffer Byte array to hold the incoming data
 * @param len Length of the buffer, number of bytes to read in
 */
void readData(uint8_t *buffer, size_t len);

/**
 * Set the A7105's ID
 *
 * @param id 32-bit identifier
 */
void setId(uint32_t id);

/**
 * Set the TX power
 *
 * @param power Output power in dBm
 */
void setPower(int32_t power);

/**
 * Sets the TxRx mode
 *
 * @aparam mode TxRx mode
 */
void setTxRxMode(enum TXRX_State mode);

/**
 * Resets the A7105, putting it into standby mode.
 */
int8_t reset();


#endif //A7105_H
