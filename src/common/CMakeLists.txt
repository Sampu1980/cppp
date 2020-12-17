cmake_minimum_required(VERSION 3.13)

set(BUILD_SHARED_LIBS OFF)

add_library(BoardCommon STATIC "")

target_sources (
    BoardCommon
    PRIVATE
        board_fault_defs.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/board_fault_defs.h
)

target_include_directories(
    BoardCommon
    PUBLIC
        ./
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${DRIVER_INCLUDE_DIR}
        ${SRC_INCLUDE_DIR}
        ${BCM_EPDM_DIR}
        ${BCM_MILB_DIR}
)

target_link_libraries(
    BoardCommon
)
