//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#include <chrono>
#include <thread>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include <ctype.h>

#include "InfnLogger.h"
#include "board_common_driver.h"
#include "RegIfException.h"
#include "board_defs.h"


const uint32 cHbLatchEnDly      = 100; // ms
const uint32 cMezzPwrEnDly      = 100; // ms
const uint32 cMezzDisableRstDly = 100; // ms
const uint32 cLedRegWaitDly     = 100; // ms
const uint32 cClockRstDly       =  500; // 15; // ms
const uint32 cClockCfgDly       = 1000; //300; // ms
const uint32 cGbResetDelayTime  =   5; // seconds


BoardCommonDriver::BoardCommonDriver(bool isSim, bool isEval)
  : mspFpgaPlRegIf(nullptr)
  , mspFpgaPlI2c0RegIf(nullptr)
  , mspFpgaPlI2c1RegIf(nullptr)
  , mspFpgaPlI2c2RegIf(nullptr)
  , mspBottomMzFPC402_1RegIf(nullptr)
  , mspBottomMzFPC402_2RegIf(nullptr)
  , mspBottomMzIoExpIf(nullptr)
  , mspBottomMzSi5394Drv(nullptr)
  , mspFpgaPlI2c3RegIf(nullptr)
  , mspTopMzFPC402_1RegIf(nullptr)
  , mspTopMzFPC402_2RegIf(nullptr)
  , mspTopMzIoExpIf(nullptr)
  , mspTopMzSi5394Drv(nullptr)
  , mspFpgaPlI2c4RegIf(nullptr)
  , mspFpgaPlMdioIf(nullptr)
  , mpBcmDriver(nullptr)
  , mIsSim(isSim)
  , mIsEval(isEval)
{
    CreateGeckoDriver();

    CreateRegIf();
}

BoardCommonDriver::~BoardCommonDriver()
{

}

void BoardCommonDriver::CreateBcmDriver()
{
#ifdef ARCH_x86
    mpBcmDriver = make_unique<gearboxsim::Bcm81725Sim>();
#else
    mpBcmDriver = make_unique<gearbox::Bcm81725>();
#endif
}

// Note: disabling power will also put in reset
int BoardCommonDriver::EnableMezzPower(bool isEnable)
{
    // FPGA spec
    // 4.2.9 Register: 0x24 LATCH_SRC
    // 6:5 RW LATCH_MEZZ_RESET_L 0x0 -> set to 1 to reset
    // 7 RW LATCH_MEZZ_PWR_ENB 0x1 -> put to 0 to disable the power
    // Do read first and only modify bit 5=0,6=0,7=0

    uint32 data;
    int retVal = -1;
    try
    {
        if (isEnable)
        {
            // 4.2.9 Register: 0x24 LATCH_SRC
            data = mspFpgaPlMiscIf->Read32(cFpgaLatchSrcReg);
            data |= cFpgaLatchSrcReg_mezz_power_en_mask;
            mspFpgaPlMiscIf->Write32(cFpgaLatchSrcReg, data);

            std::this_thread::sleep_for(std::chrono::milliseconds(cMezzPwrEnDly));
        }
        else
        {
            // Disable Power to both mezz board and put in reset...

            data = mspFpgaPlMiscIf->Read32(cFpgaLatchSrcReg);
            data &= ~(cFpgaLatchSrcReg_mezz_power_en_mask);
            data &= ~(cFpgaLatchSrcReg_mezz_reset_l_mask);  // putting in reset also
            mspFpgaPlMiscIf->Write32(cFpgaLatchSrcReg, data);
        }

        retVal = 0;
    }
    catch (regIf::RegIfException &ex)
    {
        INFN_LOG(SeverityLevel::error) << "Error: Fpga Access Failed. Ex: " << ex.GetError();
    }
    catch(exception& ex)
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception: " << ex.what();
    }
    catch( ... )
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception";
    }

    return retVal;
}

int BoardCommonDriver::LatchEnable()
{
    int retVal = -1;

    try
    {
        uint32 data;
        data = mspFpgaPlMiscIf->Read32(cFpgaLatchCtrlReg);
        data |= 0x01;
        mspFpgaPlMiscIf->Write32(cFpgaLatchCtrlReg, data);

        std::this_thread::sleep_for(std::chrono::milliseconds(cHbLatchEnDly));

        retVal = 0;
    }
    catch (regIf::RegIfException &ex)
    {
        INFN_LOG(SeverityLevel::error) << "Error: Fpga Access Failed. Ex: " << ex.GetError();
    }
    catch(exception& ex)
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception: " << ex.what();
    }
    catch( ... )
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception";
    }

    return retVal;
}

int BoardCommonDriver::EnableLedControl(boardMs::mezzBoardSpecType brdId)
{
    int retVal = -1;

    // 4.2.18 Register: 0x60[2,0x10] LED_CNTL
    try
    {
        if((brdId == boardMs::MEZZ_BRD_SPEC_BTM) || (brdId == boardMs::MEZZ_BRD_SPEC_BOTH))
        {
            mspFpgaPlMiscIf->Write32(cFpgaLedCntlReg_1, 0x101);
        }

        if((brdId == boardMs::MEZZ_BRD_SPEC_TOP) || (brdId == boardMs::MEZZ_BRD_SPEC_BOTH))
        {
            mspFpgaPlMiscIf->Write32(cFpgaLedCntlReg_2, 0x101);
        }

        retVal = 0;
    }
    catch (regIf::RegIfException &ex)
    {
        INFN_LOG(SeverityLevel::error) << "Error: Fpga Access Failed. Ex: " << ex.GetError();
    }
    catch(exception& ex)
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception: " << ex.what();
    }
    catch( ... )
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception";
    }

    return retVal;
}

int BoardCommonDriver::EnableMezzReset(boardMs::mezzBoardSpecType brdId, bool isResetEnable)
{
    uint32 data;

    int retVal = -1;

    try
    {
        if (isResetEnable)
        {
            if (brdId != boardMs::MEZZ_BRD_SPEC_BOTH)
            {
                //   todo: support single mezz reset enable?
                INFN_LOG(SeverityLevel::error) << "Only both mezz reset supported for now";
            }

            INFN_LOG(SeverityLevel::info) << "Putting Mezz Board in Reset";

            data = mspFpgaPlMiscIf->Read32(cFpgaLatchSrcReg);
            data &= ~(cFpgaLatchSrcReg_mezz_reset_l_mask);
            mspFpgaPlMiscIf->Write32(cFpgaLatchSrcReg, data);
        }
        else
        {
            INFN_LOG(SeverityLevel::info) << "Taking Mezz Board out of Reset";

            // 4.2.9 Register: 0x24 LATCH_SRC
            const uint32 cFpgaLatchSrcReg_bottom_mezz_reset_mask = (0x1 << 5);
            const uint32 cFpgaLatchSrcReg_top_mezz_reset_mask = (0x2 << 5);

            data = mspFpgaPlMiscIf->Read32(cFpgaLatchSrcReg);

            if((brdId == boardMs::MEZZ_BRD_SPEC_BTM) || (brdId == boardMs::MEZZ_BRD_SPEC_BOTH))
            {
                data |= cFpgaLatchSrcReg_bottom_mezz_reset_mask;
            }
            if((brdId == boardMs::MEZZ_BRD_SPEC_TOP) || (brdId == boardMs::MEZZ_BRD_SPEC_BOTH))
            {
                data |= cFpgaLatchSrcReg_top_mezz_reset_mask;
            }

            mspFpgaPlMiscIf->Write32(cFpgaLatchSrcReg, data);

            std::this_thread::sleep_for(std::chrono::milliseconds(cMezzDisableRstDly));
        }

        retVal = 0;
    }
    catch (regIf::RegIfException &ex)
    {
        INFN_LOG(SeverityLevel::error) << "Error: Fpga Access Failed. Ex: " << ex.GetError();
    }
    catch(exception& ex)
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception: " << ex.what();
    }
    catch( ... )
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception";
    }

    return retVal;
}

int BoardCommonDriver::PollUntilFpgaLedReady(boardMs::mezzBoardSpecType brdId)
{
    uint32 data1 = 0;
    uint32 data2 = 0;

    int retVal = -1;
    try
    {
        const uint32 cMaxWaitTime = 5000;  // ms

        uint32 maxIt = cMaxWaitTime/cLedRegWaitDly;
        uint32 i = 0;
        do
        {
            if (brdId == boardMs::MEZZ_BRD_SPEC_BOTH)
            {
                data1 = mspFpgaPlMiscIf->Read32(cFpgaLedCntlReg_1);
                data2 = mspFpgaPlMiscIf->Read32(cFpgaLedCntlReg_2);
            }
            else if (brdId == boardMs::MEZZ_BRD_SPEC_BTM)
            {
                data1 = mspFpgaPlMiscIf->Read32(cFpgaLedCntlReg_1);
                data2 = 0x111; // FPGA set bit4 status after address config done
            }
            else if (brdId == boardMs::MEZZ_BRD_SPEC_TOP)
            {
                data1 = 0x111;
                data2 = mspFpgaPlMiscIf->Read32(cFpgaLedCntlReg_2);
            }

            std::cout << "data1 = 0x" << std::hex << data1 << "\t"
                      << " data2 = 0x" << std::hex << data2 << std::dec << std::endl;

            INFN_LOG(SeverityLevel::info) << "data1 = 0x" << std::hex << data1 << "\t"
                    << " data2 = 0x" << std::hex << data2 << std::dec;

            std::this_thread::sleep_for(std::chrono::milliseconds(cLedRegWaitDly));

            if (++i > maxIt)
            {
                break;
            }

        }while( ((data1 &= 0x111) != 0x111) || ((data2 &= 0x111) != 0x111) );

        if (i <= maxIt)
        {
            retVal = 0;
        }
    }
    catch (regIf::RegIfException &ex)
    {
        INFN_LOG(SeverityLevel::error) << "Error: Fpga Access Failed. Ex: " << ex.GetError();
    }
    catch(exception& ex)
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception: " << ex.what();
    }
    catch( ... )
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception";
    }

    return retVal;
}

int BoardCommonDriver::SetFruFaultLedFlashRed()
{
    const uint32 cFpgaLedReg3_FRU_FAULT_LED_Addr = cFpgaLedReg4;

    const uint32 cFpgaLedReg3_FRU_FAULT_bitpos = 0;
    const uint32 cFpgaLedReg3_FRU_FAULT_mask   = (0b111 << cFpgaLedReg3_FRU_FAULT_bitpos);

    const uint32 cFpgaLedReg_FRU_FAULT_FLASHING_RED_bits = (0b101 << cFpgaLedReg3_FRU_FAULT_bitpos);

    int retVal = -1;

    try
    {
        uint32 data = mspFpgaPlMiscIf->Read32(cFpgaLedReg3_FRU_FAULT_LED_Addr);
        data &= ~cFpgaLedReg3_FRU_FAULT_mask;
        data |= cFpgaLedReg_FRU_FAULT_FLASHING_RED_bits;
        mspFpgaPlMiscIf->Write32(cFpgaLedReg3_FRU_FAULT_LED_Addr, data);

        retVal = 0;
    }
    catch (regIf::RegIfException &ex)
    {
        INFN_LOG(SeverityLevel::error) << "Error: Fpga Access Failed. Ex: " << ex.GetError();
    }
    catch(exception& ex)
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception: " << ex.what();
    }
    catch( ... )
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception";
    }

    return retVal;
}

int BoardCommonDriver::SetFruActiveLedFlashGreen()
{
    const uint32 cFpgaLedReg3_FRU_ACTIVE_LED_Addr = cFpgaLedReg3_1;

    const uint32 cFpgaLedReg3_FRU_ACTIVE_bitpos = 8;
    const uint32 cFpgaLedReg3_FRU_ACTIVE_mask   = (0b111 << cFpgaLedReg3_FRU_ACTIVE_bitpos);

    const uint32 cFpgaLedReg_FRU_ACTIVE_FLASHING_GREEN_bits  = (0b101 << cFpgaLedReg3_FRU_ACTIVE_bitpos);

    int retVal = -1;

    try
    {
        uint32 data = mspFpgaPlMiscIf->Read32(cFpgaLedReg3_FRU_ACTIVE_LED_Addr);
        data &= ~cFpgaLedReg3_FRU_ACTIVE_mask;
        data |= cFpgaLedReg_FRU_ACTIVE_FLASHING_GREEN_bits;
        mspFpgaPlMiscIf->Write32(cFpgaLedReg3_FRU_ACTIVE_LED_Addr, data);

        retVal = 0;
    }
    catch (regIf::RegIfException &ex)
    {
        INFN_LOG(SeverityLevel::error) << "Error: Fpga Access Failed. Ex: " << ex.GetError();
    }
    catch(exception& ex)
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception: " << ex.what();
    }
    catch( ... )
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception";
    }

    return retVal;
}

uint32 BoardCommonDriver::GetResetCause()
{
#ifdef ARCH_x86
    INFN_LOG(SeverityLevel::info) << "Sim - Returning 0";
    return 0;
#else
    if (mIsSim)
    {
        INFN_LOG(SeverityLevel::info) << "Sim - Returning 0";
        return 0;
    }
    else
    {
        // todo: Need to verify Gecko Comm before making this call
        //       If Gecko is not communicating, this call will trigger a cold boot init
        //       Need a way to address this. The call should probably throw exception in failure case

        INFN_LOG(SeverityLevel::info) << "Calling Gecko SD_GetResetCause";
        return CpuGeckoIntfService::Instance()->SD_GetResetCause();
    }
#endif
}

bool BoardCommonDriver::ClrResetCause()
{
    INFN_LOG(SeverityLevel::info) << "";

#ifdef ARCH_x86
    return true;
#else
    if (mIsSim)
    {
        INFN_LOG(SeverityLevel::info) << "Sim. Doing nothing, Returning true.";
        return true;
    }
    else
    {
        // todo: handle Gecko Access Failures!!

        int retVal = CpuGeckoIntfService::Instance()->SD_ClearResetCause();
        if (retVal != 0)
        {
            // Todo: Add retries then raise gecko comm failure fault and fail init

            INFN_LOG(SeverityLevel::error) << "Error: SD_ClearResetCause failed!! retVal: " << (uint32)retVal;
            return false;
        }
        return true;
    }
#endif

}

/*
 * FPGA Misc Status and FGPA IOExp Wrapper Functions
 */
int BoardCommonDriver::GetFpgaMiscStatus(uint32 &regVal)
{
    uint32 regOffset = boardMs::cFpgaMiscStatusRegAddr;

    int errCode = -1;

    try
    {
        regVal = mspFpgaPlMiscIf->Read32(regOffset);
        errCode = 0;
    }
    catch (regIf::RegIfException &ex)
    {
        INFN_LOG(SeverityLevel::error) << "Error: Fpga Access Failed. Ex: " << ex.GetError();
    }
    catch(exception& ex)
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception: " << ex.what();
    }
    catch( ... )
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception";
    }

    return errCode;
}

int BoardCommonDriver::GetFpgaIoExpInput(boardMs::mezzBoardIdType boardId, uint32 &regVal)
{
    const uint32 cFpgaCommandByteVal    = 0x80;    // Read Port 0, Auto Incr Enabled
    const uint32 cFpgaCommandByteOffset = 0;

    shared_ptr<FpgaI2cIf> spMezzIoExpIf;
    if (boardId == boardMs::MEZZ_BRD_TOP)
    {
        spMezzIoExpIf = mspTopMzIoExpIf;
    }
    else
    {
        spMezzIoExpIf = mspBottomMzIoExpIf;
    }

    int errCode = -1;

    try
    {
        spMezzIoExpIf->Write8(cFpgaCommandByteOffset,cFpgaCommandByteVal);

        // Read byte from each of 3 ports to capture all inputs
        //    (note auto increment is enabled)
        uint32 allPortVals = ( (spMezzIoExpIf->Read8(cFpgaCommandByteOffset    )      ) |
                               (spMezzIoExpIf->Read8(cFpgaCommandByteOffset + 1) << 8 ) |
                               (spMezzIoExpIf->Read8(cFpgaCommandByteOffset + 2) << 16));

        regVal = allPortVals;

        errCode = 0;
    }
    catch (regIf::RegIfException &ex)
    {
        INFN_LOG(SeverityLevel::error) << "Error: Fpga Access Failed. Ex: " << ex.GetError();
    }
    catch(exception& ex)
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception: " << ex.what();
    }
    catch( ... )
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception";
    }

    return errCode;
}


///////////////////////////////////////////////////////////////////////////

/*
 * Create register interfaces
 */
void BoardCommonDriver::CreateRegIf()
{
    RegIfFactory* pFactory = RegIfFactorySingleton::Instance();

    mspFpgaPlRegIf = pFactory->CreateFpgaPlRegIf();

    mspFpgaPlMiscIf = pFactory->CreateFpgaMiscRegIf();

    mspFpgaPlI2c0RegIf = pFactory->CreateFpgaPlI2cRegIf(PlI2cBus0);

    mspFpgaPlI2c1RegIf = pFactory->CreateFpgaPlI2cRegIf(PlI2cBus1);

    mspFpgaPlI2c2RegIf = pFactory->CreateFpgaPlI2cRegIf(PlI2cBus2);

    mspBottomMzFPC402_1RegIf = pFactory->CreateBottomMzFPC402_1RegIf();

    mspBottomMzFPC402_2RegIf = pFactory->CreateBottomMzFPC402_2RegIf();

    mspBottomMzIoExpIf = pFactory->CreateBottomMzIOExpanderRegIf();

    mspBottomMzSi5394Drv = pFactory->CreateBottomMzSi5394Drv();

    mspFpgaPlI2c3RegIf = pFactory->CreateFpgaPlI2cRegIf(PlI2cBus3);

    mspTopMzFPC402_1RegIf = pFactory->CreateTopMzFPC402_1RegIf();

    mspTopMzFPC402_2RegIf = pFactory->CreateTopMzFPC402_2RegIf();

    mspTopMzIoExpIf = pFactory->CreateTopMzIOExpanderRegIf();

    mspTopMzSi5394Drv = pFactory->CreateTopMzSi5394Drv();

    mspFpgaPlI2c4RegIf = pFactory->CreateFpgaPlI2cRegIf(PlI2cBus4);

    mspFpgaPlMdioIf = pFactory->CreateFpgaPlMdioRegIf();
}

void BoardCommonDriver::CreateGeckoDriver()
{
    INFN_LOG(SeverityLevel::info) << "";
#ifdef ARCH_x86
    INFN_LOG(SeverityLevel::info) << "Sim - Doing nothing";
#else
    if (mIsSim)
    {
        INFN_LOG(SeverityLevel::info) << "Sim - Doing nothing";
    }
    else
    {
        CpuGeckoIntf* pIntf = new CpuGeckoIntf();
        CpuGeckoIntfService::InstallInstance(pIntf);

        geckoStubRa *raStub = new geckoStubRa();
        RaStubService::InstallInstance(raStub);
    }

    INFN_LOG(SeverityLevel::info) << "*** Done";
#endif
}

int BoardCommonDriver::InitIoExp(boardMs::mezzBoardIdType boardId)
{
    INFN_LOG(SeverityLevel::info) << "7. Mezz Board " << (uint32)boardId << " Configure IO Expander ...";

    shared_ptr<FpgaI2cIf> spMezzIoExpIf;
    if (boardId == boardMs::MEZZ_BRD_TOP)
    {
        spMezzIoExpIf = mspTopMzIoExpIf;
    }
    else
    {
        spMezzIoExpIf = mspBottomMzIoExpIf;
    }

    int retVal = -1;
    try
    {
        // iox on Mezz - write 0 to registers 4,5,6
        spMezzIoExpIf->Write8(0x4, 0x0);
        spMezzIoExpIf->Write8(0x5, 0x0);
        spMezzIoExpIf->Write8(0x6, 0x0);

        // iox on Mezz - write c=3e, d=fc, e=e0
        spMezzIoExpIf->Write8(0xc, 0x3e);
        spMezzIoExpIf->Write8(0xd, 0xfc);
        spMezzIoExpIf->Write8(0xe, 0xe0);

        retVal = 0;
    }
    catch (regIf::RegIfException &ex)
    {
        INFN_LOG(SeverityLevel::error) << "Error: Fpga Access Failed. Ex: " << ex.GetError();
    }
    catch(exception& ex)
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception: " << ex.what();
    }
    catch( ... )
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception";
    }

    return retVal;
}

int BoardCommonDriver::InitClock(boardMs::mezzBoardIdType boardId)
{
    INFN_LOG(SeverityLevel::info) << "8.  Mezz Board " << (uint32)boardId << " Configure Si5394 clock ...";

    shared_ptr<FpgaI2cIf> spMezzIoExpIf;
    shared_ptr<Si5394>    spMezzSiDrvIf;
    if (boardId == boardMs::MEZZ_BRD_TOP)
    {
        spMezzIoExpIf = mspTopMzIoExpIf;
        spMezzSiDrvIf = mspTopMzSi5394Drv;
    }
    else
    {
        spMezzIoExpIf = mspBottomMzIoExpIf;
        spMezzSiDrvIf = mspBottomMzSi5394Drv;
    }

    int retVal = -1;
    try
    {
        //take si5394 out of reset
        spMezzIoExpIf->Write8(0x4, 0x1);

        std::this_thread::sleep_for(std::chrono::milliseconds(cClockRstDly));

        uint32 regVal = spMezzSiDrvIf->Read(0x02);
        INFN_LOG(SeverityLevel::info) << "Mezz Board " << (uint32)boardId << " Si5394 Read offset 0x02 = 0x" << std::hex << regVal << std::dec;

        spMezzSiDrvIf->Configure(msSi5394_RegList);

        std::this_thread::sleep_for(std::chrono::milliseconds(cClockCfgDly));

        spMezzSiDrvIf->ClearStatusBits();

        retVal = 0;
    }
    catch (regIf::RegIfException &ex)
    {
        INFN_LOG(SeverityLevel::error) << "Error: Fpga Access Failed. Ex: " << ex.GetError();
    }
    catch(exception& ex)
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception: " << ex.what();
    }
    catch( ... )
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception";
    }

    return retVal;
}

int BoardCommonDriver::InitBcm(boardMs::mezzBoardIdType boardId)
{
    INFN_LOG(SeverityLevel::info) << "9. Mezz Board " << (uint32)boardId
        << " Take Gearbox out of reset write IOX 0x04=C1, 5=0x01 ...";

    shared_ptr<FpgaI2cIf> spMezzIoExpIf;
    uint32    mezzMdioBusId;
    if (boardId == boardMs::MEZZ_BRD_TOP)
    {
        spMezzIoExpIf  = mspTopMzIoExpIf;
        mezzMdioBusId = cTopMezzMdioBus;
    }
    else
    {
        spMezzIoExpIf  = mspBottomMzIoExpIf;
        mezzMdioBusId = cBottomMezzMdioBus;
    }

    int retVal = -1;
    try
    {
        spMezzIoExpIf->Write8(0x4, 0xC1);
        spMezzIoExpIf->Write8(0x5, 0x01);

        sleep(cGbResetDelayTime);

        INFN_LOG(SeverityLevel::info) << "10. Mezz Board " << (uint32)boardId << " Read mdio register from bcm#1 ...";

        /*
         * param1 "bussel": 0 - bottom Mezz card; 1 - top Mezz card
         * param2 "mdio_address" : 0x4, 0x8, 0xc
         * param3 "offset": 0x1eb2c8
         * return: Gear box port ID
         */

        uint32 data;
        data = mspFpgaPlMdioIf->Read16(mezzMdioBusId, cBCM81725_1MdioAddress, cBCM81725PortIdOffset);
        INFN_LOG(SeverityLevel::info) << "10.1. Mezz Board " << (uint32)boardId << " cBCM81725_1 port Id = 0x" << std::hex << data;

        data = mspFpgaPlMdioIf->Read16(mezzMdioBusId, cBCM81725_2MdioAddress, cBCM81725PortIdOffset);
        INFN_LOG(SeverityLevel::info) << "10.2. Mezz Board " << (uint32)boardId << " cBCM81725_2 port Id = 0x" << std::hex << data;;

        data = mspFpgaPlMdioIf->Read16(mezzMdioBusId, cBCM81725_3MdioAddress, cBCM81725PortIdOffset);
        INFN_LOG(SeverityLevel::info) << "10.3. Mezz Board " << (uint32)boardId << " cBCM81725_3 port Id = 0x" << std::hex << data << std::dec;

        //TODO: load firmware

        retVal = 0;

    }
    catch (regIf::RegIfException &ex)
    {
        INFN_LOG(SeverityLevel::error) << "Error: Fpga Access Failed. Ex: " << ex.GetError();
    }
    catch(exception& ex)
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception: " << ex.what();
    }
    catch( ... )
    {
        INFN_LOG(SeverityLevel::error) << "Caught Exception";
    }

    return retVal;
}

int BoardCommonDriver::InitBcmDriver(boardMs::mezzBoardSpecType boardId)
{
    // Check if tmz or bmz, enter in here.
    if (NULL != mpBcmDriver)
    {
        int initStat = 0;

        INFN_LOG(SeverityLevel::info) << "Initializing gear box.";
        try
        {
            // todo: Remove this envStr translation
            //    Can BCM Driver take enum instead?

            std::string envStr;
            switch (boardId)
            {
                case boardMs::MEZZ_BRD_SPEC_TOP:
                    envStr = "tmz";
                    break;
                case boardMs::MEZZ_BRD_SPEC_BTM:
                    envStr = "bmz";
                    break;

                case boardMs::MEZZ_BRD_SPEC_BOTH:
                    envStr = "";
                    break;

                case boardMs::MEZZ_BRD_SPEC_NONE:
                    envStr = "hb";
                    break;
            }

            // pass in flag indicating HW config
            initStat = mpBcmDriver->init(envStr);
        }
        catch ( ... )
        {
            INFN_LOG(SeverityLevel::info) << "Gear box initialization failed.";

            initStat = -1;
        }

        if (initStat)
        {
            INFN_LOG(SeverityLevel::error) << "Bcm Gearbox Driver init failed. initStat: " << initStat;

            // todo: should we retry? return failure here?
        }
        else
        {
            INFN_LOG(SeverityLevel::info) << "Bcm Gearbox Driver init success";
        }

        return initStat;
    }
    else
    {
        INFN_LOG(SeverityLevel::error) << "Driver not created yet!!";

        return -1;
    }
}


void BoardCommonDriver::CheckMezzPwrSeq(boardMs::mezzBoardIdType brdId)
{
    const auto& p = RegIfFactorySingleton::Instance();
    shared_ptr<FpgaI2cIf> spMezzPwrSeq;

    if (brdId == boardMs::MEZZ_BRD_TOP)
    {
        spMezzPwrSeq = p->CreateFpgaPlI2c1VoltageSequencerRegIf();
    }
    else
    {
        spMezzPwrSeq = p->CreateFpgaPlI2c0VoltageSequencerRegIf();
    }

    for(uint32 i = 0; i < 10; i++)
    {
        std::string errLog;
        bool isPass = false;
        try
        {
            spMezzPwrSeq->Read8(cAdm1066MANID);
            isPass = true;
        }
        catch (regIf::RegIfException &ex)
        {
            errLog = "Error: Access Failed. Ex: " + std::to_string(ex.GetError());
        }
        catch(exception& ex)
        {
            std::string exStr(ex.what());
            errLog =  "Caught Exception: " + exStr;
        }
        catch( ... )
        {
            errLog = "Caught Exception";
        }

        if (!isPass)
        {
            INFN_LOG(SeverityLevel::error) << "Failure while reading ADM1066: " << errLog;
        }

        std::this_thread::sleep_for(
                std::chrono::milliseconds(10));
    }
}


