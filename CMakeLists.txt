cmake_minimum_required(VERSION 3.1.1)

project(2048 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt5 COMPONENTS Core Gui Quick REQUIRED)

add_definitions(
    ${Qt5Core_DEFINITIONS}
    ${Qt5Gui_DEFINITIONS}
    ${Qt5Quick_DEFINITIONS}
)

set(CMAKE_CXX_FLAGS
    ${CMAKE_CXX_FLAGS}
    ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}
    ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}
    ${Qt5Quick_EXECUTABLE_COMPILE_FLAGS}
)

set(HEADERS
    src/cell.h
    src/tile.h
    src/gameboard.h
    src/game.h
)

set(SOURCES
    src/cell.cpp
    src/tile.cpp
    src/gameboard.cpp
    src/game.cpp
    src/main.cpp
)

set(RESOURCES
    resources.qrc
)

set(TARGET 2048)

add_executable(${TARGET}
    ${HEADERS}
    ${SOURCES}
    ${RESOURCES}
)

target_include_directories(${TARGET} PRIVATE
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
)

target_compile_definitions(${TARGET} PRIVATE
    ${Qt5Core_COMPILE_DEFINITIONS}
    ${Qt5Gui_COMPILE_DEFINITIONS}
    ${Qt5Quick_COMPILE_DEFINITIONS}
    ORGANIZATION_NAME="Ivan Pinezhaninov"
    APPLICATION_NAME="${CMAKE_PROJECT_NAME}"
)

target_link_libraries(${TARGET} PRIVATE
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
)

install(TARGETS ${TARGET} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
