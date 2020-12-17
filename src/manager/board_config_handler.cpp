/*
 * board_config_handler.cpp
 *
 *  Created on: Mar 13, 2020
 */

#include "board_config_handler.h"
#include "chm6/redis_adapter/application_servicer.h"
#include "InfnLogger.h"
#include <google/protobuf/util/json_util.h>

#include <iostream>
#include <string>

using google::protobuf::Message;

using std::string;

BoardConfigHandler::BoardConfigHandler(BoardManager& manager)
    : mManager(manager)
{}

void BoardConfigHandler::onCreate(Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6BoardConfig called...";

    auto *board_config = static_cast<chm6_board::Chm6BoardConfig*>(objMsg);

    chm6_board::Chm6BoardState board_state;

    chm6_common::BaseProgState* base_state = board_state.mutable_base_state();

    base_state->mutable_config_id()->set_value(board_config->mutable_base_config()->mutable_config_id()->value());
    base_state->mutable_timestamp()->CopyFrom(board_config->mutable_base_config()->timestamp());
    base_state->set_transaction_state(chm6_common::STATE_INPROGRESS);
    base_state->set_transaction_status(chm6_common::STATUS_UNSPECIFIED);
    base_state->mutable_transaction_info()->set_value("Start respond to Chm6BoardConfig message");

    AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectUpdate(board_state);

    mManager.onCreate(board_config);

    base_state->mutable_config_id()->set_value(board_config->mutable_base_config()->mutable_config_id()->value());
    base_state->mutable_timestamp()->CopyFrom(board_config->mutable_base_config()->timestamp());
    base_state->set_transaction_state(chm6_common::STATE_COMPLETE);
    base_state->set_transaction_status(chm6_common::STATUS_SUCCESS);
    base_state->mutable_transaction_info()->set_value("End respond to Chm6BoardConfig message");

	AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectUpdate(board_state);
}

void BoardConfigHandler::onModify(Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6BoardConfig called...";

    auto *board_config = static_cast<chm6_board::Chm6BoardConfig*>(objMsg);

    chm6_board::Chm6BoardState board_state;

    chm6_common::BaseProgState* base_state = board_state.mutable_base_state();

    base_state->mutable_config_id()->set_value(board_config->mutable_base_config()->mutable_config_id()->value());
    base_state->mutable_timestamp()->CopyFrom(board_config->mutable_base_config()->timestamp());
    base_state->set_transaction_state(chm6_common::STATE_INPROGRESS);
    base_state->set_transaction_status(chm6_common::STATUS_UNSPECIFIED);
    base_state->mutable_transaction_info()->set_value("Start respond to Chm6BoardConfig message");

    AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectUpdate(board_state);

    mManager.onModify(board_config);

    base_state->mutable_config_id()->set_value(board_config->mutable_base_config()->mutable_config_id()->value());
    base_state->mutable_timestamp()->CopyFrom(board_config->mutable_base_config()->timestamp());
    base_state->set_transaction_state(chm6_common::STATE_COMPLETE);
    base_state->set_transaction_status(chm6_common::STATUS_SUCCESS);
    base_state->mutable_transaction_info()->set_value("End respond to Chm6BoardConfig message");

	AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectUpdate(board_state);
}

void BoardConfigHandler::onDelete(Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6BoardConfig called...";

    auto *board_config = static_cast<chm6_board::Chm6BoardConfig*>(objMsg);

    chm6_board::Chm6BoardState board_state;

    chm6_common::BaseProgState* base_state = board_state.mutable_base_state();

    base_state->mutable_config_id()->set_value(board_config->mutable_base_config()->mutable_config_id()->value());
    base_state->mutable_timestamp()->CopyFrom(board_config->mutable_base_config()->timestamp());
    base_state->set_transaction_state(chm6_common::STATE_INPROGRESS);
    base_state->set_transaction_status(chm6_common::STATUS_UNSPECIFIED);
    base_state->mutable_transaction_info()->set_value("Start respond to Chm6BoardConfig message");

    AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectUpdate(board_state);

    mManager.onDelete(board_config);

    base_state->mutable_config_id()->set_value(board_config->mutable_base_config()->mutable_config_id()->value());
    base_state->mutable_timestamp()->CopyFrom(board_config->mutable_base_config()->timestamp());
    base_state->set_transaction_state(chm6_common::STATE_COMPLETE);
    base_state->set_transaction_status(chm6_common::STATUS_SUCCESS);
    base_state->mutable_transaction_info()->set_value("End respond to Chm6BoardConfig message");

	AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectUpdate(board_state);
}

void BoardConfigHandler::onResync(Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6BoardConfig called...";

    auto *board_config = static_cast<chm6_board::Chm6BoardConfig*>(objMsg);

    chm6_board::Chm6BoardState board_state;

    chm6_common::BaseProgState* base_state = board_state.mutable_base_state();

    base_state->mutable_config_id()->set_value(board_config->mutable_base_config()->mutable_config_id()->value());
    base_state->mutable_timestamp()->CopyFrom(board_config->mutable_base_config()->timestamp());
    base_state->set_transaction_state(chm6_common::STATE_INPROGRESS);
    base_state->set_transaction_status(chm6_common::STATUS_UNSPECIFIED);
    base_state->mutable_transaction_info()->set_value("Start respond to Chm6BoardConfig message");

    AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectUpdate(board_state);

    mManager.onResync(board_config);

    base_state->mutable_config_id()->set_value(board_config->mutable_base_config()->mutable_config_id()->value());
    base_state->mutable_timestamp()->CopyFrom(board_config->mutable_base_config()->timestamp());
    base_state->set_transaction_state(chm6_common::STATE_COMPLETE);
    base_state->set_transaction_status(chm6_common::STATUS_SUCCESS);
    base_state->mutable_transaction_info()->set_value("End respond to Chm6BoardConfig message");

    AppServicerIntfSingleton::getInstance()->getRedisInstance()->RedisObjectUpdate(board_state);
}





