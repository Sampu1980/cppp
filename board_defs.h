/*
 * board_defs.h
 *
 *  Created on: Jan 21, 2020
 */

#ifndef CHM6_BOARD_MS_SRC_BOARDDEFS_H_
#define CHM6_BOARD_MS_SRC_BOARDDEFS_H_

#include <string>
#include "types.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/format.hpp>
#include "FpgaRegDefs.h"

namespace boardMs
{
enum ExitCode
{
    EXIT_NORMAIL = 0,
    EXIT_ERROR = 1,
    EXIT_RESTART_WARM = 64,
    EXIT_RESTART_COLD = 65,
    EXIT_GRACEFUL_SHUTDOWN = 66,
    EXIT_INVALID = 255
};

// Host board actions
typedef enum hostBoardActionType
{
    BOARD_ACTION_UNKNOWN = 0,
    POWER_UP,
    UPDATE_FW,
    INIT_HW,
    RESTART_COLD,
    RESTART_WARM,
    GRACEFUL_SHUTDOWN,
    MAX_BOARD_ACTION_NUM
}hostBoardActionType;

static std::string HostBoardActionTypeToStr(hostBoardActionType act)
{
    switch(act)
    {
        case POWER_UP:
            return std::string("Power up");
        case UPDATE_FW:
            return std::string("Update FW");
        case INIT_HW:
            return std::string("Init HW");
        case RESTART_COLD:
            return std::string("Restart cold");
        case RESTART_WARM:
            return std::string("Restart warm");
        case BOARD_ACTION_UNKNOWN:
        default:
            return std::string("Board action unspecified");
    }
}

// Board state data structures
typedef enum upgradableDeviceType
{
    PL_FPGA = 0,
    UBOOT,
    GECKO,
    MAX_UPGDDEVICE_TYPE_NUM
}upgradableDeviceType;

struct upgradableDevice
{
    upgradableDevice() {}
    upgradableDevice(std::string name, std::string hwVer)
        : mDeviceName(name)
        , mHwVersion(hwVer) {}
    upgradableDevice(upgradableDeviceType type, std::string hwVer, std::string swVer);

    ~upgradableDevice() {}

    void SetDeviceName(std::string& name)
    {
        mDeviceName = name;
    }

    void SetHwVersion(std::string& hwVer)
    {
        mHwVersion = hwVer;
    }

    void SetSwVersion(std::string& swVer)
    {
        mSwVersion = swVer;
    }

    void SetFwUpdateState(std::string& fwUpgradeState)
    {
        mFwUpdateState = fwUpgradeState;
    }

    void SetFileLocation(std::string& fileLocation)
    {
        mFileLocation = fileLocation;
    }

    void Dump(std::ostream &os);

    upgradableDeviceType mDeviceType;
    std::string          mDeviceName;
    std::string          mHwVersion;
    std::string          mSwVersion;
    std::string          mFwUpdateState;
    std::string          mFileLocation;
};

typedef enum Chm6EqptState{
    EQPT_STATE_UNKNOWN = 0,
    EQPT_STATE_BOOTING = 1,
    EQPT_STATE_BOOTING_DONE = 2,
    EQPT_STATE_BOOTING_DONE_WARM = 3,
    EQPT_STATE_BOOTING_FAIL = 4,
    EQPT_STATE_FW_UPDATING = 5,
    EQPT_STATE_FW_UPDATE_DONE = 6,
    EQPT_STATE_FW_UPDATE_FAIL = 7,
    EQPT_STATE_FW_UPDATE_COMMIT = 8,
    EQPT_STATE_HW_INITING = 9,
    EQPT_STATE_HW_INITING_DONE = 10,
    EQPT_STATE_HW_INITING_FAIL = 11,
    EQPT_STATE_RESTARTING = 12,
    EQPT_STATE_MAX
}Chm6EqptState;

static std::string Chm6EqptStateToStr(Chm6EqptState st)
{
    switch (st)
    {
    case EQPT_STATE_BOOTING:
        return std::string("EQPT_STATE_BOOTING");

    case EQPT_STATE_BOOTING_DONE:
        return std::string("EQPT_STATE_BOOTING_DONE");

    case EQPT_STATE_BOOTING_DONE_WARM:
        return std::string("EQPT_STATE_BOOTING_DONE_WARM");

    case EQPT_STATE_BOOTING_FAIL:
        return std::string("EQPT_STATE_BOOTING_FAIL");

    case EQPT_STATE_FW_UPDATING:
        return std::string("EQPT_STATE_FW_UPDATING");

    case EQPT_STATE_FW_UPDATE_DONE:
        return std::string("EQPT_STATE_FW_UPDATE_DONE");

    case EQPT_STATE_FW_UPDATE_FAIL:
        return std::string("EQPT_STATE_FW_UPDATE_FAIL");

    case EQPT_STATE_FW_UPDATE_COMMIT:
        return std::string("EQPT_STATE_FW_UPDATE_COMMIT");

    case EQPT_STATE_HW_INITING:
        return std::string("EQPT_STATE_HW_INITING");

    case EQPT_STATE_HW_INITING_DONE:
        return std::string("EQPT_STATE_HW_INITING_DONE");

    case EQPT_STATE_HW_INITING_FAIL:
        return std::string("EQPT_STATE_HW_INITING_FAIL");

    case EQPT_STATE_RESTARTING:
        return std::string("EQPT_STATE_RESTARTING");

    case EQPT_STATE_UNKNOWN:
    default:
        return std::string("EQPT_STATE_UNKNOWN");
    }
}

struct Chm6EqptInventory
{
    Chm6EqptInventory() {}
    ~Chm6EqptInventory() {}

    std::string SerialNumber;
    std::string PartNumber;
    std::string VendorName;
    std::string HardwareRevision;
    std::string ManufactureDate;
    std::string FirmwareVersion;
    std::string CleiCode;
    std::string Pon;
    std::string InsertionDate;

    void Dump(std::ostream &os)
    {
        os << "\tSerialNumber     : " << SerialNumber << std::endl
           << "\tPartNumber       : " << PartNumber << std::endl
           << "\tVendorName       : " << VendorName << std::endl
           << "\tHardwareRevision : " << HardwareRevision << std::endl
           << "\tManufactureDate  : " << ManufactureDate << std::endl
           << "\tFirmwareVersion  : " << FirmwareVersion << std::endl
           << "\tCleiCode         : " << CleiCode << std::endl
           << "\tPon              : " << Pon << std::endl
           << "\tInsertionDate    : " << InsertionDate << std::endl << std::endl;
    }
};

// Main board pm
typedef enum BoardPmId
{
    OUTLET_TEMPERATURE = 0,
    MAX_PM_ID_NUM
}BoardPmId;

struct Chm6BoardPm
{
    Chm6BoardPm() {}
    ~Chm6BoardPm(){}

    Chm6BoardPm(BoardPmId id, float32 value);

    Chm6BoardPm(std::string name, float32 value)
     : mName(name), mValue(value)
    {}

    Chm6BoardPm(const char* name, float32 value)
     : mName(name), mValue(value)
    {}

    void SetName(std::string& name)
    {
        mName = name;
    }

    void SetName(const char* name)
    {
        mName = std::string(name);
    }

    void SetValue(float32 value)
    {
        mValue = value;
    }

    static std::string BoardPmIdToName(BoardPmId id);

    static std::string BoardPmIdToDescrption(BoardPmId id);

    void Dump(std::ostream &os);

    BoardPmId   mId;
    std::string mName;
    float32     mValue;
};

typedef boost::ptr_vector<boardMs::upgradableDevice>  upgradable_device_ptr_vec;
typedef upgradable_device_ptr_vec::iterator   upgradable_device_vec_itr;

typedef boost::ptr_vector<boardMs::Chm6BoardPm>  board_pm_ptr_vec;
typedef board_pm_ptr_vec::iterator board_pm_vec_itr;

typedef enum LedStateType {
    LED_STATE_UNKNOWN = 0,
    OFF,
    YELLOW,
    FLASHING_YELLOW,
    GREEN,
    FLASHING_GREEN,
    RED,
    FLASHING_RED,
    CYCLING,            //LED is cycling between all supported colors,  No Off State - lamp test
    CYCLING_WITH_OFF,   //LED is cycling between all supported colors and OFF State - location test
    MAX_LED_STATE_NUM
}LedStateType;

static std::string LedStateTypeToStr(LedStateType st)
{
    switch(st)
    {
        case OFF:
            return std::string("OFF");
        case YELLOW:
            return std::string("YELLOW");
        case FLASHING_YELLOW:
            return std::string("FLASHING_YELLOW");
        case GREEN:
            return std::string("GREEN");
        case FLASHING_GREEN:
            return std::string("FLASHING_GREEN");
        case RED:
            return std::string("RED");
        case FLASHING_RED:
            return std::string("FLASHING_RED");
        case CYCLING:
            return std::string("CYCLING");
        case CYCLING_WITH_OFF:
            return std::string("CYCLING_WITH_OFF");
        case LED_STATE_UNKNOWN:
        default:
            return std::string("LED_STATE_UNKNOWN");
    }
}

/*
 * 4.16.25 LED LOCATION Test
 */

const uint32 cFpgaMasterCntlReg = 0xc0d0;
const uint32 cFpgaCntlReg_LOCATION_TEST_EN_mask = (0x1 << 11);

/*
 * 4.16.18 Register: 0xa0 RX_KAH_STATUS
 * 24 RO LED_TEST 0x0 - Indicate lamp test.  1 – Lamp test is running; 0 – No lamp test
 * 30 RO LOC_TEST 0x0 -  indicate location test. 1 – location test is running (above bit 24 must be 0); 0 – No location test
 */
const uint32 cFpgaRxKahStatusReg = 0xc0a0;
const uint32 cFpgaRaxKahStatus_LED_LAMP_TEST_mask = (0x1 << 24);
const uint32 cFpgaRaxKahStatus_LED_LOC_TEST_mask  = (0x1 << 30);

/*
 * 4.2.3 Register: 0x08 IMBUS_REG
 * const uint32 cFpgaImbusReg     = 0x08;
 *
 * 0 RW IMBUS_LED_EN 0x0
 * IMBUS_LED_EN - IMBUS LED test enable, active HIGH
 * If HIGH, use IMBUS_LED_SYNC to blink dual color LEDs and turn ON single color LEDs
 * If LOW, SW controls LEDs
 *
 * 1 RW IMBUS_LED_SYNC 0x0
 */
const uint32 cFpgaImbusReg_IMBUS_LED_EN_mask   = 0x1;
const uint32 cFpgaImbusReg_IMBUS_LED_SYNC_mask = (0x1 << 1);

/*
 * 4.2.19 Register: 0x60[2,0x10] LED_CNTL
 * const uint32 cFpgaLedCntlReg_1 = 0x60;
 * const uint32 cFpgaLedCntlReg_2 = 0x70;
 *
 * 4 RWT FPC402_DEVICE_ADDRESS_STATUS 0x0
 *
 * SW will set FPC402_DEVICE_ADDRESS_STATUS to HIGH by the end of FPC402 addresses assignment.
 * FPGA will not control any LED if FPC402_DEVICE_ADDRESS_STATUS is LOW.
 */
/*
 * 4.2.22 Register: 0x6C[2,0x10] LED_REG3
 * const uint32 cFpgaLedReg3_1    = 0x6c; // Bottom Mezzanine card
 * const uint32 cFpgaLedReg3_2    = 0x7c; // Top Mezzanine card
 *
 * 10:8 RW FRU_ACTIVE 0x0
 * Y/G ï¿½ Active LED
 */
const uint32 cFpgaLedReg3_FRU_ACTIVE_LED_Addr = cFpgaLedReg3_1;

const uint32 cFpgaLedReg3_FRU_ACTIVE_bitpos = 8;
const uint32 cFpgaLedReg3_FRU_ACTIVE_mask   = (0b111 << cFpgaLedReg3_FRU_ACTIVE_bitpos);

/*
 * For each FRU ACTIVE dual color LED:
 * 100 - LED solid green
 * 101 - LED flashing green
 * 110 - LED solid yellow
 * 111 - LED flashing yellow
 * Other - LED Off
 */

const uint32 cFpgaLedReg_FRU_ACTIVE_SOLID_GREEN_bits     = 0b100;
const uint32 cFpgaLedReg_FRU_ACTIVE_FLASHING_GREEN_bits  = 0b101;
const uint32 cFpgaLedReg_FRU_ACTIVE_SOLID_YELLOW_bits    = 0b110;
const uint32 cFpgaLedReg_FRU_ACTIVE_FLASHING_YELLOW_bits = 0b111;
const uint32 cFpgaLedReg_FRU_ACTIVE_OFF_bits             = 0b000;

/*
 * 4.2.23 Register: 0x80 LED_REG4
 * const uint32 cFpgaLedReg4      = 0x80;
 *
 * 2:0 RW FRU_FAULT 0x4
 */
const uint32 cFpgaLedReg3_FRU_FAULT_LED_Addr = cFpgaLedReg4;

const uint32 cFpgaLedReg3_FRU_FAULT_bitpos = 0;
const uint32 cFpgaLedReg3_FRU_FAULT_mask   = (0b111 << cFpgaLedReg3_FRU_FAULT_bitpos);

/* For each FRU FAULT single color LED (LOS):
 * 100 - LED solid red
 * 101 - LED flashing red
 * Other - LED Off
 */
const uint32 cFpgaLedReg_FRU_FAULT_SOLID_RED_bits    = 0b100;
const uint32 cFpgaLedReg_FRU_FAULT_FLASHING_RED_bits = 0b101;
const uint32 cFpgaLedReg_FRU_FAULT_OFF_bits          = 0b000;

/* QSFP LEDs
 * 2 LEDS (Y/G, RED) for each QSFP-28/QSFP-DD (Active is bi-color Y/G, LOS is red).
 */
typedef enum QSFPPortId
{
    QSFP_1 = 0,
    QSFP_2,
    QSFP_3,
    QSFP_4,
    QSFP_5,
    QSFP_6,
    QSFP_7,
    QSFP_8,
    QSFP_9,
    QSFP_10,
    QSFP_11,
    QSFP_12,
    QSFP_13,
    QSFP_14,
    QSFP_15,
    QSFP_16,
    MAX_QSFP_NUM,
    QSFP_PORT_ID_INVALID
} QSFPPortId;

static std::string QSFPPortIdToStr(QSFPPortId id)
{
    switch(id)
    {
        case QSFP_1:
            return std::string("QSFP_1");
        case QSFP_2:
            return std::string("QSFP_2");
        case QSFP_3:
            return std::string("QSFP_3");
        case QSFP_4:
            return std::string("QSFP_4");
        case QSFP_5:
            return std::string("QSFP_5");
        case QSFP_6:
            return std::string("QSFP_6");
        case QSFP_7:
            return std::string("QSFP_7");
        case QSFP_8:
            return std::string("QSFP_8");
        case QSFP_9:
            return std::string("QSFP_9");
        case QSFP_10:
            return std::string("QSFP_10");
        case QSFP_11:
            return std::string("QSFP_11");
        case QSFP_12:
            return std::string("QSFP_12");
        case QSFP_13:
            return std::string("QSFP_13");
        case QSFP_14:
            return std::string("QSFP_14");
        case QSFP_15:
            return std::string("QSFP_15");
        case QSFP_16:
            return std::string("QSFP_16");
        default:
            return std::string("Invalid QSFP port number");
    }
}

typedef enum QSFPLedType
{
    QSFP_ACTIVE,
    QSFP_LOS,
    MAX_QSFP_LED_TYPE,
} QSFPLedType;

static std::string QSFPLedTypeToStr(QSFPLedType type)
{
    switch(type)
    {
        case QSFP_ACTIVE:
            return std::string("QSFP_ACTIVE");
        case QSFP_LOS:
            return std::string("QSFP_LOS");
        default:
            return std::string("Invalid QSFP LED type number");
    }
}

/* QSFP LEDs
 *
 * 4.2.20 Register: 0x64[2,0x10] LED_REG1
 * const uint32 cFpgaLedReg1_1    = 0x64; // Bottom Mezzanine card
 * const uint32 cFpgaLedReg1_2    = 0x74; // Top Mezzanine card
 *
 * 4.2.21 Register: 0x68[2,0x10] LED_REG2
 * const uint32 cFpgaLedReg2_1    = 0x68; // Bottom Mezzanine card
 * const uint32 cFpgaLedReg2_2    = 0x78; // Top Mezzanine card
 */

const uint32 cFpgaLedReg_QSFP_LED_Addr[MAX_QSFP_LED_TYPE][MAX_QSFP_NUM] =
{
        cFpgaLedReg1_2, cFpgaLedReg1_2, cFpgaLedReg1_2, cFpgaLedReg1_2,
        cFpgaLedReg2_2, cFpgaLedReg2_2, cFpgaLedReg2_2, cFpgaLedReg2_2,
        cFpgaLedReg1_1, cFpgaLedReg1_1, cFpgaLedReg1_1, cFpgaLedReg1_1,
        cFpgaLedReg2_1, cFpgaLedReg2_1, cFpgaLedReg2_1, cFpgaLedReg2_1,

        cFpgaLedReg1_2, cFpgaLedReg1_2, cFpgaLedReg1_2, cFpgaLedReg1_2,
        cFpgaLedReg2_2, cFpgaLedReg2_2, cFpgaLedReg2_2, cFpgaLedReg2_2,
        cFpgaLedReg1_1, cFpgaLedReg1_1, cFpgaLedReg1_1, cFpgaLedReg1_1,
        cFpgaLedReg2_1, cFpgaLedReg2_1, cFpgaLedReg2_1, cFpgaLedReg2_1,
};

/* QSFP LEDs
 * 2 LEDS (Y/G, RED) for each QSFP-28/QSFP-DD (Active is bi-color Y/G, LOS is red).
 */

const uint32 cFpgaLedReg_QSFP1_ACTIVE_bitpos = 0;
const uint32 cFpgaLedReg_QSFP1_ACTIVE_mask   = (0b111 << cFpgaLedReg_QSFP1_ACTIVE_bitpos);

const uint32 cFpgaLedReg_QSFP1_LOS_bitpos    = 4;
const uint32 cFpgaLedReg_QSFP1_LOS_mask      = (0b111 << cFpgaLedReg_QSFP1_LOS_bitpos);

const uint32 cFpgaLedReg_QSFP2_ACTIVE_bitpos = 8;
const uint32 cFpgaLedReg_QSFP2_ACTIVE_mask   = (0b111 << cFpgaLedReg_QSFP2_ACTIVE_bitpos);

const uint32 cFpgaLedReg_QSFP2_LOS_bitpos    = 12;
const uint32 cFpgaLedReg_QSFP2_LOS_mask      = (0b111 << cFpgaLedReg_QSFP2_LOS_bitpos);

const uint32 cFpgaLedReg_QSFP3_ACTIVE_bitpos = 16;
const uint32 cFpgaLedReg_QSFP3_ACTIVE_mask   = (0b111 << cFpgaLedReg_QSFP3_ACTIVE_bitpos);

const uint32 cFpgaLedReg_QSFP3_LOS_bitpos    = 20;
const uint32 cFpgaLedReg_QSFP3_LOS_mask      = (0b111 << cFpgaLedReg_QSFP3_LOS_bitpos);

const uint32 cFpgaLedReg_QSFP4_ACTIVE_bitpos = 24;
const uint32 cFpgaLedReg_QSFP4_ACTIVE_mask   = (0b111 << cFpgaLedReg_QSFP4_ACTIVE_bitpos);

const uint32 cFpgaLedReg_QSFP4_LOS_bitpos    = 28;
const uint32 cFpgaLedReg_QSFP4_LOS_mask      = (0b111 << cFpgaLedReg_QSFP4_LOS_bitpos);

const uint32 cFpgaLedReg_QSFP5_ACTIVE_bitpos = 0;
const uint32 cFpgaLedReg_QSFP5_ACTIVE_mask   = (0b111 << cFpgaLedReg_QSFP5_ACTIVE_bitpos);

const uint32 cFpgaLedReg_QSFP5_LOS_bitpos    = 4;
const uint32 cFpgaLedReg_QSFP5_LOS_mask      = (0b111 << cFpgaLedReg_QSFP5_LOS_bitpos);

const uint32 cFpgaLedReg_QSFP6_ACTIVE_bitpos = 8;
const uint32 cFpgaLedReg_QSFP6_ACTIVE_mask   = (0b111 << cFpgaLedReg_QSFP6_ACTIVE_bitpos);

const uint32 cFpgaLedReg_QSFP6_LOS_bitpos    = 12;
const uint32 cFpgaLedReg_QSFP6_LOS_mask      = (0b111 << cFpgaLedReg_QSFP6_LOS_bitpos);

const uint32 cFpgaLedReg_QSFP7_ACTIVE_bitpos = 16;
const uint32 cFpgaLedReg_QSFP7_ACTIVE_mask   = (0b111 << cFpgaLedReg_QSFP7_ACTIVE_bitpos);

const uint32 cFpgaLedReg_QSFP7_LOS_bitpos    = 20;
const uint32 cFpgaLedReg_QSFP7_LOS_mask      = (0b111 << cFpgaLedReg_QSFP7_LOS_bitpos);

const uint32 cFpgaLedReg_QSFP8_ACTIVE_bitpos = 24;
const uint32 cFpgaLedReg_QSFP8_ACTIVE_mask   = (0b111 << cFpgaLedReg_QSFP8_ACTIVE_bitpos);

const uint32 cFpgaLedReg_QSFP8_LOS_bitpos    = 28;
const uint32 cFpgaLedReg_QSFP8_LOS_mask      = (0b111 << cFpgaLedReg_QSFP8_LOS_bitpos);

const uint32 cFpgaLedReg_QSFP_mask[MAX_QSFP_LED_TYPE][MAX_QSFP_NUM] =
{
        cFpgaLedReg_QSFP1_ACTIVE_mask,
        cFpgaLedReg_QSFP2_ACTIVE_mask,
        cFpgaLedReg_QSFP3_ACTIVE_mask,
        cFpgaLedReg_QSFP4_ACTIVE_mask,
        cFpgaLedReg_QSFP5_ACTIVE_mask,
        cFpgaLedReg_QSFP6_ACTIVE_mask,
        cFpgaLedReg_QSFP7_ACTIVE_mask,
        cFpgaLedReg_QSFP8_ACTIVE_mask,

        cFpgaLedReg_QSFP1_ACTIVE_mask,
        cFpgaLedReg_QSFP2_ACTIVE_mask,
        cFpgaLedReg_QSFP3_ACTIVE_mask,
        cFpgaLedReg_QSFP4_ACTIVE_mask,
        cFpgaLedReg_QSFP5_ACTIVE_mask,
        cFpgaLedReg_QSFP6_ACTIVE_mask,
        cFpgaLedReg_QSFP7_ACTIVE_mask,
        cFpgaLedReg_QSFP8_ACTIVE_mask,

        cFpgaLedReg_QSFP1_LOS_mask,
        cFpgaLedReg_QSFP2_LOS_mask,
        cFpgaLedReg_QSFP3_LOS_mask,
        cFpgaLedReg_QSFP4_LOS_mask,
        cFpgaLedReg_QSFP5_LOS_mask,
        cFpgaLedReg_QSFP6_LOS_mask,
        cFpgaLedReg_QSFP7_LOS_mask,
        cFpgaLedReg_QSFP8_LOS_mask,

        cFpgaLedReg_QSFP1_LOS_mask,
        cFpgaLedReg_QSFP2_LOS_mask,
        cFpgaLedReg_QSFP3_LOS_mask,
        cFpgaLedReg_QSFP4_LOS_mask,
        cFpgaLedReg_QSFP5_LOS_mask,
        cFpgaLedReg_QSFP6_LOS_mask,
        cFpgaLedReg_QSFP7_LOS_mask,
        cFpgaLedReg_QSFP8_LOS_mask
};

const uint32 cFpgaLedReg_QSFP_bitpos[MAX_QSFP_LED_TYPE][MAX_QSFP_NUM] =
{
        cFpgaLedReg_QSFP1_ACTIVE_bitpos,
        cFpgaLedReg_QSFP2_ACTIVE_bitpos,
        cFpgaLedReg_QSFP3_ACTIVE_bitpos,
        cFpgaLedReg_QSFP4_ACTIVE_bitpos,
        cFpgaLedReg_QSFP5_ACTIVE_bitpos,
        cFpgaLedReg_QSFP6_ACTIVE_bitpos,
        cFpgaLedReg_QSFP7_ACTIVE_bitpos,
        cFpgaLedReg_QSFP8_ACTIVE_bitpos,

        cFpgaLedReg_QSFP1_ACTIVE_bitpos,
        cFpgaLedReg_QSFP2_ACTIVE_bitpos,
        cFpgaLedReg_QSFP3_ACTIVE_bitpos,
        cFpgaLedReg_QSFP4_ACTIVE_bitpos,
        cFpgaLedReg_QSFP5_ACTIVE_bitpos,
        cFpgaLedReg_QSFP6_ACTIVE_bitpos,
        cFpgaLedReg_QSFP7_ACTIVE_bitpos,
        cFpgaLedReg_QSFP8_ACTIVE_bitpos,

        cFpgaLedReg_QSFP1_LOS_bitpos,
        cFpgaLedReg_QSFP2_LOS_bitpos,
        cFpgaLedReg_QSFP3_LOS_bitpos,
        cFpgaLedReg_QSFP4_LOS_bitpos,
        cFpgaLedReg_QSFP5_LOS_bitpos,
        cFpgaLedReg_QSFP6_LOS_bitpos,
        cFpgaLedReg_QSFP7_LOS_bitpos,
        cFpgaLedReg_QSFP8_LOS_bitpos,

        cFpgaLedReg_QSFP1_LOS_bitpos,
        cFpgaLedReg_QSFP2_LOS_bitpos,
        cFpgaLedReg_QSFP3_LOS_bitpos,
        cFpgaLedReg_QSFP4_LOS_bitpos,
        cFpgaLedReg_QSFP5_LOS_bitpos,
        cFpgaLedReg_QSFP6_LOS_bitpos,
        cFpgaLedReg_QSFP7_LOS_bitpos,
        cFpgaLedReg_QSFP8_LOS_bitpos
};

/* For each QSFP dual color LED (ACTIVE):
 * 100 - LED solid green
 * 101 - LED flashing green
 * 110 - LED solid yellow
 * 111 - LED flashing yellow
 * Other - LED Off
 */
const uint32 cFpgaLedReg_QSFP_ACTIVE_SOLID_GREEN_bits     = 0b100;
const uint32 cFpgaLedReg_QSFP_ACTIVE_FLASHING_GREEN_bits  = 0b101;
const uint32 cFpgaLedReg_QSFP_ACTIVE_SOLID_YELLOW_bits    = 0b110;
const uint32 cFpgaLedReg_QSFP_ACTIVE_FLASHING_YELLOW_bits = 0b111;
const uint32 cFpgaLedReg_QSFP_ACTIVE_OFF_bits             = 0b000;

/* For each QSFP single color LED (LOS):
 * 100 - LED solid red
 * 101 - LED flashing red
 * Other - LED Off
 */
const uint32 cFpgaLedReg_QSFP_LOS_SOLID_RED_bits     = 0b100;
const uint32 cFpgaLedReg_QSFP_LOS_FLASHING_RED_bits  = 0b101;
const uint32 cFpgaLedReg_QSFP_LOS_OFF_bits           = 0b000;

typedef enum LineLedType
{
    LINE_ACTIVE,
    LINE_LOS,
    MAX_LINE_LED_TYPE,
} LineLedType;

static std::string LINELedTypeToStr(LineLedType type)
{
    switch(type)
    {
        case LINE_ACTIVE:
            return std::string("LINE_ACTIVE");
        case LINE_LOS:
            return std::string("LINE_LOS");
        default:
            return std::string("Invalid LINE LED type number");
    }
}

typedef enum LineId
{
    LINE_1 = 0,
    LINE_2,
    MAX_LINE_NUM,
    LINE_ID_INVALID
} LineId;

static std::string LineIdToStr(LineId id)
{
    switch(id)
    {
        case LINE_1:
            return std::string("LINE_1");
        case LINE_2:
            return std::string("LINE_2");
        default:
            return std::string("Invalid LINE number");
    }
}

/* LINE LEDs
 * 4 Line LEDs (2 LEDs per Line): RED ï¿½ LOS, Y/G ï¿½ Active LED
 *
 * 4.2.22 Register: 0x6C[2,0x10] LED_REG3
 * const uint32 cFpgaLedReg3_1    = 0x6c; // Bottom Mezzanine card
 * const uint32 cFpgaLedReg3_2    = 0x7c; // Top Mezzanine card
 *
 * 4.2.23 Register: 0x80 LED_REG4
 * const uint32 cFpgaLedReg4      = 0x80;
 */
/*
 * MZ Net / FPGA Control    Top MZ Function                     Bot MZ Function
 * LOS_FAULT_LED (Red)      Line1 LOS         ( 0x80 - 6:4 )    FRU Fault   ( 0x80 - 2:0)
 * LINE_LOS_LED (Red)       None (covered by faceplate)         Line2 LOS   ( 0x6c - 6:4 )
 * LINE_ACTIVE_LED (G/Y)    None (covered by faceplate)         Line2 Active( 0x6c - 2:0 )
 * FRU_ACTIVE_LED (G/Y)     Line1 Active      ( 0x7c - 10:8 )   FRU Active  ( 0x6c - 10:8 )
 */

const uint32 cFpgaLedReg_LINE1_ACTIVE_offset = cFpgaLedReg3_2;
const uint32 cFpgaLedReg_LINE1_ACTIVE_bitpos = 8;
const uint32 cFpgaLedReg_LINE1_ACTIVE_mask   = (0b111 << cFpgaLedReg_LINE1_ACTIVE_bitpos);

const uint32 cFpgaLedReg_LINE1_LOS_offset    = cFpgaLedReg4;
const uint32 cFpgaLedReg_LINE1_LOS_bitpos    = 4;
const uint32 cFpgaLedReg_LINE1_LOS_mask      = (0b111 << cFpgaLedReg_LINE1_LOS_bitpos);

const uint32 cFpgaLedReg_LINE2_ACTIVE_offset = cFpgaLedReg3_1;
const uint32 cFpgaLedReg_LINE2_ACTIVE_bitpos = 0;
const uint32 cFpgaLedReg_LINE2_ACTIVE_mask   = (0b111 << cFpgaLedReg_LINE2_ACTIVE_bitpos);

const uint32 cFpgaLedReg_LINE2_LOS_offset    = cFpgaLedReg3_1;
const uint32 cFpgaLedReg_LINE2_LOS_bitpos    = 4;
const uint32 cFpgaLedReg_LINE2_LOS_mask      = (0b111 << cFpgaLedReg_LINE2_LOS_bitpos);

const uint32 cFpgaLedReg_LINE_LED_Addr[MAX_LINE_LED_TYPE][MAX_LINE_NUM] =
{
        cFpgaLedReg_LINE1_ACTIVE_offset, cFpgaLedReg_LINE2_ACTIVE_offset,
        cFpgaLedReg_LINE1_LOS_offset,    cFpgaLedReg_LINE2_LOS_offset
};

const uint32 cFpgaLedReg_LINE_mask[MAX_LINE_LED_TYPE][MAX_LINE_NUM] =
{
        cFpgaLedReg_LINE1_ACTIVE_mask, cFpgaLedReg_LINE2_ACTIVE_mask,
        cFpgaLedReg_LINE1_LOS_mask,    cFpgaLedReg_LINE2_LOS_mask
};

const uint32 cFpgaLedReg_LINE_bitpos[MAX_LINE_LED_TYPE][MAX_LINE_NUM] =
{
        cFpgaLedReg_LINE1_ACTIVE_bitpos, cFpgaLedReg_LINE2_ACTIVE_bitpos,
        cFpgaLedReg_LINE1_LOS_bitpos,    cFpgaLedReg_LINE2_LOS_bitpos
};

/* For each LINE dual color LED (ACTIVE):
 * 100 - LED solid green
 * 101 - LED flashing green
 * 110 - LED solid yellow
 * 111 - LED flashing yellow
 * Other - LED Off
 */
const uint32 cFpgaLedReg_LINE_ACTIVE_SOLID_GREEN_bits     = 0b100;
const uint32 cFpgaLedReg_LINE_ACTIVE_FLASHING_GREEN_bits  = 0b101;
const uint32 cFpgaLedReg_LINE_ACTIVE_SOLID_YELLOW_bits    = 0b110;
const uint32 cFpgaLedReg_LINE_ACTIVE_FLASHING_YELLOW_bits = 0b111;
const uint32 cFpgaLedReg_LINE_ACTIVE_OFF_bits             = 0b000;

/* For each LINE single color LED (LOS):
 * 100 - LED solid red
 * 101 - LED flashing red
 * Other - LED Off
 */
const uint32 cFpgaLedReg_LINE_LOS_SOLID_RED_bits     = 0b100;
const uint32 cFpgaLedReg_LINE_LOS_FLASHING_RED_bits  = 0b101;
const uint32 cFpgaLedReg_LINE_LOS_OFF_bits           = 0b000;

const std::string cEnvStrVerFgpa  = "chm6FpgaVersion";
const std::string cEnvStrVerUboot = "chm6BootBinVersion";
const std::string cEnvStrVerGecko = "chm6GeckoVersion";

// Port LED states
struct Chm6PortLedStates
{
    Chm6PortLedStates(QSFPPortId id, LedStateType activeLedState, LedStateType losLedState)
    : mPortId(id)
    , mActiveLedState(activeLedState)
    , mLosLedState(losLedState)
    {}

    ~Chm6PortLedStates() {}

    void Dump(std::ostream &os)
    {
        os << boost::format("%-5d : %-16s : %-15s") % QSFPPortIdToStr(mPortId) % LedStateTypeToStr(mActiveLedState) % LedStateTypeToStr(mLosLedState) << std::endl;
    }

    QSFPPortId mPortId;

    LedStateType mActiveLedState;

    LedStateType mLosLedState;
};

typedef boost::ptr_vector<boardMs::Chm6PortLedStates>  port_led_ptr_vec;
typedef port_led_ptr_vec::iterator   port_led_vec_itr;

// Line LED states
struct Chm6LineLedStates
{
    Chm6LineLedStates(LineId id, LedStateType activeLedState, LedStateType losLedState)
    : mLineId(id)
    , mActiveLedState(activeLedState)
    , mLosLedState(losLedState)
    {}

    ~Chm6LineLedStates() {}

    void Dump(std::ostream &os)
    {
        os << boost::format("%-5d : %-16s : %-15s") % LineIdToStr(mLineId) % LedStateTypeToStr(mActiveLedState) % LedStateTypeToStr(mLosLedState) << std::endl;
    }

    LineId mLineId;

    LedStateType mActiveLedState;

    LedStateType mLosLedState;
};


typedef boost::ptr_vector<boardMs::Chm6LineLedStates>  line_led_ptr_vec;
typedef line_led_ptr_vec::iterator line_led_vec_itr;


/*
* FPGA Misc Status and FGPA IOExp Defs
*/

const uint32 cFpgaMiscStatusRegAddr = cFpgaStatusReg;

const uint32 cPolarityHigh = 0x01;
const uint32 cPolarityLow  = 0x00;

// FPGA MISC STATUS (MS) BITS
const uint32 cFPGA_MS_BP_PRESENT                  = 1 << 0;
const uint32 cFPGA_MS_BP_CHASSIS_PG               = 1 << 1;
const uint32 cFPGA_MS_POWER_SEC_UV_OV_ALARM       = 1 << 2;
const uint32 cFPGA_MS_POWER_12V_UV_ALARM          = 1 << 3;
const uint32 cFPGA_MS_ADM_FPGA_SPARE              = 1 << 4;
const uint32 cFPGA_MS_TEMP_ALARM                  = 1 << 5;
const uint32 cFPGA_MS_HSWAP_ALERT                 = 1 << 6;
const uint32 cFPGA_MS_ETH_SWITCH_INT              = 1 << 7;
const uint32 cFPGA_MS_ETH_SWITCH_P3V3_FAIL        = 1 << 8;
const uint32 cFPGA_MS_FPGA_IO_P3V3_FAIL           = 1 << 9;
const uint32 cFPGA_MS_DCO_HOST_PWR_ON_FAULT       = 1 << 10;
const uint32 cFPGA_MS_OCXO_CLOCK_FAULT            = 1 << 11;
const uint32 cFPGA_MS_BP_LAMP_TEST_EN_IOP         = 1 << 12;
const uint32 cFPGA_MS_MEZZ_BOT_LED_I2C_BD_ERR     = 1 << 13;
const uint32 cFPGA_MS_MEZZ_BOT_LED_I2C_BD_TIMEOUT = 1 << 14;
const uint32 cFPGA_MS_MEZZ_TOP_LED_I2C_BD_ERR     = 1 << 15;
const uint32 cFPGA_MS_MEZZ_TOP_LED_I2C_BD_TIMEOUT = 1 << 16;
const uint32 cFPGA_MS_WDT_INT                     = 1 << 17;
const uint32 cFPGA_MS_PWR_12V_OV_ALARM            = 1 << 18;
const uint32 cFPGA_MS_GMCU_PWR_CYCLE              = 1 << 19;

// FPGA MEZZ IO EXP BITS
const uint32 cMIOEXP_PLLCLK_RSTB_L     = 1 << 0;
const uint32 cMIOEXP_PLLCLK_LOLB_L     = 1 << 1;
const uint32 cMIOEXP_PLLCLK_INTRB_L    = 1 << 2;
const uint32 cMIOEXP_IOX_SMBALERT_L    = 1 << 3;
const uint32 cMIOEXP_PORT_MGR_INT_L    = 1 << 4;
const uint32 cMIOEXP_MUX_SEL           = 1 << 5;
const uint32 cMIOEXP_BCM1_RESET_L      = 1 << 6;
const uint32 cMIOEXP_BCM2_RESET_L      = 1 << 7;
const uint32 cMIOEXP_BCM3_RESET_L      = 1 << 8;
const uint32 cMIOEXP_ADM_IOX_SPARE     = 1 << 9;
const uint32 cMIOEXP_P0V8_PGOOD        = 1 << 10;
const uint32 cMIOEXP_P1V8_PGOOD        = 1 << 11;
const uint32 cMIOEXP_P3V8_PGOOD        = 1 << 12;
const uint32 cMIOEXP_BCM1_INTR_L       = 1 << 13;
const uint32 cMIOEXP_BCM2_INTR_L       = 1 << 14;
const uint32 cMIOEXP_BCM3_INTR_L       = 1 << 15;
const uint32 cMIOEXP_LINE_LOS_LED_R    = 1 << 16;
const uint32 cMIOEXP_LINE_ACTIVE_LED_G = 1 << 17;
const uint32 cMIOEXP_LINE_ACTIVE_LED_Y = 1 << 18;
const uint32 cMIOEXP_FRU_ACTIVE_LED_G  = 1 << 19;
const uint32 cMIOEXP_FRU_ACTIVE_LED_Y  = 1 << 20;
const uint32 cMIOEXP_P0V8_AVS1_PGOOD   = 1 << 21;
const uint32 cMIOEXP_P0V8_AVS2_PGOOD   = 1 << 22;
const uint32 cMIOEXP_P0V8_AVS3_PGOOD   = 1 << 23;


typedef enum mezzBoardIdType
{
    MEZZ_BRD_TOP = 0,
    MEZZ_BRD_BTM,
    NUM_MEZZ_BRD_TYPES
}mezzBoardIdType;

typedef enum mezzBoardSpecType
{
    MEZZ_BRD_SPEC_TOP = boardMs::MEZZ_BRD_TOP,
    MEZZ_BRD_SPEC_BTM = boardMs::MEZZ_BRD_BTM,
    MEZZ_BRD_SPEC_BOTH,
    MEZZ_BRD_SPEC_NONE
}mezzBoardSpecType;

const uint16 cAdm1066EepromRevAddr = 0xF900;

const uint32 cOutLetTmpLLim = 72;
const uint32 cOutLetTmpHLim = 75; // C

const uint32 cColdRestartDcoDelaySec = 180;
} // namespace boardMs

#endif /* CHM6_BOARD_MS_SRC_BOARDDEFS_H_ */
