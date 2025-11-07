cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(windows_installer_probe LANGUAGES CXX)

# CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_BINARY_DIR}/cmake)

# Dependencies
find_package(sbgCMakeTools REQUIRED) # For WinDeployQt and windeployqt_build (add to conanfile in build_requirements() methode)
find_package(qt-binaries REQUIRED COMPONENTS Widgets Quick Core Gui) # Qt version
find_package(Qt6 REQUIRED COMPONENTS Widgets Quick Core Gui) # Qt Components

# Executable
add_executable(${PROJECT_NAME} main.cpp) # Add source files
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui) # Link libraries

# Windows deployment
if (WIN32)
    include(WinDeployQt)
    windeployqt_build(${PROJECT_NAME} $<TARGET_FILE_DIR:${PROJECT_NAME}> DEPLOYMENT_TARGET ${PROJECT_NAME} POST)
endif()