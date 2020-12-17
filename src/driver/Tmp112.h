/*
 * Tmp112.h
 *
 *  Created on: Sep 16, 2020
 */

#ifndef CHM6_BOARD_MS_SRC_DRIVER_TMP112_H_
#define CHM6_BOARD_MS_SRC_DRIVER_TMP112_H_

#include <mutex>

#include "types.h"
#include "FpgaI2cIf.h"

const uint32 cTmpRegOffset   = 0;
const uint32 cConfRegOffset  = 1;
const uint32 cTLowRegOffset  = 2;
const uint32 cTHighRegOffset = 3;

// Temperature Data Register
const uint32 cTmp112RegDataBits = 16;

const uint32 cTmpRegNomModeDataBits    = 12;
const uint32 cTmpRegNomModeDataShift   = 16-12;
const uint16 cTmpRegNomModeDataMask    = 0xffff >> cTmpRegNomModeDataShift;
const uint16 cTmpRegNomModeDataSignbit = 0x1 << 11;

const uint16 cNomModeMaxPositiveTmpData = 0x7ff; // device big endian data
const uint16 cNomModeMaxNegtiveTmpData  =  0xfff; // device big endian data

const uint32 cTmpRegExtModeDataBits     = 13;
const uint32 cTmpRegExtModeDataShift    = 16-13;
const uint16 cTmpRegExtModeDataMask     = 0xffff >> cTmpRegExtModeDataShift;
const uint16 cTmpRegExtModeDataSignbit = 0x1 << 12;

const uint16 cExtModeMaxPositiveTmpData = 0xfff; // device big endian data
const uint16 cExtModeMaxNegtiveTmpData  = 0x1fff; // device big endian data

const float32 cTmpResolution = 0.0625;

// Configure register

// SD: 1 - shutdown mode; 0 - continuous conversion state
const uint32 cShutdownModeBitpos = 8;
const uint16 cShutdownModeMask   = 0x1 << cShutdownModeBitpos;

// TM: 1 - Interrupt mode; 0 - Comparator mode
const uint32 cThermostatModeBitpos = 9;
const uint16 cThermostatModeMask   = 0x1 << cThermostatModeBitpos;

// POL: 0 - default, the ALERT pin becomes active low. 1 - the ALERT pin becomes active high and the state of the ALERT pin is inverted.
const uint32 cPolarityBitpos    = 10;
const uint16 cPolarityMask       = 0x1 << cPolarityBitpos;

// F1/F0: The fault queue requires consecutive fault measurements in order to trigger the alert function
const uint32 cFaultQueueF0Bitpos = 11;
const uint16 cFaultQueueF0Mask   = 0x1 << cFaultQueueF0Bitpos;
const uint32 cFaultQueueF1Bitpos = 12;
const uint16 cFaultQueueF1Mask   = 0x1 << cFaultQueueF1Bitpos;

// R1&R0: read-only. set on start up to 11 which sets the temperature register to a 12-bit resolution.
const uint32 cConverterResR0Bitpos = 13;
const uint16 cConverterResR0Mask   = 0x1 << cConverterResR0Bitpos;
const uint32 cConverterResR1Bitpos = 14;
const uint16 cConverterResR1Mask   = 0x1 << cConverterResR1Bitpos;

// OS: When the device is in shutdown mode, writing a 1 to the OS bit begins a single temperature conversion.
const uint32 cOneShotBitpos = 15;
const uint16 cOneShotMask   = 0x1 << cOneShotBitpos;

// EM: The extended mode bit configures the device for normal mode operation (EM = 0) or extended mode operation (EM = 1).
const uint32 cExtendedModeBitpos = 4;
const uint16 cExtendedModeMask   = 0x1 << cExtendedModeBitpos;

// AL: Reading the AL bit provides information about the comparator mode status. The state of the POL bit inverts the polarity of data returned from the AL bit.
const uint32 cAlertBitpos  = 5;
const uint16 cAlertMask    = 0x1 << cAlertBitpos;

// TLOW and THIGH Register
const uint32 cTmpLimitNomModeDataShift   = cTmpRegNomModeDataShift;
const uint16 cTmpLimitNomModeDataMask    = cTmpRegNomModeDataMask;
const uint16 cTmpLimitNomModeDataSignbit = cTmpRegNomModeDataSignbit;

const uint32 cTmpLimitExtModeDataShift = cTmpRegExtModeDataShift;
const uint16 cTmpLimitExtModeDataMask  = cTmpRegExtModeDataMask;
const uint16 cTmpLimitExtModeDataSignbit = cTmpRegExtModeDataSignbit;

const uint32 cDefaultTmpHighLimit = 80;
const uint32 cDefaultTmpLowLimit  = 75;

struct Tmp112ConfigData
{
    uint16 shutDownMode : 1;
    uint16 thermostatMode : 1;
    uint16 alertPolarity : 1;
    uint16 faultQue0 : 1;
    uint16 faultQue1 : 1;
    uint16 convertResR0 : 1; // Read only
    uint16 convertResR1 : 1; // Read only
    uint16 oneShot : 1;
    uint16 extendedMode : 1;
    uint16 alert : 1;
};

class Tmp112
{
public:
    Tmp112(std::string name, FpgaI2cIf *pIntf);

    ~Tmp112();

    int SetConfigModes(Tmp112ConfigData& config);

    int SetTmpLowLimit(float lowLimit);

    int SetTmpHighLimit(float highLimit);

    int GetTmperature(float& tmp);

    int GetConfigModes(Tmp112ConfigData& config);

    int GetTmpLowLimit(float& tmp);

    int GetTmpHighLimit(float& tmp);

    void DumpTmp(std::ostream& out);

    void DumpAll(std::ostream& out);

    void DumpConversion(std::ostream& out);

private:

    int GetDefaultConfig();

    void UpdateConfigCache(uint16 config);

    uint16 GetConfigBytes(Tmp112ConfigData& config);

    float GetTemperatureFromData(uint16 reading, uint16 extMode);

    uint16 GetDataFromTemperature(float tmp, uint16 extMode);

    std::string mName;

    FpgaI2cIf* mpI2cIntf;

    uint16 mConfigData; // little endian
    Tmp112ConfigData mConfigFields;

    uint16 mTmpLowLimitData;
    float  mTmpLowLimitDegree;

    uint16 mTmpHighLimitData;
    float  mTmpHighLimitDegree;

    // Debug: Convert Tmp to Data
    uint16 mOrigData;
    uint16 mShiftData;
    uint16 mSwapData;

    //Debug: Convert data to Tmp
    uint16 mDataSwap;
    uint16 mDataShift;

    std::recursive_mutex  mLock;
};

#endif /* CHM6_BOARD_MS_SRC_DRIVER_TMP112_H_ */
