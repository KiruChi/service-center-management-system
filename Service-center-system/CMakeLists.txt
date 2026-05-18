cmake_minimum_required(VERSION 3.16)

project(Coursework VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt automatic tools
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

# MongoDB
find_package(mongocxx CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)

# Sources
set(PROJECT_SOURCES
    main.cpp

    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    loginwindow.cpp
    loginwindow.h
    loginwindow.ui

    orderwindow.cpp
    orderwindow.h
    orderwindow.ui

    statisticwindow.cpp
    statisticwindow.h
    statisticwindow.ui

    informationwindow.cpp
    informationwindow.h
    informationwindow.ui

    database.cpp
    database.h

    Images.qrc
)

# Executable
qt_add_executable(Coursework
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# Libraries
target_link_libraries(Coursework PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    mongo::mongocxx_shared
    mongo::bsoncxx_shared
)

# Windows settings
set_target_properties(Coursework PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# Finalize Qt
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Coursework)
endif()
