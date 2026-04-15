cmake_minimum_required(VERSION 3.20)
project(PowerGame VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set Windows subsystem for GUI application
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

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

# Windows resource file for icon
if(WIN32)
    set(WINDOWS_RESOURCES "${CMAKE_SOURCE_DIR}/Resources/PowerGame.rc")
    if(EXISTS ${WINDOWS_RESOURCES})
        list(APPEND POWERGAME_SOURCES ${WINDOWS_RESOURCES})
    endif()
endif()

add_executable(PowerGame ${POWERGAME_SOURCES})

target_include_directories(PowerGame PRIVATE
    ${CMAKE_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/ThirdParty
)

# Find packages
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
    # Set Windows subsystem but allow console for debugging
    set_target_properties(PowerGame PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
else()
    target_compile_options(PowerGame PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

# Copy content files to build directory
add_custom_command(TARGET PowerGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Content $<TARGET_FILE_DIR:PowerGame>/Content
)

# Copy SDL2 DLLs on Windows
if(WIN32)
    add_custom_command(TARGET PowerGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2> $<TARGET_FILE_DIR:PowerGame>
    )
    add_custom_command(TARGET PowerGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2_image> $<TARGET_FILE_DIR:PowerGame>
    )
    add_custom_command(TARGET PowerGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2_ttf> $<TARGET_FILE_DIR:PowerGame>
    )
    add_custom_command(TARGET PowerGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2_mixer> $<TARGET_FILE_DIR:PowerGame>
    )
endif()

enable_testing()

file(GLOB_RECURSE TEST_SOURCES "Tests/*.cpp")
if(TEST_SOURCES)
    add_executable(PowerGameTests ${TEST_SOURCES})
    target_include_directories(PowerGameTests PRIVATE
        ${CMAKE_SOURCE_DIR}/Source
        ${CMAKE_SOURCE_DIR}/ThirdParty
    )
    target_link_libraries(PowerGameTests PRIVATE
        nlohmann_json::nlohmann_json
    )
    add_test(NAME PowerGameTests COMMAND PowerGameTests)
endif()

# Install targets
install(TARGETS PowerGame
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY Content/ DESTINATION Content)
install(DIRECTORY Docs/ DESTINATION Docs)
install(FILES README.md README_中文.md LICENSE DESTINATION .)
