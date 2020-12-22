#private comment
#inside branch
#inside branch
/*
 * BoardAdapter.h
 *
 *  Created on: Jan 21, 2020
 *
 */

#ifndef CHM6_BOARD_MS_SRC_CHM6BOARDADAPTER_IMPL_H_
#define CHM6_BOARD_MS_SRC_CHM6BOARDADAPTER_IMPL_H_

#include <iostream>
#include <string>
#include <mutex>
#include <boost/ptr_container/ptr_vector.hpp>

#include "board_defs.h"
#include "types.h"
#include "board_driver.h"

#include "SimpleLog.h"

#include "board_adapter_if.h"
#include "board_common_adapter.h"

namespace boardAda
{

using namespace boardMs;


class BoardAdapter : public BoardCommonAdapter
{
public:

    BoardAdapter(BoardDriver& boardDriver);

    ~BoardAdapter();

    void SetSlotId(uint32 slotId);

    void SetSledId(uint32 sledId);

    /*
     * APIs to configure host board
     */
    int SetHostBoardAction(hostBoardActionType action);

    void SetColdRestartDcoDelay(uint32 delaySeconds);

    /*
     * Queries
     */
    bool IsColdBoot();

    bool IsOnBench();

    bool IsChassisDiag();

    /*
     * Host board status
     */
    Chm6EqptState GetEqptState();

    Chm6EqptInventory& GetEqptInventory();

    LedStateType GetPowerLedState();

    void SetFaultLedState(LedStateType state);

    LedStateType GetFaultLedState();

    void SetActiveLedState(LedStateType state);

    LedStateType GetActiveLedState();



    void DoLedLocationTest(bool doTest);

    void SetPortLedState(QSFPPortId portId, QSFPLedType ledType, LedStateType ledState);

    port_led_ptr_vec& GetPortLedStates();

    void SetLineLedState(LineId lineId, LineLedType ledType, LedStateType ledState);

    line_led_ptr_vec& GetLineLedStates();

    void CheckLedLampLocTestState();

    virtual void CheckFaults();


    board_pm_ptr_vec& GetBoardPm();


    void DumpEqptInventory( std::ostream &os );

private:

    void Initialize();

    void InitializeFaults();

    BoardDriver&    mDriver;

    std::recursive_mutex mRMutexEqptInv;
};

} // namespace boardAda

#endif /* CHM6_BOARD_MS_SRC_CHM6BOARDADAPTER_IMPL_H_ */
