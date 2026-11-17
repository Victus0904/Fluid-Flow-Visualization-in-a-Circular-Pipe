cmake_minimum_required(VERSION 3.16)
project(FluidFlowPipe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Works with SFML 3 (SFML::Graphics) and SFML 2.6 (sfml-graphics).
find_package(SFML 3 COMPONENTS Graphics Window System QUIET)
if(SFML_FOUND)
    set(SFML_LIBS SFML::Graphics SFML::Window SFML::System)
else()
    find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
    set(SFML_LIBS sfml-graphics sfml-window sfml-system)
endif()

add_executable(fluid_flow main.cpp)
target_link_libraries(fluid_flow PRIVATE ${SFML_LIBS})

if(MSVC)
    target_compile_options(fluid_flow PRIVATE /W4)
else()
    target_compile_options(fluid_flow PRIVATE -Wall -Wextra -Wpedantic)
endif()
