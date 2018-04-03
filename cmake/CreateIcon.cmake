if(WIN32 OR UNIX AND NOT APPLE)
    find_package(ImageMagick COMPONENTS convert)
endif()

function(createIconFromSource SOURCE)
    if(NOT EXISTS ${SOURCE})
        message(FATAL_ERROR "Source file for icon not found ${SOURCE}")
    endif()

    if(APPLE)
        find_program(SIPS_EXECUTABLE NAMES sips)
        find_program(ICONUTIL_EXECUTABLE NAMES iconutil)
        if(SIPS_EXECUTABLE AND ICONUTIL_EXECUTABLE)
            set(ICON_DIR "${CMAKE_CURRENT_BINARY_DIR}/icon.iconset")
            file(MAKE_DIRECTORY ${ICON_DIR})
            set(ICON_SIZES 512 256 128 64 32 16)
            set(ICON_SIZE_MULTIPLIERS 1 2)
            foreach(SIZE ${ICON_SIZES})
                foreach(MULTIPLIER ${ICON_SIZE_MULTIPLIERS})
                    if(${MULTIPLIER} EQUAL 2)
                        set(ICON_FILE "${ICON_DIR}/icon_${SIZE}x${SIZE}@2x.png")
                    else()
                        set(ICON_FILE "${ICON_DIR}/icon_${SIZE}x${SIZE}.png")
                    endif()
                    if(NOT EXISTS ${ICON_FILE})
                        if(${MULTIPLIER} EQUAL 2)
                            message(STATUS "Creating icon ${SIZE}x${SIZE}@2x")
                        else()
                            message(STATUS "Creating icon ${SIZE}x${SIZE}")
                        endif()
                        math(EXPR ACTUL_SIZE "${SIZE} * ${MULTIPLIER}")
                        execute_process(
                            COMMAND ${SIPS_EXECUTABLE}
                                -z ${ACTUL_SIZE} ${ACTUL_SIZE} ${SOURCE} --out ${ICON_FILE}
                            OUTPUT_QUIET ERROR_QUIET
                        )
                    endif()
                endforeach()
            endforeach()
            set(ICON_FILE "${CMAKE_CURRENT_BINARY_DIR}/icon.icns")
            if(NOT EXISTS ${ICON_FILE})
                message(STATUS "Creating result icon")
                execute_process(
                    COMMAND ${ICONUTIL_EXECUTABLE}
                        --convert icns --output ${ICON_FILE} ${ICON_DIR}
                    OUTPUT_QUIET ERROR_QUIET
                )
            endif()
        else()
            message(WARNING "Sips or iconutil not found. Icon won't be created")
        endif()
    elseif(WIN32)
        if(IMAGEMAGICK_CONVERT_EXECUTABLE)
            set(ICON_FILE "${CMAKE_CURRENT_BINARY_DIR}/2048.ico")
            if(NOT EXISTS ${ICON_FILE})
                message(STATUS "Creating icon")
                execute_process(
                    COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} ${SOURCE}
                        -define icon:auto-resize=256,128,96,64,48,40,32,24,16 ${ICON_FILE}
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
                set(ICON_FILE "${CMAKE_CURRENT_BINARY_DIR}/icon-${SIZE}.png")
                if(NOT EXISTS ${ICON_FILE})
                    message(STATUS "Creating icon ${SIZE}x${SIZE}")
                    execute_process(
                        COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE}
                            ${SOURCE} -resize ${SIZE}x${SIZE} ${ICON_FILE}
                        OUTPUT_QUIET ERROR_QUIET
                    )
                endif()
                install(
                    FILES ${ICON_FILE}
                    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${SIZE}x${SIZE}/apps" RENAME 2048.png
                )
            endforeach()
        else()
            message(WARNING "Imagemagick convert not found. Icon won't be created")
        endif()
    endif()
endfunction()
