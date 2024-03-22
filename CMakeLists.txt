cmake_minimum_required(VERSION 3.24)

project(TICTACTOE)

set(WITH_CUDA OFF)
find_package(OpenCV REQUIRED)

set(SOURCES src/minimax.cpp)

add_executable(main ${SOURCES})

target_link_libraries(main ${OpenCV_LIBS})
