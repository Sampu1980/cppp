/*
 * tom_presence_map_handler.h
 *
 *  Created on: Jun 1, 2020
 */

#ifndef CHM6_BOARD_MS_SRC_MANAGER_TOM_PRESENCE_MAP_HANDLER_H_
#define CHM6_BOARD_MS_SRC_MANAGER_TOM_PRESENCE_MAP_HANDLER_H_

#include <google/protobuf/message.h>

#include "chm6/redis_adapter/callback_handler.h"
#include "board_manager.h"

class TomPresenceMapHandler : public ICallbackHandler
{
public:

    TomPresenceMapHandler(BoardManager& manager);

    virtual ~TomPresenceMapHandler() {}

    void onCreate(google::protobuf::Message* objMsg);

    void onModify(google::protobuf::Message* objMsg);

    void onDelete(google::protobuf::Message* objMsg);

    void onResync(google::protobuf::Message* objMsg);

private:
    BoardManager& mManager;
};

#endif /* CHM6_BOARD_MS_SRC_MANAGER_TOM_PRESENCE_MAP_HANDLER_H_ */
