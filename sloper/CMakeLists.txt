cmake_minimum_required(VERSION 3.16)

project(sloper VERSION 0.1 LANGUAGES CXX)

set(QT_DEFAULT_MAJOR_VERSION 5)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for cross-compilation, directly include Qt5Config.cmake
# this bypasses find_package which seems to have issues with cross-compilation
set(QT5_CONFIG_PATH "")
set(QT5_DIR_PATH "")
if(DEFINED CMAKE_SYSROOT)
    set(QT5_CONFIG_PATH "${CMAKE_SYSROOT}/usr/lib/cmake/Qt5/Qt5Config.cmake")
    set(QT5_DIR_PATH "${CMAKE_SYSROOT}/usr/lib/cmake/Qt5")
else()
    # try to find it using the known absolute path as a fallback
    set(QT5_CONFIG_PATH "/ad/eng/courses/ec/ec535/bbb/buildroot-2021.02.1/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/lib/cmake/Qt5/Qt5Config.cmake")
    set(QT5_DIR_PATH "/ad/eng/courses/ec/ec535/bbb/buildroot-2021.02.1/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/lib/cmake/Qt5")
endif()

if(EXISTS "${QT5_CONFIG_PATH}")
    message(STATUS "Found Qt5Config.cmake at: ${QT5_CONFIG_PATH}")
    set(Qt5_DIR "${QT5_DIR_PATH}" CACHE PATH "Qt5 CMake config directory" FORCE)
    # drectly include the config file
    include("${QT5_CONFIG_PATH}")
    # now find the specific components we need
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)
    set(QT_VERSION_MAJOR 5)
else()
    # if direct include doesn't work fallback to standard find_package
    message(WARNING "Qt5Config.cmake not found at ${QT5_CONFIG_PATH}, trying find_package...")
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)
    set(QT_VERSION_MAJOR 5)
endif()

set(PROJECT_SOURCES
        src/main.cpp



)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(sloper
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET sloper APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(sloper SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(sloper
            ${PROJECT_SOURCES}
            src/animatableimage.h src/animatableimage.cpp
            src/gamestate.h src/gamestate.cpp
            src/keyboardcontrolledview.h src/keyboardcontrolledview.cpp
            src/helpers.h src/helpers.cpp
            src/sonarcontrolledview.cpp src/sonarcontrolledview.h
        )
    endif()
endif()

target_link_libraries(sloper PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt5::Gui)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
# Check Qt5 version - Qt5_VERSION should be set by Qt5Config.cmake
if(DEFINED Qt5_VERSION AND Qt5_VERSION VERSION_LESS 5.15.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.sloper)
endif()
set_target_properties(sloper PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS sloper
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(sloper)
endif()
