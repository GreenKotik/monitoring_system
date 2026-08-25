cmake_minimum_required(VERSION 3.16)
project(admin_server)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)

set(SOURCES
    src/main.cpp
    src/admin_mainwindow.cpp
    src/admin_apiclient.cpp
    src/dialogs/objectdialog.cpp
    src/dialogs/sensordialog.cpp
    src/dialogs/userdialog.cpp
)

set(HEADERS
    include/admin_mainwindow.h
    include/admin_apiclient.h
    include/dialogs/objectdialog.h
    include/dialogs/sensordialog.h
    include/dialogs/userdialog.h
)

add_executable(admin_server ${SOURCES} ${HEADERS})

target_include_directories(admin_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dialogs
        ${CMAKE_SOURCE_DIR}/core/include
)

target_link_libraries(admin_server
    core_lib
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
)

install(TARGETS admin_server RUNTIME DESTINATION bin)