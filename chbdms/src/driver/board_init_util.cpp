//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include <ctype.h>

#include "InfnLogger.h"
#include "board_init_util.h"
#include "RegIfException.h"
#include "board_defs.h"
#include "board_fault_defs.h"

#define ELOG INFN_LOG(SeverityLevel::error)
#define ILOG INFN_LOG(SeverityLevel::info)

#define DBG 0
#if DBG
#define DLOG ILOG
#else
#define DLOG INFN_LOG(SeverityLevel::debug)
#endif


BoardInitUtil::BoardInitUtil()
  : mFaultSimEn(false)
  , mIsSim(false)
  , mIsEval(false)
{
    CheckEnv();

    if (IsHwEnv())
    {
        RegIfFactory* pFactory = new RegIfFactory();
        RegIfFactorySingleton::InstallInstance(pFactory);
        mspBrdDriver = std::make_shared<BoardCommonDriver>(mIsSim, mIsEval);
    }
}

BoardInitUtil::~BoardInitUtil()
{

}

/*
 * ColdInit CHM6 board after power up
 */
int BoardInitUtil::ColdInit()
{
    if (!IsHwEnv())
    {
      ILOG << "Running on sim or eval or hb only platform."
                     << " Skipping cold boot hw initialization.";

        ILOG << "Check Faults for Sim";
        if (CheckAllFaults() == false)
        {
            ILOG << "Sim Faults detected. Failing Init";

            return -1;
        }

        return 0;
    }

    boardMs::mezzBoardSpecType mezzBrdSpecId;

    if (mEnvStr == "")
    {
        mezzBrdSpecId = boardMs::MEZZ_BRD_SPEC_BOTH;
    }
    else if (mEnvStr == "bmz")
    {
        mezzBrdSpecId = boardMs::MEZZ_BRD_SPEC_BTM;
    }
    else if (mEnvStr == "tmz")
    {
        mezzBrdSpecId = boardMs::MEZZ_BRD_SPEC_TOP;
    }
    else
    {
        INFN_LOG(SeverityLevel::error) << "Env Str unexpected for HW: "
                        << mEnvStr
                        << " Not sure how to initialize Mezz Boards";

        // what todo??
        return -1;
    }

    mspBrdDriver->CreateBcmDriver();

    ILOG << "Initialize board after power up ... ";

    ILOG << "Checking Host Board Faults ... ";
    if (CheckHstBrdFaults() == false)
    {
        ILOG << "Host Board Fault(s) detected. Failing Init here";

        return -1;
    }

    ILOG << "1. Disable MZ board power and put both MZ in reset ...";

    if (mspBrdDriver->EnableMezzPower(false))
    {
        // Failure
        // todo: raise fault??

        INFN_LOG(SeverityLevel::error) << "EnableMezzPower off failed!! Init Failure";

        return -1;
    }

    ILOG << "2. Configure Latch enable ...";

    if (mspBrdDriver->LatchEnable())
    {
        // Failure
        // todo: raise fault??

        INFN_LOG(SeverityLevel::error) << "LatchEnable failed!! Init Failure";

        return -1;
    }

    ILOG << "3. Enable LED Control (both MZ) ...";

    if (mspBrdDriver->EnableLedControl(mezzBrdSpecId))
    {
        // Failure
        // todo: raise fault??

        INFN_LOG(SeverityLevel::error) << "EnableMezzPower failed!! Init Failure";

        return -1;
    }

    ILOG << "4. Enable power to MZ boards ... ";

    if (mspBrdDriver->EnableMezzPower(true))
    {
        // Failure
        // todo: raise fault??

        INFN_LOG(SeverityLevel::error) << "EnableMezzPower on failed!! Init Failure";

        return -1;
    }

    ILOG << "5. Take MZ out of reset ... ";

    if (mspBrdDriver->EnableMezzReset(mezzBrdSpecId, false))
    {
        // Failure
        // todo: raise fault??

        INFN_LOG(SeverityLevel::error) << "EnableMezzReset failed!! Init Failure";

        return -1;
    }

    ILOG << "5.a. Wait until Fpga LED controls are ready ...";

    if (mspBrdDriver->PollUntilFpgaLedReady(mezzBrdSpecId))
    {
        // Failure
        // todo: raise fault??

        INFN_LOG(SeverityLevel::error) << "PollUntilFpgaLedReady failed!! Init Failure";

        return -1;
    }

    ILOG << "5.b. Cold boot: set Fru Fault LED to FLASHING RED ...";

    if (mspBrdDriver->SetFruFaultLedFlashRed())
    {
        // Failure
        // todo: raise fault??

        INFN_LOG(SeverityLevel::error) << "SetFruFaultLedFlashRed failed!! Init Failure";

        return -1;
    }

    ILOG << "6. Initialize Mezz boards ...";

    int retCode = InitMezzBoards();
    if (retCode)
    {
        INFN_LOG(SeverityLevel::error) << " InitMezzBoards Failed with error code " << retCode;

        return -1;
    }

    ILOG << "  Initialize Gearbox BCM Driver ...";
    if (mspBrdDriver->InitBcmDriver(mezzBrdSpecId))
    {
        // Failure
        // todo: raise fault??

        INFN_LOG(SeverityLevel::error) << "InitBcmDriver failed!! Init Failure";

        return -1;
    }

    ILOG << "Board hardware init done.";

    return 0;
}

/*
 * Warm init
 */
int BoardInitUtil::WarmInit()
{
    if (!IsHwEnv())
    {
        ILOG << "Running on sim or eval or hb only platform. "
                       << "Skipping cold boot hw initialization";
        return 0;
    }

    /*
     * Warm boot: set Fru Active LED to FLASHING GREEN
     */
    if( !CheckHstBrdFaults() )
    {
        return -1;
    }

    if( "" == mEnvStr || "bmz" == mEnvStr )
    {
        if( !CheckMezBrdFaults(boardMs::MEZZ_BRD_BTM) )
          return -1;
    }

    if( "" == mEnvStr || "tmz" == mEnvStr )
    {
        if( !CheckMezBrdFaults(boardMs::MEZZ_BRD_TOP) )
          return -1;
    }
    
    ILOG << "Warm boot: set Fru Active LED to FLASHING GREEN ...";

    if (mspBrdDriver->SetFruActiveLedFlashGreen())
    {
        // Failure
        // todo: raise fault??

        return -1;
    }

    return 0;
}

uint32 BoardInitUtil::GetResetCause()
{
#ifdef ARCH_x86
    ILOG << "Sim - Returning 0";
    return 0;
#else
    if (mIsSim)
    {
        ILOG << "Sim - Returning 0";
        return 0;
    }
    else
    {
        // todo: Need to verify Gecko Comm before making this call
        //       If Gecko is not communicating, this call will trigger a cold boot init
        //       Need a way to address this. The call should probably throw exception in failure case

        ILOG << "Calling Gecko SD_GetResetCause";

        // todo: handle Gecko Access Failures!!
        //    Catch error here and report

        if (mspBrdDriver != nullptr)
        {
            return mspBrdDriver->GetResetCause();
        }
        else
        {
            INFN_LOG(SeverityLevel::error) << "Brd Driver not created yet";

            return 0;
        }
    }
#endif
}

bool BoardInitUtil::ClrResetCause()
{
    ILOG << "";

#ifdef ARCH_x86
    return true;
#else
    if (mIsSim)
    {
        ILOG << "Sim. Doing nothing, Returning true.";
        return true;
    }
    else
    {
        // todo: handle Gecko Access Failures!!
        //    Raise fault here if return false??

        if (mspBrdDriver != nullptr)
        {
            return mspBrdDriver->ClrResetCause();
        }
        else
        {
            INFN_LOG(SeverityLevel::error) << "Brd Driver not created yet";

            return true;
        }
    }
#endif

}

/*
 * Fault
 */
void BoardInitUtil::InitFaults()
{
    ILOG << "";

    InitHstBrdFaults();

    InitMezBrdFaults();
}

void BoardInitUtil::InitHstBrdFaults()
{
    DLOG << "";

    const boardMs::DigitalInputFaultData *pFltInData = nullptr;
    uint32 i = 0;
    uint32 maxFpgaHbFaultElems = boardMs::cNumFpgaHostBoardFaults;
    const bool faultSimEn = (mIsSim || mFaultSimEn);

    ILOG << "Initializing Host Board Faults";

    for(i = 0; i < maxFpgaHbFaultElems; i++)
    {
        pFltInData = &boardMs::cFpgaHostBoardFaults[i];

        if (pFltInData->mIsInitFault == false)
        {
            // Not board init fault, skip it
            continue;
        }

        boardMs::BoardFaultId faultId   = pFltInData->mFaultId;
        uint32                inputMask = pFltInData->mInputMask;
        uint32                inputPol  = pFltInData->mInputPolarity;

        INFN_LOG(SeverityLevel::debug) << "Adding faultId: " << std::setw(3)
                        << std::right << std::setfill(' ')
                        << (uint32)faultId << std::hex
                        << " inputMask: 0x"  << std::setw(5)
                        << std::right << std::setfill('0')
                        << inputMask
                        << " inputPolarity: 0x" << std::setw(5)
                        << std::right << std::setfill('0')
                        << inputPol << std::dec;

        boardMs::faultConditionType condition
            = GetFaultSimCondition(faultId);

        std::shared_ptr<boardMs::BoardFaultFpga> spBoardFault
            = make_shared<boardMs::BoardFaultFpga>(
                          *pFltInData, mspBrdDriver,
                          faultSimEn, condition);

        mBoardInitFaultMap[faultId]
            = static_pointer_cast<boardMs::Chm6BoardFault>(
                 spBoardFault);

        mvHostBoardFaults.push_back(faultId);
    } // END for(i = 0; i < maxFpgaHbFaultElems; i++)

    if (mIsSim)
    {
        return;
    }

    // Get singleton instsance of AccessFaultsMap.
    const auto& afMap = boardMs::AccessFaultsMap::getInstance();

    // Iterate Board Access Faults.
    for( const auto& kv : afMap.af() )
    {
        const auto& afval = kv.second;
        const auto& faultId = afval.boardFaultId;

        DLOG << "Creating Board ACCESS Fault: " << faultId;

        // Create a shared_ptr for the board access fault.
        std::shared_ptr<boardMs::BoardAccessFault> spBoardFault
            = make_shared<boardMs::BoardAccessFault>(afval);

        typedef boardMs::faultConditionType FCType;
        FCType condition = GetFaultSimCondition(faultId);

        // Trigger a fault if setting a fault in sim mode.
        if( faultSimEn && (FCType::FAULT_SET == condition) )
            spBoardFault->SetDevAddr( boardMs::BAD_DEV_ADDR );

        // Assign the board access fault to the map
        // keyed by the faultId.
        mBoardInitFaultMap[faultId] = spBoardFault;

        // Push the faultId into the board faults vector.
        mvHostBoardFaults.push_back(faultId);
    }

#if DBG
    // DEBUGGING
    for( const auto& kv : mBoardInitFaultMap )
        DLOG << "mBoardInitFaultMap "  << kv.first << " : " << kv.second;
    for( const auto& v : mvHostBoardFaults )
        DLOG << "mvHostBoardFaults " << v;
    // END DEBUGGING
#endif
}

void BoardInitUtil::InitMezBrdFaults()
{
    DLOG << "";
  
    const boardMs::DigitalInputFaultData *pFltInData = nullptr;
    boardMs::DigitalInputFaultData digFltData;
    uint32 i = 0;
    uint32 maxFpgaMezzFaultElems = boardMs::cNumMezzIoExpFaults;
    const bool faultSimEn = (mIsSim || mFaultSimEn);

    mvMezzBoardFaults.resize(boardMs::NUM_MEZZ_BRD_TYPES);

    ILOG << "Initializing Mezz Board Faults";

    for(i=0; i < maxFpgaMezzFaultElems; i++)
    {
        pFltInData = &boardMs::cMezzIoExpFaults[i];

        if (pFltInData->mIsInitFault == false)
        {
            // Not board init fault, skip it
            continue;
        }

        memcpy(&digFltData, pFltInData, sizeof(digFltData));

        boardMs::BoardFaultId faultId   = digFltData.mFaultId;
        uint32                inputMask = digFltData.mInputMask;
        uint32                inputPol  = digFltData.mInputPolarity;

        // Top Mezz Board Fault

        INFN_LOG(SeverityLevel::debug)
            << "Adding Top Mezz faultId: " << (uint32)faultId
            << std::hex << std::setw(5) << std::right
            << std::setfill(' ')
            << " inputMask: 0x"     << inputMask
            << " inputPolarity: 0x" << inputPol << std::dec;

        boardMs::faultConditionType condition
          = GetFaultSimCondition(faultId);
        
        std::shared_ptr<boardMs::BoardFaultMezzIoExp> spBoardFault
            = make_shared<boardMs::BoardFaultMezzIoExp>(
                digFltData, mspBrdDriver, boardMs::MEZZ_BRD_TOP,
                faultSimEn, condition);

        mBoardInitFaultMap[faultId]
            = static_pointer_cast<boardMs::Chm6BoardFault>(
                spBoardFault);

        mvMezzBoardFaults[boardMs::MEZZ_BRD_TOP].push_back(faultId);

        // Bottom Mezz Board Fault

        // Add offset to get bottom mezz fault Id
        uint32 uintBtmId
          = static_cast<uint32>(faultId) + boardMs::cMezzFaultIdOffset;
        faultId = (boardMs::BoardFaultId)uintBtmId;
        digFltData.mFaultId = faultId;

        INFN_LOG(SeverityLevel::debug)
            << "Adding Btm Mezz faultId: " << (uint32)faultId
            << std::hex
            << " inputMask: 0x" << inputMask
            << " inputPolarity: 0x" << inputPol << std::dec;

        condition = GetFaultSimCondition(faultId);
        //faultSimEn = (mIsSim || mFaultSimEn);

        spBoardFault = make_shared<boardMs::BoardFaultMezzIoExp>(
                         digFltData, mspBrdDriver,
                         boardMs::MEZZ_BRD_BTM,
                         faultSimEn, condition);

        mBoardInitFaultMap[faultId]
          = static_pointer_cast<boardMs::Chm6BoardFault>(spBoardFault);

        mvMezzBoardFaults[boardMs::MEZZ_BRD_BTM].push_back(faultId);
    } // END for(i=0; i < maxFpgaMezzFaultElems; i++)

    if (mIsSim)
    {
        return;
    }

#if 1
    // Get singleton instsance of AccessFaultsMap.
    const auto& afMap = boardMs::AccessFaultsMap::getInstance();

    // Iterate Board Mezzanine Access Faults - TOP.
    for( const auto& kv : afMap.mezaf_t() )
    {
        const auto& afval = kv.second;
        const auto& faultId = afval.boardFaultId;
        
        DLOG << "Creating Board Mezzanine ACCESS Fault TOP: " << faultId;

        typedef boardMs::faultConditionType FCType;
        FCType condition = GetFaultSimCondition(faultId);

        // Initialize Top Mezzanine
        //
        // Create a shared_ptr for the BoardAccessFault.
        std::shared_ptr<boardMs::BoardAccessFault> spBoardFault
            = make_shared<boardMs::BoardAccessFault>(afval);

        // Trigger a fault if setting a fault in sim mode.
        if( faultSimEn && (FCType::FAULT_SET == condition) )
            spBoardFault->SetDevAddr( boardMs::BAD_DEV_ADDR );

        // Assign the shared_ptr into the map keyed by faultId.
        mBoardInitFaultMap[faultId]= spBoardFault;

        // Push the faultId onto the TOP mezzanine vector.
        mvMezzBoardFaults[boardMs::MEZZ_BRD_TOP].push_back(faultId);
    }

    // Iterate Board Mezzanine Access Faults - BOTTOM.
    for( const auto& kv : afMap.mezaf_b() )
    {
        const auto& afval = kv.second;
        const auto& faultId = afval.boardFaultId;
        
        DLOG << "Creating Board Mezzanine ACCESS Fault BOTTOM: " << faultId;

        typedef boardMs::faultConditionType FCType;
        FCType condition = GetFaultSimCondition(faultId);

        // Initialize Bottom Mezzanine
        //
        // Create another shared_ptr for the BoardAccessFault.
        std::shared_ptr<boardMs::BoardAccessFault> spBoardFault
            = make_shared<boardMs::BoardAccessFault>(afval);

        // Trigger a fault if setting a fault in sim mode.
        if( faultSimEn && (FCType::FAULT_SET == condition) )
            spBoardFault->SetDevAddr( boardMs::BAD_DEV_ADDR );
        
        // Assign the shared_ptr into this map keyed by faultId.
        mBoardInitFaultMap[faultId] = spBoardFault;

        // Push the faultId onto the BOTTOM mezzanine vector.
        mvMezzBoardFaults[boardMs::MEZZ_BRD_BTM].push_back(faultId);
    }

#if DBG
    // DEBUGGING
    for( const auto& kv : mBoardInitFaultMap )
        DLOG << "mBoardInitFaultMap " << kv.first << " : " << kv.second;
    for( const auto& x : mvMezzBoardFaults[boardMs::MEZZ_BRD_TOP] )
        DLOG << "mvMezzBoardFaults TOP : " << x;
    for( const auto& x : mvMezzBoardFaults[boardMs::MEZZ_BRD_BTM] )
        DLOG << "mvMezzBoardFaults BTM: " << x;
    // END DEBUGGING
#endif

#endif
}


void BoardInitUtil::InitFaultSim(std::string faultSimStr)
{
    if (faultSimStr == "")
    {
        // Enabled with all faults cleared
        mFaultSimEn = true;
        return;
    }

    std::stringstream ss(faultSimStr);

    while(ss.good())
    {
        std::string subStr;
        std::getline(ss, subStr, ',');

        try
        {
            uint32 uintFaultId = std::stoi(subStr);

            ILOG << "From Env Sim Fault Enabled for Fault ID: " << uintFaultId;

            mvFaultSim.push_back((boardMs::BoardFaultId)uintFaultId);
        }
        catch(...)
        {
            INFN_LOG(SeverityLevel::error) << "Error parsing environment variable";
            return;
        }
    }

    mFaultSimEn = true;
}

bool BoardInitUtil::CheckAllFaults()
{
    DLOG << "";

    bool retVal;

    retVal = CheckHstBrdFaults();
    retVal = CheckMezBrdFaults(boardMs::MEZZ_BRD_TOP) && retVal;
    retVal = CheckMezBrdFaults(boardMs::MEZZ_BRD_BTM) && retVal;

    return retVal;
}

bool BoardInitUtil::CheckHstBrdFaults()
{
    DLOG << "";

    bool isAllPass = true;

    for(auto itBrdFltId = mvHostBoardFaults.begin();
        itBrdFltId != mvHostBoardFaults.end();
        ++itBrdFltId)
    {
        auto spBrdFaultFpga(
            static_pointer_cast<boardMs::BoardFaultFpga>(
                mBoardInitFaultMap[*itBrdFltId]));

        DLOG << "Checking Fault "
             << static_cast<uint32>(*itBrdFltId);

        spBrdFaultFpga->CheckFaultCondition();

        if (spBrdFaultFpga->GetCondition() == boardMs::FAULT_SET)
        {
            ILOG << "Fault Set for FaultId: "
                           << static_cast<uint32>(*itBrdFltId);

            isAllPass = false;
        }
    }

    if (isAllPass)
    {
        ILOG << " NO Host Board Faults Detected";
    }
    else
    {
        ILOG << " There are Host Board Faults Detected";
    }

    return isAllPass;
}

bool BoardInitUtil::CheckMezBrdFaults(boardMs::mezzBoardIdType boardId)
{
    DLOG << "";
    ILOG << "Mezz Board Id: "
                   << static_cast<uint32>(boardId);

    bool isAllPass = true;

    for( auto itBrdFltId = mvMezzBoardFaults[boardId].begin();
         itBrdFltId != mvMezzBoardFaults[boardId].end();
         ++itBrdFltId)
    {
        auto spBrdFaultMezz(
           static_pointer_cast<boardMs::BoardFaultMezzIoExp>(
               mBoardInitFaultMap[*itBrdFltId]));

        spBrdFaultMezz->CheckFaultCondition();

        if (spBrdFaultMezz->GetCondition() == boardMs::FAULT_SET)
        {
            isAllPass = false;
        }
    }

    if (isAllPass)
    {
        ILOG << "Mezzanine Faults Clear for Fault Id: "
             << static_cast<uint32>(boardId);
    }
    else
    {
        ILOG << "Mezzanine FAULT DETECTED for Fault Id: "
             << static_cast<uint32>(boardId);
    }

    return isAllPass;
}

boardMs::faultConditionType
BoardInitUtil::GetFaultSimCondition( boardMs::BoardFaultId faultId )
{
    boardMs::faultConditionType condition = boardMs::FAULT_CLEAR;
    if (mFaultSimEn)
    {
        std::vector<boardMs::BoardFaultId>::iterator it;
        it = std::find(mvFaultSim.begin(), mvFaultSim.end(), faultId);
        if (it != mvFaultSim.end())
        {
            ILOG << " Fault Sim Fault Set for Fault Id: " << (uint32)faultId;

            condition = boardMs::FAULT_SET;
        }
    }

    return condition;
}

void BoardInitUtil::CheckEnv()
{
    char *pEnvStr;

    if(NULL != (pEnvStr = getenv("BoardInfo")))
    {
        mEnvStr = pEnvStr;
        ILOG << "BoardInfo env val:" << mEnvStr;
    }
    else
    {
        ILOG << "BoardInfo env not present. Assuming HW Platform";

        mEnvStr = "";
    }

    if (mEnvStr == "sim")
    {
        mIsSim = true;
    }
    else if (mEnvStr == "eval")
    {
        mIsEval = true;
    }

    if(NULL != (pEnvStr = getenv("BrdInitFltSim")))
    {
        ILOG << "BrdInitFltSim env val:" << pEnvStr;

        InitFaultSim(pEnvStr);
    }
}

bool BoardInitUtil::IsHwEnv()
{
    if ((mEnvStr == "sim")  ||
        (mEnvStr == "eval") ||
        (mEnvStr == "hb") )
    {
        return false;
    }
    //else
    return true;
}

int BoardInitUtil::InitMezzBoards()
{
    int retVal = 0;

    for(const auto brdId : {boardMs::MEZZ_BRD_TOP, boardMs::MEZZ_BRD_BTM})
    {
        // Init bottom Mezz card
        if((mEnvStr == "") ||
           ((mEnvStr == "bmz") && (brdId == boardMs::MEZZ_BRD_BTM)) ||
           ((mEnvStr == "tmz") && (brdId == boardMs::MEZZ_BRD_TOP)))
        {
            // Init IO Expander
            if (mspBrdDriver->InitIoExp(brdId))
            {
                boardMs::BoardFaultId fid = boardMs::BMZ_IOEXP_ACCESS_FAIL;
                if( brdId == boardMs::MEZZ_BRD_TOP )
                {
                    fid = boardMs::TMZ_IOEXP_ACCESS_FAIL;
                }
                mBoardInitFaultMap[fid]->CheckFaultCondition();

                ELOG << "MZ card: " << (uint32)brdId
                     << " IO Expander Init Failed";

                retVal = -1;
                continue;   // allow other mezz to init
            }

            // Init Si5394 Clock
            if (mspBrdDriver->InitClock(brdId))
            {
                boardMs::BoardFaultId fid = boardMs::BMZ_I2C_CLKGEN_I2C_FAIL;
                if( brdId == boardMs::MEZZ_BRD_TOP )
                {
                    fid = boardMs::TMZ_I2C_CLKGEN_I2C_FAIL;
                }
                mBoardInitFaultMap[fid]->CheckFaultCondition();

                ELOG << "MZ card: " << (uint32)brdId
                     << " Si5394 Clock Init Failed";

                retVal = -3;
                continue;   // allow other mezz to init
            }

            // TODO: Temporary check of ADM1066 until problem is resolved
            mspBrdDriver->CheckMezzPwrSeq(brdId);

            if (CheckMezBrdFaults(brdId) == false)
            {
                ELOG << "Mezz Board: " << static_cast<uint32>(brdId)
                     << " Fault(s) detected. Aborting Init on this Mezz";
                retVal = -2;
                continue;
            }

            // Init Bcm
            if (mspBrdDriver->InitBcm(brdId))
            {
                ELOG << "MZ card: " << static_cast<uint32>(brdId)
                     << " BCM Init Failed";
                retVal = -4;
                continue;   // allow other mezz to init
            }

            ILOG << "MZ card: " << static_cast<uint32>(brdId)
                 << " Init Complete!!";
        }
        else
        {
            ILOG << "Skipping Mezz Card: " << static_cast<uint32>(brdId)
                 << " init for env: " << mEnvStr;
        }
    }

    return retVal;
}

