cmake_minimum_required(VERSION 3.16)
project(monitoring_server)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Network SerialPort Sql Widgets)

# Путь к папке core
set(CORE_BASE ${CMAKE_CURRENT_SOURCE_DIR}/../core)

# Исходники самого сервера
set(SOURCES
    src/main.cpp
    src/traymanager.cpp
    src/monitoring_service.cpp
    src/collectors/basecollector.cpp
    src/collectors/snmpcollector.cpp
    src/collectors/snmpclient.cpp
    src/collectors/modbuscollector.cpp
    src/collectors/tcpcollector.cpp
    src/collectors/tcpasciiclient.cpp
    src/collectors/tcpasciicollector.cpp
)

set(HEADERS
    include/traymanager.h 
    include/monitoring_service.h
    include/collectors/basecollector.h
    include/collectors/snmpcollector.h
    include/collectors/snmpclient.h
    include/collectors/modbuscollector.h
    include/collectors/tcpcollector.h
    include/collectors/tcpasciiclient.h
    include/collectors/tcpasciicollector.h
)

# Исходники core
set(CORE_SOURCES
    ${CORE_BASE}/src/database/dbmanager.cpp
    ${CORE_BASE}/src/database/dbmigrations.cpp
    ${CORE_BASE}/src/models/object.cpp
    ${CORE_BASE}/src/models/sensor.cpp
    ${CORE_BASE}/src/models/objecttype.cpp
    ${CORE_BASE}/src/models/sensortype.cpp
    ${CORE_BASE}/src/models/sensorreading.cpp
    ${CORE_BASE}/src/models/user.cpp
    ${CORE_BASE}/src/utils/config.cpp
    ${CORE_BASE}/src/utils/helpers.cpp
    ${CORE_BASE}/src/utils/logger.cpp          # <-- ДОБАВЛЕНО
)

set(CORE_HEADERS
    ${CORE_BASE}/include/core_global.h
    ${CORE_BASE}/include/database/dbmanager.h
    ${CORE_BASE}/include/database/dbmigrations.h
    ${CORE_BASE}/include/models/object.h
    ${CORE_BASE}/include/models/sensor.h
    ${CORE_BASE}/include/models/objecttype.h
    ${CORE_BASE}/include/models/sensortype.h
    ${CORE_BASE}/include/models/sensorreading.h
    ${CORE_BASE}/include/models/user.h
    ${CORE_BASE}/include/utils/config.h
    ${CORE_BASE}/include/utils/helpers.h
    ${CORE_BASE}/include/utils/logger.h          # <-- ДОБАВЛЕНО
)

add_executable(monitoring_server ${SOURCES} ${HEADERS} ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(monitoring_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/collectors
        ${CORE_BASE}/include
        ${CORE_BASE}/include/database
        ${CORE_BASE}/include/models
        ${CORE_BASE}/include/utils
)

target_link_libraries(monitoring_server
    Qt5::Core
    Qt5::Network
    Qt5::SerialPort
    Qt5::Sql
    Qt5::Widgets
)

install(TARGETS monitoring_server RUNTIME DESTINATION bin)

message(STATUS "CORE_BASE: ${CORE_BASE}")
