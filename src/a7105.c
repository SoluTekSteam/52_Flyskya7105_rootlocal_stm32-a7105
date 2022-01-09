#include "a7105.h"

void A7105Reset(void) {
    uint8_t RESETN = 0x00; ///< Write to this register by 0x00 to issue reset command, then it is auto clear

    a7105SpiRegWrite16(A7105_00_MODE, RESETN);
}

uint8_t A7105Calib(void) {
    uint8_t retVal = 0;
    uint8_t calibReg;

    /// Assume MFBS == 0 after reset
    a7105SpiRegWrite8(A7105_STROBE_PLL);

    /// Start filter bank (FBC, bit 0), VCO current (VCC, bit 2) and VCO
    /// bank (VBC, bit 1) calibrations all at the same time.
    /// Leave VTH and VTL at recommended values.
    a7105SpiRegWrite16(A7105_02_CALC, 0x07);

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

    a7105SpiRegWrite8(A7105_STROBE_STANDBY);

    return retVal;
}

void A7105Init(void) {

}

void setPower(uint8_t PAC_reg, uint8_t TBG_reg) {
    a7105SpiRegWrite16(A7105_28_TX_TEST, (PAC_reg << 3) | TBG_reg);
}