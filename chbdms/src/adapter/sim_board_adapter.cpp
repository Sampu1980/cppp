//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#include <ctime>

#include "sim_board_adapter.h"
#include "InfnLogger.h"

namespace simBoardAda
{
using namespace boardMs;

SimBoardAdapter::SimBoardAdapter()
    : BoardCommonAdapter()
{
    std::ostringstream  log;
    log << " Created!";
    AddLog(__func__, __LINE__, log.str());

    Initialize();
}

SimBoardAdapter::~SimBoardAdapter()
{
}

void SimBoardAdapter::SetEqptState(std::ostream &os, Chm6EqptState eqptState)
{
    mEqptState = eqptState;

    os << "CHM6 equipement state set to: " << Chm6EqptStateToStr(eqptState) << std::endl;
}

/*
 * Set EqptInventory
 */
void SimBoardAdapter::SetEqptInventory(std::ostream &os, std::string& invTypeName, std::string& invTypeValue)
{
    std::string typeName;

    if (invTypeName == "sn")
    {
        mEqptInv.SerialNumber = invTypeValue;
        typeName = std::string("SerialNumber");
    }
    else if(invTypeName == "pn")
    {
        mEqptInv.PartNumber = invTypeValue;
        typeName = std::string("PartNumber");
    }
    else if(invTypeName == "vn")
    {
        mEqptInv.VendorName = invTypeValue;
        typeName = std::string("VendorName");
    }
    else if(invTypeName == "hv")
    {
        mEqptInv.HardwareRevision = invTypeValue;
        typeName = std::string("HardwareRevision");
    }
    else if(invTypeName == "md")
    {
        mEqptInv.ManufactureDate = invTypeValue;
        typeName = std::string("ManufactureDate");
    }
    else if(invTypeName == "fv")
    {
        mEqptInv.FirmwareVersion = invTypeValue;
        typeName = std::string("FirmwareVersion");
    }
    else if(invTypeName == "cc")
    {
        mEqptInv.CleiCode = invTypeValue;
        typeName = std::string("CleiCode");
    }
    else if(invTypeName == "pon")
    {
        mEqptInv.Pon = invTypeValue;
        typeName = std::string("Pon");
    }
    else if(invTypeName == "id")
    {
        mEqptInv.InsertionDate = invTypeValue;
        typeName = std::string("InsertionDate");
    }

    else
    {
        os << "Input attribute: " << invTypeName << " doesn't exist!" << std::endl;
    }

    os << "Inventory " << typeName << " is updated to " << invTypeValue << std::endl;
}

/*
 * Set upgradable devices
 */
void SimBoardAdapter::SetUpgradableDevice(std::ostream &os, upgradableDeviceType type, std::string& hwVersion, std::string& swVersion)
{
    if (type >= MAX_UPGDDEVICE_TYPE_NUM)
    {
        os << "Invalid upgradable device type! " << std::endl;
        return;
    }

    mvUpgradableDevices[(int)type].SetHwVersion(hwVersion);
    mvUpgradableDevices[(int)type].SetSwVersion(swVersion);

    os << "Device: " << mvUpgradableDevices[(int)type].mDeviceName << " updated. HwVersion: " << hwVersion
       << " SwVersion: " << swVersion << std::endl;
 }

/*
 * Sim main board pm
 * Set single pm in pm data
 */
void SimBoardAdapter::SetBoardPm(std::ostream &os, float32 value)
{
    mvBoardPms[0].SetValue(value);

    os << "Updated board pm \"temperature\" to be: " << value << std::endl;
}

/*
 * Set power LED state
 */
void SimBoardAdapter::SetPowerLedState(std::ostream &os, LedStateType st)
{
    if (st >= MAX_LED_STATE_NUM)
    {
        os << "Invaid LED state: " << (int)st << std::endl
           << "Valid value is from 0 to " << (int)(MAX_LED_STATE_NUM-1) << std::endl;
        return;
    }

    mPowerLedState = st;

    os << "Updated power LED state to be: " << LedStateTypeToStr(st)
       << std::endl;
}

/*
 * Set Fault LED state
 */
void SimBoardAdapter::SetFaultLedState(std::ostream &os, LedStateType st)
{
    if (st >= MAX_LED_STATE_NUM)
    {
        os << "Invaid LED state: " << (int)st << std::endl
           << "Valid value is from 0 to " << (int)(MAX_LED_STATE_NUM-1) << std::endl;
        return;
    }

    SetFaultLedState(st);

    os << "Updated fault LED state to be: " << LedStateTypeToStr(st)
       << std::endl;
}

/*
 * Set Active LED state
 */
void SimBoardAdapter::SetActiveLedState(std::ostream &os, LedStateType st)
{
    if (st >= MAX_LED_STATE_NUM)
    {
        os << "Invaid LED state: " << (int)st << std::endl
           << "Valid value is from 0 to " << (int)(MAX_LED_STATE_NUM-1) << std::endl;
        return;
    }

    SetActiveLedState(st);

    os << "Updated active LED state to be: " << LedStateTypeToStr(st)
       << std::endl;
}

/*
 * Set Port LED state
 */
void SimBoardAdapter::SetPortLedState(std::ostream &os, QSFPPortId portId, QSFPLedType ledType, LedStateType ledState)
{
    SetPortLedState(portId, ledType, ledState);

    os << "SetPortLedState on : "
       << boardMs::QSFPPortIdToStr(portId)   << " "
       << boardMs::QSFPLedTypeToStr(ledType) << " "
       << " to " << boardMs::LedStateTypeToStr(ledState) << std::endl;
}

/*
 * Set Line LED state
 */
void SimBoardAdapter::SetLineLedState(std::ostream &os, LineId lineId, LineLedType ledType, LedStateType ledState)
{
    SetLineLedState(lineId, ledType, ledState);

    os << "SetLineLedState on : "
            << boardMs::LineIdToStr(lineId)   << " "
            << boardMs::LINELedTypeToStr(ledType) << " "
            << " to " << boardMs::LedStateTypeToStr(ledState)  << std::endl;
}

/*
 * Set LED lamp test
 */
void SimBoardAdapter::SetLedLampTest(std::ostream &os, bool doTest)
{
    mIsLedLampTestOn = doTest;

    os << "SetLedLampTest: " << std::boolalpha << mIsLedLampTestOn << std::noboolalpha << std::endl;
}

/* ==============================
 * APIs defined in BoardAdapterIf
 * ==============================
 */
/*
 * APIs to configure host board
 */
int SimBoardAdapter::SetHostBoardAction(hostBoardActionType action)
{
    mLastBoardAction = action;

    std::ostringstream  log;
    log << "mLastBoardAction = " << HostBoardActionTypeToStr(mLastBoardAction);
    AddLog(__func__, __LINE__, log.str());

    return 0;
}

/*
 * Board status
 */
Chm6EqptState SimBoardAdapter::GetEqptState()
{
    return mEqptState;
}

Chm6EqptInventory& SimBoardAdapter::GetEqptInventory()
{
    return mEqptInv;
}

LedStateType SimBoardAdapter::GetPowerLedState()
{
    return mPowerLedState;
}

void SimBoardAdapter::SetFaultLedState(LedStateType state)
{
    INFN_LOG(SeverityLevel::info) << " LED state: " << boardMs::LedStateTypeToStr(state);

    std::lock_guard<std::mutex> guard(mFaultLedLock);
    mFaultLedState = state;
}

LedStateType SimBoardAdapter::GetFaultLedState()
{
    if (mIsLedLampTestOn == true)
    {
        return CYCLING;
    }
    else if (mIsLedLocTestOn == true)
    {
        return CYCLING_WITH_OFF;
    }

    return mFaultLedState;
}

void SimBoardAdapter::SetActiveLedState(LedStateType state)
{
    INFN_LOG(SeverityLevel::info) << " LED state: " << boardMs::LedStateTypeToStr(state);

    std::lock_guard<std::mutex> guard(mActiveLedLock);
    mActiveLedState = state;
}

LedStateType SimBoardAdapter::GetActiveLedState()
{
    if (mIsLedLampTestOn == true)
    {
        return CYCLING;
    }
    else if (mIsLedLocTestOn == true)
    {
        return CYCLING_WITH_OFF;
    }

    return mActiveLedState;
}

void SimBoardAdapter::SetPortLedState(QSFPPortId portId, QSFPLedType ledType, LedStateType ledState)
{
    INFN_LOG(SeverityLevel::info) << "Set on : "
                   << boardMs::QSFPPortIdToStr(portId)   << " "
                   << boardMs::QSFPLedTypeToStr(ledType) << " "
                   << " to " << boardMs::LedStateTypeToStr(ledState);

    std::lock_guard<std::mutex> guard(mPortLedLock);

    if (ledType == QSFP_ACTIVE)
    {
        mvPortLedStates[(uint32)portId].mActiveLedState = ledState;
    }
    else if (ledType == QSFP_LOS)
    {
        mvPortLedStates[(uint32)portId].mLosLedState = ledState;
    }
}

port_led_ptr_vec& SimBoardAdapter::GetPortLedStates()
{
    if (mIsLedLampTestOn == true)
    {
        return mvPortLedStatesInLampTest;
    }
    else if (mIsLedLocTestOn == true)
    {
        return mvPortLedStatesInLocTest;
    }

    return mvPortLedStates;
}

void SimBoardAdapter::SetLineLedState(LineId lineId, LineLedType ledType, LedStateType ledState)
{
    INFN_LOG(SeverityLevel::info) << "Set on : "
                   << boardMs::LineIdToStr(lineId)   << " "
                   << boardMs::LINELedTypeToStr(ledType) << " "
                   << " to " << boardMs::LedStateTypeToStr(ledState);

    std::lock_guard<std::mutex> guard(mLineLedLock);

    if (ledType == LINE_ACTIVE)
    {
        mvLineLedStates[(uint32)lineId].mActiveLedState = ledState;
    }
    else if (ledType == LINE_LOS)
    {
        mvLineLedStates[(uint32)lineId].mLosLedState = ledState;
    }
}

line_led_ptr_vec& SimBoardAdapter::GetLineLedStates()
{
    if (mIsLedLampTestOn == true)
    {
        return mvLineLedStatesInLampTest;
    }
    else if (mIsLedLocTestOn == true)
    {
        return mvLineLedStatesInLocTest;
    }

    return mvLineLedStates;
}

void SimBoardAdapter::DumpEqptInventory( std::ostream &os )
{
    os << "<<<<<<<<<<<<<<<<<<< SimBoardAdapter.DumEqptInventory >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    mEqptInv.Dump(os);
}


////////////////////////////////////////////////////////////

void SimBoardAdapter::Initialize()
{
    std::cout << "SimBoardAdapter::Initialize() is called !!!" << std::endl;

    mLastBoardAction = RESTART_WARM;

    mEqptState = EQPT_STATE_BOOTING_DONE_WARM;

    // Equipment Inventory
    mEqptInv.SerialNumber = "PA3319520232";
    mEqptInv.PartNumber = "001";
    mEqptInv.VendorName = "INFINERA";
    mEqptInv.HardwareRevision = "0000.1";
    mEqptInv.ManufactureDate = "04-05-2020";
    mEqptInv.FirmwareVersion = "0.002";
    mEqptInv.CleiCode = "N/A";
    mEqptInv.Pon = "CHM6-C6";
    mEqptInv.InsertionDate = "05-04-2020";

    // Init LED states
    mPowerLedState = GREEN;
    mFaultLedState = RED;
    mActiveLedState = FLASHING_GREEN;

    InitUpgradeableDevices();

    // Init port LED states
    for (uint32 i = 0; i < MAX_QSFP_NUM; ++i)
    {
        mvPortLedStates.push_back(
                new Chm6PortLedStates(QSFPPortId(i), OFF, OFF));
    }

    // Init port LED states in lamp test
    for (uint32 i = 0; i < MAX_QSFP_NUM; ++i)
    {
        mvPortLedStatesInLampTest.push_back(
                new Chm6PortLedStates(QSFPPortId(i), CYCLING, CYCLING));
    }

    // Init port LED states in location test
    for (uint32 i = 0; i < MAX_QSFP_NUM; ++i)
    {
        mvPortLedStatesInLocTest.push_back(
                new Chm6PortLedStates(QSFPPortId(i), CYCLING_WITH_OFF, CYCLING_WITH_OFF));
    }

    // Init line LED states
    for (uint32 i = 0; i < MAX_LINE_NUM; ++i)
    {
        mvLineLedStates.push_back(
                new Chm6LineLedStates(LineId(i), OFF, OFF));
    }

    // Init line LED states in lamp test
    for (uint32 i = 0; i < MAX_LINE_NUM; ++i)
    {
        mvLineLedStatesInLampTest.push_back(
                new Chm6LineLedStates(LineId(i), CYCLING, CYCLING));
    }

    // Init line LED states in location test
    for (uint32 i = 0; i < MAX_LINE_NUM; ++i)
    {
        mvLineLedStatesInLocTest.push_back(
                new Chm6LineLedStates(LineId(i), CYCLING_WITH_OFF, CYCLING_WITH_OFF));
    }

    // Init faults cache
    for (uint32 i = 0; i < MAX_BOARD_FAULT_ID_NUM; ++i)
    {
        mvBoardFaults.push_back(
            new Chm6BoardFault((BoardFaultId)(i), true, FAULT_CLEAR));
    }

    // Init pm cache
    for (uint32 i = 0; i < MAX_PM_ID_NUM; ++i)
    {
        mvBoardPms.push_back(
                new Chm6BoardPm((BoardPmId)(i), 10.0));
    }
}

}




