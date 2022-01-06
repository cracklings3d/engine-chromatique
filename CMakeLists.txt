cmake_minimum_required(VERSION 3.21)
project(engine_chromatique)

set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    link_directories(dependencies/lib/win)
endif ()

add_executable(engine_chromatique app.cpp)

target_include_directories(engine_chromatique PUBLIC dependencies/include)

if (WIN32)
    target_link_libraries(engine_chromatique glfw3)
endif ()
