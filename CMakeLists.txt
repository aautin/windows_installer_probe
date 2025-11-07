cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(windows_installer_probe LANGUAGES CXX)

# CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_BINARY_DIR}/cmake)

# Dependencies
find_package(qt-binaries REQUIRED COMPONENTS Widgets Quick Core Gui) # Qt version
find_package(Qt6 REQUIRED COMPONENTS Widgets Quick Core Gui) # Qt Components

# Executable
add_executable(${PROJECT_NAME} main.cpp) # Add source files
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui) # Link libraries
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE msi)
endif()

# Windows deployment
if (WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")
    
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:${PROJECT_NAME}>"
        )
    endif()
endif()