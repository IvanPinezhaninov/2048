cmake_minimum_required(VERSION 3.1.1)

project(2048 LANGUAGES CXX VERSION 1.0)

set(COMPANY "Ivan Pinezhaninov")
set(COPYRIGHT "Copyright © 2018 Ivan Pinezhaninov. All rights reserved.")
set(IDENTIFIER "pinezhaninov.ivan.2048")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(GNUInstallDirs)
include(cmake/CreateIcon.cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt5 COMPONENTS Core Gui Quick REQUIRED)
if (Qt5_FOUND)
    message(STATUS "Found Qt ${Qt5_VERSION}: ${_qt5Core_install_prefix}")
endif()

add_definitions(
    ${Qt5Core_DEFINITIONS}
    ${Qt5Gui_DEFINITIONS}
    ${Qt5Quick_DEFINITIONS}
)

list(APPEND CMAKE_CXX_FLAGS
    ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}
    ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}
    ${Qt5Quick_EXECUTABLE_COMPILE_FLAGS}
)

list(REMOVE_DUPLICATES CMAKE_CXX_FLAGS)

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

set(TARGET_SOURCES
    ${HEADERS}
    ${SOURCES}
    ${RESOURCES}
)

createIconFromSource("${CMAKE_CURRENT_SOURCE_DIR}/res/icon.png")

if(APPLE)
    set(OS_BUNDLE MACOSX_BUNDLE)
    set(ICON_NAME icon.icns)
    set(ICON_FILE "${CMAKE_CURRENT_BINARY_DIR}/${ICON_NAME}")
    if(EXISTS ${ICON_FILE})
        list(APPEND TARGET_SOURCES ${ICON_FILE})
        set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    endif()
elseif(WIN32)
    set(OS_BUNDLE WIN32)
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/icon.ico")
        set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/2048.rc")
        file(WRITE ${RC_FILE} "IDI_ICON1 ICON DISCARDABLE icon.ico\n")
        list(APPEND TARGET_SOURCES ${RC_FILE})
    endif()
else()
    set(DESKTOP_ENTRY_VERSION ${PROJECT_VERSION})
    set(DESKTOP_ENTRY_EXEC "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${TARGET}")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/res/desktop.in" "${CMAKE_CURRENT_BINARY_DIR}/2048.desktop")
    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/2048.desktop"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications/"
    )
endif()

add_executable(${TARGET} ${OS_BUNDLE} ${TARGET_SOURCES})

if(APPLE)
    set_target_properties(${TARGET} PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
        MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT}
        MACOSX_BUNDLE_GUI_IDENTIFIER ${IDENTIFIER}
        MACOSX_BUNDLE_ICON_FILE ${ICON_NAME}
    )
endif()

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
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    BUNDLE DESTINATION . COMPONENT Runtime
)
