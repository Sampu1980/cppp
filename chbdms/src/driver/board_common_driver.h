//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#ifndef CHM6_BOARD_MS_SRC_DRIVER_HOST_BOARD_COMMON_DRIVER_H_
#define CHM6_BOARD_MS_SRC_DRIVER_HOST_BOARD_COMMON_DRIVER_H_

#include <iostream>
#include <string>
#include <mutex>
#include <vector>
#include <map>

#include "types.h"

#include "RegIfFactory.h"

#include "Bcm81725.h"
#include "Bcm81725Sim.h"

#include "CpuGeckoIntf.h" // Gecko Intf
#include "RaStub.h"       // Gecko's RaStub

#include "board_defs.h"

using namespace std;

#define RST_CAUSE_GECKO_POR 0x00000001
#define RST_CAUSE_GECKO_WD  0x00000002
#define RST_CAUSE_GECKO_RST 0x00000004
#define RST_CAUSE_CPU_WD    0x00000008
#define RST_CAUSE_CPU_RST   0x00000010


class BoardCommonDriver
{
public:

    BoardCommonDriver(bool isSim, bool isEval);

    ~BoardCommonDriver();

    void CreateBcmDriver();

    // Reset Cause
    uint32 GetResetCause();
    bool   ClrResetCause();

    /*
     * FPGA Misc Status and FGPA IOExp Wrapper Functions
     */
    int GetFpgaMiscStatus(uint32 &regVal);

    int GetFpgaIoExpInput(boardMs::mezzBoardIdType boardId, uint32 &regVal);

    // Host Board Related Inits ....

    int EnableMezzPower(bool isEnable);

    int LatchEnable();

    int EnableLedControl(boardMs::mezzBoardSpecType brdId);

    int EnableMezzReset(boardMs::mezzBoardSpecType brdId, bool isResetEnable);

    int PollUntilFpgaLedReady(boardMs::mezzBoardSpecType brdId);

    int SetFruFaultLedFlashRed();

    int SetFruActiveLedFlashGreen();

    // Mezz Related Inits ....

    int InitMezzBoards();

    int InitIoExp(boardMs::mezzBoardIdType boardId);

    int InitClock(boardMs::mezzBoardIdType boardId);

    int InitBcm(boardMs::mezzBoardIdType boardId);

    int InitBcmDriver(boardMs::mezzBoardSpecType boardId);

    void CheckMezzPwrSeq(boardMs::mezzBoardIdType brdId);

private:

    /*
     * Create register interfaces
     */
    void CreateRegIf();

    void CreateGeckoDriver();


    shared_ptr<RegIf> mspFpgaPlRegIf;

    shared_ptr<FpgaMiscIf> mspFpgaPlMiscIf;
    /*
     * I2C[0] assigned to MEZZ I2C[0]
     */
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c0RegIf;

    /*
     * I2C[1] assigned to MEZZ I2C[1]
     */
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c1RegIf;

    /*
     * I2C[2] assigned to MEZZ I2C[2]
     */
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c2RegIf;
    shared_ptr<FpgaI2cIf> mspBottomMzFPC402_1RegIf;
    shared_ptr<FpgaI2cIf> mspBottomMzFPC402_2RegIf;
    shared_ptr<FpgaI2cIf> mspBottomMzIoExpIf;
    shared_ptr<Si5394> mspBottomMzSi5394Drv;

    /*
     * I2C[3] assigned to MEZZ I2C[3]
     */
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c3RegIf;
    shared_ptr<FpgaI2cIf> mspTopMzFPC402_1RegIf;
    shared_ptr<FpgaI2cIf> mspTopMzFPC402_2RegIf;
    shared_ptr<FpgaI2cIf> mspTopMzIoExpIf;
    shared_ptr<Si5394> mspTopMzSi5394Drv;

    /*
     * I2C[4] assigned to SKICK I2C
     */
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c4RegIf;

    /*
     * MDIO module contains a single driver (set of registers)
     * for up to 4 sets of MDIO interfaces or buses
     * CHM6 uses 2 sets of MDIO buses Currently
     */
    shared_ptr<FpgaMdioIf> mspFpgaPlMdioIf;

#ifdef ARCH_x86
    unique_ptr<gearboxsim::Bcm81725Sim> mpBcmDriver;
#else
    unique_ptr<gearbox::Bcm81725> mpBcmDriver;
#endif

    bool mIsSim;
    bool mIsEval;
};

#endif /* CHM6_BOARD_MS_SRC_DRIVER_HOST_BOARD_COMMON_DRIVER_H_ */
