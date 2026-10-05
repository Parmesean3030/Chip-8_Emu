cmake_minimum_required(VERSION 3.31.6)

project(CHIP-8_Emulator)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.28.5
)
FetchContent_MakeAvailable(SDL2)


set(SOURCES src/Main.cpp src/Chip_8.cpp)


add_executable(Chip-8 ${SOURCES})


target_link_libraries(Chip-8 PRIVATE SDL2-static)


add_custom_command(TARGET Chip-8 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/ROMs"
    "$<TARGET_FILE_DIR:Chip-8>/ROMs"
)