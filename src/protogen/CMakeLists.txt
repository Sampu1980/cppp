cmake_minimum_required (VERSION 3.13)

set(BUILD_SHARED_LIBS OFF)

set(CMAKE_BUILD_TYPE Debug)

#Bring the headers
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/cpp")  

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cpp/*.cc")         

#Generate the static library 
add_library(protogen_cpp STATIC ${SOURCES})
