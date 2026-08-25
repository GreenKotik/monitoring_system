cmake_minimum_required(VERSION 3.16)
project(gui_client)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Svg)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/apiclient.cpp
    src/appstate.cpp
    src/widgets/mapwidget.cpp
    src/widgets/toolbarwidget.cpp
    src/widgets/sensordetailwidget.cpp
    src/widgets/breadcrumbwidget.cpp
)

set(HEADERS
    include/mainwindow.h
    include/apiclient.h
    include/appstate.h
    include/widgets/mapwidget.h
    include/widgets/toolbarwidget.h
    include/widgets/sensordetailwidget.h
    include/widgets/breadcrumbwidget.h
)

add_executable(gui_client ${SOURCES} ${HEADERS})

target_include_directories(gui_client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/widgets
        ${CMAKE_SOURCE_DIR}/core/include
)

target_link_libraries(gui_client
    core_lib
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    Qt5::Svg
)

# Ресурсы
qt5_add_resources(GUI_RESOURCES resources.qrc)
target_sources(gui_client PRIVATE ${GUI_RESOURCES})

install(TARGETS gui_client RUNTIME DESTINATION bin)