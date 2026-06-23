cmake_minimum_required(VERSION 3.20)
project(pong)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

add_executable(pong
        src/main.cpp
        src/input.h
        src/scene.h
        src/scene_manager.h
        src/main_menu_scene.h
        src/app.h
        src/utilities.h
        src/game_scene.h
        src/paddle.h
        src/ball.h
        src/texture.h
        src/audio_clip.h
        )

target_link_libraries(pong PRIVATE 
                SDL3::SDL3
                SDL3_ttf::SDL3_ttf
                SDL3_image::SDL3_image
                SDL3_mixer::SDL3_mixer)