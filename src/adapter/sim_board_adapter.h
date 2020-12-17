//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#ifndef CHM6_BOARD_MS_SRC_ADAPTER_SIM_BOARD_ADAPTER_H_
#define CHM6_BOARD_MS_SRC_ADAPTER_SIM_BOARD_ADAPTER_H_

#include <mutex>

#include "sim_board_adapter_if.h"
#include "board_adapter_if.h"
#include "board_common_adapter.h"

#include "board_defs.h"
#include "SimpleLog.h"

namespace simBoardAda
{
using namespace boardMs;

class SimBoardAdapter : public SimBoardAdapterIf
                      , public boardAda::BoardCommonAdapter
{
public:
    SimBoardAdapter();

    virtual ~SimBoardAdapter();

    /*
     * Set EqptState
     */
    void SetEqptState(std::ostream &os, Chm6EqptState eqptState);

    /*
     * Set EqptInventory
     */
    void SetEqptInventory(std::ostream &os, std::string& invTypeName, std::string& invTypeValue);

    /*
     * Set upgradable devices
     */
    void SetUpgradableDevice(std::ostream &os, upgradableDeviceType type, std::string& hwVersion, std::string& swVersion);

    /*
     * Sim main board pm
     * Set single pm in pm data
     */
    void SetBoardPm(std::ostream &os, float32 value);

    /*
     * Set power LED state
     */
    void SetPowerLedState(std::ostream &os, LedStateType st);

    /*
     * Set Fault LED state
     */
    void SetFaultLedState(std::ostream &os, LedStateType st);

    /*
     * Set Active LED state
     */
    void SetActiveLedState(std::ostream &os, LedStateType st);

    /*
     * Set Port LED state
     */
    void SetPortLedState(std::ostream &os, QSFPPortId portId, QSFPLedType ledType, LedStateType ledState);

    /*
     * Set Line LED state
     */
    void SetLineLedState(std::ostream &os, LineId lineId, LineLedType ledType, LedStateType ledState);

    /*
     * Set LED lamp test
     */
    void SetLedLampTest(std::ostream &os, bool doTest);

    /* ================================
     * APIs defined in BoardAdapterIf
     * ================================
     */
    void SetSlotId(uint32 slotId) {}

    void SetSledId(uint32 sledId) {}

    /*
     * APIs to configure host board
     */
    int SetHostBoardAction(hostBoardActionType action);

    /*
     * Queries
     */
    bool IsColdBoot() { return true;}

    bool IsOnBench() {return true;}

    bool IsChassisDiag() { return true;}

    /*
     * Board status
     */
    Chm6EqptState GetEqptState();

    Chm6EqptInventory& GetEqptInventory();

    LedStateType GetPowerLedState();

    void SetFaultLedState(LedStateType state);

    LedStateType GetFaultLedState();

    void SetActiveLedState(LedStateType state);

    LedStateType GetActiveLedState();


    void DoLedLocationTest(bool doTest) { mIsLedLocTestOn = doTest; }

    void SetPortLedState(QSFPPortId portId, QSFPLedType ledType, LedStateType ledState);

    port_led_ptr_vec& GetPortLedStates();

    void SetLineLedState(LineId lineId, LineLedType ledType, LedStateType ledState);

    line_led_ptr_vec& GetLineLedStates();

    std::string GetAid() { return std::string("No Name."); }

    std::string getName() { return std::string("No Name."); }

    /*
     * CLI commands
     */
    void DumpEqptInventory( std::ostream &os );


private:

    void Initialize();


    mutable std::mutex    mFaultLedLock;

    mutable std::mutex    mActiveLedLock;

    // port LED states
    mutable std::mutex    mPortLedLock;

    // Line LED states
    mutable std::mutex    mLineLedLock;

    port_led_ptr_vec mvPortLedStatesInLampTest;
    port_led_ptr_vec mvPortLedStatesInLocTest;

    line_led_ptr_vec mvLineLedStatesInLampTest;
    line_led_ptr_vec mvLineLedStatesInLocTest;
};

}

#endif /* CHM6_BOARD_MS_SRC_ADAPTER_SIM_BOARD_ADAPTER_H_ */
