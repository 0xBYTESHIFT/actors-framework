cmake_minimum_required(VERSION 3.0)

PROJECT(actors-framework CXX)

set(CMAKE_CXX_STANDARD 11 CACHE STRING "The C++ standard whose features are requested.")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "STANDARD CPP = ${CMAKE_CXX_STANDARD}")
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

option(ALLOW_EXAMPLES "ALLOW EXAMPLES" OFF)
option(ALLOW_TESTS "ALLOW TESTS" OFF)
option(RTTI_DISABLE "RTTI DISABLE" ON)
option(EXCEPTIONS_DISABLE "EXCEPTIONS DISABLE" ON)

message(STATUS "ALLOW_EXAMPLES = ${ALLOW_EXAMPLES}")
message(STATUS "ALLOW_TESTS = ${ALLOW_TESTS}")
message(STATUS "RTTI_DISABLE = ${RTTI_DISABLE}")
message(STATUS "EXCEPTIONS_DISABLE = ${EXCEPTIONS_DISABLE}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    if (EXCEPTIONS_DISABLE)
        message("Disable C++ exceptions.")
        string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    endif ()

    if (RTTI_DISABLE)
        message("Disable RTTI.")
        string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
    endif ()

else (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    if (EXCEPTIONS_DISABLE)
        message("Disable C++ exceptions.")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
    endif ()

    if (RTTI_DISABLE)
        message("Disable RTTI.")
        string(REGEX REPLACE "-frtti" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    endif ()

endif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")


if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    add_compile_options(
            -Wall
            -Wextra
            -Wpedantic

            -Wcast-align
            -Wcast-qual
            -Wconversion
            -Wctor-dtor-privacy
            -Wenum-compare
            -Wfloat-equal
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Woverloaded-virtual
            -Wredundant-decls
            -Wsign-conversion
            -Wsign-promo
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /WX)
endif ()

include(cmake/clang_tools.cmake)

include_directories(SYSTEM .)
include_directories(SYSTEM header)

set(HEADER_FILES
        header/actors-framework/base/actor_abstract.hpp
        header/actors-framework/base/actor.hpp
        header/actors-framework/base/address.hpp
        header/actors-framework/base/cooperative_actor.hpp
        header/actors-framework/base/basic_actor.hpp
        header/actors-framework/base/communication_module.hpp
        header/actors-framework/base/handler.hpp
        header/actors-framework/base/supervisor.hpp
        header/actors-framework/base/supervisor_abstract.hpp

        header/actors-framework/detail/pmr/container.hpp
        header/actors-framework/detail/pmr/memory_resource.hpp
        header/actors-framework/detail/aligned_allocate.hpp
        header/actors-framework/detail/any.hpp
        header/actors-framework/detail/callable_trait.hpp
        header/actors-framework/detail/intrusive_ptr.hpp
        header/actors-framework/detail/ref_counted.hpp
        header/actors-framework/detail/string_view.hpp
        header/actors-framework/detail/type_list.hpp
        header/actors-framework/detail/type_traits.hpp

        header/actors-framework/executor/policy/profiled.hpp
        header/actors-framework/executor/policy/unprofiled.hpp
        header/actors-framework/executor/policy/work_sharing.hpp
        header/actors-framework/executor/abstract_executor.hpp
        header/actors-framework/executor/executor.hpp
        header/actors-framework/executor/executable.hpp
        header/actors-framework/executor/profiled_executor.hpp
        header/actors-framework/executor/execution_device.hpp
        header/actors-framework/executor/worker.hpp

        header/actors-framework/base/message.hpp

        header/actors-framework/forwards.hpp
        header/actors-framework/link.hpp
        header/actors-framework/send.hpp
        header/actors-framework/core.hpp
        header/actors-framework/src.hpp
)

set(SOURCE_FILES
        source/src.cpp
)

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

find_package(Threads)

add_library(
        ${PROJECT_NAME}
        ${SOURCE_FILES}
        ${HEADER_FILES}
)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/header/actors-framework
        ${CMAKE_CURRENT_SOURCE_DIR}/header
)

if (ALLOW_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (ALLOW_TESTS)
    # configuration
    #############################################################################################
    #conan
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
    # conan
    #############################################################################################
    enable_testing()
    add_subdirectory(test)
endif ()
