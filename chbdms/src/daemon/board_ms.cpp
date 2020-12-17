#include "board_adapter.h"
#include "sim_board_adapter.h"
#include "chm6/redis_adapter/application_servicer.h"
#include "chm6/redis_adapter/callback_handler.h"
#include "board_manager.h"
#include "board_driver.h"
#include "InfnLogger.h"

int global_exit_code = boardMs::EXIT_INVALID;

int main()
{
    InfnLogger::initLogging("chm6_board_ms");
    InfnLogger::setLoggingSeverity(SeverityLevel::info);

    INFN_LOG(SeverityLevel::info) << "******************************************************************************";
    INFN_LOG(SeverityLevel::info) << "**********************   BoardMs Starting     ********************************";

    bool SIM_MODE = false;
    char *envVal;

    if(NULL != (envVal = getenv("BoardInfo")))
    {
        string envStr(envVal);
        INFN_LOG(SeverityLevel::info) << "FOUND \"BoardInfo\"=" << envStr;
        string chm6EnvStr = envStr.substr(0,3);
        string evalEnvStr = envStr.substr(0,4);

        if((chm6EnvStr == "sim") || (evalEnvStr == "eval"))
        {
            INFN_LOG(SeverityLevel::info) << "chm6EnvStr: " << chm6EnvStr << " evalEnvStr: " << evalEnvStr << endl;
            INFN_LOG(SeverityLevel::info) << "Not CHM6 HW. Run \"SIM_MODE\" ...\n";
            SIM_MODE =  true;
        }
        else
        {
            INFN_LOG(SeverityLevel::info) << "CHM6 HW. Run \"HW_MODE\" ...\n";
            SIM_MODE =  false;
        }
    }
    else
    {
        INFN_LOG(SeverityLevel::info) << "\"BoardInfo\" env is NOT FOUND. Run \"HW_MODE\" ...\n";
        SIM_MODE =  false; // No "chm6_boardinfo" env, assume this is HW platform
    }

    char *envChassisId;
    char *envSlotNo;
    string aid("1-4");

    if( (NULL != (envChassisId = getenv("ChassisId")))
     && (NULL != (envSlotNo = getenv("SlotNo"))) )
    {
        string chassId(envChassisId);
        INFN_LOG(SeverityLevel::info) << "FOUND \"ChassisId\"=" << chassId;

        string slotNo(envSlotNo);
        INFN_LOG(SeverityLevel::info) << "FOUND \"SlotNo\"=" << slotNo;

        aid = chassId + "-" + slotNo;
        INFN_LOG(SeverityLevel::info) << "Board aid: " << aid;
    }
    else
    {
        INFN_LOG(SeverityLevel::info) << "Default Board aid: " << aid;
    }

    char *envInitDone;
    bool initDoneFlag = false;
    if( NULL != (envInitDone = getenv("InitDone")) )
    {
        string initDone(envInitDone);

        INFN_LOG(SeverityLevel::info) << "FOUND \"InitDone\"=" << initDone;

        initDoneFlag = true;
    }

    if (false ==
        AppServicerIntfSingleton::getInstance()->ServicerInit("Hi"))
    {
        INFN_LOG(SeverityLevel::error) << "AppServicerIntfSingleton ServicerInit Failure (failed to connect to Redis). Exiting with Error...";

        exit(EXIT_ERROR);
    }

    // Create manager class with group of poller threads: faults, PM, other status
    std::unique_ptr<BoardManager> manager = std::make_unique<BoardManager>(SIM_MODE, aid, initDoneFlag);

    InfnLogger::flushLogger();

    // Don't Exit
    while (1)
    {
        if (global_exit_code != boardMs::EXIT_INVALID)
        {
            INFN_LOG(SeverityLevel::info) << "********************** global_exit_code = " << global_exit_code << " **********************";
            std::cout << "********************** global_exit_code = " << global_exit_code << " **********************" << std::endl;
            break;
        }
        sleep(1);
    }
    _exit(global_exit_code);
//    return global_exit_code;
}
