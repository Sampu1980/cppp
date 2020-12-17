#include "dco_fault_handler.h"
#include "chm6/redis_adapter/application_servicer.h"
#include "InfnLogger.h"

DcoFaultHandler::DcoFaultHandler(BoardManager& manager)
    : mManager(manager)
{}

void DcoFaultHandler::onCreate(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6DcoCardFault called...";

    auto *dco_fault = static_cast<chm6_common::Chm6DcoCardFault*>(objMsg);

    mManager.onCreate(dco_fault);
}

void DcoFaultHandler::onModify(google::protobuf::Message* objMsg)
{
    auto *dco_fault = static_cast<chm6_common::Chm6DcoCardFault*>(objMsg);

    mManager.onModify(dco_fault);
}

void DcoFaultHandler::onDelete(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6DcoCardFault called...";

    auto *dco_fault = static_cast<chm6_common::Chm6DcoCardFault*>(objMsg);

    mManager.onDelete(dco_fault);
}

void DcoFaultHandler::onResync(google::protobuf::Message* objMsg)
{
    INFN_LOG(SeverityLevel::info) << "for Chm6DcoCardFault called...";

    auto *dco_fault = static_cast<chm6_common::Chm6DcoCardFault*>(objMsg);

    mManager.onResync(dco_fault);
}








