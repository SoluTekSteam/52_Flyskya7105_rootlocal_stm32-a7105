#include "a7105.h"

void A7105Init(void) {

}

uint8_t A7105Reset(void) {
    uint8_t RESETN = 0x00; ///< Write to this register by 0x00 to issue reset command, then it is auto clear

    a7105SpiRegWrite(A7105_00_MODE, RESETN);
    HAL_Delay(1000);

    /// Set both GPIO as output and low
    A7105SetTxRxMode(TXRX_OFF);
    uint8_t result = a7105SpiRegRead(A7105_10_PLL_II) == 0x9E;
    a7105SpiStrobe(A7105_STROBE_STANDBY);

    return result;
}

void A7105SetTxRxMode(enum TXRX_State mode) {
    if(mode == TX_EN) {
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

void setPower(uint8_t PAC_reg, uint8_t TBG_reg) {
    a7105SpiRegWrite(A7105_28_TX_TEST, (PAC_reg << 3) | TBG_reg);
}