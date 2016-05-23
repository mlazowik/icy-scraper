cmake_minimum_required(VERSION 2.8)

project(chat)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

include_directories(
        ${PROJECT_SOURCE_DIR}/src
)

set(GOOGLETEST_ROOT lib/googletest/googletest CACHE STRING "Google Test source root")

include_directories(
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/include
)

set(GOOGLETEST_SOURCES
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/src/gtest-all.cc
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/src/gtest_main.cc)

foreach(_source ${GOOGLETEST_SOURCES})
    set_source_files_properties(${_source} PROPERTIES GENERATED 1)
endforeach()

add_library(googletest ${GOOGLETEST_SOURCES})

set(SOURCE_TEST_FILES test/main.cpp)
add_executable(unit_tests ${SOURCE_TEST_FILES})

add_dependencies(unit_tests googletest)

target_link_libraries(
        unit_tests
        googletest
        pthread
)

include(CTest)
enable_testing()

set(PLAYER_SOURCE_FILES
        src/scraper_player/player.cpp
        src/scraper_player/player_options.cpp
        src/scraper_player/player_options.h
        src/options/options.h
        src/common/socket.cpp
        src/common/socket.h
        src/common/io_events.cpp
        src/common/io_events.h
        src/common/stream_reader.cpp
        src/common/stream_reader.h
        src/common/stream.h
        src/common/connection.cpp
        src/common/connection.h
        src/scraper_player/scraper_player.cpp
        src/scraper_player/scraper_player.h
        src/common/connection.cpp
        src/common/connection.h
        src/common/descriptor.h src/options/options.cpp src/options/parser.h src/options/number_parser.cpp src/options/number_parser.h src/options/string_parser.cpp src/options/string_parser.h src/options/boolean_parser.cpp src/options/boolean_parser.h)

add_executable(player ${PLAYER_SOURCE_FILES})