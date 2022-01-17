cmake_minimum_required(VERSION 3.21)
project(engine_chromatique)

set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    link_directories(dependencies/lib/win $ENV{VULKAN_SDK}/lib)
endif ()

add_executable(engine_chromatique app.cpp app.hpp src/engine.cpp include/engine.hpp include/swapchain.hpp)

if (WIN32)
    target_include_directories(engine_chromatique PUBLIC
            include
            dependencies/include
            $ENV{VULKAN_SDK}/include)

    target_link_libraries(engine_chromatique
            glfw3 vulkan-1)
elseif (UNIX)
    target_include_directories(engine_chromatique PUBLIC
            include /usr/include/)
    target_link_libraries(engine_chromatique
            glfw vulkan)
endif ()
