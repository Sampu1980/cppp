/*
 * dco_state_handler.cpp
 *
 *  Created on: May 27, 2020
 */

#include "dco_state_handler.h"
#include "chm6/redis_adapter/application_servicer.h"
#include "InfnLogger.h"

DcoStateHandler::DcoStateHandler(BoardManager& manager)
    : mManager(manager)
{}

void DcoStateHandler::onCreate(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6DcoCardState called...";

    auto *dco_state = static_cast<chm6_common::Chm6DcoCardState*>(objMsg);

    mManager.onCreate(dco_state);
}

void DcoStateHandler::onModify(google::protobuf::Message* objMsg)
{
    auto *dco_state = static_cast<chm6_common::Chm6DcoCardState*>(objMsg);

    mManager.onModify(dco_state);
}

void DcoStateHandler::onDelete(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6DcoCardState called...";

    auto *dco_state = static_cast<chm6_common::Chm6DcoCardState*>(objMsg);

    mManager.onDelete(dco_state);
}

void DcoStateHandler::onResync(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6DcoCardState called...";

    auto *dco_state = static_cast<chm6_common::Chm6DcoCardState*>(objMsg);

    mManager.onResync(dco_state);
}


