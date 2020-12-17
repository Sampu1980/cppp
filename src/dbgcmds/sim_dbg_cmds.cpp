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
extern void InsertSimAdapterCmds(unique_ptr< Menu > & adapterMenu, SimAdapterCmds& adapterCmds);

/**********************************************************
 * DbgCmds for sim platform
 **********************************************************
 */
int SimDbgCmds(simBoardAda::SimBoardAdapter& adapter, BoardManager& manager)
{
#if BOOST_VERSION < 106600
    boost::asio::io_service ios;
#else
    boost::asio::io_context ios;
#endif

    AdapterCmds adapterCmds(adapter);
    SimAdapterCmds simAdapterCmds(adapter);
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

    InsertSimAdapterCmds(subMenu_adapter, simAdapterCmds);

    rootMenu -> Insert( std::move(subMenu_adapter) );

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

