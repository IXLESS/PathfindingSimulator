cmake_minimum_required(VERSION 3.16)

project(PathfindingSimulator LANGUAGES CXX)

# C++20 zaradi std::optional, strukture SFML 3 ipd.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SFML 3 (Graphics, Window, System)
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(PathfindingSimulator
    main.cpp
    BFSRunner.cpp
    DijkstraRunner.cpp
    AStarRunner.cpp
)

# Headerji (Grid.hpp, *Runner.hpp, bfs.hpp) so v isti mapi kot CMakeLists.txt
target_include_directories(PathfindingSimulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(PathfindingSimulator
    PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)
