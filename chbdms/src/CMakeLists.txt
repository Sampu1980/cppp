if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    cmake_minimum_required(VERSION 3.0.0)
else()
    cmake_minimum_required(VERSION 2.8.0)
endif()

project(chm6_tom_ms)

#configure_file(Config.h.in Config.h)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-fPIC -DZ_LINUX  -DLINUX_COMPILE)
add_definitions(-DCHM6_GECKOINTF)

# for warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
# The line below should replace the above one once all projects have no warnings.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function") # allow unused functions for now
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers") # disable warning - g++ is overly paranoid
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations") 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter") 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder") 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare") 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-type-limits ") 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function") # allow unused functions for now
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers") # disable warning - g++ is overly paranoid
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ignored-qualifiers")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstringop-truncation")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-truncation")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-function-type")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wimplicit-fallthrough")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-implicit-function-declaration")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pointer-to-int-cast")
# warnings

set(CUSTOM_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CUSTOM_STATIC_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(INSTALL_PREFIX_PATH "/opt/infinera/local")
set(INSTALL_LIB_PATH "${INSTALL_PREFIX_PATH}/lib")
set(INSTALL_ETC_PATH "${INSTALL_PREFIX_PATH}/etc")
set(INSTALL_BIN_PATH "${INSTALL_PREFIX_PATH}/bin")
set(INSTALL_SHARE_PATH "${INSTALL_PREFIX_PATH}/share")
set(INSTALL_PROFILES_PATH "${INSTALL_ETC_PATH}/profiles")
set (SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CUSTOM_LIBRARY_OUTPUT_DIRECTORY})

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/installed" CACHE PATH "default install path" FORCE )
endif()

set(CMAKE_BUILD_TYPE "Debug")

set(BOOST_INCLUDE_DIR "/usr/include/boost")

if (${ARCH} STREQUAL "arm64")
  set(ARCH_LIB_DIR "/usr/lib/aarch64-linux-gnu")
  set(PROTOBUF_INCLUDE_DIR "/usr/include/aarch64-linux-gnu")
  set(PROTOBUF_LIB_DIR "/usr/lib/aarch64-linux-gnu")
  message("*** Build Target Set For ARM 64")
endif()

if (${ARCH} STREQUAL "x86")
  set(ARCH_LIB_DIR "/usr/lib/x86_64-linux-gnu")
  set(PROTOBUF_INCLUDE_DIR "/usr/include")
  set(PROTOBUF_LIB_DIR "/usr/lib")
  message("*** Build Target Set For x86 64")
endif()

set(SRC_INCLUDE_DIR ${SRC_DIR}/include)

set(COMMON_SRC_DIR ${CMAKE_SOURCE_DIR}/common)
set(COMMON_INCLUDE_DIR ${COMMON_SRC_DIR})
set(COMMON_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

set(CLI_SRC_DIR ${CMAKE_SOURCE_DIR}/external-repos/cli)
set(CLI_INCLUDE_DIR ${CLI_SRC_DIR}/include)
set(CLI_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

set(MANAGER_SRC_DIR ${CMAKE_SOURCE_DIR}/manager)
set(MANAGER_INCLUDE_DIR ${MANAGER_SRC_DIR})
set(MANAGER_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

set(ADAPTER_SRC_DIR ${CMAKE_SOURCE_DIR}/adapter)
set(ADAPTER_INCLUDE_DIR ${ADAPTER_SRC_DIR})
set(ADAPTER_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

set(DRIVER_SRC_DIR ${CMAKE_SOURCE_DIR}/driver)
set(DRIVER_INCLUDE_DIR ${DRIVER_SRC_DIR})
set(DRIVER_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

set(DBGCMDS_SRC_DIR ${CMAKE_SOURCE_DIR}/dbgcmds)
set(DBGCMDS_INCLUDE_DIR ${DBGCMDS_SRC_DIR})
set(DBGCMDS_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

set(PROTOGEN_SRC_DIR ${CMAKE_SOURCE_DIR}/protogen)
set(PROTOGEN_INCLUDE_DIR ${PROTOGEN_SRC_DIR}/cpp)
set(PROTOGEN_LIB_DIR ${CMAKE_BINARY_DIR}/lib)


if(${ARCH} STREQUAL "x86")
  set(BCM_SDK_DIR /usr/local/include/x86_64-linux-gnu)
  set(BCM_SDK_LIB_DIR /usr/local/lib/x86_64-linux-gnu)
elseif (${ARCH} STREQUAL "arm64")
  set(BCM_SDK_DIR /usr/local/include/aarch64-linux-gnu)
  set(BCM_SDK_LIB_DIR /usr/local/lib/aarch64-linux-gnu)
endif()


set(BCM_EPDM_DIR ${BCM_SDK_DIR}/epdm)
set(BCM_EPDM_LIB_DIR ${BCM_SDK_LIB_DIR}/epdm)

set(BCM_MILB_DIR ${BCM_SDK_DIR}/milb)
set(BCM_MILB_LIB_DIR ${BCM_SDK_LIB_DIR}/milb)


set(BOARD_INIT_SRC_DIR ${CMAKE_SOURCE_DIR}/board_init)
set(BOARD_INIT_INCLUDE_DIR ${BOARD_INIT_SRC_DIR})

set(GLOBAL_LINK_LIB_DIRS
            ${COMMON_LIB_DIR}
            ${CLI_LIB_DIR}
            ${MANAGER_LIB_DIR}
            ${ADAPTER_LIB_DIR}
            ${DRIVER_LIB_DIR}
            ${DBGCMDS_LIB_DIR}
            ${PROTOGEN_LIB_DIR}
            ${ARCH_LIB_DIR}
            ${PROTOBUF_LIB_DIR}
)

set(GLOBAL_LINK_INCLUDE_DIRS
			${SRC_INCLUDE_DIR}
            ${COMMON_INCLUDE_DIR}
            ${CLI_INCLUDE_DIR}
            ${MANAGER_INCLUDE_DIR}
            ${ADAPTER_INCLUDE_DIR}
            ${DRIVER_INCLUDE_DIR}
            ${DBGCMDS_INCLUDE_DIR}
            ${PROTOGEN_INCLUDE_DIR}
            ${BOOST_INCLUDE_DIR}
            ${PROTOBUF_INCLUDE_DIR}
)

link_directories(${COMMON_LIB_DIR}
                 ${CLI_LIB_DIR}
                 ${MANAGER_LIB_DIR}
                 ${ADAPTER_LIB_DIR}
                 ${DRIVER_LIB_DIR}
                 ${DBGCMDS_LIB_DIR}
                 ${PROTOGEN_LIB_DIR}
                 ${ARCH_LIB_DIR}
                 ${PROTOBUF_LIB_DIR}
)

add_subdirectory(external-repos/cli)
add_subdirectory(protogen)
add_subdirectory(common)
add_subdirectory(driver)
add_subdirectory(adapter)
add_subdirectory(manager)
add_subdirectory(dbgcmds)
add_subdirectory(daemon)
