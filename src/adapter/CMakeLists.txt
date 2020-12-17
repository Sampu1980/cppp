set(BUILD_SHARED_LIBS OFF)

include_directories(./
		            ${CMAKE_CURRENT_SOURCE_DIR}
	                ${COMMON_INCLUDE_DIR}
	                ${SRC_INCLUDE_DIR}
	                ${DRIVER_INCLUDE_DIR}
                    ${BCM_EPDM_DIR}
                    ${BCM_MILB_DIR}
)

file(GLOB SOURCES "./*.cpp")

# Hw
add_library(BoardAdapter STATIC ${SOURCES})

target_link_libraries(
    BoardAdapter
    BoardDriver
)
