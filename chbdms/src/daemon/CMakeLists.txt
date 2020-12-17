set(CMAKE_CXX_FLAGS "-pthread")

# setting for gearbox
add_definitions(-fPIC -DZ_LINUX  -DLINUX_COMPILE)

if (${ARCH} STREQUAL "arm64")
    message("*** Build Target Set For ARM 64")
elseif(${ARCH} STREQUAL "x86")
    message("*** Build Target Set For x86 64")
    add_definitions(-DARCH_x86)
endif()

set(CMAKE_CXX_STANDARD 14)
add_definitions(-DZ_arm -DPLP_MILLENIOB_SUPPORT -DPLP_EPDM_MILLENIOB)
set(CMAKE_C_FLAGS "-DPLP_MILLENIOB_SUPPORT")
set(CMAKE_C_FLAGS "-DPLP_EPDM_MILLENIOB")

include_directories(.
					./include
		            ${CMAKE_CURRENT_SOURCE_DIR}
		            ${MANAGER_INCLUDE_DIR}
			        ${ADAPTER_INCLUDE_DIR}
			        ${DRIVER_INCLUDE_DIR}
	                ${COMMON_INCLUDE_DIR}
	                ${SRC_INCLUDE_DIR}
                    ${BCM_EPDM_DIR}
                    ${BCM_MILB_DIR}			    
			
)

# HW
add_executable(
    BoardMs 
    board_ms.cpp 
)
    
target_link_libraries(
    BoardMs
    BoardManager
    BoardAdapter
    BoardDriver
    BoardCommon
    DbgCmds
    cli::cli
    Util
    MfgEepromUtil
    eeprom_static
    jsoncpp_static
    gearbox
    libInfnLogger.a
    ${ARCH_LIB_DIR}/libboost_system.a
    ${ARCH_LIB_DIR}/libboost_thread.a
    ${ARCH_LIB_DIR}/libboost_log.a
    ${ARCH_LIB_DIR}/libboost_log_setup.a
    ${ARCH_LIB_DIR}/libboost_filesystem.a
)
    
# board init executable for Hw    
add_executable(
    BoardInitMs
    board_init.cpp 
)

target_include_directories(
    BoardInitMs
	PUBLIC
	.
	${BOARD_INIT_INCLUDE_DIR}
	${PROTOGEN_INCLUDE_DIR}
	${DRIVER_INCLUDE_DIR}
)

target_link_libraries(
    BoardInitMs
    BoardInitManager
    RegIf
    BoardInitUtil
    BoardCommon
    gearbox
    libInfnLogger.a
    ${ARCH_LIB_DIR}/libboost_system.a
    ${ARCH_LIB_DIR}/libboost_thread.a
    ${ARCH_LIB_DIR}/libboost_log.a
    ${ARCH_LIB_DIR}/libboost_log_setup.a
    ${ARCH_LIB_DIR}/libboost_filesystem.a
)
