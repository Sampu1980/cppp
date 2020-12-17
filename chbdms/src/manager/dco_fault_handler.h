#ifndef CHM6_BOARD_MS_SRC_MANAGER_DCO_FAULT_HANDLER_H_
#define CHM6_BOARD_MS_SRC_MANAGER_DCO_FAULT_HANDLER_H_

#include "chm6/redis_adapter/callback_handler.h"
#include "board_manager.h"

class DcoFaultHandler : public ICallbackHandler
{
public:

    DcoFaultHandler(BoardManager& manager);

    virtual ~DcoFaultHandler() {}

    void onCreate(google::protobuf::Message* objMsg);

    void onModify(google::protobuf::Message* objMsg);

    void onDelete(google::protobuf::Message* objMsg);

    void onResync(google::protobuf::Message* objMsg);

private:
    BoardManager& mManager;
};

#endif /* CHM6_BOARD_MS_SRC_MANAGER_DCO_FAULT_HANDLER_H_ */
