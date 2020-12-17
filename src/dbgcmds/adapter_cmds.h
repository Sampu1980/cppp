/*
 * adapter_cmds.h
 *
 *  Created on: Apr 28, 2020
 */

#ifndef CHM6_BOARD_MS_SRC_DBGCMDS_ADAPTER_CMDS_H_
#define CHM6_BOARD_MS_SRC_DBGCMDS_ADAPTER_CMDS_H_

#include "board_adapter.h"

class AdapterCmds
{
public:
    AdapterCmds(boardAda::BoardAdapterIf& adapter);
    ~AdapterCmds();

    void DumpLog(std::ostream& out);

    void DumpStatus(std::ostream& out, std::string instr);

    void ResetLog(std::ostream& out);

    void DumpEqptInventory(std::ostream& out);

    void DumpUpgradableDevices(std::ostream& out);

    void DumpFaults(std::ostream& out, uint32 faultId);

    void DumpFaultsStr(std::ostream& out, std::string faultNm);

    void DumpPms(std::ostream& out);

    void DumpLedStates(std::ostream &os);

    void SetFaultSim(std::ostream &os, uint32 faultId,
                     bool isSimEn, bool isTripped);
    void SetFaultSimStr(std::ostream &os, std::string faultNm,
                        bool isSimEn, bool isTripped);

    void PrintMaxFaultNum(std::ostream &os);

    void SetAccessFaultSim(std::ostream &os,
                           std::string faultId,
                           bool bSetFault);

private:
    boardAda::BoardAdapterIf& mAdapter;
};


#endif /* CHM6_BOARD_MS_SRC_DBGCMDS_ADAPTER_CMDS_H_ */
