cmake_minimum_required(VERSION 3.1.1)

project(2048 LANGUAGES CXX VERSION 1.0)

include(GNUInstallDirs)

set(COMPANY "Ivan Pinezhaninov")
set(COPYRIGHT "Copyright © 2018 Ivan Pinezhaninov. All rights reserved.")
set(IDENTIFIER "pinezhaninov.ivan.2048")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

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

set(ICON_NAME 2048)

if(APPLE)
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    set(MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${IDENTIFIER})
    set(MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})
    set(OS_BUNDLE MACOSX_BUNDLE)
    set(ICON_FILE res/${ICON_NAME}.icns)
    set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
elseif(WIN32)
    set(OS_BUNDLE WIN32)
    set(ICON_FILE res/${ICON_NAME}.rc)
endif()

set(TARGET 2048)

add_executable(${TARGET} ${OS_BUNDLE}
    ${HEADERS}
    ${SOURCES}
    ${RESOURCES}
    ${ICON_FILE}
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
    ORGANIZATION_NAME="${COMPANY}"
    APPLICATION_NAME="${CMAKE_PROJECT_NAME}"
)

target_link_libraries(${TARGET} PRIVATE
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
)

install(TARGETS ${TARGET}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR}
)
