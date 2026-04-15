cmake_minimum_required(VERSION 3.20)
project(PowerGame VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

file(GLOB_RECURSE POWERGAME_SOURCES
    "Source/PowerGame/Core/*.cpp"
    "Source/PowerGame/Resources/*.cpp"
    "Source/PowerGame/Faction/*.cpp"
    "Source/PowerGame/Intelligence/*.cpp"
    "Source/PowerGame/Alliance/*.cpp"
    "Source/PowerGame/Character/*.cpp"
    "Source/PowerGame/Story/*.cpp"
    "Source/PowerGame/Strategy/*.cpp"
    "Source/PowerGame/Ending/*.cpp"
    "Source/PowerGame/UI/*.cpp"
    "Source/PowerGame/Data/*.cpp"
    "Source/PowerGame/AI/*.cpp"
    "Source/PowerGame/Game/*.cpp"
)

add_executable(PowerGame ${POWERGAME_SOURCES})

target_include_directories(PowerGame PRIVATE
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/ThirdParty
)

target_link_libraries(PowerGame PRIVATE
    nlohmann_json::nlohmann_json
)

if(MSVC)
    target_compile_options(PowerGame PRIVATE /utf-8 /W3 /permissive- /Zc:__cplusplus)
else()
    target_compile_options(PowerGame PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

enable_testing()

file(GLOB_RECURSE TEST_SOURCES "Tests/*.cpp")
add_executable(PowerGameTests ${TEST_SOURCES})
target_include_directories(PowerGameTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/ThirdParty
)
target_link_libraries(PowerGameTests PRIVATE
    nlohmann_json::nlohmann_json
)
add_test(NAME PowerGameTests COMMAND PowerGameTests)
