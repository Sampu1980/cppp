set(BUILD_SHARED_LIBS OFF)

include_directories(.
		            ${CMAKE_CURRENT_SOURCE_DIR}
	                ${COMMON_INCLUDE_DIR}
	                ${SRC_INCLUDE_DIR}
                    ${BCM_EPDM_DIR}
                    ${BCM_MILB_DIR}
)

file(GLOB SOURCES "./*.cpp")

# HW 
add_library(BoardDriver STATIC ${SOURCES} )

if (${ARCH} STREQUAL "arm64")
target_link_libraries(
    BoardDriver
    BoardCommon
    libRegIf.a
    libgeckointf.a
    libgecko_access.a
)
else()
target_link_libraries(
    BoardDriver
    BoardCommon
    libRegIf.a
)
endif()

if (${ARCH} STREQUAL "arm64")
    message("*** Build Target Set For ARM 64")
elseif(${ARCH} STREQUAL "x86")
    message("*** Build Target Set For x86 64")
    add_definitions(-DARCH_x86)
endif()

add_library(gearbox "")

# gearbox "arm64" version
if (${ARCH} STREQUAL "arm64")

add_definitions(-DZ_arm -DPLP_MILLENIOB_SUPPORT -DPLP_EPDM_MILLENIOB)
set(CMAKE_C_FLAGS "-DPLP_MILLENIOB_SUPPORT")
set(CMAKE_C_FLAGS "-DPLP_EPDM_MILLENIOB")

target_sources(
    gearbox
    PRIVATE
    	Bcm81725.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Bcm81725.h
        ${CMAKE_CURRENT_LIST_DIR}/GearBoxIf.h
)

target_include_directories(gearbox PUBLIC 
                           ${UTIL_INCLUDE_DIR}              
                           ${BCM_EPDM_DIR}
                           ${BCM_MILB_DIR}
)

target_link_libraries(
	gearbox
        libRegIf.a
        libUtil.a
)

target_link_libraries(gearbox "-Wl,-whole-archive" ${BCM_EPDM_LIB_DIR}/libepdm.a ${BCM_MILB_LIB_DIR}/libphymodepil.a "-Wl,-no-whole-archive")

elseif(${ARCH} STREQUAL "x86")

target_sources(
    gearbox
    PRIVATE
    	Bcm81725Sim.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Bcm81725Sim.h
)

target_include_directories(gearbox PUBLIC 
                           ${UTIL_INCLUDE_DIR}              
)

target_link_libraries(
	gearbox
        libUtil.a
)
endif()
    
if (${ARCH} STREQUAL "arm64")
# HW INIT
add_library(BoardInitUtil STATIC board_init_util.cpp )

target_sources (
    BoardInitUtil
    PRIVATE
        board_init_util.cpp
        board_common_driver.cpp
        Bcm81725.cpp
        Bcm81725Sim.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/board_init_util.h
        ${CMAKE_CURRENT_LIST_DIR}/board_common_driver.h
        ${CMAKE_CURRENT_LIST_DIR}/Bcm81725.h
        ${CMAKE_CURRENT_LIST_DIR}/Bcm81725Sim.h
)

target_include_directories(
    BoardInitUtil
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${COMMON_INCLUDE_DIR}    
)

target_link_libraries(
    BoardInitUtil
    BoardCommon
    libRegIf.a
    libgeckointf.a
    libgecko_access.a
)

elseif(${ARCH} STREQUAL "x86")

add_library(BoardInitUtil STATIC board_init_util.cpp )

target_sources (
    BoardInitUtil
    PRIVATE
        board_init_util.cpp
        board_common_driver.cpp
        Bcm81725Sim.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/board_init_util.h
        ${CMAKE_CURRENT_LIST_DIR}/board_common_driver.h
        ${CMAKE_CURRENT_LIST_DIR}/Bcm81725Sim.h
)

target_include_directories(
    BoardInitUtil
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${COMMON_INCLUDE_DIR}    
)

target_link_libraries(
    BoardInitUtil
    BoardCommon
    libRegIf.a
)
endif()


