#include "inc/a7105_spi.h"

struct A7105_SPI_t conf;

void setA7105Spi(SPI_HandleTypeDef *spi) {
    conf.spi = spi;
}

void setA7105_SCS(GPIO_TypeDef *SCS_PORT, uint16_t SCS_PIN) {
    conf.SCS_PORT = SCS_PORT;
    conf.SCS_PIN = SCS_PIN;
}

void setA7105_GIO1(GPIO_TypeDef *GIO1_PORT, uint16_t GIO1_PIN) {
    conf.GIO1_PORT = GIO1_PORT;
    conf.GIO1_PIN = GIO1_PIN;
}

void setA7105_GIO2(GPIO_TypeDef *GIO2_PORT, uint16_t GIO2_PIN) {
    conf.GIO2_PORT = GIO2_PORT;
    conf.GIO2_PIN = GIO2_PIN;
}

void a7105SpiRegWrite8(uint8_t data) {
    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(conf.spi, &data, 1, A7105_SPI_TIMEOUT);
    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_SET);
}

void a7105SpiRegWrite16(uint8_t address, uint8_t data) {

    // Address byte[7]: Control registers (0) or strobe command(1)
    // Address byte[6]: Write to (0) or read from (1) control register
    address &= 0x3f;
    uint8_t pData[2] = {address, data};

    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(conf.spi, pData, 2, A7105_SPI_TIMEOUT);
    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_SET);
}

uint8_t a7105SpiRegRead(uint8_t reg) {
    uint8_t data[1] = {0};

    // Don't forget register read bit (bit 6 -> 0x40)
    reg &= 0x3f;
    reg |= 0x40;

    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(conf.spi, &reg, 1, A7105_SPI_TIMEOUT);
    HAL_SPI_Receive(conf.spi, data, 1, A7105_SPI_TIMEOUT);
    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_SET);

    return data[0];
}