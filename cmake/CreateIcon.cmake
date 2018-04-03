if(WIN32 OR UNIX AND NOT APPLE)
    find_package(ImageMagick COMPONENTS convert)
endif()

function(createIconFromSource SOURCE)
    if(NOT EXISTS ${SOURCE})
        message(FATAL_ERROR "Source file for icon not found ${SOURCE}")
    endif()

    if(APPLE)
        # TODO: Have to implement creating icon for MAC OS
    elseif(WIN32)
        if(IMAGEMAGICK_CONVERT_EXECUTABLE)
            set(ICON_NAME "${CMAKE_CURRENT_BINARY_DIR}/2048.ico")
            if(NOT EXISTS ${ICON_NAME})
                message(STATUS "Creating icon")
                execute_process(
                    COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} ${SOURCE}
                        -define icon:auto-resize=256,128,96,64,48,40,32,24,16 ${ICON_NAME}
                    OUTPUT_QUIET ERROR_QUIET
                )
            endif()
        else()
            message(WARNING "Imagemagick convert not found. Icon won't be created")
        endif()
    else()
        if(IMAGEMAGICK_CONVERT_EXECUTABLE)
            set(ICON_SIZES 1024 512 256 128 64 48 32 24 22 16)
            foreach(SIZE ${ICON_SIZES})
                set(ICON_NAME "${CMAKE_CURRENT_BINARY_DIR}/icon-${SIZE}.png")
                if(NOT EXISTS ${ICON_NAME})
                    message(STATUS "Creating icon ${SIZE}x${SIZE}")
                    execute_process(
                        COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE}
                            ${SOURCE} -resize ${SIZE}x${SIZE} ${ICON_NAME}
                        OUTPUT_QUIET ERROR_QUIET
                    )
                endif()
                install(
                    FILES ${ICON_NAME}
                    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${SIZE}x${SIZE}/apps" RENAME 2048.png
                )
            endforeach()
        else()
            message(WARNING "Imagemagick convert not found. Icon won't be created")
        endif()
    endif()
endfunction()
