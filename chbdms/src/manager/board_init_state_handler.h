/*
 * board_init_state_handler.h
 *
 *  Created on: May 27, 2020
 */

#ifndef CHM6_BOARD_MS_SRC_MANAGER_BOARD_INIT_STATE_HANDLER_H_
#define CHM6_BOARD_MS_SRC_MANAGER_BOARD_INIT_STATE_HANDLER_H_

#include "chm6/redis_adapter/callback_handler.h"
#include "board_manager.h"

class BoardInitStateHandler : public ICallbackHandler
{
public:

    BoardInitStateHandler(BoardManager& manager);

    virtual ~BoardInitStateHandler() {}

    void onCreate(google::protobuf::Message* objMsg);

    void onModify(google::protobuf::Message* objMsg);

    void onDelete(google::protobuf::Message* objMsg);

    void onResync(google::protobuf::Message* objMsg);

private:
    BoardManager& mManager;
};

#endif /* CHM6_BOARD_MS_SRC_MANAGER_BOARD_INIT_STATE_HANDLER_H_ */
