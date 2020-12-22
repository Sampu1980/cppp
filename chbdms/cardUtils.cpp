/*
 *  src_ne/sys/mcm/SysInit/SysInit.cpp
 *
 *  Copyright(c) Infinera 2002-2014
 */
#include <SysCommon/CtplCfg.h>
#include <SysCommon/SystemSemaphore.h>
#include <OsEncap/OsSystem.h>
#include <Constant/SysFilter.h>
#include <ZProcess/ZProcessPublic.h>
#include <SsmCoordinator/SsmCoordinator.h>
#include <Ics/IcsLocalDomainService.h>
#include <Util/SysLog.h>
#include <Util/Dbc.h>
#include <Util/StrUtils.h>
#include <ControllerSysInit/ControllerSysInit.h>
#include <OsEncap/OsProcessHandle.h>

#define _DIR_SLASH "/"
#include <unistd.h>
#define _RUNNABLE_APP(APP_PATH) (0 ==  access((APP_PATH),X_OK))
#define _MY_PID_FILE        "/tmp/SysInit.pid"
#define _GETMYPID           getpid()
#define _IS_ANOTHER_SYSINIT_RUNNING another_sysinit_running()
#define _AM_I_RUNNING       am_i_running()
#define _UNLINK_MY_PID_FILE     (void)unlink(_MY_PID_FILE)
#define _WRITE_MY_PID_FILE \
do { \
    char cmd[1024]; \
    snprintf(cmd, sizeof(cmd), "echo %d > %s", _GETMYPID, _MY_PID_FILE); \
    (void) system(cmd); \
    gIamIt = true; \
} while(0);

namespace mcm_SysInit
{
    bool gIamIt = false;

    bool another_sysinit_running()
    {
        FILE* fp = fopen(_MY_PID_FILE, "r");
        if (!fp)
            return false;

        fclose(fp);
        return true;
    }

    bool am_i_running()
    {
        return gIamIt;
    }

    const uint32 UR_WAITING_TIME = 120000;  // 2 min waiting time
};   // namespace mcm_SysInit

using namespace mcm_SysInit;

ControllerSysInit::ControllerSysInit( const char* pModuleName,      // Should pass in "SysInit"
                  std::string sFilter,
                  Platform    platform,
                  CardType    card,
                  const char* pSsmStr,
                  SysInitPlatformIf* myPIf,
                  std::vector<struct ControllerSysInit::AppProc>& pAppProcList)
    : 
    ZProcess( pModuleName ), 
    mNoKillFlag(false), 
    mNoSpawnFlag(false),
    mStandbySpawned(false),
    mExtraFilter(""),
    mSmFilter(sFilter),
    mPlatform(platform),
    mCardType(card),
    mpSsmStr(pSsmStr),
    mSysInitPlatformIf(myPIf),
    mpAppProcList(pAppProcList),
    mAppProcListSize(pAppProcList.size()),
    mSystemState(SYSTEM_INIT),
    mURWaitTime(UR_WAITING_TIME),
    mIcsAppId(0),
    IsNodeControllerInitial(false)
{
    TRC_INIT_LOCAL(0);

    ZAssert::SetThisProcessIsCritical();
    if (GetCardType() == CARD_NC)
    {
        SYSLOG(util::SEVERITY_INFO, "Is NodeController");
        IsNodeControllerInitial = true;
    }
    else if (GetCardType() == CARD_SC)
    {
        SYSLOG(util::SEVERITY_INFO, "Is ShelfController");
        IsNodeControllerInitial = false;
    }
    else { 
        SYSLOG(util::SEVERITY_INFO, "Controller UNKNOWN: !(NC or SC): Default SC");
        IsNodeControllerInitial = false;
        // default to SC
        SetCardType(CARD_SC);
    }

}


ControllerSysInit::~ControllerSysInit()
{
}

void
ControllerSysInit::AppProcessesListInit( void )
{

    char* pAppListEnv(getenv("SYSINIT_APP_LIST"));
    if (pAppListEnv)
    {
        TRC_MSG(info, 1, ("SYSINIT_APP_LIST=%s\n", pAppListEnv));

        bool runFlag;

        if (*pAppListEnv == '~')
        {
            // the list specifies the set the we should skip
            runFlag = false;
            ++pAppListEnv;
        }
        else
        {
            // the list specifies the set the we should run
            runFlag = true;
        }

        for (size_t i=0; i < mAppProcListSize; ++i)
        {
            // Initialize run flag
            mpAppProcList[i].mRun = !runFlag;
        }

        char* p = strtok( pAppListEnv, "," );
        while (p)
        {
            for (size_t i=0; i < mAppProcListSize; ++i)
            {
            if (strcmp(p, mpAppProcList[i].mpName) == 0) {
                // run or don't run this app.
                mpAppProcList[i].mRun = runFlag;
                break;
            }
            }

            p = strtok(NULL, ",");
        }
    }

    // Memcheck list
    char* pMemCheckAppListEnv(getenv("SYSINIT_MEMCHECK_APP_LIST"));

    if (pMemCheckAppListEnv)
    {
        TRC_MSG(all, 1, ("SYSINIT_MEMCHECK_APP_LIST=%s\n", pMemCheckAppListEnv));

        bool memCheckFlag;

        if (*pMemCheckAppListEnv == '~')
        {
            // the list specifies the set the we should skip
            memCheckFlag = false;
            ++pMemCheckAppListEnv;
        }
        else
        {
            // the list specifies the set the we should run
            memCheckFlag = true;
        }

        for (size_t i=0; i < mAppProcListSize; ++i)
        {
            // Initialize run flag
            mpAppProcList[i].mMemCheck = !memCheckFlag;
        }

        char* p = strtok( pMemCheckAppListEnv, "," );
        while (p)
        {
            for (size_t i=0; i < mAppProcListSize; ++i)
            {
                if (strcmp(p,  mpAppProcList[i].mpName) == 0) {
                    // mem check or don't check this app.
                    mpAppProcList[i].mMemCheck = memCheckFlag;
                    break;
                }
            }

            p = strtok(NULL, ",");
        }
    }
    DumpAppProcList();
}

std::string
ControllerSysInit::ControllerStateStr( const enum SystemState state )
{
    std::string rc = "UNKNOWN";
    switch(state) { 
        case SYSTEM_INIT:
           rc = "INIT";
           break;
        case SYSTEM_INIT2:
           rc = "INIT2";
           break;
        case SYSTEM_CONTROL_PLANE_READY:
           rc = "CONTROL_PLANE_READY";
           break;
        case SYSTEM_CONTROLLER_ACTIVE:
           rc = "CONTROLLER_ACTIVE";
           break;
        case SYSTEM_STANDBY:
           rc = "SYSTEM_STANDBY";
           break;
        case SYSTEM_STANDBY_TO_ACTIVE:
           rc = "SYSTEM_STANDBY_TO_ACTIVE";
           break;
        case SYSTEM_ACTIVE:
           rc = "SYSTEM_ACTIVE";
           break;
        default:
           rc = "UNKNOWN";
    }
    return rc; 
}

std::string
ControllerSysInit::DumpAppProcList( void )
{
    std::ostringstream ostr;
    SYSLOG_CONSOLE(util::SEVERITY_INFO, "AppProcessList:SysInit: Entries:" << mpAppProcList.size() << endl);
    for (size_t i=0; i < mpAppProcList.size(); ++i)
    {
        ostr << "Id:" << i << \
        " Name:" << mpAppProcList[i].mpName << \
        " Run:" << (mpAppProcList[i].mRun ? "True " : "False ") << \
        " State:" << ControllerStateStr(mpAppProcList[i].mStartingState) << std::ends;
        SYSLOG(util::SEVERITY_INFO, ostr.str() << endl);
        ostr.seekp(0);
    }
    return ostr.str();
}

void 
ControllerSysInit::SetPlatformAppProcessesList(std::vector<ControllerSysInit::AppProc>& pAppProcList)
{
    mpAppProcList = pAppProcList;
    mAppProcListSize = pAppProcList.size();
    AppProcessesListInit();
}

void
ControllerSysInit::SpawnApplicationProcesses( void )
{
    if (mNoSpawnFlag)
    {
        SYSLOG_CONSOLE(util::SEVERITY_WARNING, "WARNING -- SysInit is not spawning application processes for debugging purpose!!!.");
        SYSLOG_CONSOLE(util::SEVERITY_WARNING, "The following executables are supposed to be started by SysInit:");
    }

    //Starting  processes
    for (size_t i = 0; i < mAppProcListSize; i++)
    {
        std::string ExecPath = mBinPath;
        ExecPath += mpAppProcList[i].mpName;
        if (mpAppProcList[i].mRun
                && (mpAppProcList[i].mPlatform == PLAT_ALL
                    || mpAppProcList[i].mPlatform == mPlatform)
                && (mpAppProcList[i].mCardType == CARD_ALL
                    || mpAppProcList[i].mCardType == GetCardType())
                && mpAppProcList[i].mStartingState == GetSystemState()
           )
        {
            if (mNoSpawnFlag)
            {
                SYSLOG_CONSOLE(util::SEVERITY_INFO, ExecPath.c_str());
            }
            else
            {
                int runMask = 0;    // Used for CPU core allocation.  Refer to QNX man page.
                if (mpAppProcList[i].mMemCheck)
                {
                    setenv("MALLOC_INF_DEBUG", "1", 1);
                }
                HandleRuntimeProcessBeforeSpawn(mpAppProcList[i]);
                char buf[16];
                setenv("NDDS_DEBUG_LEVEL", itoa(mpAppProcList[i].mNddsDebugLevel,buf, 10), 1);
                if (mpAppProcList[i].mArgc == 0)
                {
                    SpawnProcess( ExecPath.c_str(), mpAppProcList[i].mIgnoreDeath, runMask );
                }
                else
                {
                    SpawnProcess( ExecPath.c_str(), mpAppProcList[i].mArgc, mpAppProcList[i].mArgv, mpAppProcList[i].mIgnoreDeath, runMask );
                }

                if (mpAppProcList[i].mMemCheck)
                {
                    setenv("MALLOC_INF_DEBUG", 0, 1);
                }
                setenv("NDDS_DEBUG_LEVEL", "0", 1);
            }
        }

    }
}

void
ControllerSysInit::PrintHelp(void)
{
    cout <<
    "SysInit allows only one optional parameter - [<nokill>|<nospawn>]\n"
    "    nokill - to disable auto killing of child tasks on error\n"
    "    nospawn - to disable spawning of application processes for debugging\n";
}

void
ControllerSysInit::StartProcess()
{
    SYSLOG(util::SEVERITY_INFO, "ControllerSysInit::StartProcess()");
    ControllerSysInit_StartProcess();
}

void
ControllerSysInit::ControllerSysInit_StartProcess()
{
    bool IsNodeControllerNow = false;
    if (GetCardType() == CARD_NC)
    {
        IsNodeControllerNow = true;
    }
    else if (GetCardType() == CARD_SC)
    {
        IsNodeControllerNow = false;
    }
    if (IsNodeControllerNow != IsNodeControllerInitial)
    {
        SYSLOG_CONSOLE( util::SEVERITY_INFO,
            "Detected change in SSM type: rebooting: IsNodeControllerNow != IsNodeControllerInitial");
        OsSystem::Reboot();
    }
    mSysInitPlatformIf->PlatformOnSystemStart();
    SetSystemState(SYSTEM_CONTROL_PLANE_READY);
    //system("tcpdump -p -w /dev/shmem/xcm.d -s 65535 &");
    SpawnApplicationProcesses();
    SetState(ZProcessPublic::InitState());
    SystemSemaphoreService::InstallInstance(new SystemSemaphore());
    SYSLOG(util::SEVERITY_INFO, "ControllerSysInit::StartProcess()");
}

void
ControllerSysInit::Start()
{
  // When Platform SysInit is Derived from Controller_SysInit
  // This virtual Start API is implemented by the Platform.
  // The Platform SysInit Start() calls ControllerSysInit_Start when the 
  // Platform SysInit initialization is completed.
  SYSLOG(util::SEVERITY_INFO, "ControllerSysInit::Start() !!! Wrong start");
}

void
ControllerSysInit::ControllerSysInit_Start()
{
    if( mArgc > 2 )
    {
        PrintHelp();
        exit(0);
    }

    // If one arg was passed, check option name.
    if ( mArgc == 2 )
    {
        if ( strcmp( mArgv[1], "nokill" ) == 0 )
        {
            mNoKillFlag = true;
        }
        else if ( strcmp( mArgv[1], "nospawn" ) == 0 )
        {
            mNoSpawnFlag = true;
        }
        else
        {
            PrintHelp();
            exit(0);
        }

    }
    if (_IS_ANOTHER_SYSINIT_RUNNING)
    {
        REQUIRE_STRM(0,"SysInit","Another SysInit is already running");
        TRC_MSG(err, 1, ("Another SysInit is already running.\n"));
        exit(0);
    }

    _WRITE_MY_PID_FILE;

    // Get work area environment variable name.
    std::string workAreaName = ZSysCfg::GetBaseDirEnvVar();

    // Verify that work area environment exists.
    REQUIRE_STRM( ( getenv( workAreaName.c_str() ) != NULL ), OsPublic::OsMn(),
        "Can't get environment variable ["
        << workAreaName << "] from execution parameter or environment" );

    char   pbf[255];
    (void)ZSysCfg::GetInsLinksRootDir(sizeof(pbf), pbf);
    TRC_MSG(info,1,("ZSysCfg::GetInsLinksRootDir(): %s", pbf));
    mBinPath =  pbf;
    mBinPath += _DIR_SLASH;
    mBinPath += "bin";
    mBinPath += _DIR_SLASH;
    InitIcsService();

    if (GetCardType() == CARD_NC)
    {
        IsNodeControllerInitial = true;
        SYSLOG(util::SEVERITY_INFO, "Is NodeController");
    }
    else if (GetCardType() == CARD_SC)
    {
        IsNodeControllerInitial = false;
        SYSLOG(util::SEVERITY_INFO, "Is ShelfController");
    }

    //Setting up Ssm
    SYSLOG(util::SEVERITY_INFO, "Init ICS Local Domain...");
    IcsLocalDomainService::CreateInstance();
    
    SYSLOG_CONSOLE(util::SEVERITY_INFO, "Loading SSM... filer is " << mSmFilter);

    SsmCoordinatorService::InstallInstance( new SsmCoordinator(mpSsmStr, mSmFilter) );
    SYSLOG(util::SEVERITY_INFO, "Ssm is Ready.");

    InitLocalService();
    SetState("LocalInit");
}

void
ControllerSysInit::ExitHandler( bool isFailure )
{
    SYSLOG(util::SEVERITY_INFO, "Exit Handler: ControllerSysInit.");
    ZAssert::SetStatus(ZAssert::GENERIC_FAULT_ON_COURSE);
    if (_AM_I_RUNNING)
    {
        _UNLINK_MY_PID_FILE;
    }
}

bool
ControllerSysInit::ChildDeathHandler( OsProcessHandle *pProcHandle )
{
    const char *excPath = pProcHandle->GetName();
    const char *name = strrchr(excPath, '/');
    if(name == NULL)
        name = excPath;
    else
        name++;

    SYSLOG(util::SEVERITY_INFO, "ChildDeath Handler: ControllerSysInit.");
    for (size_t i=0;i < mAppProcListSize;++i)
    {
        if(strcmp(name, mpAppProcList[i].mpName) == 0 && 
           mpAppProcList[i].mRestartable)
        {
            if (mpAppProcList[i].mArgc == 0)
            {
                uint32 argc = 3;
                char *argv[] = {(char *) excPath, "-restart", NULL};
                SpawnProcess( excPath, 
                              argc, 
                              argv, 
                              mpAppProcList[i].mIgnoreDeath );
            }
            else
            {
                SpawnProcess( excPath, 
                              mpAppProcList[i].mArgc, 
                              mpAppProcList[i].mArgv, 
                              mpAppProcList[i].mIgnoreDeath );
            }
            return false;
        }
    }

    return !mNoKillFlag;
}


void
ControllerSysInit::MoveToStandby(void)
{
    SetSystemState(SYSTEM_STANDBY);
    if ( getenv( "LATE_STANDBY_START" ) )
    {
        cout << "export LATE_STANDBY_START - processes will be started later" << endl;
    }
    else
    {
        SpawnApplicationProcesses();
        mStandbySpawned = true;
    }
    SetState(ZProcessPublic::StandbyState());
}

void
ControllerSysInit::MoveFromStandbyToActive(void)
{
}

void
ControllerSysInit::MoveToActive(void)
{
    if ( !mStandbySpawned )
    {
        SetSystemState(SYSTEM_STANDBY);
        SpawnApplicationProcesses();
        mStandbySpawned = true;
    }
    SetSystemState(SYSTEM_ACTIVE);
    SpawnApplicationProcesses();
    SetState(ZProcessPublic::ActiveState());
    SYSLOG_CONSOLE(util::SEVERITY_INFO, "MoveToActive: ControllerSysInit ");
}

void
ControllerSysInit::HandleProcessAction(const char* pAction)
{
    SYSLOG_CONSOLE(util::SEVERITY_INFO, "ControllerSysInit: Handle Action is " << pAction);
    mSysInitPlatformIf->PlatformHandleProcessAction(pAction);
}

void
ControllerSysInit::SetCardType(CardType ct)
{
    mCardType = ct;
}

void
ControllerSysInit::SetSystemState(SystemState ss)
{
    mSystemState = ss;
}

ControllerSysInit::CardType
ControllerSysInit::GetCardType(void)
{
    return mCardType;
}

ControllerSysInit::SystemState
ControllerSysInit::GetSystemState(void)
{
    return mSystemState;
}
void
ControllerSysInit::DeclareNCSystemActive(void)
{
    mSysInitPlatformIf->PlatformOnNCSystemActive();
    SYSLOG_CONSOLE(util::SEVERITY_AUDIT, "Node Controller is System Active.");
    SetState("NCSystemActive");
    SYSLOG_CONSOLE(util::SEVERITY_INFO, "ControllerSysInit: NCSystemActive");
}
void
ControllerSysInit::DeclareSCSystemActive(void)
{

    mSysInitPlatformIf->PlatformOnSCSystemActive();
    SYSLOG_CONSOLE(util::SEVERITY_AUDIT, "Shelf Controller is System Active.");
    SetState("SCSystemActive");
    SYSLOG_CONSOLE(util::SEVERITY_INFO, "ControllerSysInit: SCSystemActive");
}
void
ControllerSysInit::DeclareSystemStandby(void)
{
    mSysInitPlatformIf->PlatformOnSystemStandby();
    SetState("SystemStandby");
    if (GetCardType() == CARD_NC)
    {
        SYSLOG_CONSOLE(util::SEVERITY_INFO, "Node Controller is System Standby.");
    }
    else
    {
        SYSLOG_CONSOLE(util::SEVERITY_INFO, "Shelf Controller is System Standby.");
    }
}

uint32
ControllerSysInit::GetIcsAppId()
{
	size_t  maxIndex = sizeof(msStackLimits) / sizeof(msStackLimits[0]);
    SYSLOG(util::SEVERITY_INFO, "ControllerSysInit IcsAppId:0x" << hex << mIcsAppId);
    return mIcsAppId;
}


