//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#include "board_defs.h"

namespace boardMs
{

const char* upgradableDeviceTypeToStr[] =
{
    "PL_FPGA",
    "Uboot",
    "Gecko"
};

upgradableDevice::upgradableDevice(upgradableDeviceType type, std::string hwVer, std::string swVer)
    : mDeviceType(type)
    , mDeviceName(upgradableDeviceTypeToStr[type])
    , mHwVersion(hwVer)
    , mSwVersion(swVer) {}

void upgradableDevice::Dump(std::ostream &os)
{
    os << boost::format("%-3s : %-10s : %-9s : %-8s") % mDeviceType % mDeviceName % mHwVersion % mSwVersion << std::endl;
}

/*
 * Board PM
 */
const char* BoardPmIdToStr[] =
{
        "temperature",
};

Chm6BoardPm::Chm6BoardPm(BoardPmId id, float32 value)
    : mId(id)
    , mName(BoardPmIdToStr[id])
    , mValue(value) {}

std::string Chm6BoardPm::BoardPmIdToName(BoardPmId id)
{
    if (id < MAX_PM_ID_NUM)
    {
        return (std::string(BoardPmIdToStr[id]));
    }
    else
    {
        return std::string("");
    }
}

std::string Chm6BoardPm::BoardPmIdToDescrption(BoardPmId id)
{
    switch(id)
    {
        case OUTLET_TEMPERATURE:
            return std::string("Outlet Temperature");
        default:
            return std::string("Not specified");
    }
}

void Chm6BoardPm::Dump(std::ostream &os)
{
    os << boost::format("%-3d : %-20s : %f") % (int)mId % mName % mValue << std::endl;
}

}
