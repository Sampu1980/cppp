#include <cli/clilocalsession.h> // include boost asio
#include <cli/remotecli.h>
// TODO. NB: remotecli.h and clilocalsession.h both includes boost asio, 
// so in Windows it should appear before cli.h that include rang
// (consider to provide a global header file for the library)
#include <cli/cli.h>
#include <boost/function.hpp>

#include "manager_cmds.h"
#include "adapter_cmds.h"
#include "sim_adapter_cmds.h"
#include "driver_cmds.h"

using namespace cli;
using namespace std;
using namespace boost;

extern void InsertManagerCmds(unique_ptr< Menu > & managerMenu, ManagerCmds& managerCmds);

extern void InsertAdapterCmds(unique_ptr< Menu > & adapterMenu, AdapterCmds& adapterCmds);

extern void InsertDriverCmds(unique_ptr< Menu > & driverMenu, DriverCmds& driverCmds);
extern void InsertRegIfCmds(unique_ptr< Menu > & regifMenu, DriverCmds& driverCmds);
extern void InsertMfgEepromCmds(unique_ptr< Menu > & subMenu_eeprom, DriverCmds& driverCmds);
extern void InsertLedCmds(unique_ptr< Menu > & subMenu_led, DriverCmds& driverCmds);
extern void InsertFpgaUtilCmds(unique_ptr< Menu > & driverMenu, DriverCmds& driverCmds);
extern void InsertAdm1066Cmds(unique_ptr< Menu > & subMenu_adm1066, DriverCmds& driverCmds);

/**********************************************************
 * DbgCmds for hw platform
 **********************************************************
 */
int DbgCmds(BoardDriver& driver
            , boardAda::BoardAdapter& adapter
            , BoardManager& manager)
{
#if BOOST_VERSION < 106600
    boost::asio::io_service ios;
#else
    boost::asio::io_context ios;
#endif

    DriverCmds driverCmds(driver);
    AdapterCmds adapterCmds(adapter);
    ManagerCmds mamangerCmds(manager);

    // setup cli

    auto rootMenu = make_unique< Menu >( "board_ms_cli" );

//////////////////////////////////////////////////////////////////////////

    auto subMenu_manager = make_unique< Menu >( "manager_cli" );

    InsertManagerCmds(subMenu_manager, mamangerCmds);

    rootMenu -> Insert( std::move(subMenu_manager) );

//////////////////////////////////////////////////////////////////////////

    auto subMenu_adapter = make_unique< Menu >( "adapter_cli" );

    InsertAdapterCmds(subMenu_adapter, adapterCmds);

    rootMenu -> Insert( std::move(subMenu_adapter) );

//////////////////////////////////////////////////////////////////////////

    auto subMenu_driver = make_unique< Menu >( "driver_cli" );

    InsertDriverCmds(subMenu_driver, driverCmds);


    auto subMenu_regif = make_unique< Menu >( "regif_cli" );

    InsertRegIfCmds(subMenu_regif, driverCmds);

    subMenu_driver -> Insert(std::move(subMenu_regif));


    auto subMenu_eeprom = make_unique< Menu >( "mfgeep_cli" );

    InsertMfgEepromCmds(subMenu_eeprom, driverCmds);

    subMenu_driver -> Insert(std::move(subMenu_eeprom));


    auto subMenu_led = make_unique< Menu >( "led_cli" );

    InsertLedCmds(subMenu_led, driverCmds);

    subMenu_driver -> Insert(std::move(subMenu_led));


    auto subMenu_fpgaUtil = make_unique< Menu >( "fpga_util" );

    InsertFpgaUtilCmds(subMenu_fpgaUtil, driverCmds);

    subMenu_driver -> Insert(std::move(subMenu_fpgaUtil));


    auto subMenu_adm1066 = make_unique< Menu >( "adm1066_cli" );

    InsertAdm1066Cmds(subMenu_adm1066, driverCmds);

    subMenu_driver -> Insert(std::move(subMenu_adm1066));


    rootMenu -> Insert( std::move(subMenu_driver) );

//////////////////////////////////////////////////////////////////////////

    Cli cli( std::move(rootMenu) );

    // global exit action
    cli.ExitAction( [](auto& out){ out << "Goodbye from cli!\n"; } );
#if 0
    // local telnet session
    CliLocalTerminalSession localSession(cli, ios, std::cout, 200);
    localSession.ExitAction(
        [&ios](auto& out) // session exit action
        {
            out << "Closing App...\n";
            ios.stop();
        }
    );
#endif

    // setup server
    CliTelnetServer server(ios, 5002, cli);
    // exit action for all the connections
    server.ExitAction( [](auto& out) { out << "Terminating this session...\n"; } );
    ios.run();

    return 0;
}

