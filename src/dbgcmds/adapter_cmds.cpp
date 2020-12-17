/*
 * adapter_cmds.cpp
 *
 *  Created on: Apr 28, 2020
 */

#include <cli/cli.h>
#include <boost/function.hpp>

#include "adapter_cmds.h"

using namespace cli;
using namespace boost;

AdapterCmds::AdapterCmds(boardAda::BoardAdapterIf& adapter)
    : mAdapter(adapter)
{}

AdapterCmds::~AdapterCmds() {}

void AdapterCmds::DumpLog(std::ostream& out)
{
    mAdapter.DumpLog(out);
}

void AdapterCmds::DumpStatus(std::ostream& out, std::string instr)
{
    mAdapter.DumpStatus(out, instr);
}

void AdapterCmds::ResetLog(std::ostream& out)
{
    mAdapter.ResetLog(out);
}

void AdapterCmds::DumpEqptInventory(std::ostream& out)
{
    mAdapter.DumpEqptInventory(out);
}

void AdapterCmds::DumpUpgradableDevices(std::ostream& out)
{
    mAdapter.DumpUpgradableDevices(out);
}

void AdapterCmds::DumpFaults(std::ostream& out, uint32 faultId)
{
    mAdapter.DumpFaults(out, faultId);
}

void AdapterCmds::DumpFaultsStr(std::ostream& out, std::string faultNm)
{
    mAdapter.DumpFaults(out, faultNm);
}

void AdapterCmds::DumpPms(std::ostream& out)
{
    mAdapter.DumpPms(out);
}

void AdapterCmds::DumpLedStates(std::ostream &os)
{
    mAdapter.DumpLedStates(os);
}

void AdapterCmds::SetFaultSim(
         std::ostream &os, uint32 faultId, bool isSimEn, bool isTripped)
{
    mAdapter.SetFaultSim(os, faultId, isSimEn, isTripped);
}

void AdapterCmds::SetFaultSimStr(
         std::ostream &os, std::string faultNm, bool isSimEn, bool isTripped)
{
    mAdapter.SetFaultSim(os, faultNm, isSimEn, isTripped);
}

void AdapterCmds::PrintMaxFaultNum(std::ostream &os)
{
    mAdapter.PrintMaxFaultNum(os);
}

void AdapterCmds::SetAccessFaultSim(std::ostream &os,
                                    std::string faultId,
                                    bool bSetFault)
{
    mAdapter.SetAccessFaultSim(os,
                               faultId,
                               bSetFault);
}


//////////////////////////////////////////////////////////////////////

boost::function< void (AdapterCmds*, std::ostream&) >
    cmdDumpAdapterLog = &AdapterCmds::DumpLog;

boost::function< void (AdapterCmds*, std::ostream&, std::string) >
    cmdDumpAdapterStatus = &AdapterCmds::DumpStatus;

boost::function< void (AdapterCmds*, std::ostream&) >
    cmdResetAdapterLog = &AdapterCmds::ResetLog;

boost::function< void (AdapterCmds*, std::ostream&) >
    cmdDumpEqptInventory = &AdapterCmds::DumpEqptInventory;

boost::function< void (AdapterCmds*, std::ostream&) >
    cmdDumpUpgradableDevices = &AdapterCmds::DumpUpgradableDevices;

boost::function< void (AdapterCmds*, std::ostream&, uint) >
    cmdDumpAdapterFaults = &AdapterCmds::DumpFaults;

boost::function< void (AdapterCmds*, std::ostream&, std::string) >
    cmdDumpAdapterFaultsStr = &AdapterCmds::DumpFaultsStr;

boost::function< void (AdapterCmds*, std::ostream&) >
    cmdDumpAdapterPms = &AdapterCmds::DumpPms;

boost::function< void (AdapterCmds*, std::ostream&) >
    cmdDumpLedStates = &AdapterCmds::DumpLedStates;

boost::function< void (AdapterCmds*, std::ostream&, uint32, bool, bool) >
    cmdSetAdapterFaultSim = &AdapterCmds::SetFaultSim;

boost::function< void (AdapterCmds*, std::ostream&, std::string, bool, bool) >
    cmdSetAdapterFaultSimStr = &AdapterCmds::SetFaultSimStr;

boost::function< void (AdapterCmds*, std::ostream&) >
    cmdPrintMaxFaultNum= &AdapterCmds::PrintMaxFaultNum;

boost::function< void (AdapterCmds*, std::ostream&, std::string, bool) >
    cmdSetAdapterAccessFaultSim = &AdapterCmds::SetAccessFaultSim;


void InsertAdapterCmds(unique_ptr< Menu > & adapterMenu, AdapterCmds& adapterCmds)
{

    adapterMenu -> Insert(
            "log",
            [&](std::ostream& out){ cmdDumpAdapterLog(&adapterCmds, out); },
            "Dump adapter log" );

    adapterMenu -> Insert(
            "status",
            [&](std::ostream& out, std::string instr){ cmdDumpAdapterStatus(&adapterCmds, out, instr); },
            "Dump adapter status" );

    adapterMenu -> Insert(
            "resetlog",
            [&](std::ostream& out){ cmdResetAdapterLog(&adapterCmds, out); },
            "Reset adapter log" );

    adapterMenu -> Insert(
            "eqpt_inv",
            [&](std::ostream& out){ cmdDumpEqptInventory(&adapterCmds, out); },
            "Dump equipment inventory" );

    adapterMenu -> Insert(
            "devices",
            [&](std::ostream& out){ cmdDumpUpgradableDevices(&adapterCmds, out); },
            "Dump upgradable devices"
               );

    adapterMenu -> Insert(
            "dump_fault",
            [&](std::ostream& out, uint32 faultId){ cmdDumpAdapterFaults(&adapterCmds, out, faultId); },
            "Dump faults in adapter cache\n"
                "\t\tfaultId: uint32 ID of fault to dump. Use 255 or any value >= max to dump all faults");

    adapterMenu -> Insert(
            "dump_fault",
            [&](std::ostream& out, std::string faultName){ cmdDumpAdapterFaultsStr(&adapterCmds, out, faultName); },
            "Dump faults in adapter cache\n"
                "\t\tfaultName: name of fault or ALL_FAULTS to dump all faults");

    adapterMenu -> Insert(
            "set_fault_sim",
            [&](std::ostream& out, uint32 faultId, bool isSimEn, bool isTripped)
                { cmdSetAdapterFaultSim(&adapterCmds, out, faultId, isSimEn, isTripped); },
            "Set Fault Sim in adapter faults. When enabled the sim fault condition will override the real fault state\n"
                "\t\tfaultId   - faultId from the list given in dump_fault; set to 255 or anything >= max to update all faults \n"
                "\t\tisSimEn   - true: enable sim; false: disable sim \n"
                "\t\tisTripped - true: sim fault state set to FAULT_SET; false: sim fault state set to FAULT_CLEAR" );

    adapterMenu -> Insert(
            "set_fault_sim",
            [&](std::ostream& out, std::string faultName, bool isSimEn, bool isTripped)
                { cmdSetAdapterFaultSimStr(&adapterCmds, out, faultName, isSimEn, isTripped); },
            "Set Fault Sim in adapter faults. When enabled the sim fault condition will override the real fault state\n"
                "\t\tfaultName - faultName: name of fault or ALL_FAULTS to update all faults \n"
                "\t\tisSimEn   - true: enable sim; false: disable sim \n"
                "\t\tisTripped - true: sim fault state set to FAULT_SET; false: sim fault state set to FAULT_CLEAR" );

    adapterMenu -> Insert(
            "print_max_fault_num",
            [&](std::ostream& out){ cmdPrintMaxFaultNum(&adapterCmds, out); },
            "Print max fault number"
               );

    adapterMenu -> Insert(
            "pm",
            [&](std::ostream& out){ cmdDumpAdapterPms(&adapterCmds, out); },
            "Dump Pms in adapter cache" );

    adapterMenu -> Insert(
            "led",
            [&](std::ostream& out){ cmdDumpLedStates(&adapterCmds, out); },
            "Dump LED states in adapter cache" );

    adapterMenu -> Insert(
            "set_access_fault_sim",
            [&](std::ostream& out,
                std::string faultId,
                bool bSetFault)
	        { cmdSetAdapterAccessFaultSim(&adapterCmds, out,
                                              faultId,
                                              bSetFault); },
            "Set Access Fault Sim in adapter faults.\n"
                "\t\tfaultName - faultName: name of fault or ALL_FAULTS to update all faults \n"
                "\t\tbSetFault   - true: sets the fault; false: resets the fault. \n" );
}
