project(benchmark CXX)

set(SOURCE_FILES main.cpp utils.cpp)

message(STATUS "PROJECT_NAME = ${PROJECT_NAME}")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} actors-framework)
