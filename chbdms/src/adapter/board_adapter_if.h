//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#ifndef CHM6_BOARD_MS_SRC_ADAPTER_BOARD_ADAPTER_IF_H_
#define CHM6_BOARD_MS_SRC_ADAPTER_BOARD_ADAPTER_IF_H_

#include <iostream>
#include <string>

#include "board_defs.h"
#include "board_fault_defs.h"

namespace boardAda
{

using namespace boardMs;

class BoardAdapterIf
{
public:

    BoardAdapterIf() {}

    virtual ~BoardAdapterIf() {}

    virtual void SetSlotId(uint32 slotId) {}

    virtual void SetSledId(uint32 sledId) {}

    /*
     * APIs to configure host board
     */

    virtual int SetHostBoardAction(hostBoardActionType action) {return 0;}

    virtual void SetColdRestartDcoDelay(uint32 delaySeconds) {}

    /*
     * Queries
     */
    virtual bool IsColdBoot() { return true;}

    virtual bool IsOnBench() {return true;}

    virtual bool IsChassisDiag() { return true;}

    /*
     * Board status
     */
    virtual boardMs::Chm6EqptState GetEqptState()
        {return boardMs::EQPT_STATE_UNKNOWN;}

    virtual Chm6EqptInventory& GetEqptInventory() = 0;

    virtual LedStateType GetPowerLedState() { return GREEN;}

    virtual void SetFaultLedState(LedStateType state) {}

    virtual LedStateType GetFaultLedState() {return LED_STATE_UNKNOWN;}

    virtual void SetActiveLedState(LedStateType state) {}

    virtual LedStateType GetActiveLedState() {return LED_STATE_UNKNOWN;}

    virtual upgradable_device_ptr_vec& GetUpgradableDevices() = 0;

    virtual void DoLedLocationTest(bool doTest) {}

    virtual void SetPortLedState(boardMs::QSFPPortId portId,
                                 boardMs::QSFPLedType ledType,
                                 boardMs::LedStateType ledState) {}

    virtual port_led_ptr_vec& GetPortLedStates() = 0;

    virtual void SetLineLedState(boardMs::LineId lineId,
                                 boardMs::LineLedType ledType,
                                 boardMs::LedStateType ledState) {}

    virtual line_led_ptr_vec& GetLineLedStates() = 0;

    virtual void CheckLedLampLocTestState() {}

    // Run diagnostic tests

    // Get temperatures

    virtual std::string GetAid() { return std::string("No Name."); }

    virtual std::string getName() { return std::string("No Name."); }

    /*
     * Board fault
     */
    virtual board_fault_ptr_vec& GetBoardFault() = 0;

    virtual void CheckFaults() {}

    /*
     * Board PM
     */

    virtual board_pm_ptr_vec& GetBoardPm() = 0;

    // CLI commands
    virtual void DumpLog(std::ostream &os) {}

    virtual void DumpStatus(std::ostream &os, std::string cmd) {}

    virtual void ResetLog( std::ostream &os ) {}

    virtual void DumpEqptInventory( std::ostream &os ) {}

    virtual void DumpUpgradableDevices( std::ostream &os ) {}

    virtual void DumpFaults(std::ostream &os, uint32 faultId) {}

    virtual void DumpFaults(std::ostream &os, std::string faultName) {}

    virtual void DumpPms(std::ostream &os) {}

    virtual void DumpLedStates(std::ostream &os) {}

    virtual void SetFaultSim(std::ostream &os, uint32 faultId,
                             bool isSimEn, bool isTripped) {}

    virtual void SetFaultSim(std::ostream &os, std::string faultNm,
                             bool isSimEn, bool isTripped) {}

    virtual void PrintMaxFaultNum(std::ostream &os) {}

    virtual void SetAccessFaultSim(std::ostream &os,
                                   std::string faultId,
                                   bool bSetFault) {}
};

} // namespace boardAda

#endif // CHM6_BOARD_MS_SRC_ADAPTER_BOARD_ADAPTER_IF_H_

