set(BUILD_SHARED_LIBS OFF)

add_definitions(-fPIC)

file(GLOB SOURCES "./*.cpp")

# Hw
add_library(BoardManager STATIC 
		    ${SOURCES}
)

target_include_directories(BoardManager PUBLIC 
                           .
    						${BOOST_INCLUDE_DIR}
    						${PROTOBUF_INCLUDE_DIR}
    						${COMMON_INCLUDE_DIR}
    						${ADAPTER_INCLUDE_DIR}
    						${DRIVER_INCLUDE_DIR}
    						${SRC_INCLUDE_DIR}
    						${PROTOGEN_INCLUDE_DIR}
                            ${BCM_EPDM_DIR}
                            ${BCM_MILB_DIR}
)

target_link_libraries(
    BoardManager
    ${ARCH_LIB_DIR}/libboost_system.a
    ${ARCH_LIB_DIR}/libboost_thread.a
    ${PROTOBUF_LIB_DIR}/libprotobuf.so
    redisadapter
    BoardAdapter
    protogen_cpp
    dl
)

add_library(BoardInitManager STATIC 
            board_init_manager.cpp
)

target_include_directories(BoardInitManager PUBLIC
                           ${CMAKE_CURRENT_LIST_DIR}
                           ${COMMON_INCLUDE_DIR}
                           ${SRC_INCLUDE_DIR}
                           ${PROTOGEN_INCLUDE_DIR}
                           ${DRIVER_INCLUDE_DIR}
                           ${BCM_EPDM_DIR}
                           ${BCM_MILB_DIR} 
)

target_link_libraries(
    BoardInitManager
    ${ARCH_LIB_DIR}/libboost_system.a
    ${ARCH_LIB_DIR}/libboost_thread.a
    ${PROTOBUF_LIB_DIR}/libprotobuf.so
    redisadapter
    protogen_cpp
    dl
)