#include <cli/cli.h>
#include <boost/function.hpp>

#include "sim_adapter_cmds.h"

using namespace cli;
using namespace boost;
using namespace boardMs;

SimAdapterCmds::SimAdapterCmds(simBoardAda::SimBoardAdapterIf& adapter)
    : mSimAdapter(adapter)
{}
SimAdapterCmds::~SimAdapterCmds()
{}

void SimAdapterCmds::SetEqptState(std::ostream& out, int eqptState)
{
    if (eqptState >= (int)EQPT_STATE_MAX)
    {
        out << "Invalid eqpt state. Valid input: 0 ~ " << ((int)EQPT_STATE_MAX -1) << std::endl;
        return;
    }
    mSimAdapter.SetEqptState(out, (Chm6EqptState)eqptState);
}

void SimAdapterCmds::SetEqptInventory(std::ostream& out, std::string invTypeName, std::string invTypeValue)
{
    mSimAdapter.SetEqptInventory(out, invTypeName, invTypeValue);
}

void SimAdapterCmds::SetUpgradableDevice(std::ostream& out, int deviceType, std::string hwVersion, std::string swVersion)
{
    if (deviceType >= (int)MAX_UPGDDEVICE_TYPE_NUM)
    {
        out << "Invalide device type. Valid input: 0 ~ " << (int)(MAX_UPGDDEVICE_TYPE_NUM - 1) << std::endl;
        return;
    }
    mSimAdapter.SetUpgradableDevice(out, (upgradableDeviceType)deviceType, hwVersion, swVersion);
}

void SimAdapterCmds::SetBoardPm(std::ostream& out, float32 value)
{
    mSimAdapter.SetBoardPm(out, value);
}

void SimAdapterCmds::SetFaultLedState(std::ostream &os, int st)
{
    if ( (st != RED)
      && (st != FLASHING_RED)
      && (st != OFF) )
    {
        os << "Invalid Fru LOS LED state.\n"
                "Valid state: \n"
                        "\t1 - LED OFF\n"
                        "\t6 - LED RED\n"
                        "\t7 - LED FLASHING_RED\n" << std::endl;
        return;
    }

    mSimAdapter.SetFaultLedState(os, (LedStateType)st);
}

void SimAdapterCmds::SetActiveLedState(std::ostream &os, int st)
{
    if ( (st != GREEN)
      && (st != FLASHING_GREEN)
      && (st != YELLOW)
      && (st != FLASHING_YELLOW)
      && (st != OFF) )
    {
        os << "Invalid Fru ACTIVE LED state.\n"
                "Valid state: \n"
                "\t1 - LED OFF\n"
                "\t2 - LED YELLOW\n"
                "\t3 - LED FLASHING_YELLOW\n"
                "\t4 - LED GREEN\n"
                "\t5 - LED FLASHING_GREEN" << std::endl;
        return;
    }

    mSimAdapter.SetActiveLedState(os, (LedStateType)st);
}

void SimAdapterCmds::SetPortLedState(std::ostream &out, int portId, int ledType, int ledState)
{
    if (portId >= boardMs::MAX_QSFP_NUM)
    {
        out << "Invalid PORT ID. Valid input(0 based): 0 ~ 15" << std::endl;
        return;
    }

    if (ledType >= boardMs::MAX_QSFP_LED_TYPE)
    {
        out << "Invalid Mezz QSFP LED type. Valid input: 0 - Active LED; 1 - Los LED" << std::endl;
        return;
    }

    if ((QSFPLedType)ledType == QSFP_ACTIVE)
    {
        if ( (ledState != GREEN)
          && (ledState != FLASHING_GREEN)
          && (ledState != YELLOW)
          && (ledState != FLASHING_YELLOW)
          && (ledState != OFF) )
        {
            out << "Invalid Mezz QSFP ACTIVE LED state.\n"
                    "Valid state: \n"
                    "\t1 - LED OFF\n"
                    "\t2 - LED YELLOW\n"
                    "\t3 - LED FLASHING_YELLOW\n"
                    "\t4 - LED GREEN\n"
                    "\t5 - LED FLASHING_GREEN" << std::endl;
            return;
        }
    }

    else if ((QSFPLedType)ledType == QSFP_LOS)
    {
        if ( (ledState != RED)
          && (ledState != FLASHING_RED)
          && (ledState != OFF) )
        {
            out << "Invalid Mezz QSFP LOS LED state.\n"
                    "Valid state: \n"
                            "\t1 - LED OFF\n"
                            "\t6 - LED RED\n"
                            "\t7 - LED FLASHING_RED\n" << std::endl;
            return;
        }
    }

    mSimAdapter.SetPortLedState(out, (QSFPPortId)portId, (QSFPLedType)ledType, (LedStateType)ledState);
}

void SimAdapterCmds::SetLineLedState(std::ostream &out, int lineId, int ledType, int ledState)
{
    if (lineId >= boardMs::MAX_LINE_NUM)
    {
        out << "Invalid Line ID. Valid input: 0 - Line_1; 1 - Line_2" << std::endl;
        return;
    }

    if (ledType >= boardMs::MAX_LINE_LED_TYPE)
    {
        out << "Invalid Mezz LINE LED type. Valid input: 0 - Active LED; 1 - Los LED" << std::endl;
        return;
    }

    if ((LineLedType)ledType == LINE_ACTIVE)
    {
        if ( (ledState != GREEN)
          && (ledState != FLASHING_GREEN)
          && (ledState != YELLOW)
          && (ledState != FLASHING_YELLOW)
          && (ledState != OFF) )
        {
            out << "Invalid LINE ACTIVE LED state.\n"
                    "Valid state: \n"
                    "\t1 - LED OFF\n"
                    "\t2 - LED YELLOW\n"
                    "\t3 - LED FLASHING_YELLOW\n"
                    "\t4 - LED GREEN\n"
                    "\t5 - LED FLASHING_GREEN" << std::endl;
            return;
        }
    }

    else if ((LineLedType)ledType == LINE_LOS)
    {
        if ( (ledState != RED)
          && (ledState != FLASHING_RED)
          && (ledState != OFF) )
        {
            out << "Invalid Mezz LINE LOS LED state.\n"
                    "Valid state: \n"
                            "\t1 - LED OFF\n"
                            "\t6 - LED RED\n"
                            "\t7 - LED FLASHING_RED\n" << std::endl;
            return;
        }
    }

    mSimAdapter.SetLineLedState(out, (LineId)lineId, (LineLedType)ledType, (LedStateType)ledState);
}

void SimAdapterCmds::SetLedLampTest(std::ostream &os, int doTest)
{
    mSimAdapter.SetLedLampTest(os, (bool)doTest);
}

////////////////////////////////////////////////////////////

boost::function< void (SimAdapterCmds*, std::ostream&, int) > cmdSetEqptState = &SimAdapterCmds::SetEqptState;
boost::function< void (SimAdapterCmds*, std::ostream&, std::string, std::string) > cmdSetEqptInventory = &SimAdapterCmds::SetEqptInventory;
boost::function< void (SimAdapterCmds*, std::ostream&, int, std::string, std::string) > cmdSetUpgradableDevice = &SimAdapterCmds::SetUpgradableDevice;
boost::function< void (SimAdapterCmds*, std::ostream&, float32) > cmdSetBoardPm = &SimAdapterCmds::SetBoardPm;
boost::function< void (SimAdapterCmds*, std::ostream&, int) > cmdSetFaultLedState = &SimAdapterCmds::SetFaultLedState;
boost::function< void (SimAdapterCmds*, std::ostream&, int) > cmdSetActiveLedState = &SimAdapterCmds::SetActiveLedState;
boost::function< void (SimAdapterCmds*, std::ostream&, int, int, int) > cmdSetPortLedState = &SimAdapterCmds::SetPortLedState;
boost::function< void (SimAdapterCmds*, std::ostream&, int, int, int) > cmdSetLineLedState = &SimAdapterCmds::SetLineLedState;
boost::function< void (SimAdapterCmds*, std::ostream&, int) > cmdSetLedLampTest = &SimAdapterCmds::SetLedLampTest;

void InsertSimAdapterCmds(unique_ptr< Menu > & adapterMenu, SimAdapterCmds& adapterCmds)
{
    adapterMenu -> Insert(
            "set_eqpt_state",
            [&](std::ostream& out, int state){ cmdSetEqptState(&adapterCmds, out, state); },
            "Set Equipment state",
            {"Equipment state:\n"
                    "\t\t0: UNKNOWN\n"
                    "\t\t1: BOOTING\n"
                    "\t\t2: BOOTING_DONE\n"
                    "\t\t3: BOOTING_DONE_WARM\n"
                    "\t\t4: BOOTING_FAIL\n"
                    "\t\t5: FW_UPDATING\n"
                    "\t\t6: FW_UPDATE_DONE\n"
                    "\t\t7: FW_UPDATE_FAIL\n"
                    "\t\t8: FW_UPDATE_COMMIT\n"
                    "\t\t9: HW_INITING\n"
                    "\t\t10:HW_INITING_DONE\n"
                    "\t\t11:HW_INITING_FAIL\n"
                    "\t\t12:RESTARTING"} );

    adapterMenu -> Insert(
            "set_inv",
            [&](std::ostream& out, std::string inv, std::string value){ cmdSetEqptInventory(&adapterCmds, out, inv, value); },
            "Set eqpt inventory",
            {"attribute:(\n"
                    "\t\tsn:SerialNumber,\n"
                    "\t\tpn:PartNumber,\n"
                    "\t\tvn:VendorName,\n"
                    "\t\thv:HardwareRevision,\n"
                    "\t\tmd:ManufactureDate,\n"
                    "\t\tfv:FirmwareVersion,\n"
                    "\t\tcc:CleiCode,\n"
                    "\t\tpon:Pon,\n"
                    "\t\tid:InsertionDate)",
             "value"} );

    adapterMenu -> Insert(
            "set_device",
            [&](std::ostream& out, int type, std::string hwVersion, std::string swVersion){ cmdSetUpgradableDevice(&adapterCmds, out, type, hwVersion, swVersion); },
            "Set upgradable device",
            {"device ID: See dump of \"devices\"", "hwVersion", "swVersion"} );

    adapterMenu -> Insert(
            "set_pm",
            [&](std::ostream& out, float32 value){ cmdSetBoardPm(&adapterCmds, out, value); },
            "Set board pm \"temperature\" value",
            {"value"} );

    adapterMenu -> Insert(
            "set_fault_led",
            [&](std::ostream& out, int led){ cmdSetFaultLedState(&adapterCmds, out, led); },
            "Set fault LED state",
            {"LED state:\n"
                    "\t\t1: OFF\n"
                    "\t\t6: RED\n"
                    "\t\t7: FLASHING_RED"} );

    adapterMenu -> Insert(
            "set_active_led",
            [&](std::ostream& out, int led){ cmdSetActiveLedState(&adapterCmds, out, led); },
            "Set active LED state",
            {"LED state:\n"
                    "\t\t1: OFF\n"
                    "\t\t2: YELLOW\n"
                    "\t\t3: FLASHING_YELLOW\n"
                    "\t\t4: GREEN\n"
                    "\t\t5: FLASHING_GREEN"} );

    adapterMenu -> Insert(
            "set_port_led",
            [&](std::ostream& out, int portId, int ledType, int ledState)
            {
                cmdSetPortLedState(&adapterCmds, out, portId, ledType, ledState);
            },
            "Set port LED state.\n"
            "\tValid \"portId\"(0 based): 0 ~ 15\n"
            "\tValid \"ledType\": 0 - Active LED; 1 - Los LED\n"
            "\tValid Active ledState:\n"
                    "\t\t1 - LED OFF\n"
                    "\t\t2 - LED YELLOW\n"
                    "\t\t3 - LED FLASHING_YELLOW\n"
                    "\t\t4 - LED GREEN\n"
                    "\t\t5 - LED FLASHING_GREEN\n"
            "\tValid Los ledState: \n"
                    "\t\t1 - LED OFF\n"
                    "\t\t6 - LED RED\n"
                    "\t\t7 - LED FLASHING_RED\n",
            {"portId", "ledType", "ledState"} );

    adapterMenu -> Insert(
            "set_line_led",
            [&](std::ostream& out, int lineId, int ledType, int ledState)
            {
                cmdSetLineLedState(&adapterCmds, out, lineId, ledType, ledState);
            },
            "Set line LED state.\n"
            "\tValid \"lineId\": 0 - LINE_1; 1 - LINE_2.\n"
            "\tValid \"ledType\": 0 - Active LED; 1 - Los LED\n"
            "\tValid Active ledState:\n"
                    "\t\t1 - LED OFF\n"
                    "\t\t2 - LED YELLOW\n"
                    "\t\t3 - LED FLASHING_YELLOW\n"
                    "\t\t4 - LED GREEN\n"
                    "\t\t5 - LED FLASHING_GREEN\n"
            "\tValid Los ledState: \n"
                    "\t\t1 - LED OFF\n"
                    "\t\t6 - LED RED\n"
                    "\t\t7 - LED FLASHING_RED\n",
            {"lineId", "ledType", "ledState"} );

    adapterMenu -> Insert(
            "set_lamp_test",
            [&](std::ostream& out, int doTest){ cmdSetLedLampTest(&adapterCmds, out, doTest); },
            "Enable or disable LED lamp test",
            {"doTest(0: disable; >=1: enable)"} );
}

