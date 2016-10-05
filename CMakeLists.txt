cmake_minimum_required(VERSION 3.5)
project(software_renderer)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -D_GNU_SOURCE -D_THREAD_SAFE -std=c++11")

set(SOURCE_FILES
        main.cpp
        Color.cpp
        Color.h
        Rasterizer.cpp
        Rasterizer.h
        Model.cpp
        Model.h
        TGAImage.cpp
        TGAImage.h
        Definitions.h
        AffineTransforms.cpp
        AffineTransforms.h
        OldMainRoutines.cpp
        OldMainRoutines.h
        Shaders.h)

find_package(SDL REQUIRED)
find_package(Eigen3 REQUIRED)

MESSAGE(STATUS "SDL_INCLUDE_DIR: " ${SDL_INCLUDE_DIR})
MESSAGE(STATUS "EIGEN3_INCLUDE_DIR: " ${EIGEN3_INCLUDE_DIR})

INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL_LIBRARY})
