#=========================================================================
#
# Copyright (C) 2018 Ivan Pinezhaninov <ivan.pinezhaninov@gmail.com>
#
# This file is part of the 2048 Game.
#
# The 2048 Game is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The 2048 Game is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the 2048 Game.  If not, see <http://www.gnu.org/licenses/>.
#
#=========================================================================


cmake_minimum_required(VERSION 3.1.1)

project(2048 LANGUAGES CXX VERSION 1.0)

set(COMPANY "Ivan Pinezhaninov")
set(COPYRIGHT "Copyright © 2018 Ivan Pinezhaninov. All rights reserved.")
set(IDENTIFIER "pinezhaninov.ivan.2048")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(GNUInstallDirs)
include(cmake/CreateIcon.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt5 5.1 COMPONENTS Core Gui Quick Sql REQUIRED)
if (Qt5_FOUND)
    message(STATUS "Found Qt ${Qt5_VERSION}: ${_qt5Core_install_prefix}")
endif()

add_definitions(
    ${Qt5Core_DEFINITIONS}
    ${Qt5Gui_DEFINITIONS}
    ${Qt5Quick_DEFINITIONS}
    ${Qt5Sql_DEFINITIONS}
)

list(APPEND CMAKE_CXX_FLAGS
    ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}
    ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}
    ${Qt5Quick_EXECUTABLE_COMPILE_FLAGS}
    ${Qt5Sql_EXECUTABLE_COMPILE_FLAGS}
)

list(REMOVE_DUPLICATES CMAKE_CXX_FLAGS)

set(HEADERS
    src/cell.h
    src/tile.h
    src/gameboard.h
    src/game.h
    src/gamecontroller.h
    src/gamestate.h
    src/movedirection.h
    src/tilespec.h
    src/turnspec.h
    src/gamespec.h
    src/storage.h
    src/storageworker.h
)

SET(MOC_HEADERS
    src/cell.h
    src/tile.h
    src/gameboard.h
    src/game.h
    src/gamecontroller.h
    src/storage.h
    src/storageworker.h
)

set(SOURCES
    src/cell.cpp
    src/tile.cpp
    src/gameboard.cpp
    src/game.cpp
    src/gamecontroller.cpp
    src/tilespec.cpp
    src/turnspec.cpp
    src/gamespec.cpp
    src/storage.cpp
    src/storageworker.cpp
    src/main.cpp
)

set(RESOURCES
    resources.qrc
)

qt5_wrap_cpp(SOURCES ${MOC_HEADERS})
qt5_add_resources(SOURCES ${RESOURCES})


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

target_include_directories(${TARGET} PRIVATE
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt5Sql_INCLUDE_DIRS}
)

target_compile_definitions(${TARGET} PRIVATE
    ${Qt5Core_COMPILE_DEFINITIONS}
    ${Qt5Gui_COMPILE_DEFINITIONS}
    ${Qt5Quick_COMPILE_DEFINITIONS}
    ${Qt5Sql_COMPILE_DEFINITIONS}
    ORGANIZATION_NAME="${COMPANY}"
    APPLICATION_NAME="${CMAKE_PROJECT_NAME}"
)

target_link_libraries(${TARGET} PRIVATE
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
    ${Qt5Sql_LIBRARIES}
)

install(TARGETS ${TARGET}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    BUNDLE DESTINATION . COMPONENT Runtime
)

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
    include(cmake/DeployMacOsX.cmake)
endif()
