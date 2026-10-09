cmake_minimum_required(VERSION 3.16)
project(gui_client)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Svg Sql)

set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../core)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/apiclient.cpp
    src/appstate.cpp
    src/widgets/mapwidget.cpp
    src/widgets/toolbarwidget.cpp
    src/widgets/breadcrumbwidget.cpp
    src/widgets/objectdetailwidget.cpp
    src/widgets/sensordetailwidget.cpp
    src/widgets/sensorchartwidget.cpp
    ${CORE_DIR}/src/models/object.cpp
    ${CORE_DIR}/src/models/sensor.cpp
    ${CORE_DIR}/src/models/sensorreading.cpp
    ${CORE_DIR}/src/models/objecttype.cpp
    ${CORE_DIR}/src/models/sensortype.cpp
    ${CORE_DIR}/src/models/user.cpp
    ${CORE_DIR}/src/database/dbmanager.cpp
    ${CORE_DIR}/src/database/dbmigrations.cpp
    ${CORE_DIR}/src/utils/config.cpp
    ${CORE_DIR}/src/utils/helpers.cpp
    ${CORE_DIR}/src/utils/logger.cpp
)

set(HEADERS
    include/mainwindow.h
    include/apiclient.h
    include/appstate.h
    include/widgets/mapwidget.h
    include/widgets/toolbarwidget.h
    include/widgets/breadcrumbwidget.h
    include/widgets/objectdetailwidget.h
    include/widgets/sensordetailwidget.h
    include/widgets/sensorchartwidget.h
    ${CORE_DIR}/include/core_global.h
    ${CORE_DIR}/include/models/object.h
    ${CORE_DIR}/include/models/sensor.h
    ${CORE_DIR}/include/models/sensorreading.h
    ${CORE_DIR}/include/models/objecttype.h
    ${CORE_DIR}/include/models/sensortype.h
    ${CORE_DIR}/include/models/user.h
    ${CORE_DIR}/include/database/dbmanager.h
    ${CORE_DIR}/include/database/dbmigrations.h
    ${CORE_DIR}/include/utils/config.h
    ${CORE_DIR}/include/utils/helpers.h
    ${CORE_DIR}/include/utils/logger.h
)

add_executable(gui_client ${SOURCES} ${HEADERS})

target_include_directories(gui_client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/widgets
        ${CORE_DIR}/include
)

target_link_libraries(gui_client
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    Qt5::Svg
    Qt5::Sql
)

# ИСПРАВЛЕНО: правильный путь к файлу ресурсов
qt5_add_resources(GUI_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../resources/resources.qrc)
target_sources(gui_client PRIVATE ${GUI_RESOURCES})

install(TARGETS gui_client RUNTIME DESTINATION bin)