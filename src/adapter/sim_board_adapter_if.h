//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#ifndef CHM6_BOARD_MS_SRC_ADAPTER_SIM_BOARD_ADAPTER_IF_H_
#define CHM6_BOARD_MS_SRC_ADAPTER_SIM_BOARD_ADAPTER_IF_H_

#include <iostream>
#include <string>

#include "board_defs.h"
#include "board_fault_defs.h"

namespace simBoardAda
{
using namespace boardMs;

class SimBoardAdapterIf
{
public:

    virtual ~SimBoardAdapterIf() {}

    /*
     * Set EqptState
     */
    virtual void SetEqptState(std::ostream &os, Chm6EqptState eqptState) {}

    /*
     * Set EqptInventory
     */
    virtual void SetEqptInventory(std::ostream &os, std::string& invTypeName, std::string& invTypeValue) {}

    /*
     * Set upgradable devices
     */
    virtual void SetUpgradableDevice(std::ostream &os, upgradableDeviceType type, std::string& hwVersion, std::string& swVersion){}

    /*
     * Sim main board pm
     * Set single pm in pm data
     */
    virtual void SetBoardPm(std::ostream &os, float32 value) {}

    /*
     * Set Fault LED state
     */
    virtual void SetFaultLedState(std::ostream &os, LedStateType st) {}

    /*
     * Set Active LED state
     */
    virtual void SetActiveLedState(std::ostream &os, LedStateType st) {}

    /*
     * Set Port LED state
     */
    virtual void SetPortLedState(std::ostream &os, QSFPPortId portId, QSFPLedType ledType, LedStateType ledState) {}

    /*
     * Set Line LED state
     */
    virtual void SetLineLedState(std::ostream &os, LineId lineId, LineLedType ledType, LedStateType ledState) {}

    /*
     * Set LED lamp test
     */
    virtual void SetLedLampTest(std::ostream &os, bool doTest) {}
};

}

#endif /* CHM6_BOARD_MS_SRC_ADAPTER_SIM_BOARD_ADAPTER_IF_H_ */
