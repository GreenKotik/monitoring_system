cmake_minimum_required(VERSION 3.16)
project(admin_server)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ДОБАВИТЬ Sql в список компонентов
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Sql)

set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../core)

message(STATUS "CORE_DIR: ${CORE_DIR}")

set(SOURCES
    src/main.cpp
    src/admin_mainwindow.cpp
    src/admin_apiclient.cpp
    src/dialogs/objectdialog.cpp
    src/dialogs/sensordialog.cpp
    src/dialogs/userdialog.cpp
    # Файлы core
    ${CORE_DIR}/src/models/object.cpp
    ${CORE_DIR}/src/models/sensor.cpp
    ${CORE_DIR}/src/models/objecttype.cpp
    ${CORE_DIR}/src/models/sensortype.cpp
    ${CORE_DIR}/src/models/sensorreading.cpp
    ${CORE_DIR}/src/models/user.cpp
    ${CORE_DIR}/src/database/dbmanager.cpp
    ${CORE_DIR}/src/database/dbmigrations.cpp
    ${CORE_DIR}/src/utils/config.cpp
    ${CORE_DIR}/src/utils/helpers.cpp
)

set(HEADERS
    include/admin_mainwindow.h
    include/admin_apiclient.h
    include/dialogs/objectdialog.h
    include/dialogs/sensordialog.h
    include/dialogs/userdialog.h
    # Заголовки core
    ${CORE_DIR}/include/core_global.h
    ${CORE_DIR}/include/models/object.h
    ${CORE_DIR}/include/models/sensor.h
    ${CORE_DIR}/include/models/objecttype.h
    ${CORE_DIR}/include/models/sensortype.h
    ${CORE_DIR}/include/models/sensorreading.h
    ${CORE_DIR}/include/models/user.h
    ${CORE_DIR}/include/database/dbmanager.h
    ${CORE_DIR}/include/database/dbmigrations.h
    ${CORE_DIR}/include/utils/config.h
    ${CORE_DIR}/include/utils/helpers.h
)

add_executable(admin_server ${SOURCES} ${HEADERS})

target_include_directories(admin_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dialogs
        ${CORE_DIR}/include
        ${CORE_DIR}/include/models
        ${CORE_DIR}/include/database
        ${CORE_DIR}/include/utils
)

# ДОБАВИТЬ Qt5::Sql в link libraries
target_link_libraries(admin_server
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    Qt5::Sql
)

install(TARGETS admin_server RUNTIME DESTINATION bin)