cmake_minimum_required (VERSION 2.8.12)
project (pomerol_example CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
#include(CommonDefs)

# Disable build in source
if (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message(FATAL_ERROR "In source builds are disabled. Please use a separate build directory.")
endif()
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_package(pomerol)
message(STATUS "Pomerol includes : ${pomerol_INCLUDE_DIRS}")
message(STATUS "Pomerol libraries : ${pomerol_LIBRARIES}")

include_directories(${pomerol_INCLUDE_DIRS})
include_directories(include)

set (sources
src/pomerol/OperatorPresetsExtra.cpp
src/pomerol/LatticePresetsExtra.cpp
src/external/AtomicCoefficients.cpp
)

add_executable(atomic atomic.cpp ${sources})
target_link_libraries(atomic ${pomerol_LIBRARIES})

find_package (GSL REQUIRED)
message(STATUS "GSL includes: " ${GSL_INCLUDE_DIR} )
message(STATUS "GSL libs: ${GSL_LIBRARIES}" )
#target_include_directories(atomic PUBLIC ${GSL_INCLUDE_DIR})
target_link_libraries(atomic ${GSL_LIBRARIES})
