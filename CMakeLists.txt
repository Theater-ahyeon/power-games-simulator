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
    "Source/PowerGame/GUI/*.cpp"
    "Source/PowerGame/Audio/*.cpp"
)

# Add explicit source files for GUI TutorialSystem
set(GUI_TUTORIAL_SOURCES
    "Source/PowerGame/GUI/TutorialSystem.h"
    "Source/PowerGame/GUI/TutorialSystem.cpp"
)

# Add explicit source files for Game systems
set(GAME_SYSTEM_SOURCES
    "Source/PowerGame/Game/AchievementSystem.h"
    "Source/PowerGame/Game/AchievementSystem.cpp"
    "Source/PowerGame/Game/GameStateManager.h"
    "Source/PowerGame/Game/GameStateManager.cpp"
)

add_executable(PowerGame ${POWERGAME_SOURCES})

target_include_directories(PowerGame PRIVATE
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/ThirdParty
)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

target_link_libraries(PowerGame PRIVATE
    nlohmann_json::nlohmann_json
    SDL2::SDL2
    SDL2::SDL2_image
    SDL2::SDL2_ttf
    SDL2::SDL2_mixer
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
