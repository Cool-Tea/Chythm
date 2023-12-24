cmake_minimum_required(VERSION 3.0.0)

project(Chythm)

find_package(SDL2 REQUIRED)
find_library(NAME SDL2_image REQUIRED)
find_library(NAME SDL2_ttf REQUIRED)
find_library(NAME SDL2_mixer REQUIRED)
find_library(NAME SDL2_gfx REQUIRED)
link_libraries(SDL2_image SDL2_ttf SDL2_mixer SDL2_gfx)

aux_source_directory(src DIR_SRCS)

add_executable(Demo ${DIR_SRCS})

include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(Demo 
    ${SDL2_LIBRARIES} 
    ${SDL2_image_LIBRARIES} 
    ${SDL2_ttf_LIBRARIES} 
    ${SDL2_mixer_LIBRARIES}
    ${SDL2_gfx_LIBRARIES}
)