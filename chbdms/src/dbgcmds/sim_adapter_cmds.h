/*
 * sim_adapter_cmds.h
 *
 *  Created on: Apr 28, 2020
 */

#ifndef CHM6_BOARD_MS_SRC_DBGCMDS_SIM_ADAPTER_CMDS_H_
#define CHM6_BOARD_MS_SRC_DBGCMDS_SIM_ADAPTER_CMDS_H_

#include "sim_board_adapter.h"

using namespace boardMs;

class SimAdapterCmds
{
public:
    SimAdapterCmds(simBoardAda::SimBoardAdapterIf& adapter);
    ~SimAdapterCmds();

    void SetEqptState(std::ostream& out, int eqptState);

    void SetEqptInventory(std::ostream& out, std::string invTypeName, std::string invTypeValue);

    void SetUpgradableDevice(std::ostream& out, int deviceType, std::string hwVersion, std::string swVersion);

    void SetBoardPm(std::ostream& out, float32 value);

    void SetFaultLedState(std::ostream &os, int st);

    void SetActiveLedState(std::ostream &os, int st);

    void SetPortLedState(std::ostream &os, int portId, int ledType, int ledState);

    void SetLineLedState(std::ostream &os, int lineId, int ledType, int ledState);

    void SetLedLampTest(std::ostream &os, int doTest);

private:
    simBoardAda::SimBoardAdapterIf& mSimAdapter;
};

#endif /* CHM6_BOARD_MS_SRC_DBGCMDS_SIM_ADAPTER_CMDS_H_ */
