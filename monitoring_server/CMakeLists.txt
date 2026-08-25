cmake_minimum_required(VERSION 3.16)
project(monitoring_server)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Network SerialPort)

set(SOURCES
    src/main.cpp
    src/collectors/basecollector.cpp
    src/collectors/snmpcollector.cpp
    src/collectors/modbuscollector.cpp
    src/collectors/mqttcollector.cpp
)

set(HEADERS
    include/collectors/basecollector.h
    include/collectors/snmpcollector.h
    include/collectors/modbuscollector.h
    include/collectors/mqttcollector.h
)

add_executable(monitoring_server ${SOURCES} ${HEADERS})

target_include_directories(monitoring_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/core/include
)

target_link_libraries(monitoring_server
    core_lib
    Qt5::Core
    Qt5::Network
    Qt5::SerialPort
)

# Подключение Net-SNMP на Linux
if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SNMP REQUIRED netsnmp)
    target_link_libraries(monitoring_server ${SNMP_LIBRARIES})
    target_include_directories(monitoring_server PRIVATE ${SNMP_INCLUDE_DIRS})
endif()

install(TARGETS monitoring_server RUNTIME DESTINATION bin)