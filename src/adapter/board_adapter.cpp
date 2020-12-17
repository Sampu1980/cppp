/*
 * board_adapter.cpp
 *
 *  Created on: Jan 21, 2020
 */

#include <iomanip>

#include "board_adapter.h"
#include "InfnLogger.h"
#include "board_fault_defs.h"

#define DBG 0
#if DBG
#define DLOG INFN_LOG(SeverityLevel::info)
#else
#define DLOG INFN_LOG(SeverityLevel::debug)
#endif

namespace boardAda
{
using namespace boardMs;

BoardAdapter::BoardAdapter(BoardDriver& boardDriver)
    : BoardCommonAdapter()
    , mDriver(boardDriver)
{
    std::ostringstream  log;
    log << " Created!";
    AddLog(__func__, __LINE__, log.str());
    INFN_LOG(SeverityLevel::info) << log.str();

    Initialize();
}

BoardAdapter::~BoardAdapter()
{

}

void BoardAdapter::SetSlotId(uint32 slotId)
{
    INFN_LOG(SeverityLevel::info) << "BoardAdapter::SetSlotId() is called !!!"
         << "slotId: "  << slotId;
}

void BoardAdapter::SetSledId(uint32 sledId)
{
    INFN_LOG(SeverityLevel::info) << "BoardAdapter::SetSledId() is called !!!"
         << "sledId: "  << sledId;
}

/*
 * APIs to configure host board
 */
int BoardAdapter::SetHostBoardAction(hostBoardActionType action)
{
    mLastBoardAction = action;

    std::ostringstream  log;
    log << "mLastBoardAction = " << HostBoardActionTypeToStr(mLastBoardAction);
    AddLog(__func__, __LINE__, log.str());
    INFN_LOG(SeverityLevel::info) << log.str();

    if (action == POWER_UP)
    {

    }

    else if (action == UPDATE_FW)
    {

    }

    else if (action == INIT_HW)
    {

    }

    else if (action == RESTART_COLD)
    {
        mDriver.ResetFruLedsOnReboot(true);
    }

    else if (action == RESTART_WARM)
    {
        mDriver.ResetFruLedsOnReboot(false);
    }

    else if (action == GRACEFUL_SHUTDOWN)
    {

    }

    return 0;
}

void BoardAdapter::SetColdRestartDcoDelay(uint32 delaySeconds)
{
    INFN_LOG(SeverityLevel::info) << "DCO cold boot delay Seconds = " << delaySeconds;

    mDriver.SetColdRestartDcoDelay(delaySeconds);
}

/*
 * Queries
 */
bool BoardAdapter::IsColdBoot()
{
    return true;
}

bool BoardAdapter::IsOnBench()
{
    return true;
}

bool BoardAdapter::IsChassisDiag()
{
    return true;
}

Chm6EqptState BoardAdapter::GetEqptState()
{
    return mEqptState;
}

Chm6EqptInventory& BoardAdapter::GetEqptInventory()
{
    std::lock_guard<std::recursive_mutex> lock(mRMutexEqptInv);
    mDriver.GetEqptInventory(mEqptInv);
    return mEqptInv;
}

LedStateType BoardAdapter::GetPowerLedState()
{
    return mPowerLedState; // Always GREEN after power up
}

void BoardAdapter::SetFaultLedState(LedStateType state)
{
    mDriver.SetFruFaultLedState(state);
}

LedStateType BoardAdapter::GetFaultLedState()
{
    if (mIsLedLampTestOn == true)
    {
        mFaultLedState = CYCLING;
    }
    else if (mIsLedLocTestOn == true)
    {
        mFaultLedState = CYCLING_WITH_OFF;
    }
    else
    {
        mDriver.GetFruFaultLedState(mFaultLedState);
    }

    return mFaultLedState;
}

void BoardAdapter::SetActiveLedState(LedStateType state)
{
    mDriver.SetFruActiveLedState(state);
}

LedStateType BoardAdapter::GetActiveLedState()
{
    if (mIsLedLampTestOn == true)
    {
        mActiveLedState = CYCLING;
    }
    else if (mIsLedLocTestOn == true)
    {
        mActiveLedState = CYCLING_WITH_OFF;
    }
    else
    {
        mDriver.GetFruActiveLedState(mActiveLedState);
    }

    return mActiveLedState;
}

void BoardAdapter::DoLedLocationTest(bool doTest)
{
    mDriver.LedLocationTest(doTest);
}

void BoardAdapter::SetPortLedState(QSFPPortId portId, QSFPLedType ledType, LedStateType ledState)
{
    mDriver.SetMezzQsfpLedState(portId, ledType, ledState);
}

port_led_ptr_vec& BoardAdapter::GetPortLedStates()
{
    for (port_led_vec_itr itr = mvPortLedStates.begin(); itr != mvPortLedStates.end(); itr++)
    {
        if (mIsLedLampTestOn == true)
        {
            (*itr).mActiveLedState = CYCLING;
            (*itr).mLosLedState = CYCLING;
        }
        else if (mIsLedLocTestOn == true)
        {
            (*itr).mActiveLedState = CYCLING_WITH_OFF;
            (*itr).mLosLedState = CYCLING_WITH_OFF;
        }
        else
        {
            QSFPPortId portId = (*itr).mPortId;

            LedStateType state;
            mDriver.GetMezzQsfpLedState(portId, QSFP_ACTIVE, state);
            (*itr).mActiveLedState = state;

            mDriver.GetMezzQsfpLedState(portId, QSFP_LOS, state);
            (*itr).mLosLedState = state;
        }
    }

    return mvPortLedStates;
}

void BoardAdapter::SetLineLedState(LineId lineId, LineLedType ledType, LedStateType ledState)
{
    mDriver.SetMezzLineLedState(lineId, ledType, ledState);
}

line_led_ptr_vec& BoardAdapter::GetLineLedStates()
{
    for (line_led_vec_itr itr = mvLineLedStates.begin(); itr != mvLineLedStates.end(); itr++)
    {
        if (mIsLedLampTestOn == true)
        {
            (*itr).mActiveLedState = CYCLING;
            (*itr).mLosLedState = CYCLING;
        }
        else if (mIsLedLocTestOn == true)
        {
            (*itr).mActiveLedState = CYCLING_WITH_OFF;
            (*itr).mLosLedState = CYCLING_WITH_OFF;
        }
        else
        {
            LineId lineId = (*itr).mLineId;

            LedStateType state;
            mDriver.GetMezzLineLedState(lineId, LINE_ACTIVE, state);
            (*itr).mActiveLedState = state;

            mDriver.GetMezzLineLedState(lineId, LINE_LOS, state);
            (*itr).mLosLedState = state;
        }
    }

    return mvLineLedStates;
}

void BoardAdapter::CheckLedLampLocTestState()
{
    mDriver.CheckLedLampLocTestState(mIsLedLampTestOn, mIsLedLocTestOn);
}

void BoardAdapter::CheckFaults()
{
    for (auto &it : mvBoardFaults)
    {
        it.CheckFaultCondition();
    }
}

board_pm_ptr_vec& BoardAdapter::GetBoardPm()
{
    for (board_pm_vec_itr itr = mvBoardPms.begin(); itr != mvBoardPms.end(); itr++)
    {
        (*itr).mValue = mDriver.GetPm((*itr).mId);
    }

    return mvBoardPms;
}

void BoardAdapter::DumpEqptInventory( std::ostream &os )
{
    os << "<<<<<<<<<<<<<<<<<<< BoardAdapter.DumEqptInventory >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    std::lock_guard<std::recursive_mutex> lock(mRMutexEqptInv);
    Chm6EqptInventory  EqptInv;
    mDriver.GetEqptInventory(EqptInv);
    EqptInv.Dump(os);
}

////////////////////////////////////////////////////////////

void BoardAdapter::Initialize()
{
    INFN_LOG(SeverityLevel::info) << "BoardAdapter::Initialize() is called !!!";

    InitUpgradeableDevices();

    InitializeFaults();

    // Init pm cache
    for (uint32 i = 0; i < MAX_PM_ID_NUM; ++i)
    {
        mvBoardPms.push_back(
                new Chm6BoardPm((BoardPmId)(i), 0.0));
    }

    // Init Fru LED states
    mPowerLedState = GREEN;
    mFaultLedState = LED_STATE_UNKNOWN;
    mActiveLedState = LED_STATE_UNKNOWN;

    // Init port LED states
    for (uint32 i = 0; i < MAX_QSFP_NUM; ++i)
    {
        mvPortLedStates.push_back(
                new Chm6PortLedStates(QSFPPortId(i), LED_STATE_UNKNOWN, LED_STATE_UNKNOWN));
    }

    // Init line LED states
    for (uint32 i = 0; i < MAX_LINE_NUM; ++i)
    {
        mvLineLedStates.push_back(
                new Chm6LineLedStates(LineId(i), LED_STATE_UNKNOWN, LED_STATE_UNKNOWN));
    }
}

void BoardAdapter::InitializeFaults()
{
    INFN_LOG(SeverityLevel::info) << "";

    // Init faults cache
    for (uint32 i = 0; i < MAX_BOARD_FAULT_ID_NUM; ++i)
    {
        mvBoardFaults.push_back(
            new Chm6BoardFault(
                (BoardFaultId)(i), false, FAULT_UNKNOWN));
    }

    const boardMs::DigitalInputFaultData *pFltInData;
    uint32 maxFpgaHbFaultElems = boardMs::cNumFpgaHostBoardFaults;

    INFN_LOG(SeverityLevel::info) << "Initializing Host Board Faults";

    for(uint32 i = 0; i < maxFpgaHbFaultElems; i++)
    {
        pFltInData = &boardMs::cFpgaHostBoardFaults[i];

        boardMs::BoardFaultId faultId   = pFltInData->mFaultId;
        uint32                inputMask = pFltInData->mInputMask;
        uint32                inputPol  = pFltInData->mInputPolarity;

        INFN_LOG(SeverityLevel::debug) << "Adding faultId: "
                           << std::setw(3)
                           << std::right << std::setfill(' ')
                           << static_cast<uint32>(faultId) << std::hex
                       << " inputMask: 0x"
                           << std::setw(5)
                           << std::right << std::setfill('0')
                           << inputMask
                       << " inputPolarity: 0x"
                           << std::setw(5)
                           << std::right << std::setfill('0')
                           << inputPol << std::dec;

        mvBoardFaults.replace(
            faultId,
            new boardMs::BoardFaultFpga(
	        *pFltInData,
                mDriver.GetBrdCmnDriver(),
                false,
                FAULT_UNKNOWN));
    }


    DLOG << "Initializing Host Board ACCESS Faults";
    
    // Get singleton instsance of RT_AccessFaultsMap.
    RT_AccessFaultsMap& afMap = RT_AccessFaultsMap::getInstance();

    // Iterate Board Access Faults.
    for( const auto& kv : afMap.af() )
    {
        const auto& afval = kv.second;
        DLOG << "Creating HOST ACCESS Fault: "
                       << afval.boardFaultId;

        auto baf = new boardMs::BoardAccessFault(afval);
        mvBoardFaults.replace(afval.boardFaultId, baf);
    }

    DLOG << "Host Board ACCESS Faults initialized...";

    DLOG << "Initializing Mezzanine ACCESS Faults";
    
    // Iterate Mezzanine Access Faults BOTTOM.
    for( const auto& kv : afMap.mezaf_b() )
    {
        const auto& afval = kv.second;
        DLOG << "Creating MEZZANINE ACCESS Fault: "
             << afval.boardFaultId;

        auto baf = new boardMs::BoardAccessFault(afval);
        mvBoardFaults.replace(afval.boardFaultId, baf);
    }

    // Iterate Mezzanine Access Faults TOP.
    for( const auto& kv : afMap.mezaf_t() )
    {
        const auto& afval = kv.second;
        DLOG << "Creating MEZZANINE ACCESS Fault: "
             << afval.boardFaultId;

        auto baf = new boardMs::BoardAccessFault(afval);
        mvBoardFaults.replace(afval.boardFaultId, baf);
    }

    DLOG << "Mezzanine Board ACCESS Faults initialized...";
}
// END BoardAdapter::InitializeFaults()

} // END namespace boardAda

