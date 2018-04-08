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

if(NOT EXISTS ${_qt5Core_install_prefix})
    message(FATAL_ERROR "Qt root directory not found")
endif()

set(QT_ROOT_DIR ${_qt5Core_install_prefix})
set(BUNDLE_DIR "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app")
set(BUNDLE_RESOURCES_DIR "${BUNDLE_DIR}/Contents/Resources")
set(BUNDLE_PLUGINS_DIR "${BUNDLE_DIR}/Contents/PlugIns")

# install qt.conf

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
    "[Paths]\nPlugins = PlugIns\n"
    "Imports = Resources/qml\n"
    "Qml2Imports = Resources/qml\n"
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
    DESTINATION "${BUNDLE_RESOURCES_DIR}"
)

# install libqcocoa.dylib

install(
    FILES "${QT_ROOT_DIR}/plugins/platforms/libqcocoa.dylib"
    DESTINATION "${BUNDLE_PLUGINS_DIR}/platforms"
)

# install libqtquick2plugin.dylib

install(
    FILES "${QT_ROOT_DIR}/qml/QtQuick.2/libqtquick2plugin.dylib"
    DESTINATION "${BUNDLE_PLUGINS_DIR}/qml"
)

install(
    FILES "${QT_ROOT_DIR}/qml/QtQuick.2/qmldir"
    DESTINATION "${BUNDLE_RESOURCES_DIR}/qml/QtQuick.2"
)

# create libqtquick2plugin.dylib symlink

install(CODE "
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
        \"../../../PlugIns/qml/libqtquick2plugin.dylib\"
        \"${BUNDLE_RESOURCES_DIR}/qml/QtQuick.2/libqtquick2plugin.dylib\"
    )"
)

# install libwindowplugin.dylib

install(
    FILES "${QT_ROOT_DIR}/qml/QtQuick/Window.2/libwindowplugin.dylib"
    DESTINATION "${BUNDLE_PLUGINS_DIR}/qml"
)

install(
    FILES "${QT_ROOT_DIR}/qml/QtQuick/Window.2/qmldir"
    DESTINATION "${BUNDLE_RESOURCES_DIR}/qml/QtQuick/Window.2"
)

# create libwindowplugin.dylib symlink

install(CODE "
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
        \"../../../../PlugIns/qml/libwindowplugin.dylib\"
        \"${BUNDLE_RESOURCES_DIR}/qml/QtQuick/Window.2/libwindowplugin.dylib\"
    )"
)

# fixup bundle

set(FIXUP_LIBS
    "${BUNDLE_PLUGINS_DIR}/platforms/libqcocoa.dylib"
    "${BUNDLE_PLUGINS_DIR}/qml/libqtquick2plugin.dylib"
    "${BUNDLE_PLUGINS_DIR}/qml/libwindowplugin.dylib"
)

install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"${BUNDLE_DIR}\" \"${FIXUP_LIBS}\" \"${QT_ROOT_DIR}/lib\")"
)