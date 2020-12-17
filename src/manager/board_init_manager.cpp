/*
 * board_init_manager.cpp
 *
 */

#include "board_init_manager.h"
#include "board_init_util.h"
#include "InfnLogger.h"
#include "chm6/redis_adapter/application_servicer.h"
#include "infinera/chm6/common/v2/board_init_state.pb.h"

using chm6_common::Chm6BoardInitState;

BoardInitManager::BoardInitManager()
    : mBootReason(chm6_common::BOOT_REASON_UNSPECIFIED)
    , mResetCauseBits(0)
    , mPrevInitStateDone(false)
{
    mpState = new chm6_common::Chm6BoardInitState();
    mpBoardUtil = new BoardInitUtil();
}

int BoardInitManager::initialize()
{
    INFN_LOG(SeverityLevel::info) << " Starting connection to NorthBound";

    // NorthBound Connection Bring Up
    initNbConnection(); // todo - should this be done first? Requires Redis up

    mpBoardUtil->InitFaults();

    bool isOk = true;

    if (initHw() == false)
    {
        INFN_LOG(SeverityLevel::info) << " Init HW Failed";
        // not returning failure in this case
        //  raise alarm and declare init failure

        isOk = false;
    }

    updateInitState(isOk);

    return 0;
}

bool BoardInitManager::initHw()
{
    INFN_LOG(SeverityLevel::info) << "";

    getResetCause();

    if (isForceColdBootEnvSet())
    {
        INFN_LOG(SeverityLevel::info) << "ENV VARIABLE SET. FORCING Cold boot init and declaring boot reason cold";

        mBootReason = chm6_common::BOOT_REASON_COLD;
    }

    int errCode = 0;
    if (mBootReason == chm6_common::BOOT_REASON_COLD)
    {
        INFN_LOG(SeverityLevel::info) << "Performing Cold Boot Init";

        errCode = mpBoardUtil->ColdInit();
    }
    else
    {
        INFN_LOG(SeverityLevel::info) << "Performing Warm Boot Init";

        if (!mPrevInitStateDone)
        {
            errCode = mpBoardUtil->WarmInit();
        }
    }

    if (errCode != 0)
    {
        INFN_LOG(SeverityLevel::error) << "Failure detected. errCode: " << errCode;
        return false;
    }

    clrResetCause();

    return true;
}

void BoardInitManager::initNbConnection()
{
    if (false ==
        AppServicerIntfSingleton::getInstance()->ServicerInit("CHM6_BoardInit"))
    {
        INFN_LOG(SeverityLevel::error) << "AppServicerIntfSingleton ServicerInit Failure (failed to connect to Redis). Exiting with Error...";

        exit(1);
    }

    readState();
    createState();
}

void BoardInitManager::createState()
{
    INFN_LOG(SeverityLevel::info) << " Setting InitState IN PROGRESS";

    mpState->mutable_base_state()->mutable_config_id()->set_value("1");
    mpState->set_init_state(chm6_common::STATE_INPROGRESS);

    AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectCreate(*mpState);

    INFN_LOG(SeverityLevel::info) << "BoardInitState Redis Object Creation Success!!";
}

void BoardInitManager::readState()
{
    INFN_LOG(SeverityLevel::info) << " Reading State from Redis";

    chm6_common::Chm6BoardInitState brdInitState;

    brdInitState.mutable_base_state()->mutable_config_id()->set_value("1");
    brdInitState.mutable_base_state()->mutable_timestamp()->set_seconds(time(NULL));
    brdInitState.mutable_base_state()->mutable_timestamp()->set_nanos(0);

    try
    {
        AppServicerIntfSingleton::getInstance()
          ->getRedisInstance()->RedisObjectRead(brdInitState);
    }
    catch(std::exception const &ex)
    {
        // Exception expected if state object is not present in Redis

        INFN_LOG(SeverityLevel::debug) << "Exception caught: " << ex.what();
        return;
    }
    catch(...)
    {
        INFN_LOG(SeverityLevel::debug) << "Exception caught";

        return;
    }

    INFN_LOG(SeverityLevel::info) << " Read State from Redis";

    if (brdInitState.init_state() == chm6_common::STATE_COMPLETE)
    {
        INFN_LOG(SeverityLevel::info) << "Detected Init State was Previously Completed";

        mPrevInitStateDone = true;
    }
}

void BoardInitManager::updateInitState(bool isOk)
{
    chm6_common::Status initStat;
    if (isOk)
    {
        INFN_LOG(SeverityLevel::info) << " Setting InitState Success!!";

        initStat = chm6_common::STATUS_SUCCESS;
    }
    else
    {
        INFN_LOG(SeverityLevel::info) << " Setting InitState Failure!!";

        initStat = chm6_common::STATUS_FAIL;

        // Capture which fault(s) caused failure
        updateFaults();

        // todo: Add Init Failure Fault once defined
    }

    mpState->set_init_state(chm6_common::STATE_COMPLETE);
    mpState->set_init_status(initStat);

    INFN_LOG(SeverityLevel::info) << "Updating Redis";

    mpState->set_boot_reason(mBootReason);
    // todo: add ResetCause to proto and update it here

    // todo: low power mode value

    AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectUpdate(*mpState);
}

void BoardInitManager::updateFaults()
{
    const BoardInitUtil::BoardFaultMapType& driverFaultMap = mpBoardUtil->GetFaults();

    google::protobuf::Map< std::string, hal_common::FaultType_FaultDataType >* pStateFaultMap = mpState->mutable_init_fault()->mutable_fault();

    for(auto &it : driverFaultMap)
    {

        boardMs::faultConditionType fltCondition = it.second->GetCondition();

        // For BoardInit, only adding fault if fault is set
        if (fltCondition == boardMs::FAULT_SET)
        {
            std::string fltName(it.second->GetName());

            bool newValue = MsFaultConditionToProtoFaultCondition(fltCondition);
            wrapper::Bool newBoolVal = MsFaultToProtoFault(fltCondition);
 
            std::string faultKey(fltName);
            // Key
            //  todo do we need key = Name-<direction>-<location>
            //faultKey += "-NA-NA";

            INFN_LOG(SeverityLevel::info) << "Updating Fault for Key: " << faultKey << " condition: " << newValue;

            hal_common::FaultType_FaultDataType faultData;
            faultData.mutable_fault_name()->set_value(fltName);
            faultData.mutable_value()->set_value(newValue);
            faultData.set_fault_value(newBoolVal);
 
            google::protobuf::MapPair<std::string, hal_common::FaultType_FaultDataType> faultPair(faultKey, faultData);
            pStateFaultMap->insert(faultPair);
        }
    }
}

void BoardInitManager::getResetCause()
{
    INFN_LOG(SeverityLevel::info) << "";

    mResetCauseBits = mpBoardUtil->GetResetCause();

    INFN_LOG(SeverityLevel::info) << " Reset Cause Bits: 0x" << std::hex << mResetCauseBits;

    std::string strBootReason;
    if ((mResetCauseBits & RST_CAUSE_GECKO_POR) != 0)
    {
        mBootReason = chm6_common::BOOT_REASON_COLD;
        strBootReason = "BOOT_REASON_COLD";
    }
    else
    {
        mBootReason = chm6_common::BOOT_REASON_WARM;
        strBootReason = "BOOT_REASON_WARM";
    }

    INFN_LOG(SeverityLevel::info) << "Reboot Reason Enum: " << strBootReason;
}

void BoardInitManager::clrResetCause()
{
    INFN_LOG(SeverityLevel::info) << "";

    if ( ! mpBoardUtil->ClrResetCause() )
    {
        INFN_LOG(SeverityLevel::error) << " Failed to Clear Reset Cause!!!";

        // todo: what should we do here?

    }
}

bool BoardInitManager::isForceColdBootEnvSet()
{
    char *pEnvStr;
    std::string envStr;

    bool retVal = false;

    if(NULL != (pEnvStr = getenv("ForceColdInit")))
    {
        envStr = pEnvStr;
        if (envStr == "true")
        {
            INFN_LOG(SeverityLevel::info) << "Env Variable ForceColdInit = true";
            retVal = true;
        }
        else
        {
            INFN_LOG(SeverityLevel::info) << "Env Variable ForceColdInit != true";
        }
    }
    else
    {
        INFN_LOG(SeverityLevel::debug) << "Env Variable ForceColdInit not present";
    }

    return retVal;
}

bool BoardInitManager::MsFaultConditionToProtoFaultCondition(boardMs::faultConditionType inCondition)
{
        bool fltConditon = false;

        switch (inCondition)
        {
            case boardMs::FAULT_SET:
                fltConditon = true;
                break;

            case boardMs::FAULT_CLEAR:
                fltConditon = false;
                break;

            case boardMs::FAULT_UNKNOWN:
            default:
                fltConditon = false;
                break;
        }

        return fltConditon;
}

wrapper::Bool BoardInitManager::MsFaultToProtoFault(boardMs::faultConditionType msFaultCondition)
{
    wrapper::Bool fltConditon = wrapper::BOOL_UNSPECIFIED;

    switch (msFaultCondition)
    {
        case boardMs::FAULT_SET:
            fltConditon = wrapper::BOOL_TRUE;
            break;

        case boardMs::FAULT_CLEAR:
            fltConditon = wrapper::BOOL_FALSE;
            break;

        case boardMs::FAULT_UNKNOWN:
        default:
            fltConditon = wrapper::BOOL_UNSPECIFIED;
            break;
    }

    return fltConditon;
}


