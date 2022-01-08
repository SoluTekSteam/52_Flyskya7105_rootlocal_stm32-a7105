#pragma once
#ifndef A7105_SPI_H
#define A7105_SPI_H

#include "stdint.h"
#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define A7105_SPI_TIMEOUT                   100

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

    void a7105SpiRegWrite16(uint8_t address, uint8_t data);

    void a7105SpiRegWrite8(uint8_t data);

    uint8_t a7105SpiRegRead(uint8_t reg);

#ifdef __cplusplus
extern "C" {
#endif

#endif //A7105_SPI_H
