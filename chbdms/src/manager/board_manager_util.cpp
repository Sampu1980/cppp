/*
 * board_manager_util.cpp
 *
 *  Created on: May 29, 2020
 */

#include "board_manager_util.h"

boardMs::hostBoardActionType BoardManagerUtil::ProtoBoardActionToMsBoardAction(hal_common::BoardAction boardAction)
{
    boardMs::hostBoardActionType act;

    switch (boardAction)
    {
        case hal_common::BoardAction::BOARD_ACTION_POWER_UP:
        {
            act = boardMs::POWER_UP;
            break;
        }

        case hal_common::BoardAction::BOARD_ACTION_UPDATE_FW:
        {
            act = boardMs::UPDATE_FW;
            break;
        }

        case hal_common::BoardAction::BOARD_ACTION_INIT_HW:
        {
            act = boardMs::INIT_HW;
            break;
        }

        case hal_common::BoardAction::BOARD_ACTION_RESTART_COLD:
        {
            act = boardMs::RESTART_COLD;
            break;
        }

        case hal_common::BoardAction::BOARD_ACTION_RESTART_WARM:
        {
            act = boardMs::RESTART_WARM;
            break;
        }

        default:
        {
            act = boardMs::BOARD_ACTION_UNKNOWN;
            break;
        }
    }

    return act;
}

bool BoardManagerUtil::MsFaultConditionToProtoFaultCondition(boardMs::faultConditionType msFaultCondition)
{
        bool fltConditon = false;

        switch (msFaultCondition)
        {
            case boardMs::FAULT_SET:
                fltConditon = true;
                break;

            case boardMs::FAULT_CLEAR:
                fltConditon = false;
                break;

            case boardMs::FAULT_UNKNOWN:
            default:
                fltConditon = false;
                break;
        }

        return fltConditon;
}

wrapper::Bool BoardManagerUtil::MsFaultConditionToInfnProtoFaultCondition(boardMs::faultConditionType msFaultCondition)
{
    wrapper::Bool fltConditon = wrapper::BOOL_UNSPECIFIED;

    switch (msFaultCondition)
    {
        case boardMs::FAULT_SET:
            fltConditon = wrapper::BOOL_TRUE;
            break;

        case boardMs::FAULT_CLEAR:
            fltConditon = wrapper::BOOL_FALSE;
            break;

        case boardMs::FAULT_UNKNOWN:
        default:
            fltConditon = wrapper::BOOL_UNSPECIFIED;
            break;
    }

    return fltConditon;
}

hal_common::LedState BoardManagerUtil::MsLedStateToProtoLedState(boardMs::LedStateType st)
{
    hal_common::LedState ret = hal_common::LED_STATE_UNSPECIFIED;
    switch (st)
    {
    case boardMs::LED_STATE_UNKNOWN:
        ret = hal_common::LED_STATE_UNSPECIFIED;
        break;

    case boardMs::OFF:
        ret = hal_common::LED_STATE_OFF;
        break;

    case boardMs::YELLOW:
        ret = hal_common::LED_STATE_YELLOW;
        break;

    case boardMs::FLASHING_YELLOW:
        ret = hal_common::LED_STATE_FLASHING_YELLOW;
        break;

    case boardMs::GREEN:
        ret = hal_common::LED_STATE_GREEN;
        break;

    case boardMs::FLASHING_GREEN:
        ret = hal_common::LED_STATE_FLASHING_GREEN;
        break;

    case boardMs::RED:
        ret = hal_common::LED_STATE_RED;
        break;

    case boardMs::FLASHING_RED:
        ret = hal_common::LED_STATE_FLASHING_RED;
        break;

    case boardMs::CYCLING:
        ret = hal_common::LED_STATE_CYCLING;
        break;

    case boardMs::CYCLING_WITH_OFF:
        ret = hal_common::LED_STATE_CYCLING_WITH_OFF;
        break;

    default:
        break;
    }

    return ret;
}

boardMs::LedStateType BoardManagerUtil::ProtoLedStateToMsLedState(hal_common::LedState st)
{
    boardMs::LedStateType ret = boardMs::LED_STATE_UNKNOWN;

    switch (st)
    {
    case hal_common::LED_STATE_UNSPECIFIED:
        ret = boardMs::LED_STATE_UNKNOWN;
        break;

    case hal_common::LED_STATE_OFF:
        ret = boardMs::OFF;
        break;

    case hal_common::LED_STATE_YELLOW:
        ret = boardMs::YELLOW;
        break;

    case hal_common::LED_STATE_FLASHING_YELLOW:
        ret = boardMs::FLASHING_YELLOW;
        break;

    case hal_common::LED_STATE_GREEN:
        ret = boardMs::GREEN;
        break;

    case hal_common::LED_STATE_FLASHING_GREEN:
        ret = boardMs::FLASHING_GREEN;
        break;

    case hal_common::LED_STATE_RED:
        ret = boardMs::RED;
        break;

    case hal_common::LED_STATE_FLASHING_RED:
        ret = boardMs::FLASHING_RED;
        break;

    case hal_common::LED_STATE_CYCLING:
        ret = boardMs::CYCLING;
        break;

    default:
        break;
    }

    return ret;
}

hal_common::EqptState BoardManagerUtil::MsEqptStateToProtoEqptState(boardMs::Chm6EqptState st)
{
    hal_common::EqptState ret = hal_common::EQPT_STATE_UNSPECIFIED;

    switch (st)
    {
    case boardMs::EQPT_STATE_UNKNOWN:
        ret = hal_common::EQPT_STATE_UNSPECIFIED;
        break;

    case boardMs::EQPT_STATE_BOOTING:
        ret = hal_common::EQPT_STATE_BOOTING;
        break;

    case boardMs::EQPT_STATE_BOOTING_DONE:
        ret = hal_common::EQPT_STATE_BOOTING_DONE;
        break;

    case boardMs::EQPT_STATE_BOOTING_DONE_WARM:
        ret = hal_common::EQPT_STATE_BOOTING_DONE_WARM;
        break;

    case boardMs::EQPT_STATE_BOOTING_FAIL:
        ret = hal_common::EQPT_STATE_BOOTING_FAIL;
        break;

    case boardMs::EQPT_STATE_FW_UPDATING:
        ret = hal_common::EQPT_STATE_FW_UPDATING;
        break;

    case boardMs::EQPT_STATE_FW_UPDATE_DONE:
        ret = hal_common::EQPT_STATE_FW_UPDATE_DONE;
        break;

    case boardMs::EQPT_STATE_FW_UPDATE_FAIL:
        ret = hal_common::EQPT_STATE_FW_UPDATE_FAIL;
        break;

    case boardMs::EQPT_STATE_FW_UPDATE_COMMIT:
        ret = hal_common::EQPT_STATE_FW_UPDATE_COMMIT;
        break;

    case boardMs::EQPT_STATE_HW_INITING:
        ret = hal_common::EQPT_STATE_HW_INITING;
        break;

    case boardMs::EQPT_STATE_HW_INITING_DONE:
        ret = hal_common::EQPT_STATE_HW_INITING_DONE;
        break;

    case boardMs::EQPT_STATE_HW_INITING_FAIL:
        ret = hal_common::EQPT_STATE_HW_INITING_FAIL;
        break;

    case boardMs::EQPT_STATE_RESTARTING:
        ret = hal_common::EQPT_STATE_RESTARTING;
        break;

    default:
        break;
    }

    return ret;
}

boardMs::QSFPPortId BoardManagerUtil::ProtoPortIdToMsPortId(hal_common::PortId id)
{
    boardMs::QSFPPortId ret = boardMs::QSFP_PORT_ID_INVALID;

    switch(id)
    {
    case hal_common::PORT_ID_1:
        ret = boardMs::QSFP_1;
        break;

    case hal_common::PORT_ID_2:
        ret = boardMs::QSFP_2;
        break;

    case hal_common::PORT_ID_3:
        ret = boardMs::QSFP_3;
        break;

    case hal_common::PORT_ID_4:
        ret = boardMs::QSFP_4;
        break;

    case hal_common::PORT_ID_5:
        ret = boardMs::QSFP_5;
        break;

    case hal_common::PORT_ID_6:
        ret = boardMs::QSFP_6;
        break;

    case hal_common::PORT_ID_7:
        ret = boardMs::QSFP_7;
        break;

    case hal_common::PORT_ID_8:
        ret = boardMs::QSFP_8;
        break;

    case hal_common::PORT_ID_9:
        ret = boardMs::QSFP_9;
        break;

    case hal_common::PORT_ID_10:
        ret = boardMs::QSFP_10;
        break;

    case hal_common::PORT_ID_11:
        ret = boardMs::QSFP_11;
        break;

    case hal_common::PORT_ID_12:
        ret = boardMs::QSFP_12;
        break;

    case hal_common::PORT_ID_13:
        ret = boardMs::QSFP_13;
        break;

    case hal_common::PORT_ID_14:
        ret = boardMs::QSFP_14;
        break;

    case hal_common::PORT_ID_15:
        ret = boardMs::QSFP_15;
        break;

    case hal_common::PORT_ID_16:
        ret = boardMs::QSFP_16;
        break;

    default:
        break;
    }

    return ret;
}

hal_common::PortId BoardManagerUtil::MsPortIdToProtoPortId(boardMs::QSFPPortId id)
{
    hal_common::PortId ret = hal_common::PORT_ID_UNSPECIFIED;

    switch(id)
    {
    case boardMs::QSFP_1:
        ret = hal_common::PORT_ID_1;
        break;

    case boardMs::QSFP_2:
        ret = hal_common::PORT_ID_2;
        break;

    case boardMs::QSFP_3:
        ret = hal_common::PORT_ID_3;
        break;

    case boardMs::QSFP_4:
        ret = hal_common::PORT_ID_4;
        break;

    case boardMs::QSFP_5:
        ret = hal_common::PORT_ID_5;
        break;

    case boardMs::QSFP_6:
        ret = hal_common::PORT_ID_6;
        break;

    case boardMs::QSFP_7:
        ret = hal_common::PORT_ID_7;
        break;

    case boardMs::QSFP_8:
        ret = hal_common::PORT_ID_8;
        break;

    case boardMs::QSFP_9:
        ret = hal_common::PORT_ID_9;
        break;

    case boardMs::QSFP_10:
        ret = hal_common::PORT_ID_10;
        break;

    case boardMs::QSFP_11:
        ret = hal_common::PORT_ID_11;
        break;

    case boardMs::QSFP_12:
        ret = hal_common::PORT_ID_12;
        break;

    case boardMs::QSFP_13:
        ret = hal_common::PORT_ID_13;
        break;

    case boardMs::QSFP_14:
        ret = hal_common::PORT_ID_14;
        break;

    case boardMs::QSFP_15:
        ret = hal_common::PORT_ID_15;
        break;

    case boardMs::QSFP_16:
        ret = hal_common::PORT_ID_16;
        break;

    default:
        break;
    }

    return ret;
}

boardMs::LineId BoardManagerUtil::ProtoLineIdToMsLineId(hal_common::LineId id)
{
    boardMs::LineId ret = boardMs::LINE_ID_INVALID;

    switch (id)
    {
    case hal_common::LINE_ID_1:
        ret = boardMs::LINE_1;
        break;

    case hal_common::LINE_ID_2:
        ret = boardMs::LINE_2;
        break;

    default:
        break;
    }

    return ret;
}

hal_common::LineId BoardManagerUtil::MsLineIdToProtoLineId(boardMs::LineId id)
{
    hal_common::LineId ret = hal_common::LINE_ID_UNSPECIFIED;

    switch (id)
    {
    case boardMs::LINE_1:
        ret = hal_common::LINE_ID_1;
        break;

    case boardMs::LINE_2:
        ret = hal_common::LINE_ID_2;
        break;

    default:
        break;
    }

    return ret;
}
