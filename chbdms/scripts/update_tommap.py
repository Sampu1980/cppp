import grpc
import threading
import sys
import time

from infinera.chm6.common.v2 import sys_constants_pb2

from grpc_client_adaptor import CrudService

from infinera.hal.chm6.v1 import chm6_constants_pb2

def create_tom_map(board_id):
    tom_map = sys_constants_pb2.TomPresenceMap()
    tom_map.base_state.config_id.value = "hostboard-" + str(board_id)
    tom_map.base_state.mark_for_delete.value = False
    tom_map.base_state.timestamp.value = time.ctime()
    
    tom_map.aid = "hostboard-" + str(board_id)
    
    tom_map.tom_presence_map = 6
    
    return tom_map


def update(board_id):
    try:
        crud = CrudService()
        tommap = create_tom_map(board_id)
        print(crud.create(tommap))
    except grpc.RpcError as err:
        print("Set failed")
        print(err.details)

if __name__ == '__main__':
    board_id = sys.argv[1]
    update(board_id)