//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#include <iostream>
#include "InfnLogger.h"
#include "board_init_manager.h"

using namespace std;


int main()
{
    cout << "********************** Board Init Ms Starting ********************************" << endl;

    InfnLogger::initLogging("chm6_board_init_ms");
    InfnLogger::setLoggingSeverity(SeverityLevel::info);

    INFN_LOG(SeverityLevel::info) << "******************************************************************************";
    INFN_LOG(SeverityLevel::info) << "********************** BoardInitMs Starting ********************************";

    BoardInitManager *pInitMgr = new BoardInitManager();

    int err_code = pInitMgr->initialize();

    INFN_LOG(SeverityLevel::info) << "********************** BoardInitMs Exiting with err code: " << err_code;

    delete pInitMgr;

    _exit(err_code);
}


