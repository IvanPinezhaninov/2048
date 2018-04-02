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
if (Qt5_FOUND)
    message(STATUS "Found Qt ${Qt5_VERSION}: ${_qt5Core_install_prefix}")
endif()

if(WIN32 OR UNIX AND NOT APPLE)
    find_package(ImageMagick COMPONENTS convert REQUIRED)
endif()

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

set(TARGET_SOURCES
    ${HEADERS}
    ${SOURCES}
    ${RESOURCES}
)

if(APPLE)
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    set(MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${IDENTIFIER})
    set(MACOSX_BUNDLE_ICON_FILE 2048)
    set(OS_BUNDLE MACOSX_BUNDLE)
    set(ICON_FILE res/2048.icns)
    set(TARGET_SOURCES ${TARGET_SOURCES} ${ICON_FILE})
    set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
elseif(WIN32)
    set(OS_BUNDLE WIN32)
    add_custom_command(OUTPUT 2048.ico
        COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/res/icon.png"
            -define icon:auto-resize=256,128,96,64,48,40,32,24,16
            "${CMAKE_CURRENT_BINARY_DIR}/2048.ico"
        COMMENT "Create icon"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/res/icon.png"
    )
    add_custom_target(icon ALL DEPENDS 2048.ico)
    set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/2048.rc")
    file(WRITE ${RC_FILE} "IDI_ICON1 ICON DISCARDABLE 2048.ico\n")
    set(TARGET_SOURCES ${TARGET_SOURCES} ${RC_FILE})
else()
    set(ICON_SIZES 1024 512 256 128 64 48 32 24 22 16)
    foreach(SIZE ${ICON_SIZES})
        set(ICON_NAME icon-${SIZE}.png)
        list(APPEND ICON_NAMES ${ICON_NAME})
        add_custom_command(OUTPUT ${ICON_NAME}
            COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/res/icon.png" -resize ${SIZE}x${SIZE} ${ICON_NAME}
            COMMENT "Resizing icon"
            MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/res/icon.png"
        )
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${ICON_NAME}" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${SIZE}x${SIZE}/apps" RENAME 2048.png)
    endforeach()
    add_custom_target(icon ALL DEPENDS ${ICON_NAMES})
    set(DESKTOP_ENTRY_VERSION ${PROJECT_VERSION})
    set(DESKTOP_ENTRY_EXEC "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${TARGET}")
    configure_file(res/desktop.in "${CMAKE_CURRENT_BINARY_DIR}/2048.desktop")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/2048.desktop" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications/")
endif()

add_executable(${TARGET} ${OS_BUNDLE} ${TARGET_SOURCES})

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
