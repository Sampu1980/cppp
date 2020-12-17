cmake_minimum_required(VERSION 3.13)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CUSTOM_LIBRARY_OUTPUT_DIRECTORY})
set(DBGCMDS_BUILT_FILES ${CMAKE_BINARY_DIR}/lib/libDbgCmds${CMAKE_STATIC_LIBRARY_SUFFIX})

set(BUILD_SHARED_LIBS OFF)

add_definitions(-fPIC)

include_directories(. 
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CLI_INCLUDE_DIR} 
                    ${COMMON_INCLUDE_DIR}
                    ${SRC_INCLUDE_DIR}
                    ${DRIVER_INCLUDE_DIR}
	                ${ADAPTER_INCLUDE_DIR}
	                ${MANAGER_INCLUDE_DIR}
	                ${PROTOGEN_INCLUDE_DIR} 
                    ${BCM_EPDM_DIR}
                    ${BCM_MILB_DIR}
)

file(GLOB SOURCES "./*.cpp")

# HW 
add_library(DbgCmds STATIC ${SOURCES})

target_link_libraries(
    DbgCmds
    libRegIf.a
    )
