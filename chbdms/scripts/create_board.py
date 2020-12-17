import grpc
import threading
import sys
import time

from infinera.chm6.board.v2 import board_config_pb2

from grpc_client_adaptor import CrudService

from infinera.hal.common.v1 import sys_constants_pb2

def create_board_object(board_id):
    board_config = board_config_pb2.Chm6BoardConfig()
    board_config.base_config.config_id.value = "hostboard-" + str(board_id)
    board_config.base_config.mark_for_delete.value = False
    board_config.base_config.timestamp.value = time.ctime()
    
    board_config.hal.common_config.host_card_action = 2
    
    board_config.hal.dco_card_action = 3
    
    return board_config


def create(board_id):
    try:
        crud = CrudService()
        board = create_board_object(board_id)
        print(crud.create(board))
    except grpc.RpcError as err:
        print("Set failed")
        print(err.details)

if __name__ == '__main__':
    board_id = sys.argv[1]
    create(board_id)