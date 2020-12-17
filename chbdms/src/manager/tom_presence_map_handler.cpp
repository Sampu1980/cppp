/*
 * tom_presence_map_handler.cpp
 *
 *  Created on: Jun 1, 2020
 */

#include "tom_presence_map_handler.h"
#include "chm6/redis_adapter/application_servicer.h"

#include "InfnLogger.h"

TomPresenceMapHandler::TomPresenceMapHandler(BoardManager& manager)
    : mManager(manager)
{}

void TomPresenceMapHandler::onCreate(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6TomPresenceMap called...";

    auto *tom_presence_map = static_cast<chm6_common::Chm6TomPresenceMap*>(objMsg);

    mManager.onCreate(tom_presence_map);
}

void TomPresenceMapHandler::onModify(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6TomPresenceMap called...";

    auto *tom_presence_map = static_cast<chm6_common::Chm6TomPresenceMap*>(objMsg);

    mManager.onModify(tom_presence_map);
}

void TomPresenceMapHandler::onDelete(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6TomPresenceMap called...";

    auto *tom_presence_map = static_cast<chm6_common::Chm6TomPresenceMap*>(objMsg);

    mManager.onDelete(tom_presence_map);
}

void TomPresenceMapHandler::onResync(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6TomPresenceMap called...";

    auto *tom_presence_map = static_cast<chm6_common::Chm6TomPresenceMap*>(objMsg);

    mManager.onResync(tom_presence_map);
}

