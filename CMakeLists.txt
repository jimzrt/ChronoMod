# Generated from ChronoMod.pro.

cmake_minimum_required(VERSION 3.16)
project(ChronoMod VERSION "1.4.0" LANGUAGES CXX)
configure_file(config.h.in config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Global settings to disable tests and examples for all dependencies
set(BUILD_TESTING OFF CACHE INTERNAL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(gtest_force_shared_crt ON CACHE INTERNAL "" FORCE)

set(QUAZIP_QT_MAJOR_VERSION 6 CACHE STRING "Force QuaZip to use Qt6" FORCE)


set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/myappico.rc")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Core5Compat)
find_package(Qt6 COMPONENTS Widgets)
find_package(Qt6 COMPONENTS Network)
find_package(Qt6 COMPONENTS Multimedia)


qt_add_executable(ChronoMod WIN32 MACOSX_BUNDLE
    ChronoCrypto.cpp ChronoCrypto.h
    Patch.h
    WorkerThread.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    resourcebin.cpp resourcebin.h
    resourceentry.h
    resourceentrymodel.cpp resourceentrymodel.h
    resourceentryproxymodel.cpp resourceentryproxymodel.h
    VGMAudioPlayer.cpp VGMAudioPlayer.h
    ${APP_ICON_RESOURCE_WINDOWS}
)

target_compile_definitions(ChronoMod PUBLIC
    QUAZIP_STATIC
)

#add_library(quazip STATIC IMPORTED) # or STATIC instead of SHARED
#set_target_properties(quazip PROPERTIES
#  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/3rdparty/QuaZip/lib/libquazip1-qt6.a"
#  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/3rdparty/QuaZip/include/QuaZip-Qt6-1.2/quazip"
#)

SET(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
SET(QUAZIP_USE_QT_ZLIB OFF CACHE INTERNAL "" FORCE)
#SET(Qt6ZlibPrivate_DIR:PATH F:/Qt/6.2.3/mingw_64/lib/cmake/Qt6ZlibPrivate)
#SET(Qt6Zlib_DIR:PATH F:/Qt/6.2.3/mingw_64/lib/cmake/Qt6ZlibPrivate)



set(ZLIB_COMPAT ON CACHE INTERNAL "" FORCE)
set(ZLIB_ROOT "${CMAKE_SOURCE_DIR}/3rdparty/zlib" CACHE INTERNAL "" FORCE)
set(ZLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/3rdparty/zlib" CACHE INTERNAL "" FORCE)

# Disable zlib tests and examples
set(ZLIB_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(BUILD_TESTING OFF CACHE INTERNAL "" FORCE)

add_subdirectory("3rdparty/zlib")

add_subdirectory("3rdparty/quazip")

# Add vgmstream - disable all unnecessary components
set(BUILD_CLI OFF CACHE INTERNAL "" FORCE)
set(BUILD_AUDACIOUS OFF CACHE INTERNAL "" FORCE)
set(BUILD_V123 OFF CACHE INTERNAL "" FORCE)
set(BUILD_STATIC ON CACHE INTERNAL "" FORCE)
set(USE_MPEG OFF CACHE INTERNAL "" FORCE)
set(USE_VORBIS OFF CACHE INTERNAL "" FORCE)
set(USE_FFMPEG OFF CACHE INTERNAL "" FORCE)
set(USE_G7221 OFF CACHE INTERNAL "" FORCE)
set(USE_G719 OFF CACHE INTERNAL "" FORCE)
set(USE_ATRAC9 OFF CACHE INTERNAL "" FORCE)
set(USE_CELT OFF CACHE INTERNAL "" FORCE)
set(USE_SPEEX OFF CACHE INTERNAL "" FORCE)
set(BUILD_TESTING OFF CACHE INTERNAL "" FORCE)
add_subdirectory("3rdparty/vgmstream")


target_include_directories(ChronoMod PRIVATE
    3rdparty/vgmstream/src
    3rdparty/vgmstream/ext_includes
)

target_link_libraries(ChronoMod PUBLIC
    Qt::Core
    Qt::Core5Compat
    Qt::Gui
    Qt::Widgets
    Qt::Network
    Qt::Multimedia
    QuaZip::QuaZip
    zlib
    libvgmstream
)


# Resources:
set(style_resource_files
    "qdarkstyle/dark/rc/.keep"
    "qdarkstyle/dark/rc/arrow_down.png"
    "qdarkstyle/dark/rc/arrow_down@2x.png"
    "qdarkstyle/dark/rc/arrow_down_disabled.png"
    "qdarkstyle/dark/rc/arrow_down_disabled@2x.png"
    "qdarkstyle/dark/rc/arrow_down_focus.png"
    "qdarkstyle/dark/rc/arrow_down_focus@2x.png"
    "qdarkstyle/dark/rc/arrow_down_pressed.png"
    "qdarkstyle/dark/rc/arrow_down_pressed@2x.png"
    "qdarkstyle/dark/rc/arrow_left.png"
    "qdarkstyle/dark/rc/arrow_left@2x.png"
    "qdarkstyle/dark/rc/arrow_left_disabled.png"
    "qdarkstyle/dark/rc/arrow_left_disabled@2x.png"
    "qdarkstyle/dark/rc/arrow_left_focus.png"
    "qdarkstyle/dark/rc/arrow_left_focus@2x.png"
    "qdarkstyle/dark/rc/arrow_left_pressed.png"
    "qdarkstyle/dark/rc/arrow_left_pressed@2x.png"
    "qdarkstyle/dark/rc/arrow_right.png"
    "qdarkstyle/dark/rc/arrow_right@2x.png"
    "qdarkstyle/dark/rc/arrow_right_disabled.png"
    "qdarkstyle/dark/rc/arrow_right_disabled@2x.png"
    "qdarkstyle/dark/rc/arrow_right_focus.png"
    "qdarkstyle/dark/rc/arrow_right_focus@2x.png"
    "qdarkstyle/dark/rc/arrow_right_pressed.png"
    "qdarkstyle/dark/rc/arrow_right_pressed@2x.png"
    "qdarkstyle/dark/rc/arrow_up.png"
    "qdarkstyle/dark/rc/arrow_up@2x.png"
    "qdarkstyle/dark/rc/arrow_up_disabled.png"
    "qdarkstyle/dark/rc/arrow_up_disabled@2x.png"
    "qdarkstyle/dark/rc/arrow_up_focus.png"
    "qdarkstyle/dark/rc/arrow_up_focus@2x.png"
    "qdarkstyle/dark/rc/arrow_up_pressed.png"
    "qdarkstyle/dark/rc/arrow_up_pressed@2x.png"
    "qdarkstyle/dark/rc/base_icon.png"
    "qdarkstyle/dark/rc/base_icon@2x.png"
    "qdarkstyle/dark/rc/base_icon_disabled.png"
    "qdarkstyle/dark/rc/base_icon_disabled@2x.png"
    "qdarkstyle/dark/rc/base_icon_focus.png"
    "qdarkstyle/dark/rc/base_icon_focus@2x.png"
    "qdarkstyle/dark/rc/base_icon_pressed.png"
    "qdarkstyle/dark/rc/base_icon_pressed@2x.png"
    "qdarkstyle/dark/rc/branch_closed.png"
    "qdarkstyle/dark/rc/branch_closed@2x.png"
    "qdarkstyle/dark/rc/branch_closed_disabled.png"
    "qdarkstyle/dark/rc/branch_closed_disabled@2x.png"
    "qdarkstyle/dark/rc/branch_closed_focus.png"
    "qdarkstyle/dark/rc/branch_closed_focus@2x.png"
    "qdarkstyle/dark/rc/branch_closed_pressed.png"
    "qdarkstyle/dark/rc/branch_closed_pressed@2x.png"
    "qdarkstyle/dark/rc/branch_end.png"
    "qdarkstyle/dark/rc/branch_end@2x.png"
    "qdarkstyle/dark/rc/branch_end_disabled.png"
    "qdarkstyle/dark/rc/branch_end_disabled@2x.png"
    "qdarkstyle/dark/rc/branch_end_focus.png"
    "qdarkstyle/dark/rc/branch_end_focus@2x.png"
    "qdarkstyle/dark/rc/branch_end_pressed.png"
    "qdarkstyle/dark/rc/branch_end_pressed@2x.png"
    "qdarkstyle/dark/rc/branch_line.png"
    "qdarkstyle/dark/rc/branch_line@2x.png"
    "qdarkstyle/dark/rc/branch_line_disabled.png"
    "qdarkstyle/dark/rc/branch_line_disabled@2x.png"
    "qdarkstyle/dark/rc/branch_line_focus.png"
    "qdarkstyle/dark/rc/branch_line_focus@2x.png"
    "qdarkstyle/dark/rc/branch_line_pressed.png"
    "qdarkstyle/dark/rc/branch_line_pressed@2x.png"
    "qdarkstyle/dark/rc/branch_more.png"
    "qdarkstyle/dark/rc/branch_more@2x.png"
    "qdarkstyle/dark/rc/branch_more_disabled.png"
    "qdarkstyle/dark/rc/branch_more_disabled@2x.png"
    "qdarkstyle/dark/rc/branch_more_focus.png"
    "qdarkstyle/dark/rc/branch_more_focus@2x.png"
    "qdarkstyle/dark/rc/branch_more_pressed.png"
    "qdarkstyle/dark/rc/branch_more_pressed@2x.png"
    "qdarkstyle/dark/rc/branch_open.png"
    "qdarkstyle/dark/rc/branch_open@2x.png"
    "qdarkstyle/dark/rc/branch_open_disabled.png"
    "qdarkstyle/dark/rc/branch_open_disabled@2x.png"
    "qdarkstyle/dark/rc/branch_open_focus.png"
    "qdarkstyle/dark/rc/branch_open_focus@2x.png"
    "qdarkstyle/dark/rc/branch_open_pressed.png"
    "qdarkstyle/dark/rc/branch_open_pressed@2x.png"
    "qdarkstyle/dark/rc/checkbox_checked.png"
    "qdarkstyle/dark/rc/checkbox_checked@2x.png"
    "qdarkstyle/dark/rc/checkbox_checked_disabled.png"
    "qdarkstyle/dark/rc/checkbox_checked_disabled@2x.png"
    "qdarkstyle/dark/rc/checkbox_checked_focus.png"
    "qdarkstyle/dark/rc/checkbox_checked_focus@2x.png"
    "qdarkstyle/dark/rc/checkbox_checked_pressed.png"
    "qdarkstyle/dark/rc/checkbox_checked_pressed@2x.png"
    "qdarkstyle/dark/rc/checkbox_indeterminate.png"
    "qdarkstyle/dark/rc/checkbox_indeterminate@2x.png"
    "qdarkstyle/dark/rc/checkbox_indeterminate_disabled.png"
    "qdarkstyle/dark/rc/checkbox_indeterminate_disabled@2x.png"
    "qdarkstyle/dark/rc/checkbox_indeterminate_focus.png"
    "qdarkstyle/dark/rc/checkbox_indeterminate_focus@2x.png"
    "qdarkstyle/dark/rc/checkbox_indeterminate_pressed.png"
    "qdarkstyle/dark/rc/checkbox_indeterminate_pressed@2x.png"
    "qdarkstyle/dark/rc/checkbox_unchecked.png"
    "qdarkstyle/dark/rc/checkbox_unchecked@2x.png"
    "qdarkstyle/dark/rc/checkbox_unchecked_disabled.png"
    "qdarkstyle/dark/rc/checkbox_unchecked_disabled@2x.png"
    "qdarkstyle/dark/rc/checkbox_unchecked_focus.png"
    "qdarkstyle/dark/rc/checkbox_unchecked_focus@2x.png"
    "qdarkstyle/dark/rc/checkbox_unchecked_pressed.png"
    "qdarkstyle/dark/rc/checkbox_unchecked_pressed@2x.png"
    "qdarkstyle/dark/rc/line_horizontal.png"
    "qdarkstyle/dark/rc/line_horizontal@2x.png"
    "qdarkstyle/dark/rc/line_horizontal_disabled.png"
    "qdarkstyle/dark/rc/line_horizontal_disabled@2x.png"
    "qdarkstyle/dark/rc/line_horizontal_focus.png"
    "qdarkstyle/dark/rc/line_horizontal_focus@2x.png"
    "qdarkstyle/dark/rc/line_horizontal_pressed.png"
    "qdarkstyle/dark/rc/line_horizontal_pressed@2x.png"
    "qdarkstyle/dark/rc/line_vertical.png"
    "qdarkstyle/dark/rc/line_vertical@2x.png"
    "qdarkstyle/dark/rc/line_vertical_disabled.png"
    "qdarkstyle/dark/rc/line_vertical_disabled@2x.png"
    "qdarkstyle/dark/rc/line_vertical_focus.png"
    "qdarkstyle/dark/rc/line_vertical_focus@2x.png"
    "qdarkstyle/dark/rc/line_vertical_pressed.png"
    "qdarkstyle/dark/rc/line_vertical_pressed@2x.png"
    "qdarkstyle/dark/rc/radio_checked.png"
    "qdarkstyle/dark/rc/radio_checked@2x.png"
    "qdarkstyle/dark/rc/radio_checked_disabled.png"
    "qdarkstyle/dark/rc/radio_checked_disabled@2x.png"
    "qdarkstyle/dark/rc/radio_checked_focus.png"
    "qdarkstyle/dark/rc/radio_checked_focus@2x.png"
    "qdarkstyle/dark/rc/radio_checked_pressed.png"
    "qdarkstyle/dark/rc/radio_checked_pressed@2x.png"
    "qdarkstyle/dark/rc/radio_unchecked.png"
    "qdarkstyle/dark/rc/radio_unchecked@2x.png"
    "qdarkstyle/dark/rc/radio_unchecked_disabled.png"
    "qdarkstyle/dark/rc/radio_unchecked_disabled@2x.png"
    "qdarkstyle/dark/rc/radio_unchecked_focus.png"
    "qdarkstyle/dark/rc/radio_unchecked_focus@2x.png"
    "qdarkstyle/dark/rc/radio_unchecked_pressed.png"
    "qdarkstyle/dark/rc/radio_unchecked_pressed@2x.png"
    "qdarkstyle/dark/rc/toolbar_move_horizontal.png"
    "qdarkstyle/dark/rc/toolbar_move_horizontal@2x.png"
    "qdarkstyle/dark/rc/toolbar_move_horizontal_disabled.png"
    "qdarkstyle/dark/rc/toolbar_move_horizontal_disabled@2x.png"
    "qdarkstyle/dark/rc/toolbar_move_horizontal_focus.png"
    "qdarkstyle/dark/rc/toolbar_move_horizontal_focus@2x.png"
    "qdarkstyle/dark/rc/toolbar_move_horizontal_pressed.png"
    "qdarkstyle/dark/rc/toolbar_move_horizontal_pressed@2x.png"
    "qdarkstyle/dark/rc/toolbar_move_vertical.png"
    "qdarkstyle/dark/rc/toolbar_move_vertical@2x.png"
    "qdarkstyle/dark/rc/toolbar_move_vertical_disabled.png"
    "qdarkstyle/dark/rc/toolbar_move_vertical_disabled@2x.png"
    "qdarkstyle/dark/rc/toolbar_move_vertical_focus.png"
    "qdarkstyle/dark/rc/toolbar_move_vertical_focus@2x.png"
    "qdarkstyle/dark/rc/toolbar_move_vertical_pressed.png"
    "qdarkstyle/dark/rc/toolbar_move_vertical_pressed@2x.png"
    "qdarkstyle/dark/rc/toolbar_separator_horizontal.png"
    "qdarkstyle/dark/rc/toolbar_separator_horizontal@2x.png"
    "qdarkstyle/dark/rc/toolbar_separator_horizontal_disabled.png"
    "qdarkstyle/dark/rc/toolbar_separator_horizontal_disabled@2x.png"
    "qdarkstyle/dark/rc/toolbar_separator_horizontal_focus.png"
    "qdarkstyle/dark/rc/toolbar_separator_horizontal_focus@2x.png"
    "qdarkstyle/dark/rc/toolbar_separator_horizontal_pressed.png"
    "qdarkstyle/dark/rc/toolbar_separator_horizontal_pressed@2x.png"
    "qdarkstyle/dark/rc/toolbar_separator_vertical.png"
    "qdarkstyle/dark/rc/toolbar_separator_vertical@2x.png"
    "qdarkstyle/dark/rc/toolbar_separator_vertical_disabled.png"
    "qdarkstyle/dark/rc/toolbar_separator_vertical_disabled@2x.png"
    "qdarkstyle/dark/rc/toolbar_separator_vertical_focus.png"
    "qdarkstyle/dark/rc/toolbar_separator_vertical_focus@2x.png"
    "qdarkstyle/dark/rc/toolbar_separator_vertical_pressed.png"
    "qdarkstyle/dark/rc/toolbar_separator_vertical_pressed@2x.png"
    "qdarkstyle/dark/rc/transparent.png"
    "qdarkstyle/dark/rc/transparent@2x.png"
    "qdarkstyle/dark/rc/transparent_disabled.png"
    "qdarkstyle/dark/rc/transparent_disabled@2x.png"
    "qdarkstyle/dark/rc/transparent_focus.png"
    "qdarkstyle/dark/rc/transparent_focus@2x.png"
    "qdarkstyle/dark/rc/transparent_pressed.png"
    "qdarkstyle/dark/rc/transparent_pressed@2x.png"
    "qdarkstyle/dark/rc/window_close.png"
    "qdarkstyle/dark/rc/window_close@2x.png"
    "qdarkstyle/dark/rc/window_close_disabled.png"
    "qdarkstyle/dark/rc/window_close_disabled@2x.png"
    "qdarkstyle/dark/rc/window_close_focus.png"
    "qdarkstyle/dark/rc/window_close_focus@2x.png"
    "qdarkstyle/dark/rc/window_close_pressed.png"
    "qdarkstyle/dark/rc/window_close_pressed@2x.png"
    "qdarkstyle/dark/rc/window_grip.png"
    "qdarkstyle/dark/rc/window_grip@2x.png"
    "qdarkstyle/dark/rc/window_grip_disabled.png"
    "qdarkstyle/dark/rc/window_grip_disabled@2x.png"
    "qdarkstyle/dark/rc/window_grip_focus.png"
    "qdarkstyle/dark/rc/window_grip_focus@2x.png"
    "qdarkstyle/dark/rc/window_grip_pressed.png"
    "qdarkstyle/dark/rc/window_grip_pressed@2x.png"
    "qdarkstyle/dark/rc/window_minimize.png"
    "qdarkstyle/dark/rc/window_minimize@2x.png"
    "qdarkstyle/dark/rc/window_minimize_disabled.png"
    "qdarkstyle/dark/rc/window_minimize_disabled@2x.png"
    "qdarkstyle/dark/rc/window_minimize_focus.png"
    "qdarkstyle/dark/rc/window_minimize_focus@2x.png"
    "qdarkstyle/dark/rc/window_minimize_pressed.png"
    "qdarkstyle/dark/rc/window_minimize_pressed@2x.png"
    "qdarkstyle/dark/rc/window_undock.png"
    "qdarkstyle/dark/rc/window_undock@2x.png"
    "qdarkstyle/dark/rc/window_undock_disabled.png"
    "qdarkstyle/dark/rc/window_undock_disabled@2x.png"
    "qdarkstyle/dark/rc/window_undock_focus.png"
    "qdarkstyle/dark/rc/window_undock_focus@2x.png"
    "qdarkstyle/dark/rc/window_undock_pressed.png"
    "qdarkstyle/dark/rc/window_undock_pressed@2x.png"
)

qt6_add_resources(ChronoMod "style"
    PREFIX
        "/qss_icons/dark"
    BASE
        "qdarkstyle/dark"
    FILES
        ${style_resource_files}
)
set(style1_resource_files
    "qdarkstyle/dark/style.qss"
)

qt6_add_resources(ChronoMod "style1"
    PREFIX
        "/qdarkstyle/dark"
    BASE
        "qdarkstyle/dark"
    FILES
        ${style1_resource_files}
)



install(TARGETS ChronoMod
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
