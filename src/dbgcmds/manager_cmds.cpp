//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#include <cli/cli.h>
#include <boost/function.hpp>

#include "manager_cmds.h"

using namespace cli;
using namespace boost;

ManagerCmds::ManagerCmds(BoardManager& manager)
    : manager(manager)
{}
/*
 * CLI commands to dump internal logs
 */
void ManagerCmds::DumpLog(std::ostream& out)
{
    manager.DumpLog(out);
}

void ManagerCmds::DumpStatus(std::ostream& out, std::string instr)
{
    manager.DumpStatus(out, instr);
}

void ManagerCmds::ResetLog(std::ostream& out)
{
    manager.ResetLog(out);
}

/*
 * CLI commands to dump internal data cache
 */
void ManagerCmds::DumpBoardConfig(std::ostream& out)
{
    manager.DumpBoardConfig(out);
}

void ManagerCmds::DumpBoardState(std::ostream& out)
{
    manager.DumpBoardState(out);
}

void ManagerCmds::DumpBoardFault(std::ostream& out)
{
    manager.DumpBoardFault(out);
}

void ManagerCmds::DumpBoardPm(std::ostream& out)
{
    manager.DumpBoardPm(out);
}

void ManagerCmds::SetRestartWarm(std::ostream& out)
{
    manager.SetRestartWarm(out);
}

void ManagerCmds::SetRestartCold(std::ostream& out)
{
    manager.SetRestartCold(out);
}

void ManagerCmds::SetGracefulShutdown(std::ostream& out)
{
    manager.SetGracefulShutdown(out);
}

void ManagerCmds::DumpSwVersion(std::ostream& out)
{
    manager.DumpSwVersion(out);
}

//////////////////////////////////////////////////////////////

boost::function< void (ManagerCmds*, std::ostream&) > cmdDumpManagerLog = &ManagerCmds::DumpLog;
boost::function< void (ManagerCmds*, std::ostream&, std::string) > cmdDumpManagerStatus = &ManagerCmds::DumpStatus;
boost::function< void (ManagerCmds*, std::ostream&) > cmdResetManagerLog = &ManagerCmds::ResetLog;
boost::function< void (ManagerCmds*, std::ostream&) > cmdDumpBoardConfig = &ManagerCmds::DumpBoardConfig;
boost::function< void (ManagerCmds*, std::ostream&) > cmdDumpBoardState = &ManagerCmds::DumpBoardState;
boost::function< void (ManagerCmds*, std::ostream&) > cmdDumpBoardFault = &ManagerCmds::DumpBoardFault;
boost::function< void (ManagerCmds*, std::ostream&) > cmdDumpBoardPm = &ManagerCmds::DumpBoardPm;
boost::function< void (ManagerCmds*, std::ostream&) > cmdSetRestartWarm = &ManagerCmds::SetRestartWarm;
boost::function< void (ManagerCmds*, std::ostream&) > cmdSetRestartCold = &ManagerCmds::SetRestartCold;
boost::function< void (ManagerCmds*, std::ostream&) > cmdSetGracefulShutdown = &ManagerCmds::SetGracefulShutdown;
boost::function< void (ManagerCmds*, std::ostream&) > cmdDumpSwVersion = &ManagerCmds::DumpSwVersion;

void InsertManagerCmds(unique_ptr< Menu > & managerMenu, ManagerCmds& managerCmds)
{

    managerMenu -> Insert(
            "log",
            [&](std::ostream& out){ cmdDumpManagerLog(&managerCmds, out); },
            "Dump manager log" );

    managerMenu -> Insert(
            "status",
            [&](std::ostream& out, std::string instr){ cmdDumpManagerStatus(&managerCmds, out, instr); },
            "Dump manager status" );

    managerMenu -> Insert(
            "resetlog",
            [&](std::ostream& out){ cmdResetManagerLog(&managerCmds, out); },
            "Reset manager log" );

    managerMenu -> Insert(
            "config",
            [&](std::ostream& out){ cmdDumpBoardConfig(&managerCmds, out); },
            "Dump board configurations" );

    managerMenu -> Insert(
            "state",
            [&](std::ostream& out){ cmdDumpBoardState(&managerCmds, out); },
            "Dump board states" );

    managerMenu -> Insert(
            "fault",
            [&](std::ostream& out){ cmdDumpBoardFault(&managerCmds, out); },
            "Dump faults in manager cache" );

    managerMenu -> Insert(
            "pm",
            [&](std::ostream& out){ cmdDumpBoardPm(&managerCmds, out); },
            "Dump Pms in manager cache" );

    managerMenu -> Insert(
            "restart_warm",
            [&](std::ostream& out){ cmdSetRestartWarm(&managerCmds, out); },
            "Warm restart CHM6" );

    managerMenu -> Insert(
            "restart_cold",
            [&](std::ostream& out){ cmdSetRestartCold(&managerCmds, out); },
            "Cold restart CHM6" );

    managerMenu -> Insert(
            "graceful_shutdown",
            [&](std::ostream& out){ cmdSetGracefulShutdown(&managerCmds, out); },
            "Graceful shutdown CHM6" );

    managerMenu -> Insert(
            "sw_version",
            [&](std::ostream& out){ cmdDumpSwVersion(&managerCmds, out); },
            "Dump SW Version" );
}
