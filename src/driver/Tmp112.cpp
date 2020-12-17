/*
 * Tmp112.cpp
 *
 * Created on: Sep 16, 2020
 */

#include "Tmp112.h"

#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <string>

#include <boost/format.hpp>

#include "InfnLogger.h"
#include "RegIfException.h"

Tmp112::Tmp112(std::string name, FpgaI2cIf *pIntf)
  : mName(name),
    mpI2cIntf(pIntf),
    mOrigData(0),
    mShiftData(0),
    mSwapData(0)
{
    GetDefaultConfig();
    GetTmpLowLimit(mTmpLowLimitDegree);
    GetTmpHighLimit(mTmpHighLimitDegree);

    INFN_LOG(SeverityLevel::info) << "Created TMP112: " << name;
}

Tmp112::~Tmp112()
{
}

int Tmp112::SetConfigModes(Tmp112ConfigData& config)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    uint16 configData = GetConfigBytes(config);

    try
    {
        mpI2cIntf->Write16(cConfRegOffset, configData);

        mConfigFields = config;
    }

    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: SetConfigModes() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    return 0;
}

int Tmp112::SetTmpLowLimit(float lowLimit)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    uint16 data = GetDataFromTemperature(lowLimit, mConfigFields.extendedMode);

    try
    {
        mpI2cIntf->Write16(cTLowRegOffset, data);
    }

    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: SetTmpLowLimit() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    mTmpLowLimitDegree = lowLimit;
    mTmpLowLimitData = data;

    INFN_LOG(SeverityLevel::info) << mName
                    << " SetTmpLowLimit() to:" << mTmpLowLimitDegree << " C"
                    << "; WriteBytes = 0x" << std::setfill('0') << std::setw(4) << std::hex << mTmpLowLimitData << std::dec;

    return 0;
}

int Tmp112::SetTmpHighLimit(float highLimit)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    uint16 data = GetDataFromTemperature(highLimit, mConfigFields.extendedMode);

    try
    {
        mpI2cIntf->Write16(cTHighRegOffset, data);
    }

    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: SetTmpHighLimit() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    mTmpHighLimitDegree = highLimit;
    mTmpHighLimitData = data;

    INFN_LOG(SeverityLevel::info) << mName
                    << " SetTmpHighLimit() to:" << mTmpHighLimitDegree << "C"
                    << "; WriteBytes = 0x" << std::setfill('0') << std::setw(4) << std::hex << mTmpHighLimitData << std::dec;

    return 0;
}

int Tmp112::GetTmperature(float& tmp)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    try
    {
        uint16 data = mpI2cIntf->Read16(cTmpRegOffset);

        tmp = GetTemperatureFromData(data, mConfigFields.extendedMode);
    }

    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: GetTmperature() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    return 0;
}

int Tmp112::GetConfigModes(Tmp112ConfigData& config)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    try
    {
        GetDefaultConfig();
    }

    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: GetConfigModes() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    config = mConfigFields;

    return 0;
}

int Tmp112::GetTmpLowLimit(float& tmp)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    uint16 data = 0;
    try
    {
        data = mpI2cIntf->Read16(cTLowRegOffset);
    }

    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: GetTmpLowLimit() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    mTmpLowLimitData = data;

    mTmpLowLimitDegree = GetTemperatureFromData(data, mConfigFields.extendedMode);

    tmp = mTmpLowLimitDegree;

    INFN_LOG(SeverityLevel::info) << mName
                    << " GetTmpLowLimit() tmp:" << mTmpLowLimitDegree << "C"
                    << "; ReadBytes = 0x" << std::setfill('0') << std::setw(4) << std::hex << mTmpLowLimitData << std::dec;

    return 0;
}

int Tmp112::GetTmpHighLimit(float& tmp)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    uint16 data = 0;
    try
    {
        data = mpI2cIntf->Read16(cTHighRegOffset);
    }

    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: GetTmpHighLimit() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    mTmpHighLimitData = data;

    mTmpHighLimitDegree = GetTemperatureFromData(data, mConfigFields.extendedMode);

    tmp = mTmpHighLimitDegree;

    INFN_LOG(SeverityLevel::info) << mName
                    << " GetTmpHighLimit() tmp:" << mTmpHighLimitDegree << "C"
                    << "; ReadBytes = 0x" << std::setfill('0') << std::setw(4) << std::hex << mTmpHighLimitData << std::dec;

    return 0;
}

void Tmp112::DumpTmp(std::ostream& out)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    float tmp;
    GetTmperature(tmp);

    out << mName << "\tTemperature(C): " << tmp << std::endl << std::endl;
}

void Tmp112::DumpAll(std::ostream& out)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    out << std::endl <<  "Name: " << mName << std::endl;

    out << std::endl << "================== Configurations ================== " << std::endl;

    out << std::endl << "configure data = 0x" << std::hex << mConfigData << std::dec << std::endl << std::endl;

    out << "shutDownMode   : " << mConfigFields.shutDownMode << std::endl;
    out << "thermostatMode : " << mConfigFields.thermostatMode << std::endl;
    out << "alertPolarity  : " << mConfigFields.alertPolarity << std::endl;
    out << "faultQue0      : " << mConfigFields.faultQue0 << std::endl;
    out << "faultQue1      : " << mConfigFields.faultQue1 << std::endl;
    out << "convertResR0   : " << mConfigFields.convertResR0 << std::endl;
    out << "convertResR1   : " << mConfigFields.convertResR1 << std::endl;
    out << "oneShot        : " << mConfigFields.oneShot << std::endl;
    out << "extendedMode   : " << mConfigFields.extendedMode << std::endl;
    out << "alert          : " << mConfigFields.alert << std::endl;

    out << std::endl << "================== Current Temperatures ================== " << std::endl;

    float tmp;
    GetTmperature(tmp);

    out << "\tCurrent Temperature(C): " << tmp << std::endl;

    out << "\tTmp Low Limit(C): " << mTmpLowLimitDegree << std::endl;

    out << "\tTmp High Limit(C): " << mTmpHighLimitDegree << std::endl << std::endl;
}

void Tmp112::DumpConversion(std::ostream& out)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    out << std::endl << "================== Normal Mode: Convert Tmp to Data ================== " << std::endl;

    float testTmp[] = {150
                     , 128
                     , 127.9375
                     , 100
                     , 80
                     , 75
                     , 50
                     , 25
                     , 0.25
                     , 0
                     , -0.25
                     , -25
                     , -55 };

    std::vector<float> tmp(testTmp, testTmp + sizeof(testTmp) / sizeof(float));

    out << std::endl << "Tmp(C)\t\tOrigData\tShiftData\tSwapData" << std::endl;

    for (std::vector<float>::iterator it = tmp.begin()+1; it != tmp.end(); ++it)
    {
        GetDataFromTemperature(*it, 0);

        out << *it
            << "\t\t0x" << std::setfill('0') << std::setw(4) << std::hex << mOrigData << std::dec
            << "\t\t0x" << std::setfill('0') << std::setw(4) << std::hex << mShiftData << std::dec
            << "\t\t0x" << std::setfill('0') << std::setw(4) << std::hex << mSwapData << std::dec
            << std::dec << std::endl;
    }

    out << std::endl << "================== Extended Mode: Convert Tmp to Data ================== " << std::endl;

    out << std::endl << "Tmp(C)\t\tOrigData\tShiftData\tSwapData" << std::endl;

    for (std::vector<float>::iterator it = tmp.begin(); it != tmp.end(); ++it)
    {
        GetDataFromTemperature(*it, 1);

        out << *it
            << "\t\t0x" << std::setfill('0') << std::setw(4) << std::hex << mOrigData << std::dec
            << "\t\t0x" << std::setfill('0') << std::setw(4) << std::hex << mShiftData << std::dec
            << "\t\t0x" << std::setfill('0') << std::setw(4) << std::hex << mSwapData << std::dec
            << std::dec << std::endl;
    }

    out << std::endl << "================== Normal Mode: Convert Data to Tmp ================== " << std::endl;

    uint16 test12BitsData[] = {
                           0xf07f
                         , 0x0064
                         , 0x0050
                         , 0x004b
                         , 0x0032
                         , 0x0019
                         , 0x4000
                         , 0x0000
                         , 0xc0ff
                         , 0x00e7
                         , 0x00c9 };

    std::vector<uint16> t12BitsData(test12BitsData, test12BitsData + sizeof(test12BitsData) / sizeof(uint16));

    out << std::endl << "Data\t\tDataSwap\tDataShift\tTmp(C)" << std::endl;

    for (std::vector<uint16>::iterator it = t12BitsData.begin(); it != t12BitsData.end(); ++it)
    {
        float tmp = GetTemperatureFromData(*it, 0);

        out << "0x" << std::setfill('0') << std::setw(4) << std::hex << (*it) << std::dec
            << "\t\t0x" << std::setfill('0') << std::setw(4) << std::hex << mDataSwap << std::dec
            << "\t\t0x" << std::setfill('0') << std::setw(4) << std::hex << mDataShift << std::dec
            << "\t\t" << tmp
            << std::dec << std::endl;
    }

    out << std::endl << "================== Extended Mode: Convert Data to Tmp ================== " << std::endl;

    uint16 test13BitsData[] = {
            0x004b
          , 0x0040
          , 0xf83f
          , 0x0032
          , 0x0028
          , 0x8025
          , 0x0019
          , 0x800c
          , 0x2000
          , 0x0000
          , 0xe0ff
          , 0x80f3
          , 0x80e4 };

    std::vector<uint16> t13BitsData(test13BitsData, test13BitsData + sizeof(test13BitsData) / sizeof(uint16));

    out << std::endl << "Data\t\tDataSwap\tDataShift\tTmp(C)" << std::endl;

    for (std::vector<uint16>::iterator it = t13BitsData.begin(); it != t13BitsData.end(); ++it)
    {
        float tmp = GetTemperatureFromData(*it, 1);

        out << "0x" << std::setfill('0') << std::setw(4) << std::hex << (*it) << std::dec
            << "\t\t0x" << std::setfill('0') << std::setw(4) << std::hex << mDataSwap << std::dec
            << "\t\t0x" << std::setfill('0') << std::setw(4) << std::hex << mDataShift << std::dec
            << "\t\t" << tmp
            << std::dec << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////

int Tmp112::GetDefaultConfig()
{
    try
    {
        uint16 data = mpI2cIntf->Read16(cConfRegOffset);

        UpdateConfigCache(data);
    }

    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: GetDefaultConfig() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    return 0;
}

void Tmp112::UpdateConfigCache(uint16 data)
{
    mConfigData = data;

    // Swap bytes because TMP112 is big endian
    uint16 config = (data << 8) | (data >> 8);

    mConfigFields.shutDownMode   = (config & cShutdownModeMask)   >> cShutdownModeBitpos;
    mConfigFields.thermostatMode = (config & cThermostatModeMask) >> cThermostatModeBitpos;
    mConfigFields.alertPolarity  = (config & cPolarityMask)       >> cPolarityBitpos;
    mConfigFields.faultQue0      = (config & cFaultQueueF0Mask)   >> cFaultQueueF0Bitpos;
    mConfigFields.faultQue1      = (config & cFaultQueueF1Mask)   >> cFaultQueueF1Bitpos;
    mConfigFields.convertResR0   = (config & cConverterResR0Mask) >> cConverterResR0Bitpos;
    mConfigFields.convertResR1   = (config & cConverterResR1Mask) >> cConverterResR1Bitpos;
    mConfigFields.oneShot        = (config & cOneShotMask)        >> cOneShotBitpos;
    mConfigFields.extendedMode   = (config & cExtendedModeMask)   >> cExtendedModeBitpos;
    mConfigFields.alert          = (config & cAlertMask)          >> cAlertBitpos;
}

uint16 Tmp112::GetConfigBytes(Tmp112ConfigData& config)
{
    uint16 data = 0;

    data |= ((config.shutDownMode   & 0x1) << cShutdownModeBitpos);
    data |= ((config.thermostatMode & 0x1) << cThermostatModeBitpos);
    data |= ((config.alertPolarity  & 0x1) << cPolarityBitpos);
    data |= ((config.faultQue0      & 0x1) << cFaultQueueF0Bitpos);
    data |= ((config.faultQue1      & 0x1) << cFaultQueueF1Bitpos);
    data |= ((config.convertResR0   & 0x1) << cConverterResR0Bitpos);
    data |= ((config.convertResR1   & 0x1) << cConverterResR1Bitpos);
    data |= ((config.oneShot        & 0x1) << cOneShotBitpos);
    data |= ((config.extendedMode   & 0x1) << cExtendedModeBitpos);
    data |= ((config.alert          & 0x1) << cAlertBitpos);

    uint16 ret = (data << 8) | (data >> 8);

    return ret;
}

float Tmp112::GetTemperatureFromData(uint16 reading, uint16 extMode)
{
    uint16 data = (reading << 8) | (reading >> 8);
    mDataSwap = data;

    uint16 signBit = 0;
    uint16 dataMask = 0;
    uint32 dataShift = 0;
    float tmp = 0;

    if (extMode == 1) // Extended mode
    {
        dataMask = cTmpRegExtModeDataMask;
        dataShift  = cTmpRegExtModeDataShift;
        signBit = cTmpRegExtModeDataSignbit;
    }
    else
    {
        dataMask = cTmpRegNomModeDataMask;
        dataShift = cTmpRegNomModeDataShift;
        signBit = cTmpRegNomModeDataSignbit;
    }

    data = (data >> dataShift) & dataMask;
    mDataShift = data;

    if ((data & signBit) == 0) // Positive data
    {
        tmp = data * cTmpResolution;
    }
    else // Negative data
    {
        tmp = (((~data) & dataMask) + 1) * cTmpResolution * (-1);
    }

    return tmp;
}

uint16 Tmp112::GetDataFromTemperature(float tmp, uint16 extMode)
{
    uint16 data = 0;
    uint16 dataMask = 0;
    uint32 dataShift = 0;
    uint16 maxPosData = 0;
    uint16 maxNegData = 0;

    if (extMode == 1) // Extended mode
    {
        dataMask   = cTmpRegExtModeDataMask;
        dataShift  = cTmpRegExtModeDataShift;
        maxPosData = cExtModeMaxPositiveTmpData;
        maxNegData = cExtModeMaxNegtiveTmpData;
    }
    else
    {
        dataMask  = cTmpRegNomModeDataMask;
        dataShift = cTmpRegNomModeDataShift;
        maxPosData = cNomModeMaxPositiveTmpData;
        maxNegData = cNomModeMaxNegtiveTmpData;
    }

    if (tmp >= 0)
    {
        data = tmp / cTmpResolution;
        if (data > maxPosData)
        {
            data = maxPosData;
        }
    }
    else
    {
        data = fabs(tmp) / cTmpResolution;
        data = (((~data) & dataMask) + 1);

        if (data > maxNegData)
        {
            data = maxNegData;
        }
    }

    mOrigData = data;

    data = (data & dataMask) << dataShift;
    mShiftData = data;

    uint16 ret = (data << 8) | (data >> 8);
    mSwapData = ret;

    return ret;
}



