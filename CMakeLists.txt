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

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/options)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/networking)

set(PLAYER_SOURCE_FILES
        src/scraper_player/player.cpp
        src/scraper_player/player_options.cpp
        src/scraper_player/player_options.h
        src/scraper_player/scraper_player.cpp
        src/scraper_player/scraper_player.h src/scraper_player/header_reader.cpp src/scraper_player/header_reader.h src/scraper_player/metadata_reader.cpp src/scraper_player/metadata_reader.h)

add_executable(player ${PLAYER_SOURCE_FILES})

add_dependencies(player options stream io networking string_utils)

target_link_libraries(player options stream io networking string_utils)