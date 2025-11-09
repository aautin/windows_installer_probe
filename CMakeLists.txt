cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(windows_installer_probe LANGUAGES CXX)

# CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_BINARY_DIR}/cmake)

# Dependencies
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Executable
add_executable(${PROJECT_NAME} main.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)
if (WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE msi)
endif()