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

/// A7105 Config struct
struct A7105_SPI_t {
    SPI_HandleTypeDef *spi;
    GPIO_TypeDef *SCS_PORT;
    uint16_t SCS_PIN;
    GPIO_TypeDef *GIO1_PORT;
    uint16_t GIO1_PIN;
    GPIO_TypeDef *GIO2_PORT;
    uint16_t GIO2_PIN;
};

/**
 * @brief Set Spi Interface
 * @param spi SPI handle Structure definition
 */
void setA7105Spi(SPI_HandleTypeDef *spi);

/**
 * @brief Chip Select port
 * @param SCS_PORT General Purpose I/O
 * @param SCS_PIN Pin I/O
 */
void setA7105_SCS(GPIO_TypeDef *SCS_PORT, uint16_t SCS_PIN);

void setA7105_GIO1(GPIO_TypeDef *GIO1_PORT, uint16_t GIO1_PIN);

void setA7105_GIO2(GPIO_TypeDef *GIO2_PORT, uint16_t GIO2_PIN);

/**
* @bref Writes a value to the given register
* @param address Address of the register to write to
* @param data Value to write into the register
* @return Value returned from slave when writing the register
*/
void a7105SpiRegWrite(uint8_t address, uint8_t data);

/**
* @brief Sends a strobe command to the A7105
* @param Strobe command state
* @return Value returned from slave when writing the register
*/
void a7105SpiStrobe(enum A7105_Strobe data);

/**
* @brief Reads a value from the given register
* @param reg Address of the register to read
* @return The value of the register
*/
uint8_t a7105SpiRegRead(uint8_t reg);

#ifdef __cplusplus
extern "C" {
#endif

#endif //A7105_SPI_H
