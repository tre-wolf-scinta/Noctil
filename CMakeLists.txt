cmake_minimum_required(VERSION 3.20)
project(Noctil VERSION 2.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

include_directories(include)


# Needed for event loop
find_package(Threads REQUIRED)

add_executable(noctil_engine
    src/main.cpp
    src/core/EventLoop.cpp
)


target_link_libraries(noctil_engine PRIVATE Threads::Threads)
