cmake_minimum_required(VERSION 3.16)

project(Paperbase VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/app_icon.rc")
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES

        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

    qt_add_executable(Paperbase
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        core/paper.cpp
        core/paper.h
        core/paper_manager.cpp
        core/paper_manager.h

        storage/json_utils.cpp
        storage/json_utils.h

        external/nlohmann/json.hpp

        filesystem/pdf_utils.h
        filesystem/pdf_utils.cpp

        bibliography/bibliography.cpp
        bibliography/bibliography.h

        resources/resources.qrc

        ${APP_ICON_RESOURCE_WINDOWS}

    )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Paperbase APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Paperbase SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Paperbase
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Paperbase PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Paperbase)
endif()
set_target_properties(Paperbase PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Paperbase
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_include_directories(Paperbase PRIVATE
    ${CMAKE_SOURCE_DIR}/external
)


