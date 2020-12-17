//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#ifndef CHM6_BOARD_MS_SRC_DRIVER_HOST_BOARD_INIT_UTIL_H_
#define CHM6_BOARD_MS_SRC_DRIVER_HOST_BOARD_INIT_UTIL_H_

#include <iostream>
#include <string>
#include <mutex>
#include <vector>
#include <map>

#include "types.h"

#include "board_common_driver.h"

#include "board_defs.h"
#include "board_fault_defs.h"


using namespace std;

#define RST_CAUSE_GECKO_POR 0x00000001
#define RST_CAUSE_GECKO_WD  0x00000002
#define RST_CAUSE_GECKO_RST 0x00000004
#define RST_CAUSE_CPU_WD    0x00000008
#define RST_CAUSE_CPU_RST   0x00000010


class BoardInitUtil
{
public:

    BoardInitUtil();

    ~BoardInitUtil();

    /*
     * ColdInit CHM6 board after power up
     */
    int ColdInit();

    /*
     * Warm init
     */
    int WarmInit();

    // Reset Cause
    uint32 GetResetCause();
    bool   ClrResetCause();

    /*
     * Faults
     */
    typedef std::map<boardMs::BoardFaultId, std::shared_ptr<boardMs::Chm6BoardFault>> BoardFaultMapType;

    void InitFaults();

    const BoardFaultMapType& GetFaults()
    {
        return (const BoardFaultMapType&)(mBoardInitFaultMap);
    }

private:

    void CheckEnv();

    bool IsHwEnv();

    int InitMezzBoards();

    void InitHstBrdFaults();

    void InitMezBrdFaults();

    void InitFaultSim(std::string faultSimStr);

    bool CheckAllFaults();

    bool CheckHstBrdFaults();

    bool CheckMezBrdFaults(boardMs::mezzBoardIdType boardId);

    boardMs::faultConditionType GetFaultSimCondition(boardMs::BoardFaultId faultId);

    BoardFaultMapType mBoardInitFaultMap;
    std::vector<boardMs::BoardFaultId> mvHostBoardFaults;
    std::vector<std::vector<boardMs::BoardFaultId>> mvMezzBoardFaults;
    bool mFaultSimEn;
    std::vector<boardMs::BoardFaultId> mvFaultSim;

    bool mIsSim;
    bool mIsEval;
    std::string mEnvStr;

    std::shared_ptr<BoardCommonDriver> mspBrdDriver;
};

#endif /* CHM6_BOARD_MS_SRC_DRIVER_HOST_BOARD_INIT_UTIL_H_ */
