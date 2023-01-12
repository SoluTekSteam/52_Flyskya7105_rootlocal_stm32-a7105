#include "a7105.h"

void A7105Init(void) {
    A7105Reset();
    /// After reset 16MHz crystal and 500Kbps data rate are already
    /// configured. The default base frequency is then 2400.001454 MHz.
    /// A channel width of 500KHz is recommended. 16MHz/(4*500KHz)-1 = 7,
    /// leave all the other bits as they are.
    a7105SpiRegWrite(A7105_10_PLL_II, 0xf0 | (7 << 1));

    /// Note: To stay in the 2.4 to 2.483 GHz ISM band, only channels 1
    /// through 166 should be used (with "Auto IF Exchange" enabled, master
    /// sends on 1-166, slave sends on 0-165).

    /**
     * Current understanding of the datasheet (concerning Auto IF):
     * Master sends on channel n, sets ULS=0 to receive on channel n-1.
     * Slave then has to send on channel n-1, but sets ULS=1 to receive on
     * channel n-1+1=n. Besides the bad english, the datasheet is also
     * not clear about what to do if the channel width is not 500KHz, as
     * ULS always shifts by 500KHz.
     */

    /// Demodulator DC estimation average mode: 0x1 is recommended.
    /// Preamble pattern detection length: 0x2 recommended for 500Kbps
    a7105SpiRegWrite(A7105_20_CODE_II, 0x4 | (0x1 << 4) | 0x2);

    /// Demodulator DC estimation mode: Average and hold (0x2)
    a7105SpiRegWrite(A7105_29_RX_DEM_TEST_I, 0x07 | (0x2 << 5));

    /// Enable auto IF offset (shift frequency while receiving) and FIFO mode
    a7105SpiRegWrite(A7105_01_MODE_CONTROL, (1 << 5) | (1 << 1));

    /// Set BPF bandwidth to 500KHz (bits 6 and 5 shall always be set to 1)
    /// Although it always reads 0, we assume 500KHz is already selected
    /// after reset.
    /// a7105SpiRegWrite(A7105_18_RX, (0x3 << 5) | 0x02);

    /// Disable FIFO extension (FPM = 0) and segmentation (PSA = 0)
    a7105SpiRegWrite(A7105_04_FIFO_II, 0);

    /// Set FIFO size to 1 (for testing only)
    a7105SpiRegWrite(A7105_03_FIFO_I, 0);

}

uint8_t A7105Reset(void) {
    uint8_t RESETN = 0x00; ///< Write to this register by 0x00 to issue reset command, then it is auto clear
    uint8_t result;
    a7105SpiRegWrite(A7105_00_MODE, RESETN);
    HAL_Delay(1000);

    /// Set both GPIO as output and low
    A7105SetTxRxMode(TXRX_OFF);
    result = a7105SpiRegRead(A7105_10_PLL_II) == 0x9E;
    a7105SpiStrobe(A7105_STROBE_STANDBY);

    return result;
}

void A7105SetChannel(uint8_t chnl) {
    a7105SpiRegWrite(A7105_0F_PLL_I, chnl);
}

void A7105SetTxRxMode(enum TXRX_State mode) {
    if (mode == TX_EN) {
        a7105SpiRegWrite(A7105_0B_GPIO1_PIN_I, 0x33);
        a7105SpiRegWrite(A7105_0C_GPIO2_PIN_II, 0x31);
    } else if (mode == RX_EN) {
        a7105SpiRegWrite(A7105_0B_GPIO1_PIN_I, 0x31);
        a7105SpiRegWrite(A7105_0C_GPIO2_PIN_II, 0x33);
    } else {
        /// The A7105 seems to some with a cross-wired power-amp (A7700)
        /// On the XL7105-D03, TX_EN -> RXSW and RX_EN -> TXSW
        /// This means that sleep mode is wired as RX_EN = 1 and TX_EN = 1
        /// If there are other amps in use, we'll need to fix this
        a7105SpiRegWrite(A7105_0B_GPIO1_PIN_I, 0x33);
        a7105SpiRegWrite(A7105_0C_GPIO2_PIN_II, 0x33);
    }
}

uint8_t A7105Calib(void) {
    uint8_t retVal = 0;
    uint8_t calibReg;

    /// Assume MFBS == 0 after reset
    a7105SpiStrobe(A7105_STROBE_PLL);

    /// Start filter bank (FBC, bit 0), VCO current (VCC, bit 2) and VCO
    /// bank (VBC, bit 1) calibrations all at the same time.
    /// Leave VTH and VTL at recommended values.
    a7105SpiRegWrite(A7105_02_CALC, 0x07);

    /// We could poll the end of the calibration and use a timer to determine
    /// if it takes too long. That's not easily portable, so we waste some
    /// time instead.
    HAL_Delay(1000U);

    calibReg = a7105SpiRegRead(A7105_02_CALC);

    /// Check calibration timeouts
    if (A7105_TEST_BIT(calibReg, 0)) {
        retVal |= 0x10; ///< IF filter bank calibration took too long
    }

    if (A7105_TEST_BIT(calibReg, 1)) {
        retVal |= 0x01; ///< VCO bank calibration took too long
    }

    if (A7105_TEST_BIT(calibReg, 2)) {
        retVal |= 0x04; ///< VCO current calibration took too long
    }

    /// Check calibration success
    /// FBCF bit: Indicates calib failure
    if (A7105_TEST_BIT(a7105SpiRegRead(A7105_22_IF_CALIB_I), 4)) {
        retVal |= 0x20; ///< IF calib not successful
    }

    /// VBCF bit: Indicates calib failure
    if (A7105_TEST_BIT(a7105SpiRegRead(A7105_25_VCO_SBCAL_I), 3)) {
        retVal |= 0x02; ///< VCO bank calibration not successful
    }

    /// FVCC bit: Indicates calib failure
    if (A7105_TEST_BIT(a7105SpiRegRead(A7105_24_VCO_CURCAL), 4)) {
        retVal |= 0x08; ///< VCO current calib not successful
    }

    a7105SpiStrobe(A7105_STROBE_STANDBY);

    return retVal;
}

void A7105SetPowerReg(uint8_t PAC_reg, uint8_t TBG_reg) {
    a7105SpiRegWrite(A7105_28_TX_TEST, (PAC_reg << 3) | TBG_reg);
}

void A7105SetPower(TxPower power) {
    uint8_t PAC_reg, TBG_reg;

    switch (power) {
        case 0:
            PAC_reg = 0;
            TBG_reg = 0;
            break;
        case 1:
            PAC_reg = 0;
            TBG_reg = 1;
            break;
        case 2:
            PAC_reg = 0;
            TBG_reg = 2;
            break;
        case 3:
            PAC_reg = 0;
            TBG_reg = 4;
            break;
        case 4:
            PAC_reg = 1;
            TBG_reg = 5;
            break;
        case 5:
            PAC_reg = 2;
            TBG_reg = 7;
            break;
        case 6:
        case 7:
            PAC_reg = 3;
            TBG_reg = 7;
            break;
        default:
            PAC_reg = 0;
            TBG_reg = 0;
            break;
    }

    A7105SetPowerReg(PAC_reg, TBG_reg);
}

void setId(uint32_t id) {
    a7105SpiSetId(id);
}