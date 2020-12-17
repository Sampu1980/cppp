/*
 * board_manager_util.h
 *
 *  Created on: May 29, 2020
 */

#ifndef CHM6_BOARD_MS_SRC_MANAGER_BOARD_MANAGER_UTIL_H_
#define CHM6_BOARD_MS_SRC_MANAGER_BOARD_MANAGER_UTIL_H_

#include "board_defs.h"
#include "board_proto_defs.h"
#include "board_fault_defs.h"

class BoardManagerUtil
{
public:

    static boardMs::hostBoardActionType ProtoBoardActionToMsBoardAction(hal_common::BoardAction boardAction);

    static bool MsFaultConditionToProtoFaultCondition(boardMs::faultConditionType msFaultCondition);

    static wrapper::Bool MsFaultConditionToInfnProtoFaultCondition(boardMs::faultConditionType msFaultCondition);

    static hal_common::LedState MsLedStateToProtoLedState(boardMs::LedStateType st);

    static boardMs::LedStateType ProtoLedStateToMsLedState(hal_common::LedState st);

    static hal_common::EqptState MsEqptStateToProtoEqptState(boardMs::Chm6EqptState st);

    static boardMs::QSFPPortId ProtoPortIdToMsPortId(hal_common::PortId id);

    static hal_common::PortId MsPortIdToProtoPortId(boardMs::QSFPPortId id);

    static boardMs::LineId ProtoLineIdToMsLineId(hal_common::LineId id);

    static hal_common::LineId MsLineIdToProtoLineId(boardMs::LineId id);
};



#endif /* CHM6_BOARD_MS_SRC_MANAGER_BOARD_MANAGER_UTIL_H_ */
