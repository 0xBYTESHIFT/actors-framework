project(testsuite CXX)

include_directories(.)

set(HEADER_FILES
        network/connection_identifying.hpp
        network/fake_multiplexer.hpp
        forwards.hpp
        )

set(SOURCE_FILES
        source/network/connection_identifying.cpp
        source/network/fake_multiplexer.cpp
        )

add_library(
        ${PROJECT_NAME}
        ${SOURCE_FILES}
        ${HEADER_FILES}
)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/network
)
