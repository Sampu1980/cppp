/*
 * board_init_state_handler.cpp
 *
 *  Created on: May 27, 2020
 */

#include "board_init_state_handler.h"
#include "chm6/redis_adapter/application_servicer.h"
#include "InfnLogger.h"
#include <google/protobuf/util/json_util.h>

#include <iostream>
#include <string>

using google::protobuf::util::MessageToJsonString;
using google::protobuf::Message;

using std::string;

BoardInitStateHandler::BoardInitStateHandler(BoardManager& manager)
    : mManager(manager)
{}

void BoardInitStateHandler::onCreate(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "OnCreate called";

    auto *brdInitState = static_cast<chm6_common::Chm6BoardInitState*>(objMsg);

    mManager.onCreate(brdInitState);
}

void BoardInitStateHandler::onModify(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "onModify called";

    auto *brdInitState = static_cast<chm6_common::Chm6BoardInitState*>(objMsg);
    string data;
    MessageToJsonString(*brdInitState, &data);
    INFN_LOG(SeverityLevel::info) << data;

    mManager.onModify(brdInitState);
}

void BoardInitStateHandler::onDelete(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "OnDelete called";

    auto *brdInitState = static_cast<chm6_common::Chm6BoardInitState*>(objMsg);
    string data;
    MessageToJsonString(*brdInitState, &data);
    INFN_LOG(SeverityLevel::info) << data;

    mManager.onDelete(brdInitState);
}

void BoardInitStateHandler::onResync(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "onResync called";

    auto *brdInitState = static_cast<chm6_common::Chm6BoardInitState*>(objMsg);
    string data;
    MessageToJsonString(*brdInitState, &data);
    INFN_LOG(SeverityLevel::info) << data;

    mManager.onResync(brdInitState);
}


