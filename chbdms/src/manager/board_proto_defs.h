#ifndef CHM6_BOARD_MS_SRC_MANAGER_BOARD_PROTO_DEFS_H_
#define CHM6_BOARD_MS_SRC_MANAGER_BOARD_PROTO_DEFS_H_

#include <google/protobuf/message.h>

#include "infinera/chm6/board/v3/board_config.pb.h"
#include "infinera/chm6/board/v3/board_state.pb.h"
#include "infinera/chm6/board/v3/board_fault.pb.h"
#include "infinera/chm6/board/v3/board_pm.pb.h"
#include "infinera/chm6/common/v2/dco_card.pb.h"
#include "infinera/chm6/common/v2/tom_presence_map.pb.h"
#include "infinera/chm6/common/v2/board_init_state.pb.h"
#include "infinera/wrappers/v1/infn_datatypes.pb.h"

namespace chm6_board  = infinera::chm6::board::v3;
namespace chm6_common = infinera::chm6::common::v2;
namespace hal_board   = infinera::hal::board::v2;
namespace hal_chm6    = infinera::hal::chm6::v2;
namespace hal_common  = infinera::hal::common::v2;
namespace wrapper     = infinera::wrappers::v1;

#endif /* CHM6_BOARD_MS_SRC_MANAGER_BOARD_PROTO_DEFS_H_ */
