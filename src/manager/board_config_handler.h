/*
 * board_config_handler.h
 *
 *  Created on: Mar 13, 2020
 */

#ifndef CHM6_BOARD_MS_SRC_MANAGER_BOARD_CONFIG_HANDLER_H_
#define CHM6_BOARD_MS_SRC_MANAGER_BOARD_CONFIG_HANDLER_H_

#include "chm6/redis_adapter/callback_handler.h"
#include "board_manager.h"

class BoardConfigHandler : public ICallbackHandler
{
public:

    BoardConfigHandler(BoardManager& Manager);

    virtual ~BoardConfigHandler() {}

    void onCreate(google::protobuf::Message* objMsg);

    void onModify(google::protobuf::Message* objMsg);

    void onDelete(google::protobuf::Message* objMsg);

    void onResync(google::protobuf::Message* objMsg);

private:
    BoardManager& mManager;
};

#endif /* CHM6_BOARD_MS_SRC_MANAGER_BOARD_CONFIG_HANDLER_H_ */
