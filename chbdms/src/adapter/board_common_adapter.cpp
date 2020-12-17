/*
 * board_common_adapter.cpp
 *
 *  Created on: 9/1, 2020
 */

#include "board_common_adapter.h"
#include "InfnLogger.h"
#include "board_fault_defs.h"

namespace boardAda
{
using namespace boardMs;

#define DBG 0
#if DBG
#define DLOG INFN_LOG(SeverityLevel::info)
#else
#define DLOG INFN_LOG(SeverityLevel::debug)
#endif

#define cAllFaultsString "ALL_FAULTS"

BoardCommonAdapter::BoardCommonAdapter()
    : mLastBoardAction(BOARD_ACTION_UNKNOWN)
    , mEqptState(EQPT_STATE_UNKNOWN)
    , mPowerLedState(GREEN)
    , mFaultLedState(LED_STATE_UNKNOWN)
    , mActiveLedState(LED_STATE_UNKNOWN)
    , mIsLedLampTestOn(false)
    , mIsLedLocTestOn(false)
    , mpLog(new SimpleLog::Log(2000))
{
    std::ostringstream  log;
    log << " Created!";
    AddLog(__func__, __LINE__, log.str());
    INFN_LOG(SeverityLevel::info) << log.str();
}

BoardCommonAdapter::~BoardCommonAdapter()
{
    if (mpLog)
    {
        delete mpLog;
    }
}

upgradable_device_ptr_vec& BoardCommonAdapter::GetUpgradableDevices()
{
    return mvUpgradableDevices;
}

// Fault
board_fault_ptr_vec& BoardCommonAdapter::GetBoardFault()
{
    // TODO: Update faults from driver
    return mvBoardFaults;
}

/*
 * Board PM
 */

board_pm_ptr_vec& BoardCommonAdapter::GetBoardPm()
{
    return mvBoardPms;
}

void BoardCommonAdapter::DumpLog(std::ostream &os)
{
    os << "<<<<<<<<<<<<<<<<<<< BoardAdapter.DumpLog >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    std::lock_guard<std::mutex> guard(mLogLock);

    if (mpLog)
    {
        mpLog->DisplayLog( os );
    }
    else
    {
        os << "BoardAdapter Log is not created!" << std::endl;
    }
}

void BoardCommonAdapter::DumpStatus(std::ostream &os, std::string cmd)
{
    os << "BoardAdapter::DumpStatus(): cmd = " << cmd << std::endl;
}

void BoardCommonAdapter::ResetLog( std::ostream &os )
{
    os << "<<<<<<<<<<<<<<<<<<< BoardAdapter.ResetLog >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    std::lock_guard<std::mutex> guard(mLogLock);

    if (mpLog)
    {
        mpLog->ResetLog();
        os << "BoardAdapter Log has been reset!" << std::endl;
    }
}

void BoardCommonAdapter::DumpUpgradableDevices( std::ostream &os )
{
    os << "<<<<<<<<<<<<<<<<<<< BoardAdapter.DumpUpgradableDevices >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    os << boost::format("%-3s : %-10s : %-8s : %-8s") % "ID" % "DeviceName" % "HwVersion" % "SwVersion" << std::endl;

    for (upgradable_device_vec_itr itr = mvUpgradableDevices.begin(); itr != mvUpgradableDevices.end(); itr++)
    {
        (*itr).Dump(os);
    }
}

void BoardCommonAdapter::DumpFaults(std::ostream &os,
				    uint32 faultId)
{
    os << "<<<<<<<<<<<<<<<<<<< BoardAdapter.DumpFaults "
       << ">>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    if (faultId >= MAX_BOARD_FAULT_ID_NUM)
    {
        os << "**** Dumping All Faults...." << endl << endl;

        Chm6BoardFault::DumpHeader(os);

        for (board_fault_vec_itr itr = mvBoardFaults.begin();
             itr != mvBoardFaults.end();
             itr++)
        {
            (*itr).Dump(os);
        }
    }
    else
    {
        os << "**** Dumping Fault for ID: " << faultId
           << " ...." << endl << endl;

        mvBoardFaults[faultId].DumpHeaderSpecific(os);
        mvBoardFaults[faultId].DumpSpecific(os);
    }
    os << endl;
}

void BoardCommonAdapter::DumpFaults(std::ostream &os,
				    std::string faultName)
{
    if (faultName == cAllFaultsString)
    {
        DumpFaults(os, MAX_BOARD_FAULT_ID_NUM);
    }
    else
    {
        uint32 faultId = LookupFaultId(faultName);

        if (faultId < MAX_BOARD_FAULT_ID_NUM)
        {
            DumpFaults(os, faultId);
        }
        else
        {
            os << "Fault Name Not Found: " << faultName << endl;
        }
    }
    os << endl;
}

void BoardCommonAdapter::SetFaultSim(std::ostream &os, uint32 faultId, bool isSimEn, bool isTripped)
{
    os << "<<<<<<<<<<<<<<<<<<< BoardAdapter.SetFaultSim >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    if (faultId >= MAX_BOARD_FAULT_ID_NUM)
    {
        os << "faultId >= Max " << (uint32)MAX_BOARD_FAULT_ID_NUM << " so updating all faults" << endl;
        os << "Setting All Faults Sim. isSimEnable: " << isSimEn << " isTripped: " << isTripped << endl << endl;

        INFN_LOG(SeverityLevel::info) << "Setting All Faults Sim. isSimEnable: " << isSimEn << " isTripped: " << isTripped;

        for (board_fault_vec_itr itr = mvBoardFaults.begin(); itr != mvBoardFaults.end(); itr++)
        {
            (*itr).SetSimCondition(isTripped ? FAULT_SET : FAULT_CLEAR);
            (*itr).SetSimEnable(isSimEn);
        }
    }
    else
    {
        os << "Setting Faults Sim for faultId: " << faultId << " isSimEnable: " << isSimEn << " isTripped: " << isTripped << endl << endl;

        INFN_LOG(SeverityLevel::info) << "Setting Faults Sim for faultId: " << faultId << " isSimEnable: " << isSimEn << " isTripped: " << isTripped;

        mvBoardFaults[faultId].SetSimCondition(isTripped ? FAULT_SET : FAULT_CLEAR);
        mvBoardFaults[faultId].SetSimEnable(isSimEn);
    }

    DumpFaults(os, faultId);
}

void BoardCommonAdapter::SetFaultSim(std::ostream &os, std::string faultName, bool isSimEn, bool isTripped)
{
    if (faultName == cAllFaultsString)
    {
        SetFaultSim(os, MAX_BOARD_FAULT_ID_NUM, isSimEn, isTripped);
    }
    else
    {
        uint32 faultId = LookupFaultId(faultName);

        if (faultId < MAX_BOARD_FAULT_ID_NUM)
        {
            SetFaultSim(os, faultId, isSimEn, isTripped);
        }
        else
        {
            os << "Fault Name Not Found: " << faultName << endl;
        }
    }
}

void BoardCommonAdapter::PrintMaxFaultNum(std::ostream &os)
{
    os << "<<<<<<<<<<<<<<<<<<< BoardAdapter.PrintMaxFaultNum >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    os << "Max Fault Number : " << (uint32) MAX_BOARD_FAULT_ID_NUM << endl << endl;
}

// Returns >= MAX_BOARD_FAULT_ID_NUM if faultName not found
uint32 BoardCommonAdapter::LookupFaultId(std::string faultName)
{
    uint32 i = 0;
    for (board_fault_vec_itr itr = mvBoardFaults.begin(); itr != mvBoardFaults.end(); itr++)
    {
        if (faultName == (*itr).GetName())
        {
            break;
        }
        i++;
    }
    return i;
}

void BoardCommonAdapter::DumpPms(std::ostream &os)
{
    os << "<<<<<<<<<<<<<<<<<<< BoardAdapter.DumpPms >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    for (board_pm_vec_itr itr = mvBoardPms.begin(); itr != mvBoardPms.end(); itr++)
    {
        (*itr).Dump(os);
    }
}

void BoardCommonAdapter::DumpLedStates(std::ostream &os)
{
    os << "<<<<<<<<<<<<<<<<<<< BoardAdapter.DumpLedStates >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    os << "mIsLedLampTestOn : " << std::boolalpha << mIsLedLampTestOn << std::endl;
    os << "mIsLedLocTestOn  : " << std::boolalpha << mIsLedLocTestOn << std::endl << std::endl;

    os << "On SIM: Below LED states reflect LED configurations. Do NOT reflect LED colors during lamp and location tests." << std::endl;

    os << "Power LED state   : " << LedStateTypeToStr(mPowerLedState) << std::endl
       << "Fault LED state   : " << LedStateTypeToStr(mFaultLedState) << std::endl
       << "Active LED state  : " << LedStateTypeToStr(mActiveLedState) << std::endl;

    os << boost::format("%-5d : %-16s : %-15s") % "PortID" % "Active LED state" % "Los LED state" << std::endl;
    for (port_led_vec_itr itr = mvPortLedStates.begin(); itr != mvPortLedStates.end(); itr++)
    {
        (*itr).Dump(os);
    }

    os << boost::format("%-5d : %-16s : %-15s") % "LineID" % "Active LED state" % "Los LED state" << std::endl;

    for (line_led_vec_itr itr = mvLineLedStates.begin(); itr != mvLineLedStates.end(); itr++)
    {
        (*itr).Dump(os);
    }
}

////////////////////////////////////////////////////////////


void BoardCommonAdapter::InitUpgradeableDevices()
{
    // Upgradable devices
    for (uint i = 0; i < MAX_UPGDDEVICE_TYPE_NUM; ++i)
    {
        char *pEnvStr;
        std::string swVer;
        switch(i)
        {
            case PL_FPGA:
            {
                if (NULL == (pEnvStr = std::getenv(cEnvStrVerFgpa.c_str())))
                {
                    INFN_LOG(SeverityLevel::info) << "Env Variable " << cEnvStrVerFgpa << " Not Present";
                    swVer = "0";
                }
                else
                {
                    swVer = pEnvStr;
                    INFN_LOG(SeverityLevel::info) << "Env Variable " << cEnvStrVerFgpa << " = " << swVer;
                }

                break;
            }

            case UBOOT:
            {
                if (NULL == (pEnvStr = std::getenv(cEnvStrVerUboot.c_str())))
                {
                    INFN_LOG(SeverityLevel::info) << "Env Variable " << cEnvStrVerUboot << " Not Present";
                    swVer = "0";
                }
                else
                {
                    swVer = pEnvStr;
                    INFN_LOG(SeverityLevel::info) << "Env Variable " << cEnvStrVerUboot << " = " << swVer;
                }

                break;
            }

            case GECKO:
            {
                if (NULL == (pEnvStr = std::getenv(cEnvStrVerGecko.c_str())))
                {
                    INFN_LOG(SeverityLevel::info) << "Env Variable " << cEnvStrVerGecko << " Not Present";
                    swVer = "0";
                }
                else
                {
                    swVer = pEnvStr;
                    INFN_LOG(SeverityLevel::info) << "Env Variable " << cEnvStrVerGecko << " = " << swVer;
                }

                break;
            }

            default:
            {
                swVer = "0";
                break;
            }
        }

        mvUpgradableDevices.push_back(
                new upgradableDevice((upgradableDeviceType)(i), "00.1", swVer));
    }

}

void BoardCommonAdapter::AddLog(const std::string &func, uint32 line, const std::string &text)
{
    std::lock_guard<std::mutex> guard(mLogLock);

    ostringstream  os;
    os << "BoardAdapter::" << func << ":" << line << ": " << text;

    if (mpLog)
    {
        mpLog->AddRecord( os.str().c_str() );
    }
}

// Converts a fault Id string and returns the equivalent
// numerical value for the access fault enumeration.
//
const uint32 BoardCommonAdapter::StrToFaultId(
    RT_AccessFaultsMap& afMap, std::string strfaultId )
{
    // The converted fault Id number.
    uint32 faultId = BOARD_FAULT_INVALID;

    try
    {
        // Determines if the string is actually a number.
        // Converts string to number, but if the string
        // is not a number, an exception is thrown.
        //
        faultId = std::stoi(strfaultId);
    }
    catch(...)
    {
        // The string is not a fault Id number, so we
        // lookup the string in the access fault
        // board and mezzanine maps.
        //
        for( const auto& kv : afMap.af() )
          if( kv.second.fidName == strfaultId )
          {
              DLOG << "Fault: " << strfaultId
                   << " : id = " << kv.second.boardFaultId;
              return kv.second.boardFaultId;
          }

        // BOTTOM Mezz
        for( const auto& kv : afMap.mezaf_b() )
          if( kv.second.fidName == strfaultId )
          {
              DLOG << "Fault: " << strfaultId
                   << " : id = " << kv.second.boardFaultId;
              return kv.second.boardFaultId;
          }

        // TOP Mezz
        for( const auto& kv : afMap.mezaf_t() )
          if( kv.second.fidName == strfaultId )
          {
              DLOG << "Fault: " << strfaultId
                   << " : id = " << kv.second.boardFaultId;
              return kv.second.boardFaultId;
          }
    }

    return faultId;
}

void BoardCommonAdapter::DoSetAccessFaultSim( std::ostream &os,
                                              std::string strfaultId,
                                              bool bSetFault )
{
    os << "<<<<<<<<<<<<<<<<<<< "
       << "BoardAdapter.SetAccessFaultSim "
       << ">>>>>>>>>>>>>>>>>>>"
       << endl << endl;

    uint32 faultId = BOARD_FAULT_INVALID;

    // Read the "BoardInfo" environment variable.
    // If the variable does NOT exist OR the variable
    // contents is NOT "sim", then we are NOT running
    // in simulation mode.
    //
    const char *pEnvStr = getenv( "BoardInfo" );
    if( NULL == pEnvStr
        || string( pEnvStr ) != string( "sim" ) )
    {
        RT_AccessFaultsMap& afMap
            = RT_AccessFaultsMap::getInstance();

        // Clean up the supplied string candidate by
        // replace dashes with underscores in the string
        // and convert the entire string to uppercase
        // to enable matching with the enumeration values.
        //
        std::replace( strfaultId.begin(), strfaultId.end(),
                      '-', '_' );
        std::transform( strfaultId.begin(), strfaultId.end(),
                        strfaultId.begin(), ::toupper );

        // Convert the fault Id string to a fault Id number.
        //
        faultId = StrToFaultId( afMap, strfaultId );
        DLOG << faultId;

        // Continue processing only if the fault Id
        // is a valid access fault (either a valid board
        // fault or a valid mezzanine fault.)
        //
        if( afMap.IsValidAccessFault( faultId )
            || afMap.IsValidMezAccessFault_b( faultId )
            || afMap.IsValidMezAccessFault_t( faultId ) )
        {            
#if DBG
            string s( "Setting Access Faults Sim for faultId: " );
            s += to_string( faultId ) + " SetFault: "
              + to_string( bSetFault );
            DLOG << s;
            os << s << endl;
#endif
            try
            {
                // Get the BoardAccessFault object from the
                // fault object collection.
                //
                auto& f = static_cast<BoardAccessFault&>(
                            mvBoardFaults[faultId] );

                // Set the test device address or restore
                // the test device address to its proper
                // value, accordingly.
                //
                if( false == bSetFault )
                {
                    f.RestoreDevAddr();
                }
                else
                {
                    f.SetDevAddr( boardMs::BAD_DEV_ADDR );
                }
            }
            INFN_CATCH()
        }
        else
        {
            string s( "faultId " );
            s += "'" + strfaultId;
            s += "' is not a valid access fault.";

            os << s << endl;
            INFN_LOG(SeverityLevel::info) << s;

            return;
        }
    }
    else
    {
        string s( "Access Faults " );
        s += "are unavailable in SIM mode.";

        os << s << endl << endl;
        INFN_LOG(SeverityLevel::info) << s;

        return;
    }

    DumpFaults( os, faultId );
}

void BoardCommonAdapter::SetAccessFaultSim( std::ostream &os,
                                            std::string strfaultId,
                                            bool bSetFault )
{
    try
    {
        DoSetAccessFaultSim(os, strfaultId, bSetFault);
    }
    INFN_CATCH()
}

} // namespace boardAda
