/*
 * board_init_manager.h
 *
 */

#ifndef CHM6_BOARD_MS_SRC_MANAGER_BOARD_INIT_MANAGER_H_
#define CHM6_BOARD_MS_SRC_MANAGER_BOARD_INIT_MANAGER_H_

#include "infinera/chm6/common/v2/board_init_state.pb.h"
#include "infinera/wrappers/v1/infn_datatypes.pb.h"
#include "board_init_util.h"

namespace chm6_common = infinera::chm6::common::v2;
namespace hal_common  = infinera::hal::common::v2;
namespace wrapper     = infinera::wrappers::v1;

class BoardInitManager
{
public:

    BoardInitManager();
    ~BoardInitManager()
    {
        delete mpState;
        delete mpBoardUtil;
    }

    int initialize();

private:

    void initNbConnection();
    bool initHw();
    void updateInitState(bool isOk);
    void updateFaults();
    void createState();
    void readState();
    void getResetCause();
    void clrResetCause();
    bool isForceColdBootEnvSet();
    bool MsFaultConditionToProtoFaultCondition(boardMs::faultConditionType cond);
    wrapper::Bool MsFaultToProtoFault(boardMs::faultConditionType cond);
  
    chm6_common::Chm6BoardInitState* mpState;
    BoardInitUtil* mpBoardUtil;

    chm6_common::BootReason mBootReason;
    uint32                  mResetCauseBits;

    bool mPrevInitStateDone;
};

#endif /* CHM6_BOARD_MS_SRC_MANAGER_BOARD_INIT_MANAGER_H_ */
