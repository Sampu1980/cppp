/*
 * board_manager.h
 *
 */

#ifndef CHM6_BOARD_MS_SRC_MANAGER_BOARD_MANAGER_H_
#define CHM6_BOARD_MS_SRC_MANAGER_BOARD_MANAGER_H_

#include <boost/ptr_container/ptr_vector.hpp>
#include <mutex>

#include "board_proto_defs.h"
#include "board_adapter.h"
#include "board_driver.h"
#include "board_state_collector.h"
#include "board_defs.h"
#include "SimpleLog.h"

class BoardManager : public BoardStateCollectWorker
{
public:

    BoardManager(bool isSim, std::string aid, bool initDone);

    ~BoardManager();

    // Callback functions to handle board config message
    void onCreate(chm6_board::Chm6BoardConfig* boardCfgMsg);

    void onModify(chm6_board::Chm6BoardConfig* boardCfgMsg);

    void onDelete(chm6_board::Chm6BoardConfig* boardCfgMsg);

    void onResync(chm6_board::Chm6BoardConfig* boardCfgMsg);

    // Callback functions to handle DCO state message
    void onCreate(chm6_common::Chm6DcoCardState* dcoStateMsg);

    void onModify(chm6_common::Chm6DcoCardState* dcoStateMsg);

    void onDelete(chm6_common::Chm6DcoCardState* dcoStateMsg);

    void onResync(chm6_common::Chm6DcoCardState* dcoStateMsg);

    // Callback functions to handle DCO fault message
    void onCreate(chm6_common::Chm6DcoCardFault* dcoFaultMsg);

    void onModify(chm6_common::Chm6DcoCardFault* dcoFaultMsg);

    void onDelete(chm6_common::Chm6DcoCardFault* dcoFaultMsg);

    void onResync(chm6_common::Chm6DcoCardFault* dcoFaultMsg);

    // Callback functions to handle DCO pm message
    void onCreate(chm6_common::Chm6DcoCardPm* dcoPmMsg);

    void onModify(chm6_common::Chm6DcoCardPm* dcoPmMsg);

    void onDelete(chm6_common::Chm6DcoCardPm* dcoPmMsg);

    void onResync(chm6_common::Chm6DcoCardPm* dcoPmMsg);

    // Callback functions to handle TOM present map message
    void onCreate(chm6_common::Chm6TomPresenceMap* TomPresenceMsg);

    void onModify(chm6_common::Chm6TomPresenceMap* TomPresenceMsg);

    void onDelete(chm6_common::Chm6TomPresenceMap* TomPresenceMsg);

    void onResync(chm6_common::Chm6TomPresenceMap* TomPresenceMsg);

    // Callback functions to handle Board Init State message
    void onCreate(chm6_common::Chm6BoardInitState* pBrdState);

    void onModify(chm6_common::Chm6BoardInitState* pBrdState);

    void onDelete(chm6_common::Chm6BoardInitState* pBrdState);

    void onResync(chm6_common::Chm6BoardInitState* pBrdState);

    // Called whenever pm strobe is ready
    void onPmStrobe();

    // BoardStateCollectWorker APIs
    void CollectFaults();

    void CollectPm();

    void CollectState();

    // CLI commands
    void DumpLog(std::ostream &os);

    void DumpStatus(std::ostream &os, std::string cmd);

    void ResetLog( std::ostream &os );

    void DumpBoardConfig(std::ostream& out);

    void DumpBoardState(std::ostream& out);

    void DumpBoardFault(std::ostream& out);

    void DumpBoardPm(std::ostream& out);

    // Reboot
    void SetRestartWarm(std::ostream& out);

    void SetRestartCold(std::ostream& out);

    void SetGracefulShutdown(std::ostream& out);

    void DumpSwVersion(std::ostream& out);

private:

    void CreateDataCache();

    void Initialize();

    void CreateCallbackHandler();

    void CreateStateCollector();

    void UpdateBoardFaults(chm6_board::Chm6BoardFault& boardFault);

    void UpdateBoardPm();

    void UpdateEqptStates(chm6_board::Chm6BoardState& boardState);

    void UpdateEqptInventory();

    void UpdateLedStates(chm6_board::Chm6BoardState& boardState);

    void UpdateLastBootInfo();

    void UpdateUpgradableDevices(chm6_board::Chm6BoardState& boardState);

    void UpdateTomPresenceMap();

    void UpdateDcoCapabilities();

    void UpdateDcoSyncReady();

    void UpdateSwVersion();

    void UpdateFanControlFlags();

    int  HandleBoardConfig(chm6_board::Chm6BoardConfig* boardCfgMsg);

    int HandleHostCardAction(hal_common::BoardAction hostCardAction);

    int HandleDcoCardAction(hal_common::BoardAction dcoCardAction);

    int HandleFaultLed(hal_common::LedState state);

    int HandleActiveLed(hal_common::LedState state);

    int HandleLedLocationTest(bool doTest);

    int HandlePortLeds(const google::protobuf::Map< google::protobuf::uint32, hal_common::PortLed >& new_port_leds);

    int HandleLineLeds(const google::protobuf::Map< google::protobuf::uint32, hal_common::LineLed >& new_line_leds);

    void HandleBoardInitStateChange(chm6_common::Chm6BoardInitState* pBrdState);

    void HandleDcoCardStateChange(chm6_common::Chm6DcoCardState* dcoStateMsg);

    void HandleDcoCardFault(chm6_common::Chm6DcoCardFault* dcoFaultMsg);

    void HandleDcoCardPm(chm6_common::Chm6DcoCardPm* dcoPmMsg);

    void UpdateBoardInitFaults(chm6_common::Chm6BoardInitState* pBrdState);

    void ResetBoardConfig();

    void AddLog(const std::string &func, uint32 line, std::ostringstream &text);

    void PollInitDone();

    void InitializeSwVersion();

    int ProcessBoardConfigInDb();

    void SendBoardStateToRedis(chm6_board::Chm6BoardState& boardState);

    void RelayDcoCardConfigToDpMs(chm6_common::Chm6DcoConfig& dco_config);

    std::string mAid;

    std::shared_ptr<boardAda::BoardAdapterIf> mspAdapter;

    std::shared_ptr<BoardDriver> mspDriver;

    std::unique_ptr<BoardStateCollector> mupCollector;

    // Config cache
    std::unique_ptr<chm6_board::Chm6BoardConfig> mupBoardConfig;
    std::mutex    mBoardConfigLock;

    hal_common::BoardAction mHostBoardAction;

    // State cache
    std::unique_ptr<chm6_board::Chm6BoardState> mupBoardState;
    std::mutex    mBoardStateLock;

    // Fault cache
    std::unique_ptr<chm6_board::Chm6BoardFault> mupBoardFault;

    // PM cache
    std::unique_ptr<chm6_board::Chm6BoardPm> mupBoardPm;

    // Internal message cache
    chm6_common::Chm6TomPresenceMap mTomPresenceMap;

    hal_chm6::DcoCapabilities mDcoCapabilities;

    wrapper::Bool mDcoSyncReady;

    wrapper::Bool mAltTempOorh;

    wrapper::Bool mAltTempOorl;

    chm6_common::Chm6DcoCardFault mDcoCardFault;

    chm6_common::Chm6DcoCardPm mDcoCardPm;
    hal_chm6::DcoPm mDcoPm; // TODO: remove later

    hal_common::UpgradableDeviceType mDcoUpgradableDevices;

    hal_common::FaultType mBoardInitFault;

    std::string mSwVersion;

    std::string mBaseOsVersion;

    std::string mBaseInfraVersion;

    bool mFirstState;

    bool mFirstFault;

    bool mFirstPm;

    bool mFirstDcoCardAction;

    SimpleLog::Log*  mpLog;
    mutable std::mutex    mLogLock;

    boost::thread mThrCli;
    bool mThrExit;

    bool mIsSim;

    bool mIsInitDone;
    bool mIsBrdInitSuccess;

    std::string mLastRebootReason;

    std::string mLastRebootTimestamp;

    static const std::string cEnvStrVerLc;
    static const std::string cEnvStrVerBaseOs;
    static const std::string cEnvStrVerBaseInfra;

};

#endif /* CHM6_BOARD_MS_SRC_MANAGER_BOARD_MANAGER_H_ */
