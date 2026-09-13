cmake_minimum_required(VERSION 3.31)
project(Timber)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.1.0
)

FetchContent_MakeAvailable(SFML)

add_executable(Timber main.cpp)

target_link_libraries(Timber PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
)