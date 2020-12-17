/*
 * board_manager.cpp
 *
 *  Created on: Mar 13, 2020
 */

#include <chrono>
#include <thread>
#include <string>
#include <stdlib.h>
#include <ctime>
#include <json/json.h>

#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/util/message_differencer.h>
#include "InfnLogger.h"
#include "chm6/redis_adapter/application_servicer.h"
#include "board_manager.h"
#include "board_manager_util.h"
#include "board_config_handler.h"
#include "dco_state_handler.h"
#include "dco_fault_handler.h"
//#include "dco_pm_handler.h"
#include "tom_presence_map_handler.h"
#include "board_init_state_handler.h"
#include "board_adapter.h"
#include "sim_board_adapter.h"
#include "board_fault_defs.h"

using google::protobuf::util::MessageToJsonString;
using google::protobuf::Message;

extern int DbgCmds(BoardDriver& driver, boardAda::BoardAdapter& adapter, BoardManager& manager);
extern int SimDbgCmds(simBoardAda::SimBoardAdapter& adapter, BoardManager& manager);

extern int global_exit_code;

const std::string BoardManager::cEnvStrVerLc         = "chm6LcVersion";
const std::string BoardManager::cEnvStrVerBaseOs     = "chm6BaseOsVersion";
const std::string BoardManager::cEnvStrVerBaseInfra  = "chm6BaseInfraVersion";

const uint32 cWarmBootDelay = 20;
const uint32 cColdBootDelay = 30;

BoardManager::BoardManager(bool isSim, std::string aid, bool initDone)
    : mAid(aid)
    , mspAdapter(nullptr)
    , mspDriver(nullptr)
    , mupCollector(nullptr)
    , mupBoardConfig(nullptr)
    , mupBoardState(nullptr)
    , mupBoardFault(nullptr)
    , mupBoardPm(nullptr)
    , mFirstState(true)
    , mFirstFault(true)
    , mFirstPm(true)
    , mFirstDcoCardAction(true)
    , mpLog(new SimpleLog::Log(2000))
    , mThrExit(false)
    , mIsSim(isSim)
    , mIsInitDone(initDone)
    , mIsBrdInitSuccess(false)
    , mDcoSyncReady(wrapper::BOOL_FALSE)
    , mAltTempOorh(wrapper::BOOL_FALSE)
    , mAltTempOorl(wrapper::BOOL_FALSE)
{
    std::ostringstream  log;
    log << "Created!";
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();
    log.str("");

    CreateDataCache();

    log << "CreateDataCache() done.";
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();
    log.str("");

    mThrCli = boost::thread(boost::bind(
            &BoardManager::PollInitDone, this
            ));

    mThrCli.detach();

    log << "Spawning Redis Callback Handler Thread.";
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();
    log.str("");

    // work around
    boost::thread(boost::bind(
            &BoardManager::CreateCallbackHandler, this
            )).detach();

    log << "CreateCallbackHandler() done.";
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();
    log.str("");

    InitializeSwVersion();

    if (mIsSim)
    {
        log << "Skip Waiting for BoardInit Ms on Sim Env";
        AddLog(__func__, __LINE__, log);
        INFN_LOG(SeverityLevel::info) << log.str();
        log.str("");

        mIsInitDone = true;
        mIsBrdInitSuccess = true;
        mLastRebootReason = std::string("Warmboot");

        time_t curr_time;
        tm * curr_tm;
        char date_string[100];
        char time_string[100];

        time(&curr_time);
        curr_tm = localtime(&curr_time);

        strftime(time_string, 50, "T%TZ", curr_tm);
        strftime(date_string, 50, "%F", curr_tm);

        std::cout << date_string << std::endl;
        std::cout << time_string << std::endl;

//           Format: "2020-08-19T11:23:03Z"
        mLastRebootTimestamp = std::string(date_string) + std::string(time_string);
        
        log << "SIM mLastRebootTimestamp: " << mLastRebootTimestamp;
        AddLog(__func__, __LINE__, log);
        INFN_LOG(SeverityLevel::info) << log.str();
    }
}

BoardManager::~BoardManager()
{
    INFN_LOG(SeverityLevel::info) << "~BoardManager() destructor ... ";

    mThrExit = true;

    if (mpLog)
    {
        delete mpLog;
    }

    std::ostringstream  log;
    log << " destructor done";
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();
}

// Callback functions to handle board config message
void BoardManager::onCreate(chm6_board::Chm6BoardConfig* boardCfgMsg)
{
    std::ostringstream  log;
    log << ":Chm6BoardConfig  aid: " << boardCfgMsg->mutable_base_config()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();
    log.str("");

    if (mspAdapter == NULL)
    {
        string data;
        MessageToJsonString(*boardCfgMsg, &data);

        log << data << std::endl << "Warning: Not ready to process Chm6BoardConfig";
        AddLog(__func__, __LINE__, log);
        INFN_LOG(SeverityLevel::info) << log.str();

        return;
    }

    HandleBoardConfig(boardCfgMsg);
}

void BoardManager::onModify(chm6_board::Chm6BoardConfig* boardCfgMsg)
{
    std::ostringstream  log;
    log << ":Chm6BoardConfig  aid: " << boardCfgMsg->mutable_base_config()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();
    log.str("");

    if (mspAdapter == NULL)
    {
        string data;
        MessageToJsonString(*boardCfgMsg, &data);

        log << data << std::endl << "Warning: Not ready to process Chm6BoardConfig";
        AddLog(__func__, __LINE__, log);
        INFN_LOG(SeverityLevel::info) << log.str();

        return;
    }

    HandleBoardConfig(boardCfgMsg);
}

void BoardManager::onDelete(chm6_board::Chm6BoardConfig* boardCfgMsg)
{
    string data;
    MessageToJsonString(*boardCfgMsg, &data);

    std::ostringstream  log;
    log << ":Chm6BoardConfig  aid: " << boardCfgMsg->mutable_base_config()->config_id().value()
        << std::endl << data;
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    ResetBoardConfig();
}

void BoardManager::onResync(chm6_board::Chm6BoardConfig* boardCfgMsg)
{
    std::ostringstream  log;
    log << ":Chm6BoardConfig  aid: " << boardCfgMsg->mutable_base_config()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();
    log.str("");

    if (mspAdapter == NULL)
    {
        string data;
        MessageToJsonString(*boardCfgMsg, &data);

        log << data << std::endl << "Warning: Not ready to process Chm6BoardConfig";
        AddLog(__func__, __LINE__, log);
        INFN_LOG(SeverityLevel::info) << log.str();

        return;
    }

    HandleBoardConfig(boardCfgMsg);
}

// Callback functions to handle DCO state message
void BoardManager::onCreate(chm6_common::Chm6DcoCardState* dcoStateMsg)
{
    std::ostringstream  log;
    log << ":Chm6DcoCardState  aid: " << dcoStateMsg->mutable_base_state()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    HandleDcoCardStateChange(dcoStateMsg);
}

void BoardManager::onModify(chm6_common::Chm6DcoCardState* dcoStateMsg)
{
//    string data;
//    MessageToJsonString(*dcoStateMsg, &data);
//
//    std::ostringstream  log;
//    log << ":Chm6DcoCardState  aid: " << dcoStateMsg->mutable_base_state()->config_id().value()
//        << std::endl << data;
//    INFN_LOG(SeverityLevel::info) << log.str();

    HandleDcoCardStateChange(dcoStateMsg);
}

void BoardManager::onDelete(chm6_common::Chm6DcoCardState* dcoStateMsg)
{
    string data;
    MessageToJsonString(*dcoStateMsg, &data);

    std::ostringstream  log;
    log << ":Chm6DcoCardState  aid: " << dcoStateMsg->mutable_base_state()->config_id().value()
        << std::endl << data;
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    mDcoCapabilities.Clear();
    mDcoSyncReady = wrapper::BOOL_FALSE;
}

void BoardManager::onResync(chm6_common::Chm6DcoCardState* dcoStateMsg)
{
    std::ostringstream  log;
    log << ":Chm6DcoCardState  aid: " << dcoStateMsg->mutable_base_state()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    HandleDcoCardStateChange(dcoStateMsg);
}

// Callback functions to handle DCO fault message
void BoardManager::onCreate(chm6_common::Chm6DcoCardFault* dcoFaultMsg)
{
    std::ostringstream  log;
    log << ":Chm6DcoCardFault  aid: " << dcoFaultMsg->mutable_base_fault()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    HandleDcoCardFault(dcoFaultMsg);
}

void BoardManager::onModify(chm6_common::Chm6DcoCardFault* dcoFaultMsg)
{
    std::ostringstream  log;
    log << ":Chm6DcoCardFault  aid: " << dcoFaultMsg->mutable_base_fault()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    HandleDcoCardFault(dcoFaultMsg);
}

void BoardManager::onDelete(chm6_common::Chm6DcoCardFault* dcoFaultMsg)
{
    std::ostringstream  log;
    log << ":Chm6DcoCardFault  aid: " << dcoFaultMsg->mutable_base_fault()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    // DO NOTHING
}

void BoardManager::onResync(chm6_common::Chm6DcoCardFault* dcoFaultMsg)
{
    std::ostringstream  log;
    log << ":Chm6DcoCardFault  aid: " << dcoFaultMsg->mutable_base_fault()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    HandleDcoCardFault(dcoFaultMsg);
}

// Callback functions to handle DCO pm message
void BoardManager::onCreate(chm6_common::Chm6DcoCardPm* dcoPmMsg)
{
    HandleDcoCardPm(dcoPmMsg);
}

void BoardManager::onModify(chm6_common::Chm6DcoCardPm* dcoPmMsg)
{
    HandleDcoCardPm(dcoPmMsg);
}

void BoardManager::onDelete(chm6_common::Chm6DcoCardPm* dcoPmMsg)
{
    string data;
    MessageToJsonString(*dcoPmMsg, &data);

    std::ostringstream  log;
    log << ":Chm6DcoCardPm  aid: " << dcoPmMsg->mutable_base_pm()->config_id().value()
        << std::endl << data;
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    // DO NOTHING
}

void BoardManager::onResync(chm6_common::Chm6DcoCardPm* dcoPmMsg)
{
    HandleDcoCardPm(dcoPmMsg);
}

// Callback functions to handle TOM present map message
void BoardManager::onCreate(chm6_common::Chm6TomPresenceMap* TomPresenceMsg)
{
    std::ostringstream  log;
    log << ":Chm6TomPresenceMap  aid: " << TomPresenceMsg->mutable_base_state()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    onModify(TomPresenceMsg);
}

void BoardManager::onModify(chm6_common::Chm6TomPresenceMap* TomPresenceMsg)
{
    string data;
    MessageToJsonString(*TomPresenceMsg, &data);

    std::ostringstream  log;
    log << ":Chm6TomPresenceMap  aid: " << TomPresenceMsg->mutable_base_state()->config_id().value()
        << std::endl << data;
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    if (TomPresenceMsg->has_tom_presence_map())
    {
        mTomPresenceMap.CopyFrom(*TomPresenceMsg);

        UpdateTomPresenceMap();
    }
}

void BoardManager::onDelete(chm6_common::Chm6TomPresenceMap* TomPresenceMsg)
{
    string data;
    MessageToJsonString(*TomPresenceMsg, &data);

    std::ostringstream  log;
    log << ":Chm6TomPresenceMap  aid: " << TomPresenceMsg->mutable_base_state()->config_id().value()
        << std::endl << data;
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    mTomPresenceMap.Clear();

    UpdateTomPresenceMap();
}

void BoardManager::onResync(chm6_common::Chm6TomPresenceMap* TomPresenceMsg)
{
    std::ostringstream  log;
    log << ":Chm6TomPresenceMap  aid: " << TomPresenceMsg->mutable_base_state()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    onModify(TomPresenceMsg);
}

// Callback functions to handle BoardInit state message
void BoardManager::onCreate(chm6_common::Chm6BoardInitState* pBrdState)
{
    std::ostringstream  log;
    log << ":Chm6BoardInitState  aid: " << pBrdState->mutable_base_state()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    HandleBoardInitStateChange(pBrdState);
}

void BoardManager::onModify(chm6_common::Chm6BoardInitState* pBrdState)
{
    std::ostringstream  log;
    log << ":Chm6BoardInitState  aid: " << pBrdState->mutable_base_state()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    HandleBoardInitStateChange(pBrdState);
}

void BoardManager::onDelete(chm6_common::Chm6BoardInitState* pBrdState)
{
    string data;
    MessageToJsonString(*pBrdState, &data);

    std::ostringstream  log;
    log << ":Chm6BoardInitState  aid: " << pBrdState->mutable_base_state()->config_id().value()
        << std::endl << data;
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    // Do nothing ?
}

void BoardManager::onResync(chm6_common::Chm6BoardInitState* pBrdState)
{
    std::ostringstream  log;
    log << ":Chm6BoardInitState  aid: " << pBrdState->mutable_base_state()->config_id().value();
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    HandleBoardInitStateChange(pBrdState);
}

void BoardManager::onPmStrobe()
{
    INFN_LOG(SeverityLevel::info) << "BoardManager::onPmStrobe() called";

    boost::unique_lock<boost::mutex> lock(mPm_strobe_cond_mutex);
    mPm_strobe_ready = true;

    mPm_strobe_cond.notify_one();

    INFN_LOG(SeverityLevel::info) << "BoardManager::onPmStrobe() done";
}

void BoardManager::CollectFaults()
{
    chm6_board::Chm6BoardFault boardFault;

    if (mIsBrdInitSuccess)
    {
        mspAdapter->CheckFaults();
    }
    else
    {
        INFN_LOG(SeverityLevel::debug) << "Skipping CheckFaults due to Board Init Failure";
    }

    UpdateBoardFaults(boardFault);

    if (mFirstFault == true)
    {
        AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectCreate(*(mupBoardFault.get()));

        std::string fault_data;
        MessageToJsonString(*(mupBoardFault.get()), &fault_data);
        INFN_LOG(SeverityLevel::info) << "First update: " << fault_data;

        mFirstFault = false;
    }
    else
    {
        if (boardFault.has_hal())
        {
            AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectUpdate(*(mupBoardFault.get()));

            std::string fault_data;
            MessageToJsonString(boardFault, &fault_data);
            INFN_LOG(SeverityLevel::info) << fault_data;
        }
    }

    // TODO: remove later
    CollectPm();
}

void BoardManager::CollectPm()
{
    if (!mIsBrdInitSuccess)
    {
        INFN_LOG(SeverityLevel::debug) << "Skipping due to Board Init Failure";
        return;
    }

    mupBoardPm->mutable_hal()->clear_pm();

    // Copy dco pm first
    if (mDcoPm.has_dsp_temperature())
    {
        std::string key("dsp_temperature");

        hal_common::PmType_PmDataType data;

        data.mutable_pm_data_name()->set_value(key);
        data.mutable_float_val()->set_value(mDcoPm.dsp_temperature().value());
        data.set_direction(hal_common::DIRECTION_NA);
        data.set_location(hal_common::LOCATION_NA);

        google::protobuf::MapPair<std::string, hal_common::PmType_PmDataType> dpair(key, data);

        mupBoardPm->mutable_hal()->mutable_pm()->insert(dpair);
    }

    if (mDcoPm.has_pic_temperature())
    {
        std::string key("pic_temperature");

        hal_common::PmType_PmDataType data;

        data.mutable_pm_data_name()->set_value(key);
        data.mutable_float_val()->set_value(mDcoPm.pic_temperature().value());
        data.set_direction(hal_common::DIRECTION_NA);
        data.set_location(hal_common::LOCATION_NA);

        google::protobuf::MapPair<std::string, hal_common::PmType_PmDataType> dpair(key, data);

        mupBoardPm->mutable_hal()->mutable_pm()->insert(dpair);
    }

    if (mDcoPm.has_module_case_temperature())
    {
        std::string key("module_case_temperature");

        hal_common::PmType_PmDataType data;

        data.mutable_pm_data_name()->set_value(key);
        data.mutable_float_val()->set_value(mDcoPm.module_case_temperature().value());
        data.set_direction(hal_common::DIRECTION_NA);
        data.set_location(hal_common::LOCATION_NA);

        google::protobuf::MapPair<std::string, hal_common::PmType_PmDataType> dpair(key, data);

        mupBoardPm->mutable_hal()->mutable_pm()->insert(dpair);
    }

    // Add in board pm
    UpdateBoardPm();

    AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectStream(*(mupBoardPm.get()));

    if (mFirstPm == true)
    {
        std::string pm_data;
        MessageToJsonString(*(mupBoardPm.get()), &pm_data);
        INFN_LOG(SeverityLevel::info) << "First update: " << pm_data;

        mFirstPm = false;
    }
}

void BoardManager::CollectState()
{
    std::lock_guard<std::mutex> guard(mBoardStateLock);

    chm6_board::Chm6BoardState boardState;

    if (mFirstState == true)
    {
        UpdateEqptStates(boardState);

        UpdateEqptInventory();

        UpdateLedStates(boardState);

        UpdateLastBootInfo();

        UpdateUpgradableDevices(boardState);

        UpdateSwVersion();

        INFN_LOG(SeverityLevel::info) << "First update host board state: ";

        SendBoardStateToRedis(boardState);

        mFirstState = false;
    }
    else
    {
        if (mIsBrdInitSuccess)
        {
            UpdateEqptStates(boardState);

            UpdateLedStates(boardState);

            UpdateUpgradableDevices(boardState);

            if (boardState.has_hal())
            {
                SendBoardStateToRedis(boardState);
            }
        }
    }
}

// CLI commands
void BoardManager::DumpLog(std::ostream &os)
{
    os << "<<<<<<<<<<<<<<<<<<< BoardManager.DumpLog >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    std::lock_guard<std::mutex> guard(mLogLock);

    if (mpLog)
    {
        mpLog->DisplayLog( os );
    }
    else
    {
        os << "BoardManager Log is not created!" << std::endl;
    }
}

void BoardManager::DumpStatus(std::ostream &os, std::string cmd)
{
    os << "BoardManager::DumpStatus(): cmd = " << cmd << std::endl;
}

void BoardManager::ResetLog( std::ostream &os )
{
    os << "<<<<<<<<<<<<<<<<<<< BoardManager.ResetLog >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    std::lock_guard<std::mutex> guard(mLogLock);

    if (mpLog)
    {
        mpLog->ResetLog();
        os << "BoardManager Log has been reset!" << std::endl;
    }
}

void BoardManager::DumpBoardConfig(std::ostream& out)
{
    out << "<<<<<<<<<<<<<<<<<<< BoardManager.DumpBoardConfig >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    string data;
    MessageToJsonString(*(mupBoardConfig.get()), &data);

    Json::Value root;

    Json::Reader().parse(data, root);

    out << Json::StyledWriter().write(root) << std::endl;
}

void BoardManager::DumpBoardState(std::ostream& out)
{
    out << "<<<<<<<<<<<<<<<<<<< BoardManager.DumpBoardState >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    string data;
    MessageToJsonString(*(mupBoardState.get()), &data);

    Json::Value root;

    Json::Reader().parse(data, root);

    out << Json::StyledWriter().write(root) << std::endl;
}

void BoardManager::DumpBoardFault(std::ostream& out)
{
    out << "<<<<<<<<<<<<<<<<<<< BoardManager.DumpBoardFault >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    string data;
    MessageToJsonString(*(mupBoardFault.get()), &data);

    Json::Value root;

    Json::Reader().parse(data, root);

    out << Json::StyledWriter().write(root) << std::endl;

    out << "<<<<<<<<<<<<<<<<<<< DcoCardFault >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    string data1;
    MessageToJsonString(mDcoCardFault, &data1);

    Json::Value root1;

    Json::Reader().parse(data1, root1);

    out << Json::StyledWriter().write(root1) << std::endl;
}

void BoardManager::DumpBoardPm(std::ostream& out)
{
    out << "<<<<<<<<<<<<<<<<<<< BoardManager.DumpBoardPm >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    string data;
    MessageToJsonString(*(mupBoardPm.get()), &data);

    Json::Value root;

    Json::Reader().parse(data, root);

    out << Json::StyledWriter().write(root) << std::endl;

    out << "<<<<<<<<<<<<<<<<<<< DcoPm >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    string dco_pm;
    MessageToJsonString(mDcoPm, &dco_pm);

    Json::Value root1;

    Json::Reader().parse(dco_pm, root1);

    out << Json::StyledWriter().write(root1) << std::endl;
}

// Reboot
void BoardManager::SetRestartWarm(std::ostream& out)
{
    out << "<<<<<<<<<<<<<<<<<<< BoardManager.SetRestartWarm >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;
    global_exit_code = boardMs::EXIT_RESTART_WARM;
}

void BoardManager::SetRestartCold(std::ostream& out)
{
    out << "<<<<<<<<<<<<<<<<<<< BoardManager.SetRestartCold >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;
    global_exit_code = boardMs::EXIT_RESTART_COLD;
}

void BoardManager::SetGracefulShutdown(std::ostream& out)
{
    out << "<<<<<<<<<<<<<<<<<<< BoardManager.SetGracefulShutdown >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;
    global_exit_code = boardMs::EXIT_GRACEFUL_SHUTDOWN;
}

void BoardManager::DumpSwVersion(std::ostream& out)
{
    out << "<<<<<<<<<<<<<<<<<<< BoardManager.DumpVersion >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    out << "SwVersion: " << mSwVersion        << std::endl;
    out << "BaseOs   : " << mBaseOsVersion    << std::endl;
    out << "BaseInfra: " << mBaseInfraVersion << std::endl;
}

///////////////////////////////////////////////////////////////////

void BoardManager::CreateDataCache()
{
    /* =========================
     * message Chm6BoardConfig
     * =========================
     */
    mupBoardConfig = std::make_unique<chm6_board::Chm6BoardConfig>();

    mHostBoardAction = hal_common::BOARD_ACTION_UNSPECIFIED;

    /* =========================
     * message Chm6BoardState
     * =========================
     */
    mupBoardState = std::make_unique<chm6_board::Chm6BoardState>();

    /*
     * message Chm6BoardState
     * .infinera.chm6.common.vx.BaseProgState base_state = 1;
     */
    chm6_common::BaseProgState* base_state = mupBoardState->mutable_base_state();

    base_state->mutable_config_id()->set_value(mAid);
    base_state->mutable_timestamp()->set_seconds(time(NULL));
    base_state->mutable_timestamp()->set_nanos(0);
    base_state->mutable_mark_for_delete()->set_value(false);

    /*
     * message Chm6BoardState
     * .infinera.hal.chm6.vx.BoardState.OperationalState hal = 2;
     * .infinera.hal.board.vx.BoardState.OperationalState common_state = 1;
     */
    hal_chm6::BoardState_OperationalState* hal = mupBoardState->mutable_hal();
    hal_board::BoardState_OperationalState* common_state = hal->mutable_common_state();

    // .infinera.hal.common.vx.EqptType type = 1;
    common_state->set_type(hal_common::EQPT_TYPE_CARD);

    // Default values for fan control flags after reboot
    hal->set_fan_increase(wrapper::BOOL_FALSE);
    hal->set_fan_decrease(wrapper::BOOL_FALSE);

    std::string state_data;
    MessageToJsonString(*(mupBoardState.get()), &state_data);
    INFN_LOG(SeverityLevel::info) << state_data;

    /* =========================
     * message Chm6BoardFault
     * =========================
     */
    mupBoardFault = std::make_unique<chm6_board::Chm6BoardFault>();

    /*
     * message Chm6BoardFault
     * .infinera.chm6.common.vx.BaseFault base_fault = 1;
     */
    chm6_common::BaseFault* base_fault = mupBoardFault->mutable_base_fault();
    // .google.protobuf.StringValue config_id = 1;
    base_fault->mutable_config_id()->set_value(mAid);
    // .google.protobuf.Timestamp timestamp = 2;
    base_fault->mutable_timestamp()->set_seconds(time(NULL));
    base_fault->mutable_timestamp()->set_nanos(0);
    // .google.protobuf.BoolValue mark_for_delete = 3;
    base_fault->mutable_mark_for_delete()->set_value(false);

    std::string fault_data;
    MessageToJsonString(*(mupBoardFault.get()), &fault_data);
    INFN_LOG(SeverityLevel::info) << fault_data;

    /* =========================
     * message Chm6BoardPm
     * =========================
     */
    mupBoardPm = std::make_unique<chm6_board::Chm6BoardPm>();

    /*
     * message Chm6BoardPm
     * .infinera.chm6.common.vx.BasePm base_pm = 1;
     */
    chm6_common::BasePm* base_pm = mupBoardPm->mutable_base_pm();

    // .google.protobuf.StringValue config_id = 1;
    base_pm->mutable_config_id()->set_value(mAid);
    // .google.protobuf.Timestamp timestamp = 2;
    base_pm->mutable_timestamp()->set_seconds(time(NULL));
    base_pm->mutable_timestamp()->set_nanos(0);
    // .google.protobuf.BoolValue mark_for_delete = 3;
    base_pm->mutable_mark_for_delete()->set_value(false);

    std::string pm_data;
    MessageToJsonString(*(mupBoardPm.get()), &pm_data);
    INFN_LOG(SeverityLevel::info) << pm_data;
}

void BoardManager::Initialize()
{
    // Initialize Adapter, Driver
    if (mIsSim)
    {
        INFN_LOG(SeverityLevel::info) << "Creating Sim Adapter";

        // Create sim adapter class
        mspAdapter = std::make_shared<simBoardAda::SimBoardAdapter>();

    }
    else
    {
        INFN_LOG(SeverityLevel::info) << "Creating Driver and Adapter";

        // Create drive class and initialize Hardware
        mspDriver = std::make_shared<BoardDriver>();

        // Create adapter class
        mspAdapter = std::make_shared<boardAda::BoardAdapter>((BoardDriver&)*mspDriver);
    }

    ProcessBoardConfigInDb();

    CreateStateCollector();
    INFN_LOG(SeverityLevel::info) << " CreateStateCollector() done.";

    INFN_LOG(SeverityLevel::info) << "----------- start debug cli -----------";
    if (mIsSim)
    {
        SimDbgCmds((simBoardAda::SimBoardAdapter&)(*mspAdapter), *this);
    }
    else
    {
        DbgCmds((BoardDriver&)*mspDriver, (boardAda::BoardAdapter&)*mspAdapter, *this);
    }
}

void BoardManager::CreateCallbackHandler()
{
    INFN_LOG(SeverityLevel::info) << "BoardManager::CreateCallbackHandler()...";

    google::protobuf::Message* boardConfig = new chm6_board::Chm6BoardConfig;
    std::unique_ptr<ICallbackHandler> boardConfigHandler = std::unique_ptr<ICallbackHandler>(new BoardConfigHandler(*this));

    google::protobuf::Message* dcoState = new chm6_common::Chm6DcoCardState;
    std::unique_ptr<ICallbackHandler> dcoStateHandler = std::unique_ptr<ICallbackHandler>(new DcoStateHandler(*this));

    google::protobuf::Message* dcoFault = new chm6_common::Chm6DcoCardFault;
    std::unique_ptr<ICallbackHandler> dcoFaultHandler = std::unique_ptr<ICallbackHandler>(new DcoFaultHandler(*this));

//    google::protobuf::Message* dcoPm = new chm6_common::Chm6DcoCardPm;
//    std::unique_ptr<ICallbackHandler> dcoPmHandler = std::unique_ptr<ICallbackHandler>(new DcoPmHandler(*this));

    google::protobuf::Message* tomPresenceMap = new chm6_common::Chm6TomPresenceMap;
    std::unique_ptr<ICallbackHandler> tomPresenceMapHandler = std::unique_ptr<ICallbackHandler>(new TomPresenceMapHandler(*this));

    google::protobuf::Message* pBrdInitMsg = new chm6_common::Chm6BoardInitState;
    std::unique_ptr<ICallbackHandler> brdInitHandler = std::unique_ptr<ICallbackHandler>(new BoardInitStateHandler(*this));

    AppServicerIntfSingleton::getInstance()->RegisterCallbacks(boardConfig, std::move(boardConfigHandler));

    AppServicerIntfSingleton::getInstance()->RegisterCallbacks(dcoState, std::move(dcoStateHandler));

    AppServicerIntfSingleton::getInstance()->RegisterCallbacks(dcoFault, std::move(dcoFaultHandler));

//    AppServicerIntfSingleton::getInstance()->RegisterCallbacks(dcoPm, std::move(dcoPmHandler));

    AppServicerIntfSingleton::getInstance()->RegisterCallbacks(tomPresenceMap, std::move(tomPresenceMapHandler));

    AppServicerIntfSingleton::getInstance()->RegisterCallbacks(pBrdInitMsg, std::move(brdInitHandler));

    try
    {
        AppServicerIntfSingleton::getInstance()->Run();
    }
    catch (std::exception const &excp)
    {
        INFN_LOG(SeverityLevel::info) << "Exception caught while subscribing!" << excp.what();
        if ( (global_exit_code != boardMs::EXIT_RESTART_WARM)
          && (global_exit_code != boardMs::EXIT_RESTART_COLD) )
        {
            global_exit_code = EXIT_ERROR;
        }

    }
    catch (...)
    {
        INFN_LOG(SeverityLevel::info) << "Exception caught while subscribing! Unknown excpetion";
        if ( (global_exit_code != boardMs::EXIT_RESTART_WARM)
          && (global_exit_code != boardMs::EXIT_RESTART_COLD) )
        {
            global_exit_code = EXIT_ERROR;
        }
    }

    INFN_LOG(SeverityLevel::info) << "BoardManager::CreateCallbackHandler() returned!! ";
    return;
}

void BoardManager::CreateStateCollector()
{
    INFN_LOG(SeverityLevel::info) << "BoardManager::CreateStateCollector()";

    std::string name("BoardStateCollector");

    int stateInterval = 10; // 10 seconds - TBD

    int faultInterval = 1; // 1 seconds - TBD

    mupCollector = std::make_unique<BoardStateCollector>(*this, name, stateInterval,faultInterval, true);

    mupCollector->Collect();
}

void BoardManager::UpdateBoardFaults(chm6_board::Chm6BoardFault& boardFault)
{
    // Get faults from adapter layer
    boardMs::board_fault_ptr_vec& adapterBoardFaults = mspAdapter->GetBoardFault();

    bool boardHasFault = false;

    std::map<std::string, bool> mapFaultSimEn;

    /*
     * message Chm6BoardFault
     * .infinera.hal.common.vx.FaultType hal = 2;
     */
    // map<string, .infinera.hal.common.vx.FaultType.FaultDataType> fault = 1;
    google::protobuf::Map< std::string, hal_common::FaultType_FaultDataType >* boardFaultMap = mupBoardFault->mutable_hal()->mutable_fault();

    for (boardMs::board_fault_vec_itr itr = adapterBoardFaults.begin(); itr != adapterBoardFaults.end(); itr++)
    {
        // fault name as key
        std::string key((*itr).GetName());

        if ((*itr).GetCondition() != boardMs::FAULT_UNKNOWN)
        {
            bool newValue = BoardManagerUtil::MsFaultConditionToProtoFaultCondition((*itr).GetCondition());

            wrapper::Bool newFaultValue = BoardManagerUtil::MsFaultConditionToInfnProtoFaultCondition((*itr).GetCondition());

            if ((*itr).GetSimEnable() == true)
            {
                mapFaultSimEn.insert(std::make_pair(key, true));
            }

            // Compare value with cache
            if ( (((*boardFaultMap)[key].has_value() == false) || ((*boardFaultMap)[key].value().value() != newValue))
              || ((*boardFaultMap)[key].fault_value() != newFaultValue))
            {
                INFN_LOG(SeverityLevel::info) << "Board Fault Condition change detected for fault: " << key  <<
                                  " condition: " << Chm6BoardFault::BoardFltCondiToStr((*itr).GetCondition());

                //Update cache
                // .google.protobuf.StringValue fault_name = 1;
                (*boardFaultMap)[key].mutable_fault_name()->set_value(key);
                // .google.protobuf.BoolValue value = 2;
                (*boardFaultMap)[key].mutable_value()->set_value(newValue);

                (*boardFaultMap)[key].set_direction(hal_common::DIRECTION_NA);
                (*boardFaultMap)[key].set_location(hal_common::LOCATION_NA);
                (*boardFaultMap)[key].set_fault_value(newFaultValue);

                // Save the change
                hal_common::FaultType_FaultDataType fdata;

                fdata.mutable_fault_name()->set_value(key);
                fdata.mutable_value()->set_value(newValue);
                fdata.set_direction(hal_common::DIRECTION_NA);
                fdata.set_location(hal_common::LOCATION_NA);
                fdata.set_fault_value(newFaultValue);

                google::protobuf::MapPair<std::string, hal_common::FaultType_FaultDataType> fpair(key, fdata);
                boardFault.mutable_hal()->mutable_fault()->insert(fpair);
            }

            // Update Chm6BoardState
            boardHasFault = boardHasFault || newValue;
        }
    }

    // Add in Chm6DcoCardFault
    if (mDcoCardFault.has_hal())
    {
        const google::protobuf::Map< std::string, hal_common::FaultType_FaultDataType >& dcoFaultMap = mDcoCardFault.mutable_hal()->fault();

        for (auto iter = dcoFaultMap.cbegin(); iter != dcoFaultMap.cend(); ++iter)
        {
            if (iter->second.has_fault_name() && iter->second.has_value())
            {
                std::string key = iter->first;

                if ( ((*boardFaultMap)[key].has_value() == false) || ((*boardFaultMap)[key].value().value() != iter->second.value().value())
                  || ((*boardFaultMap)[key].fault_value() != iter->second.fault_value()))
                {
                    //Update cache
                    // .google.protobuf.StringValue fault_name = 1;
                    (*boardFaultMap)[key].mutable_fault_name()->set_value(iter->second.fault_name().value());
                    // .google.protobuf.BoolValue value = 2;
                    (*boardFaultMap)[key].mutable_value()->set_value(iter->second.value().value());

                    (*boardFaultMap)[key].set_direction(iter->second.direction());
                    (*boardFaultMap)[key].set_location(iter->second.location());

                    (*boardFaultMap)[key].set_fault_value(iter->second.fault_value());

                    // Save the change
                    hal_common::FaultType_FaultDataType fdata;

                    fdata.mutable_fault_name()->set_value(iter->second.fault_name().value());
                    fdata.mutable_value()->set_value(iter->second.value().value());
                    fdata.set_direction(iter->second.direction());
                    fdata.set_location(iter->second.location());
                    fdata.set_fault_value(iter->second.fault_value());

                    google::protobuf::MapPair<std::string, hal_common::FaultType_FaultDataType> fpair(key, fdata);
                    boardFault.mutable_hal()->mutable_fault()->insert(fpair);
                }

                // Update Chm6BoardState
                boardHasFault = boardHasFault || iter->second.value().value();
            }
        }
    }

    // Board Init Faults
    if (mBoardInitFault.fault_size() != 0)
    {
        const ::google::protobuf::Map<std::string, hal_common::FaultType_FaultDataType>&
            mapInitFlt = mBoardInitFault.fault();

        for(auto &it : mapInitFlt)
        {
            std::string key(it.first);

            if ( (((*boardFaultMap)[key].has_value() == false) || ((*boardFaultMap)[key].value().value() != it.second.value().value()))
              || ((*boardFaultMap)[key].fault_value() != it.second.fault_value()) )
            {
                // check for fault sim override before processing board init fault
                std::map<std::string, bool>::iterator itFaultSim = mapFaultSimEn.find(key);
                if (itFaultSim != mapFaultSimEn.end())
                {
                    if (itFaultSim->second == true)
                    {
                        INFN_LOG(SeverityLevel::debug) << "Board Fault Sim Enable Overriding Board Init Fault State ";

                        continue;
                    }
                }

                // todo: demote this print
                INFN_LOG(SeverityLevel::info) << "BoardInit Fault detected for fault: " << key;

                //Update cache
                // .google.protobuf.StringValue fault_name = 1;
                (*boardFaultMap)[key].mutable_fault_name()->set_value(it.second.fault_name().value());
                // .google.protobuf.BoolValue value = 2;
                (*boardFaultMap)[key].mutable_value()->set_value(it.second.value().value());

                (*boardFaultMap)[key].set_direction(hal_common::DIRECTION_NA);
                (*boardFaultMap)[key].set_location(hal_common::LOCATION_NA);

                (*boardFaultMap)[key].set_fault_value(it.second.fault_value());

                // Save the change
                hal_common::FaultType_FaultDataType fdata;

                fdata.mutable_fault_name()->set_value(it.second.fault_name().value());
                fdata.mutable_value()->set_value(it.second.value().value());
                fdata.set_direction(hal_common::DIRECTION_NA);
                fdata.set_location(hal_common::LOCATION_NA);
                fdata.set_fault_value(it.second.fault_value());

                google::protobuf::MapPair<std::string, hal_common::FaultType_FaultDataType> fpair(key, fdata);
                boardFault.mutable_hal()->mutable_fault()->insert(fpair);
            }

            // Update Chm6BoardState
            boardHasFault = boardHasFault || it.second.value().value();
        }
    }

    /*
     * message Chm6BoardState
     * .infinera.hal.chm6.vx.BoardState.OperationalState hal = 2;
     * .infinera.hal.board.vx.BoardState.OperationalState common_state = 1;
     */
    hal_board::BoardState_OperationalState* common_state = mupBoardState->mutable_hal()->mutable_common_state();

    // .google.protobuf.BoolValue has_fault = 9;
    bool currentHasFault = common_state->mutable_has_fault()->value();

    wrapper::Bool boardHasFaultVal;
    if (boardHasFault == false)
    {
        boardHasFaultVal = wrapper::BOOL_FALSE;
    }
    else
    {
        boardHasFaultVal = wrapper::BOOL_TRUE;
    }

    wrapper::Bool currentBoardHasFaultVal = common_state->board_has_fault();

    if ((boardHasFault != currentHasFault) || (boardHasFaultVal != currentBoardHasFaultVal))
    {
        common_state->mutable_has_fault()->set_value(boardHasFault);

        common_state->set_board_has_fault(boardHasFaultVal);
    }

    /*
     * message Chm6BoardFault
     * .infinera.chm6.common.vx.BaseFault base_fault = 1;
     */
    chm6_common::BaseFault* base_fault = mupBoardFault->mutable_base_fault();
    // .google.protobuf.Timestamp timestamp = 2;
    base_fault->mutable_timestamp()->set_seconds(time(NULL));
    base_fault->mutable_timestamp()->set_nanos(0);

    boardFault.mutable_base_fault()->CopyFrom(*base_fault);
}

void BoardManager::UpdateBoardPm()
{
    if (mspAdapter != nullptr)
    {
        // Get pm from adapter
        boardMs::board_pm_ptr_vec& adapterBoardPm = mspAdapter->GetBoardPm();

        /*
         * message Chm6BoardPm
         * .infinera.hal.common.vx.PmType hal = 2;
         */
        // map<string, .infinera.hal.common.vx.PmType.PmDataType> pm = 3;
        google::protobuf::Map< std::string, hal_common::PmType_PmDataType >* cacheMap = mupBoardPm->mutable_hal()->mutable_pm();

        for (boardMs::board_pm_vec_itr itr = adapterBoardPm.begin(); itr != adapterBoardPm.end(); itr++)
        {
            std::string key((*itr).mName);
            float64 value = (*itr).mValue;

            // Update cache
            hal_common::PmType_PmDataType data;

            // .google.protobuf.StringValue pm_data_name = 1;
            data.mutable_pm_data_name()->set_value(key);
            // .google.protobuf.FloatValue float_val = 7;
            data.mutable_float_val()->set_value(value);

            data.set_direction(hal_common::DIRECTION_NA);
            data.set_location(hal_common::LOCATION_NA);

            google::protobuf::MapPair<std::string, hal_common::PmType_PmDataType> dpair(key, data);

            cacheMap->insert(dpair);
        }
    }

    /*
     * message Chm6BoardPm
     * .infinera.chm6.common.vx.BasePm base_pm = 1;
     */
    chm6_common::BasePm* base_pm = mupBoardPm->mutable_base_pm();
    // .google.protobuf.Timestamp timestamp = 2;
    base_pm->mutable_timestamp()->set_seconds(time(NULL));
    base_pm->mutable_timestamp()->set_nanos(0);
}

void BoardManager::UpdateEqptStates(chm6_board::Chm6BoardState& boardState)
{
    // Get eqptState from adapter
    boardMs::Chm6EqptState state = mspAdapter->GetEqptState();

    hal_common::EqptState newState = BoardManagerUtil::MsEqptStateToProtoEqptState(state);

    /*
     * message Chm6BoardState
     * .infinera.hal.chm6.vx.BoardState.OperationalState hal = 2;
     * .infinera.hal.board.vx.BoardState.OperationalState common_state = 1;
     */
    hal_chm6::BoardState_OperationalState* hal = mupBoardState->mutable_hal();
    hal_board::BoardState_OperationalState* common_state = hal->mutable_common_state();
    // .infinera.hal.common.vx.EqptState state = 2;
    hal_common::EqptState CurrentState = common_state->state();

    if (CurrentState != newState)
    {
        // Update cache data
        common_state->set_state(newState);

        // Save the change
        boardState.mutable_hal()->mutable_common_state()->set_state(newState);
    }
}

void BoardManager::UpdateEqptInventory()
{
    boardMs::Chm6EqptInventory& adaInv = mspAdapter->GetEqptInventory();

    /*
     * message Chm6BoardState
     * .infinera.hal.chm6.vx.BoardState.OperationalState hal = 2;
     * .infinera.hal.board.vx.BoardState.OperationalState common_state = 1;
     */
    hal_chm6::BoardState_OperationalState* hal = mupBoardState->mutable_hal();
    hal_board::BoardState_OperationalState* common_state = hal->mutable_common_state();

    // .infinera.hal.common.vx.EqptInventoryType hw_info = 3;
    common_state->mutable_hw_info()->mutable_serial_number()->set_value(adaInv.SerialNumber);
    common_state->mutable_hw_info()->mutable_part_number()->set_value(adaInv.PartNumber);
    common_state->mutable_hw_info()->mutable_vendor_name()->set_value(adaInv.VendorName);
    common_state->mutable_hw_info()->mutable_hardware_revision()->set_value(adaInv.HardwareRevision);
    common_state->mutable_hw_info()->mutable_manufacture_date()->set_value(adaInv.ManufactureDate);
    common_state->mutable_hw_info()->mutable_firmware_version()->set_value(adaInv.FirmwareVersion);
    common_state->mutable_hw_info()->mutable_clei_code()->set_value(adaInv.CleiCode);
    common_state->mutable_hw_info()->mutable_pon()->set_value(adaInv.Pon);
    common_state->mutable_hw_info()->mutable_insertion_date()->set_value(adaInv.InsertionDate);
}

void BoardManager::UpdateLedStates(chm6_board::Chm6BoardState& boardState)
{
    /*
     * message Chm6BoardState
     * .infinera.hal.chm6.vx.BoardState.OperationalState hal = 2;
     * .infinera.hal.board.vx.BoardState.OperationalState common_state = 1;
     */
    hal_chm6::BoardState_OperationalState* hal = mupBoardState->mutable_hal();
    hal_board::BoardState_OperationalState* common_state = hal->mutable_common_state();

    // .infinera.hal.common.vx.LedState power_led_state = 4;
    boardMs::LedStateType powerLedState = mspAdapter->GetPowerLedState();

    hal_common::LedState newPowerLedState   = BoardManagerUtil::MsLedStateToProtoLedState(powerLedState);

    if (common_state->power_led_state() != newPowerLedState)
    {
        // Update cache
        common_state->set_power_led_state(newPowerLedState);

        // Save the change
        boardState.mutable_hal()->mutable_common_state()->set_power_led_state(newPowerLedState);
    }

    mspAdapter->CheckLedLampLocTestState();

    // .infinera.hal.common.vx.LedState fault_led_state = 5;
    boardMs::LedStateType faultLedState = mspAdapter->GetFaultLedState();

    hal_common::LedState newFaultLedState   = BoardManagerUtil::MsLedStateToProtoLedState(faultLedState);

    if (common_state->fault_led_state() != newFaultLedState)
    {
        // Update cache
        common_state->set_fault_led_state(newFaultLedState);

        // Save the change
        boardState.mutable_hal()->mutable_common_state()->set_fault_led_state(newFaultLedState);
    }

    // .infinera.hal.common.vx.LedState active_led_state = 6;
    boardMs::LedStateType activeLedState = mspAdapter->GetActiveLedState();

    hal_common::LedState newActiveLedState   = BoardManagerUtil::MsLedStateToProtoLedState(activeLedState);

    if (common_state->active_led_state() != newActiveLedState)
    {
        // Update cache
        common_state->set_active_led_state(newActiveLedState);

        // Save the change
        boardState.mutable_hal()->mutable_common_state()->set_active_led_state(newActiveLedState);
    }

    // map<uint32, .infinera.hal.common.v2.PortLed> port_led_states = 16;
    google::protobuf::Map< google::protobuf::uint32, hal_common::PortLed >* cachePortLedStates = common_state->mutable_port_led_states();

    boardMs::port_led_ptr_vec& adaPortLedStates = mspAdapter->GetPortLedStates();

    for (boardMs::port_led_vec_itr itr = adaPortLedStates.begin(); itr != adaPortLedStates.end(); itr++)
    {
        uint32 key = (uint32)(*itr).mPortId;

        hal_common::LedState newPortActiveLedState = BoardManagerUtil::MsLedStateToProtoLedState((*itr).mActiveLedState);
        hal_common::LedState newPortLosLedState    = BoardManagerUtil::MsLedStateToProtoLedState((*itr).mLosLedState);

        if ( ((*cachePortLedStates)[key].port_active_led() != newPortActiveLedState)
          || ((*cachePortLedStates)[key].port_los_led() != newPortLosLedState) )
        {
            hal_common::PortId portId = BoardManagerUtil::MsPortIdToProtoPortId((*itr).mPortId);

            // Update cache
            // .infinera.hal.common.vx.PortId port_id = 1;
            (*cachePortLedStates)[key].set_port_id(portId);
            // .infinera.hal.common.vx.LedState port_active_led = 2;
            (*cachePortLedStates)[key].set_port_active_led(newPortActiveLedState);
            // .infinera.hal.common.vx.LedState port_los_led = 3;
            (*cachePortLedStates)[key].set_port_los_led(newPortLosLedState);

            // Save changes
            hal_common::PortLed data;
            data.set_port_id(portId);
            data.set_port_active_led(newPortActiveLedState);
            data.set_port_los_led(newPortLosLedState);

            google::protobuf::MapPair<google::protobuf::uint32, hal_common::PortLed> portLed(key, data);
            boardState.mutable_hal()->mutable_common_state()->mutable_port_led_states()->insert(portLed);
        }
    }

    // map<uint32, .infinera.hal.common.v2.LineLed> line_led_states = 17;
    google::protobuf::Map< google::protobuf::uint32, hal_common::LineLed >* cacheLineLedStates = common_state->mutable_line_led_states();

    boardMs::line_led_ptr_vec& adaLineLedStates = mspAdapter->GetLineLedStates();

    for (line_led_vec_itr itr = adaLineLedStates.begin(); itr != adaLineLedStates.end(); itr++)
    {
        uint32 key = (*itr).mLineId;

        hal_common::LedState newLineActiveLedState = BoardManagerUtil::MsLedStateToProtoLedState((*itr).mActiveLedState);
        hal_common::LedState newLineLosLedState    = BoardManagerUtil::MsLedStateToProtoLedState((*itr).mLosLedState);

        if ( ((*cacheLineLedStates)[key].line_active_led() != newLineActiveLedState)
          || ((*cacheLineLedStates)[key].line_los_led() != newLineLosLedState) )
        {
            hal_common::LineId lineId = BoardManagerUtil::MsLineIdToProtoLineId((*itr).mLineId);

            // Update cache
            // .infinera.hal.common.vx.LineId line_id = 1;
            (*cacheLineLedStates)[key].set_line_id(lineId);
            // .infinera.hal.common.vx.LedState line_active_led = 2;
            (*cacheLineLedStates)[key].set_line_active_led(newLineActiveLedState);
            // .infinera.hal.common.vx.LedState line_los_led = 3;
            (*cacheLineLedStates)[key].set_line_los_led(newLineLosLedState);

            // Save change
            hal_common::LineLed data;

            data.set_line_id(lineId);
            data.set_line_active_led(newLineActiveLedState);
            data.set_line_los_led(newLineLosLedState);

            google::protobuf::MapPair<google::protobuf::uint32, hal_common::LineLed> lineLed(key, data);
            boardState.mutable_hal()->mutable_common_state()->mutable_line_led_states()->insert(lineLed);
        }
    }
}

void BoardManager::UpdateLastBootInfo()
{
    /*
     * message Chm6BoardState
     * .infinera.hal.chm6.vx.BoardState.OperationalState hal = 2;
     * .infinera.hal.board.vx.BoardState.OperationalState common_state = 1;
     */
    hal_chm6::BoardState_OperationalState* hal = mupBoardState->mutable_hal();
    hal_board::BoardState_OperationalState* common_state = hal->mutable_common_state();

    // .google.protobuf.StringValue last_boot_reason = 7;
    common_state->mutable_last_boot_reason()->set_value(mLastRebootReason);
    // .google.protobuf.StringValue last_boot_timestamp = 8;
    common_state->mutable_last_boot_timestamp()->set_value(mLastRebootTimestamp);
}

void BoardManager::UpdateUpgradableDevices(chm6_board::Chm6BoardState& boardState)
{
    // Get status from adapter
    boardMs::upgradable_device_ptr_vec & adapterUpgradableDevices = mspAdapter->GetUpgradableDevices();

    /*
     *  message Chm6BoardState
     * .infinera.hal.chm6.vx.BoardState.OperationalState hal = 2;
     * .infinera.hal.board.vx.BoardState.OperationalState common_state = 1;
     * .infinera.hal.common.vx.UpgradableDeviceType upgradable_devices = 10;
     */
    // .map<string, .infinera.hal.common.vx.UpgradableDeviceType.UpgradableDevice> upgradable_devices = 1;
    google::protobuf::Map< std::string, hal_common::UpgradableDeviceType_UpgradableDevice >* CacheDeviceMap
      = mupBoardState->mutable_hal()->mutable_common_state()->mutable_upgradable_devices()->mutable_upgradable_devices();

    for(int i = 0; i < adapterUpgradableDevices.size(); i++)
    {

        std::string key = adapterUpgradableDevices[i].mDeviceName;

        std::string newHwVersion = adapterUpgradableDevices[i].mHwVersion;
        std::string newSwVersion = adapterUpgradableDevices[i].mSwVersion;
        std::string newFwUpdateState = adapterUpgradableDevices[i].mFwUpdateState;
        std::string newFileLocaiton  = adapterUpgradableDevices[i].mFileLocation;
        hal_common::FwUpdateStatus newFwUpdateStatus = hal_common::FW_UPDATE_STATUS_CURRENT;

        if ( ((*CacheDeviceMap)[key].mutable_hw_version()->value() !=  newHwVersion)
          || ((*CacheDeviceMap)[key].mutable_sw_version()->value() !=  newSwVersion)
          || ((*CacheDeviceMap)[key].mutable_fw_update_state()->value() !=  newFwUpdateState)
          || ((*CacheDeviceMap)[key].mutable_file_location()->value() !=  newFileLocaiton)
          || ((*CacheDeviceMap)[key].fw_update_status() !=  newFwUpdateStatus) )
        {
            // Update cache
            (*CacheDeviceMap)[key].mutable_device_name()->set_value(key);
            (*CacheDeviceMap)[key].mutable_hw_version()->set_value(newHwVersion);
            (*CacheDeviceMap)[key].mutable_sw_version()->set_value(newSwVersion);
            (*CacheDeviceMap)[key].mutable_fw_update_state()->set_value(newFwUpdateState);
            (*CacheDeviceMap)[key].mutable_file_location()->set_value(newFileLocaiton);
            (*CacheDeviceMap)[key].set_fw_update_status(newFwUpdateStatus);

            // Save change
            hal_common::UpgradableDeviceType_UpgradableDevice device;

            device.mutable_device_name()->set_value(key);
            device.mutable_hw_version()->set_value(newHwVersion);
            device.mutable_sw_version()->set_value(newSwVersion);
            device.mutable_fw_update_state()->set_value(newFwUpdateState);
            device.mutable_file_location()->set_value(newFileLocaiton);
            device.set_fw_update_status(newFwUpdateStatus);

            google::protobuf::MapPair<std::string, hal_common::UpgradableDeviceType_UpgradableDevice> devPair(key, device);
            boardState.mutable_hal()->mutable_common_state()->mutable_upgradable_devices()->mutable_upgradable_devices()->insert(devPair);
        }
    }

    // Add in DCO upgradable devices
    if (mDcoUpgradableDevices.upgradable_devices_size() != 0)
    {
        auto dcoUpgradableDves = mDcoUpgradableDevices.upgradable_devices();
        for (auto iter = dcoUpgradableDves.cbegin(); iter != dcoUpgradableDves.cend(); ++iter)
        {
            std::string key = iter->first;

            // data from DCO
            std::string newDevName = iter->second.device_name().value();
            std::string newHwVersion = iter->second.hw_version().value();
            std::string newSwVersion = iter->second.sw_version().value();
            std::string newFwUpdateState = iter->second.fw_update_state().value();
            std::string newFileLocaiton  = iter->second.file_location().value();
            hal_common::FwUpdateStatus newFwUpdateStatus = iter->second.fw_update_status();

            if ( ((*CacheDeviceMap)[key].mutable_device_name()->value() !=  newDevName)
              || ((*CacheDeviceMap)[key].mutable_hw_version()->value() !=  newHwVersion)
              || ((*CacheDeviceMap)[key].mutable_sw_version()->value() !=  newSwVersion)
              || ((*CacheDeviceMap)[key].mutable_fw_update_state()->value() !=  newFwUpdateState)
              || ((*CacheDeviceMap)[key].mutable_file_location()->value() !=  newFileLocaiton)
              || ((*CacheDeviceMap)[key].fw_update_status() !=  newFwUpdateStatus) )
            {
                // Update cache
                (*CacheDeviceMap)[key].mutable_device_name()->set_value(newDevName);
                (*CacheDeviceMap)[key].mutable_hw_version()->set_value(newHwVersion);
                (*CacheDeviceMap)[key].mutable_sw_version()->set_value(newSwVersion);
                (*CacheDeviceMap)[key].mutable_fw_update_state()->set_value(newFwUpdateState);
                (*CacheDeviceMap)[key].mutable_file_location()->set_value(newFileLocaiton);
                (*CacheDeviceMap)[key].set_fw_update_status(newFwUpdateStatus);

                // Save change
                hal_common::UpgradableDeviceType_UpgradableDevice device;

                device.mutable_device_name()->set_value(key);
                device.mutable_hw_version()->set_value(newHwVersion);
                device.mutable_sw_version()->set_value(newSwVersion);
                device.mutable_fw_update_state()->set_value(newFwUpdateState);
                device.mutable_file_location()->set_value(newFileLocaiton);
                device.set_fw_update_status(newFwUpdateStatus);

                google::protobuf::MapPair<std::string, hal_common::UpgradableDeviceType_UpgradableDevice> devPair(key, device);
                boardState.mutable_hal()->mutable_common_state()->mutable_upgradable_devices()->mutable_upgradable_devices()->insert(devPair);
            }
        }
    }
}

void BoardManager::UpdateDcoSyncReady()
{
    std::lock_guard<std::mutex> guard(mBoardStateLock);

    chm6_board::Chm6BoardState boardState;

    if (mupBoardState->mutable_hal()->mutable_common_state()->sync_ready() != mDcoSyncReady)
    {
        // Update cache
        mupBoardState->mutable_hal()->mutable_common_state()->set_sync_ready(mDcoSyncReady);

        // Save change
        boardState.mutable_hal()->mutable_common_state()->set_sync_ready(mDcoSyncReady);

        SendBoardStateToRedis(boardState);
    }
}

void BoardManager::UpdateTomPresenceMap()
{
    std::lock_guard<std::mutex> guard(mBoardStateLock);

    chm6_board::Chm6BoardState boardState;
    /*
     * message Chm6BoardState
     * .infinera.hal.chm6.vx.BoardState.OperationalState hal = 2;
     * .infinera.hal.board.vx.BoardState.OperationalState common_state = 1;
     */
    // .google.protobuf.UInt32Value tom_presence_map = 11;
    if (mTomPresenceMap.has_tom_presence_map())
    {
        if (mupBoardState->mutable_hal()->mutable_common_state()->has_tom_presence_map())
        {
            if (mupBoardState->mutable_hal()->mutable_common_state()->tom_presence_map().value() != mTomPresenceMap.tom_presence_map().value())
            {
                // Update cache
                mupBoardState->mutable_hal()->mutable_common_state()->mutable_tom_presence_map()->set_value(mTomPresenceMap.mutable_tom_presence_map()->value());

                // Save change
                boardState.mutable_hal()->mutable_common_state()->mutable_tom_presence_map()->set_value(mTomPresenceMap.mutable_tom_presence_map()->value());
            }
        }
        else
        {
            // Update cache
            mupBoardState->mutable_hal()->mutable_common_state()->mutable_tom_presence_map()->set_value(mTomPresenceMap.mutable_tom_presence_map()->value());

            // Save change
            boardState.mutable_hal()->mutable_common_state()->mutable_tom_presence_map()->set_value(mTomPresenceMap.mutable_tom_presence_map()->value());
        }
    }
    else
    {
        mupBoardState->mutable_hal()->mutable_common_state()->release_tom_presence_map();
    }

    SendBoardStateToRedis(boardState);
}

void BoardManager::UpdateDcoCapabilities()
{
    std::lock_guard<std::mutex> guard(mBoardStateLock);

    chm6_board::Chm6BoardState boardState;

    /*
     * message Chm6BoardState
     * .infinera.hal.chm6.vx.BoardState.OperationalState hal = 2;
     * .infinera.hal.chm6.vx.DcoCapabilities dco_capabilities = 2;
     */
    hal_chm6::DcoCapabilities* dco_capabilities = mupBoardState->mutable_hal()->mutable_dco_capabilities();

    if (!google::protobuf::util::MessageDifferencer::Equals((*dco_capabilities), mDcoCapabilities))
    {
        dco_capabilities->CopyFrom(mDcoCapabilities);

        boardState.mutable_hal()->mutable_dco_capabilities()->CopyFrom(mDcoCapabilities);

        SendBoardStateToRedis(boardState);
    }
}

void BoardManager::UpdateSwVersion()
{
    hal_chm6::BoardState_OperationalState* hal = mupBoardState->mutable_hal();
    hal_board::BoardState_OperationalState* common_state = hal->mutable_common_state();

    common_state->mutable_software_version()->set_value(mSwVersion);
}

void BoardManager::UpdateFanControlFlags()
{
    std::lock_guard<std::mutex> guard(mBoardStateLock);

    chm6_board::Chm6BoardState boardState;

    /*
     * message Chm6BoardState
     * .infinera.hal.chm6.vx.BoardState.OperationalState hal = 2;
     * .infinera.wrappers.vx.Bool fan_increase = 3;
     * .infinera.wrappers.v1.Bool fan_decrease = 4;
     */
    hal_chm6::BoardState_OperationalState* hal = mupBoardState->mutable_hal();

    if (hal->fan_increase() != mAltTempOorh)
    {
        hal->set_fan_increase(mAltTempOorh);

        boardState.mutable_hal()->set_fan_increase(mAltTempOorh);
    }

    if (hal->fan_decrease() != mAltTempOorl)
    {
        hal->set_fan_decrease(mAltTempOorl);

        boardState.mutable_hal()->set_fan_decrease(mAltTempOorl);
    }

    if (boardState.has_hal())
    {
        SendBoardStateToRedis(boardState);
    }
}

int  BoardManager::HandleBoardConfig(chm6_board::Chm6BoardConfig* boardCfgMsg)
{
    std::lock_guard<std::mutex> guard(mBoardConfigLock);

    string data;
    MessageToJsonString(*boardCfgMsg, &data);

    std::ostringstream  log;
    log << data;
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    if (boardCfgMsg->has_hal())
    {
        const hal_chm6::BoardConfig_Config& hal = boardCfgMsg->hal();
        if (hal.has_common_config())
        {
            const hal_board::BoardConfig_Config& newConfig = hal.common_config();

            // .infinera.hal.common.vx.BoardAction host_card_action = 1;
            if (newConfig.host_card_action() != hal_common::BOARD_ACTION_UNSPECIFIED)
            {
                HandleHostCardAction(newConfig.host_card_action());
            }

            // .infinera.hal.common.vx.LedState fault_led = 2;
            if (newConfig.fault_led() != hal_common::LED_STATE_UNSPECIFIED)
            {
                HandleFaultLed(newConfig.fault_led());
            }

            // .infinera.hal.common.vx.LedState active_led = 3;
            if (newConfig.active_led() != hal_common::LED_STATE_UNSPECIFIED)
            {
                HandleActiveLed(newConfig.active_led());
            }

            // .google.protobuf.BoolValue led_location_test = 8;
            if ((newConfig.has_led_location_test())
             || (newConfig.do_led_location_test() != wrapper::BOOL_UNSPECIFIED))
            {
                HandleLedLocationTest(newConfig.led_location_test().value());
            }

            // map<uint32, .infinera.hal.common.vx.PortLed> port_leds = 9;
            const google::protobuf::Map< google::protobuf::uint32, hal_common::PortLed >& new_port_leds = newConfig.port_leds();

            if (newConfig.port_leds_size() != 0)
            {
                HandlePortLeds(new_port_leds);
            }

            // map<uint32, .infinera.hal.common.vx.LineLed> line_leds = 10;
            const google::protobuf::Map< google::protobuf::uint32, hal_common::LineLed >& new_line_leds = newConfig.line_leds();

            if (newConfig.line_leds_size() != 0)
            {
                HandleLineLeds(new_line_leds);
            }
        }

        // DCO card configurations
        chm6_common::Chm6DcoConfig dco_config;

        if (hal.dco_card_action() != hal_common::BOARD_ACTION_UNSPECIFIED)
        {
            dco_config.set_dco_card_action(hal.dco_card_action());
        }

//        if (hal.has_icdp_node_info())
//        {
//            dco_config.mutable_icdp_node_info()->CopyFrom(hal.icdp_node_info());
//        }

        if ( (dco_config.dco_card_action() != hal_common::BOARD_ACTION_UNSPECIFIED)
           /*|| (dco_config.has_icdp_node_info())*/ )
        {
            RelayDcoCardConfigToDpMs(dco_config);
        }

        if (hal.dco_card_action() != hal_common::BOARD_ACTION_UNSPECIFIED)
        {
            HandleDcoCardAction(hal.dco_card_action());
        }
    }

    mupBoardConfig->Clear();
    mupBoardConfig->CopyFrom(*boardCfgMsg);

    return 0;
}

int BoardManager::HandleHostCardAction(hal_common::BoardAction hostCardAction)
{
    std::ostringstream  log;
    log << " hostCardAction: " << hal_common::BoardAction_Name(hostCardAction);
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    mHostBoardAction = hostCardAction;

    if (mHostBoardAction != hal_common::BOARD_ACTION_UNSPECIFIED)
    {
        boardMs::hostBoardActionType act = BoardManagerUtil::ProtoBoardActionToMsBoardAction(hostCardAction);
        mspAdapter->SetHostBoardAction(act);

        if (mHostBoardAction == hal_common::BoardAction::BOARD_ACTION_RESTART_WARM)
        {
            log << " Start host RESTART_WARM delay: " << cWarmBootDelay;
            AddLog(__func__, __LINE__, log);
            INFN_LOG(SeverityLevel::info) << log.str();
            boost::this_thread::sleep(boost::posix_time::seconds(cWarmBootDelay));

            global_exit_code = boardMs::EXIT_RESTART_WARM;
        }
        else if (mHostBoardAction == hal_common::BoardAction::BOARD_ACTION_RESTART_COLD)
        {
            chm6_common::Chm6DcoConfig dco_config;
            dco_config.set_dco_card_action(hal_common::BoardAction::BOARD_ACTION_GRACEFUL_SHUTDOWN);

            RelayDcoCardConfigToDpMs(dco_config);

            log << " Start host RESTART_COLD delay: " << cColdBootDelay;
            AddLog(__func__, __LINE__, log);
            INFN_LOG(SeverityLevel::info) << log.str();
            boost::this_thread::sleep(boost::posix_time::seconds(cColdBootDelay));


            global_exit_code = boardMs::EXIT_RESTART_COLD;
        }
        else if (mHostBoardAction == hal_common::BoardAction::BOARD_ACTION_GRACEFUL_SHUTDOWN)
        {
            chm6_common::Chm6DcoConfig dco_config;
            dco_config.set_dco_card_action(hal_common::BoardAction::BOARD_ACTION_GRACEFUL_SHUTDOWN);

            RelayDcoCardConfigToDpMs(dco_config);

            log << " Start host GRACEFUL_SHUTDOWN delay: " << cColdBootDelay;
            AddLog(__func__, __LINE__, log);
            INFN_LOG(SeverityLevel::info) << log.str();
            boost::this_thread::sleep(boost::posix_time::seconds(cColdBootDelay));

            global_exit_code = boardMs::EXIT_GRACEFUL_SHUTDOWN;
        }
    }
    return 0;
}

int BoardManager::HandleDcoCardAction(hal_common::BoardAction dcoCardAction)
{
    std::ostringstream  log;
    log << "  dcoCardAction: " << hal_common::BoardAction_Name(dcoCardAction);
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    if (dcoCardAction == hal_common::BoardAction::BOARD_ACTION_RESTART_COLD)
    {
        log << " Start DCO RESTART_COLD delay: " << boardMs::cColdRestartDcoDelaySec;
        AddLog(__func__, __LINE__, log);
        INFN_LOG(SeverityLevel::info) << log.str();

        // Set DCO card to shutdown
        mspAdapter->SetColdRestartDcoDelay(boardMs::cColdRestartDcoDelaySec);
    }

    return 0;
}

int BoardManager::HandleFaultLed(hal_common::LedState state)
{
    std::ostringstream  log;
    log << "Set  FruFaultLed: " << state;
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    boardMs::LedStateType adaState = BoardManagerUtil::ProtoLedStateToMsLedState(state);

    mspAdapter->SetFaultLedState(adaState);

    return 0;
}

int BoardManager::HandleActiveLed(hal_common::LedState state)
{
    std::ostringstream  log;
    log << "Set  FruActiveLed: " << state;
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    boardMs::LedStateType adaState = BoardManagerUtil::ProtoLedStateToMsLedState(state);

    mspAdapter->SetActiveLedState(adaState);

    return 0;
}

int BoardManager::HandleLedLocationTest(bool doTest)
{
    std::ostringstream  log;
    log << "Do  LedLocationTest: " << doTest;
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    mspAdapter->DoLedLocationTest(doTest);

    return 0;
}

int BoardManager::HandlePortLeds(const google::protobuf::Map< google::protobuf::uint32, hal_common::PortLed >& new_port_leds)
{
    std::ostringstream  log;

    for(auto iter = new_port_leds.cbegin(); iter != new_port_leds.cend(); iter++)
    {
        const hal_common::PortLed& portLed = (*iter).second;

        boardMs::QSFPPortId portId = BoardManagerUtil::ProtoPortIdToMsPortId(portLed.port_id());

        log << std::endl << "Proto PortId = " << hal_common::PortId_Name(portLed.port_id())
            << " ;boardMS::QSFPPortId = " << boardMs::QSFPPortIdToStr(portId);
        AddLog(__func__, __LINE__, log);
        INFN_LOG(SeverityLevel::info) << log.str();
        log.str("");

        if (portId != boardMs::QSFP_PORT_ID_INVALID)
        {
            if (portLed.port_active_led() != hal_common::LED_STATE_UNSPECIFIED)
            {
                boardMs::LedStateType active_led = BoardManagerUtil::ProtoLedStateToMsLedState(portLed.port_active_led());

                mspAdapter->SetPortLedState(portId, boardMs::QSFP_ACTIVE, active_led);

                log << "Set Active LED for: " << hal_common::PortId_Name(portLed.port_id())
                    << " to: " <<  hal_common::LedState_Name(portLed.port_active_led());
                AddLog(__func__, __LINE__, log);
                INFN_LOG(SeverityLevel::info) << log.str();
                log.str("");
            }

            if (portLed.port_los_led() != hal_common::LED_STATE_UNSPECIFIED)
            {
                boardMs::LedStateType los_led = BoardManagerUtil::ProtoLedStateToMsLedState(portLed.port_los_led());

                mspAdapter->SetPortLedState(portId, boardMs::QSFP_LOS, los_led);

                log << "Set Los LED for port: " << hal_common::PortId_Name(portLed.port_id())
                    << " to: " <<  hal_common::LedState_Name(portLed.port_los_led());
                AddLog(__func__, __LINE__, log);
                INFN_LOG(SeverityLevel::info) << log.str();
                log.str("");
            }
        }
    }

    return 0;
}

int BoardManager::HandleLineLeds(const google::protobuf::Map< google::protobuf::uint32, hal_common::LineLed >& new_line_leds)
{
    std::ostringstream  log;

    for(auto iter = new_line_leds.cbegin(); iter != new_line_leds.cend(); iter++)
    {
        const hal_common::LineLed& lineLed = (*iter).second;

        boardMs::LineId lineId = BoardManagerUtil::ProtoLineIdToMsLineId(lineLed.line_id());

        log << std::endl << "Proto LineId = " << hal_common::LineId_Name(lineLed.line_id())
            << " ;boardMS::LineId = " << boardMs::LineIdToStr(lineId);
        AddLog(__func__, __LINE__, log);
        INFN_LOG(SeverityLevel::info) << log.str();
        log.str("");

        if (lineId != boardMs::LINE_ID_INVALID)
        {
            if (lineLed.line_active_led() != hal_common::LED_STATE_UNSPECIFIED)
            {
                boardMs::LedStateType active_led = BoardManagerUtil::ProtoLedStateToMsLedState(lineLed.line_active_led());

                mspAdapter->SetLineLedState(lineId, boardMs::LINE_ACTIVE, active_led);

                log << "Set Active LED for: " << hal_common::LineId_Name(lineLed.line_id())
                    << " to: " <<  hal_common::LedState_Name(lineLed.line_active_led());
                AddLog(__func__, __LINE__, log);
                INFN_LOG(SeverityLevel::info) << log.str();
                log.str("");
            }

            if (lineLed.line_los_led() != hal_common::LED_STATE_UNSPECIFIED)
            {
                boardMs::LedStateType los_led = BoardManagerUtil::ProtoLedStateToMsLedState(lineLed.line_los_led());

                mspAdapter->SetLineLedState(lineId, boardMs::LINE_LOS, los_led);

                log << "Set Los LED for line: " << hal_common::LineId_Name(lineLed.line_id())
                    << " to: " <<  hal_common::LedState_Name(lineLed.line_los_led());
                AddLog(__func__, __LINE__, log);
                INFN_LOG(SeverityLevel::info) << log.str();
                log.str("");
            }
        }
    }

    return 0;
}

void BoardManager::HandleBoardInitStateChange(chm6_common::Chm6BoardInitState* pBrdState)
{
    string stateData;
    MessageToJsonString(*pBrdState, &stateData);
    INFN_LOG(SeverityLevel::info) << stateData;
    std::ostringstream  log;
    log << stateData;
    AddLog(__func__, __LINE__, log);
    log.str("");

    if (pBrdState->init_state() == chm6_common::STATE_COMPLETE)
    {
        INFN_LOG(SeverityLevel::info) << " Detected Init Complete";

        if (pBrdState->init_status() == chm6_common::STATUS_SUCCESS)
        {
            INFN_LOG(SeverityLevel::info) << "Received init_status SUCCESS";

            mIsBrdInitSuccess = true;
        }
        else if (pBrdState->init_status() == chm6_common::STATUS_FAIL)
        {
            INFN_LOG(SeverityLevel::info) << "Received init_status FAIL";

            mIsBrdInitSuccess = false;
        }

        UpdateBoardInitFaults(pBrdState);

        if (pBrdState->boot_reason() == chm6_common::BOOT_REASON_COLD)
        {
            mLastRebootReason = std::string("Coldboot");
        }
        else if (pBrdState->boot_reason() == chm6_common::BOOT_REASON_WARM)
        {
            mLastRebootReason = std::string("Warmboot");
        }

        if (mIsInitDone == false)
        {
            time_t curr_time;
            tm * curr_tm;
            char date_string[100];
            char time_string[100];

            time(&curr_time);
            curr_tm = localtime(&curr_time);

            strftime(time_string, 50, "T%TZ", curr_tm);
            strftime(date_string, 50, "%F", curr_tm);

            std::cout << date_string << std::endl;
            std::cout << time_string << std::endl;

//           Format: "2020-08-19T11:23:03Z"
            mLastRebootTimestamp = std::string(date_string) + std::string(time_string);

            log << "HW mLastRebootReason: " << mLastRebootReason
                << " mLastRebootTimestamp: " << mLastRebootTimestamp;
             AddLog(__func__, __LINE__, log);
             INFN_LOG(SeverityLevel::info) << log.str();

            mIsInitDone = true;
        }
    }
}

void BoardManager::HandleDcoCardStateChange(chm6_common::Chm6DcoCardState* dcoStateMsg)
{
    if (dcoStateMsg->has_dco_capabilities())
    {
        if (!google::protobuf::util::MessageDifferencer::Equals(dcoStateMsg->dco_capabilities(), mDcoCapabilities))
        {
            string data;
            MessageToJsonString(dcoStateMsg->dco_capabilities(), &data);

            std::ostringstream  log;
            log << data;
            AddLog(__func__, __LINE__, log);
            INFN_LOG(SeverityLevel::info) << log.str();
        }

        mDcoCapabilities.MergeFrom(dcoStateMsg->dco_capabilities());

        UpdateDcoCapabilities();
    }

    if (dcoStateMsg->has_dco_zynq_version())
    {
        std::ostringstream  log;
        log << "dco_zynq_version = " << dcoStateMsg->dco_zynq_version().value();
        AddLog(__func__, __LINE__, log);
        INFN_LOG(SeverityLevel::info) << log.str();
    }

    if (dcoStateMsg->has_pm())
    {
        mDcoPm.MergeFrom(dcoStateMsg->pm());
    }

    if (dcoStateMsg->has_upgradable_devices())
    {
        mDcoUpgradableDevices.MergeFrom(dcoStateMsg->upgradable_devices());
    }

    if (wrapper::BOOL_UNSPECIFIED != dcoStateMsg->sync_ready())
    {
        if (mDcoSyncReady != dcoStateMsg->sync_ready())
        {
            std::ostringstream  log;
            log << "mDcoSyncReady = " << mDcoSyncReady << " change to " << dcoStateMsg->sync_ready();
            AddLog(__func__, __LINE__, log);
            INFN_LOG(SeverityLevel::info) << log.str();
        }

        mDcoSyncReady = dcoStateMsg->sync_ready();

        UpdateDcoSyncReady();
    }

    if ( ((dcoStateMsg->alt_temp_oorh() != wrapper::BOOL_UNSPECIFIED) && (dcoStateMsg->alt_temp_oorh() != mAltTempOorh))
      || ((dcoStateMsg->alt_temp_oorl() != wrapper::BOOL_UNSPECIFIED) && (dcoStateMsg->alt_temp_oorl() != mAltTempOorl)) )
    {
        mAltTempOorh = dcoStateMsg->alt_temp_oorh();
        mAltTempOorl = dcoStateMsg->alt_temp_oorl();

        UpdateFanControlFlags();
    }
}

void BoardManager::HandleDcoCardFault(chm6_common::Chm6DcoCardFault* dcoFaultMsg)
{
    if (dcoFaultMsg->has_hal())
    {
        mDcoCardFault.MergeFrom(*dcoFaultMsg);

        string data;
        MessageToJsonString(*dcoFaultMsg, &data);

        std::ostringstream  log;
        log << data;
        AddLog(__func__, __LINE__, log);
        INFN_LOG(SeverityLevel::info) << log.str();
    }
}

void BoardManager::HandleDcoCardPm(chm6_common::Chm6DcoCardPm* dcoPmMsg)
{

    mDcoCardPm.CopyFrom(*dcoPmMsg);

    mupBoardPm->clear_hal();

    // Copy dco pm first
    mupBoardPm->mutable_hal()->CopyFrom(dcoPmMsg->dco_pm());

    // Add in board pm
    UpdateBoardPm();

    AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectStream(*(mupBoardPm.get()));

    if (mFirstPm == true)
    {
        std::string pm_data;
        MessageToJsonString(*(mupBoardPm.get()), &pm_data);
        INFN_LOG(SeverityLevel::info) << "First update: " << pm_data;

        mFirstPm = false;
    }
}

void BoardManager::UpdateBoardInitFaults(chm6_common::Chm6BoardInitState* pBrdState)
{
    INFN_LOG(SeverityLevel::info) << "";

    mBoardInitFault.CopyFrom(pBrdState->init_fault());
}

void BoardManager::ResetBoardConfig()
{
    mupBoardConfig->Clear();
    mHostBoardAction = hal_common::BOARD_ACTION_UNSPECIFIED;

    mFirstDcoCardAction = true;
}

void BoardManager::AddLog(const std::string &func, uint32 line, std::ostringstream &text)
{
    std::lock_guard<std::mutex> guard(mLogLock);

    ostringstream  os;
    os << "BoardManager::" << func << ":" << line << ": " << text.str();

    if (mpLog)
    {
        mpLog->AddRecord( os.str().c_str() );
    }
}

void BoardManager::PollInitDone()
{
    INFN_LOG(SeverityLevel::info) << " Polling for init completion...";

    while (!mThrExit)
    {
        if (mIsInitDone)
        {
            // Complete Initialization
            INFN_LOG(SeverityLevel::info) << " Completing Initialization...";

            Initialize();   // Creates CLI and does not return currently. todo: change this

            return; // just in case we reach here
        }

        sleep(1);
    }
}

void BoardManager::InitializeSwVersion()
{
    INFN_LOG(SeverityLevel::info) << "";

    char *pEnvStr;
    std::string strLcVer;
    if (NULL == (pEnvStr = std::getenv(cEnvStrVerLc.c_str())))
    {
        INFN_LOG(SeverityLevel::info) << "Env Variable " << cEnvStrVerLc << " Not Present";

        strLcVer = "0";
    }
    else
    {
        strLcVer = pEnvStr;

        INFN_LOG(SeverityLevel::info) << "Env Variable " << cEnvStrVerLc << " = " << strLcVer;
    }

    std::string strBaseOs;
    if (NULL == (pEnvStr = std::getenv(cEnvStrVerBaseOs.c_str())))
    {
        INFN_LOG(SeverityLevel::info) << "Env Variable " << cEnvStrVerBaseOs << " Not Present";

        strBaseOs = "0";
    }
    else
    {
        strBaseOs = pEnvStr;

        INFN_LOG(SeverityLevel::info) << "Env Variable " << cEnvStrVerBaseOs << " = " << strBaseOs;
    }

    std::string strBaseInfra;
    if (NULL == (pEnvStr = std::getenv(cEnvStrVerBaseInfra.c_str())))
    {
        INFN_LOG(SeverityLevel::info) << "Env Variable " << cEnvStrVerBaseInfra << " Not Present";

        strBaseInfra = "0";
    }
    else
    {
        strBaseInfra = pEnvStr;

        INFN_LOG(SeverityLevel::info) << "Env Variable " << cEnvStrVerBaseInfra << " = " << strBaseInfra;
    }

    mSwVersion        = strLcVer;
    mBaseOsVersion    = strBaseOs;
    mBaseInfraVersion = strBaseInfra;

    INFN_LOG(SeverityLevel::info) << "***** SW VERSION: "  << mSwVersion <<
                             " BaseOs: "    << mBaseOsVersion <<
                             " BaseInfra: " << mBaseInfraVersion;
}

int BoardManager::ProcessBoardConfigInDb()
{
    // Process BoardConfig from redis DB
    INFN_LOG(SeverityLevel::info) << "Read and process BoardConfig from redis DB...";

    chm6_board::Chm6BoardConfig configData;

    configData.mutable_base_config()->mutable_config_id()->set_value(mAid);
    configData.mutable_base_config()->mutable_timestamp()->set_seconds(time(NULL));
    configData.mutable_base_config()->mutable_timestamp()->set_nanos(0);

    try
    {
        AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectRead(configData);
    }
    catch(std::exception const &excp)
    {
        INFN_LOG(SeverityLevel::info) << "Exception caught in ProcessBoardConfigInDb(): " << excp.what()
                       << " - Redis DB may NOT have Chm6BoardConfig message.";
        return 0;
    }
    catch(...)
    {
        INFN_LOG(SeverityLevel::info) << "Redis DB may NOT have Chm6BoardConfig message.";

        return 0;
    }

    INFN_LOG(SeverityLevel::info) << "Read BoardConfig from redis DB done.";

    int ret = HandleBoardConfig(&configData);

    INFN_LOG(SeverityLevel::info) << "Process BoardConfig from redis DB done.";

    return ret;
}

void BoardManager::SendBoardStateToRedis(chm6_board::Chm6BoardState& boardState)
{
    chm6_common::BaseProgState* base_state = mupBoardState->mutable_base_state();
    base_state->mutable_timestamp()->set_seconds(time(NULL));
    base_state->mutable_timestamp()->set_nanos(0);

    boardState.mutable_base_state()->CopyFrom(*base_state);

    AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectUpdate(*(mupBoardState.get()));

    std::string state_data;
    MessageToJsonString(boardState, &state_data);
    INFN_LOG(SeverityLevel::info) << state_data;
}

void BoardManager::RelayDcoCardConfigToDpMs(chm6_common::Chm6DcoConfig& dco_config)
{

    std::string config_data;
    MessageToJsonString(dco_config, &config_data);

    std::ostringstream  log;
    log << "  DCO config: " << config_data;
    AddLog(__func__, __LINE__, log);
    INFN_LOG(SeverityLevel::info) << log.str();

    // .infinera.chm6.common.vx.BaseState base_state = 1;
    dco_config.mutable_base_state()->mutable_config_id()->set_value("Chm6Internal");
    dco_config.mutable_base_state()->mutable_timestamp()->set_seconds(time(NULL));
    dco_config.mutable_base_state()->mutable_timestamp()->set_nanos(0);
    dco_config.mutable_base_state()->mutable_mark_for_delete()->set_value(false);

    if (mFirstDcoCardAction)
    {
        AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectCreate(dco_config);
        mFirstDcoCardAction = false;
    }

    else
    {
        AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectUpdate(dco_config);
    }
}
