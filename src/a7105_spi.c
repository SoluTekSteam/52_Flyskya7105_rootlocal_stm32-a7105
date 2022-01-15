#include "a7105_spi.h"

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

void a7105SpiStrobe(A7105_Strobe data) {
    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(conf.spi, (uint8_t *) &data, 1, A7105_SPI_TIMEOUT);
    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_SET);
}

void a7105SpiSetId(uint32_t id) {
    uint8_t data[4];
    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_RESET);

    uint8_t reg = A7105_06_ID_DATA;
    HAL_SPI_Transmit(conf.spi, &reg, 1, A7105_SPI_TIMEOUT);

    data[0] = (id >> 24) & 0xFF;
    data[1] = (id >> 16) & 0xFF;
    data[2] = (id >> 8) & 0xFF;
    data[3] = (id >> 0) & 0xFF;

    HAL_SPI_Transmit(conf.spi, data, 4, A7105_SPI_TIMEOUT);

    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_SET);
}

uint32_t a7105SpiGetId(void) {
    uint32_t ID;
    uint8_t idBytes[4];

    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_RESET);
    /// We could read this straight into ID, but the result would depend on
    /// the endiness of the architecture.

    uint8_t reg = A7105_06_ID_DATA;
    HAL_SPI_Transmit(conf.spi, &reg, 1, A7105_SPI_TIMEOUT);
    HAL_SPI_Receive(conf.spi, idBytes, 4, A7105_SPI_TIMEOUT);
    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_SET);

    ID = (uint32_t) idBytes[0] << 24;
    ID |= (uint32_t) idBytes[1] << 16;
    ID |= (uint32_t) idBytes[2] << 8;
    ID |= (uint32_t) idBytes[3] << 0;


    return (ID);
}

void a7105SpiRegWrite(uint8_t address, uint8_t data) {

    /// Address byte[7]: Control registers (0) or strobe command(1)
    /// Address byte[6]: Write to (0) or read from (1) control register
    address &= 0x3f;
    uint8_t pData[2] = {address, data};

    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(conf.spi, pData, 2, A7105_SPI_TIMEOUT);
    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_SET);
}

uint8_t a7105SpiRegRead(uint8_t reg) {
    uint8_t data[1] = {0};
    /// Don't forget register read bit (bit 6 -> 0x40)
    reg &= 0x3f;
    reg |= 0x40;

    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(conf.spi, &reg, 1, A7105_SPI_TIMEOUT);
    HAL_SPI_Receive(conf.spi, data, 1, A7105_SPI_TIMEOUT);
    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_SET);

    return data[0];
}

void A7105SpiWriteData(uint8_t *data, size_t len, uint8_t channel) {
    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_RESET);

    uint8_t reg = A7105_STROBE_RST_WRPTR;
    HAL_SPI_Transmit(conf.spi, &reg, 1, A7105_SPI_TIMEOUT);
    reg = A7105_05_FIFO_DATA;
    HAL_SPI_Transmit(conf.spi, &reg, 1, A7105_SPI_TIMEOUT);

    /// transmit data
    HAL_SPI_Transmit(conf.spi, data, len, A7105_SPI_TIMEOUT);

    HAL_GPIO_WritePin(conf.SCS_PORT, conf.SCS_PIN, GPIO_PIN_SET);
    a7105SpiRegWrite(A7105_0F_PLL_I, channel);
    a7105SpiStrobe(A7105_STROBE_TX);
}

void A7105SpiReadData(uint8_t *buffer, size_t len) {
    a7105SpiStrobe(A7105_STROBE_RST_RDPTR);

    for (size_t i = 0; i < len; i++) {
        buffer[i] = a7105SpiRegRead(A7105_05_FIFO_DATA);
    }
}