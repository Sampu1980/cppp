//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#ifndef CHM6_BOARD_MS_SRC_DBGCMDS_MANAGER_CMDS_H_
#define CHM6_BOARD_MS_SRC_DBGCMDS_MANAGER_CMDS_H_

#include "board_manager.h"

class ManagerCmds
{
public:
    ManagerCmds(BoardManager& manager);
    ~ManagerCmds() {}

    /*
     * CLI commands to dump internal logs
     */
    void DumpLog(std::ostream& out);

    void DumpStatus(std::ostream& out, std::string instr);

    void ResetLog(std::ostream& out);

    /*
     * CLI commands to dump internal data cache
     */
    void DumpBoardConfig(std::ostream& out);

    void DumpBoardState(std::ostream& out);

    void DumpBoardFault(std::ostream& out);

    void DumpBoardPm(std::ostream& out);

    /*
     * CLI commands to restart
     */
    void SetRestartWarm(std::ostream& out);

    void SetRestartCold(std::ostream& out);

    void SetGracefulShutdown(std::ostream& out);

    void DumpSwVersion(std::ostream& out);

private:
    BoardManager& manager;
};

#endif /* CHM6_BOARD_MS_SRC_DBGCMDS_MANAGER_CMDS_H_ */
