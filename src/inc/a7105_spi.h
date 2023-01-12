#pragma once
#ifndef A7105_SPI_H
#define A7105_SPI_H

#include "stdint.h"
#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define A7105_SPI_TIMEOUT                   100     ///< spi timeout


/// A7105 states for strobe
typedef enum A7105_Strobe {
    A7105_STROBE_SLEEP = 0x80,
    A7105_STROBE_IDLE = 0x90,
    A7105_STROBE_STANDBY = 0xA0,
    A7105_STROBE_PLL = 0xB0,
    A7105_STROBE_RX = 0xC0,
    A7105_STROBE_TX = 0xD0,
    A7105_STROBE_RST_WRPTR = 0xE0,
    A7105_STROBE_RST_RDPTR = 0xF0,
} A7105_Strobe;

/// Register addresses
typedef enum A7105_Register_address {
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
    A7105_0F_PLL_I = 0x0F,              ///< CHN [7:0]: LO channel number select.
    A7105_10_PLL_II = 0x10,
    A7105_11_PLL_III = 0x11,
    A7105_12_PLL_IV = 0x12,
    A7105_13_PLL_V = 0x13,
    A7105_14_TX_I = 0x14,
    A7105_15_TX_II = 0x15,
    A7105_16_DELAY_I = 0x16,
    A7105_17_DELAY_II = 0x17,
    A7105_18_RX = 0x18,
    A7105_19_RX_GAIN_I = 0x19,          /// RX Gain Register I
    A7105_1A_RX_GAIN_II = 0x1A,         /// RX Gain Register II
    A7105_1B_RX_GAIN_III = 0x1B,        /// RX Gain Register III
    A7105_1C_RX_GAIN_IV = 0x1C,         /// RX Gain Register IV
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
} A7105_Register_address;

/// A7105 Config struct
typedef struct A7105_SPI_t {
    SPI_HandleTypeDef *spi;
    GPIO_TypeDef *SCS_PORT;
    uint16_t SCS_PIN;
    GPIO_TypeDef *GIO1_PORT;
    uint16_t GIO1_PIN;
    GPIO_TypeDef *GIO2_PORT;
    uint16_t GIO2_PIN;
} A7105_SPI_t;

/// Power amp is ~+16dBm so:
typedef enum TxPower {
    TXPOWER_100uW,  // -23dBm == PAC=0 TBG=0
    TXPOWER_300uW,  // -20dBm == PAC=0 TBG=1
    TXPOWER_1mW,    // -16dBm == PAC=0 TBG=2
    TXPOWER_3mW,    // -11dBm == PAC=0 TBG=4
    TXPOWER_10mW,   // -6dBm  == PAC=1 TBG=5
    TXPOWER_30mW,   // 0dBm   == PAC=2 TBG=7
    TXPOWER_100mW,  // 1dBm   == PAC=3 TBG=7
    TXPOWER_150mW,  // 1dBm   == PAC=3 TBG=7
    TXPOWER_LAST,
} TxPower;

/**
 * @brief Low Level SCS
 */
extern void SCS_LO();

/**
 * @brief High Level SCS
 */
extern void SCS_HI();

/**
 * @brief Set Spi Interface
 * @param spi SPI handle Structure definition
 */
extern void setA7105Spi(SPI_HandleTypeDef *spi);

/**
 * @brief Chip Select port
 * @param SCS_PORT General Purpose I/O
 * @param SCS_PIN Pin I/O
 */
extern void setA7105_SCS(GPIO_TypeDef *SCS_PORT, uint16_t SCS_PIN);

extern void setA7105_GIO1(GPIO_TypeDef *GIO1_PORT, uint16_t GIO1_PIN);

extern void setA7105_GIO2(GPIO_TypeDef *GIO2_PORT, uint16_t GIO2_PIN);

/**
* @bref Writes a value to the given register
* @param address Address of the register to write to
* @param data Value to write into the register
* @return Value returned from slave when writing the register
*/
extern void a7105SpiRegWrite(uint8_t address, uint8_t data);

/**
* @brief Sends a strobe command to the A7105
* @param Strobe command state
* @return Value returned from slave when writing the register
*/
extern void a7105SpiStrobe(enum A7105_Strobe data);

/**
* @brief Reads a value from the given register
* @param reg Address of the register to read
* @return The value of the register
*/
extern uint8_t a7105SpiRegRead(uint8_t reg);

extern void a7105SpiSetId(uint32_t id);

extern uint32_t a7105SpiGetId(void);

extern void A7105SpiWriteData(uint8_t *data, size_t len, uint8_t channel);

extern void A7105SpiReadData(uint8_t *buffer, size_t len);

#ifdef __cplusplus
extern "C" {
#endif

#endif //A7105_SPI_H
