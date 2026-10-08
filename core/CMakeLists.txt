cmake_minimum_required(VERSION 3.16)
project(core)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ПРАВИЛЬНО: добавляем SQL компонент в find_package
find_package(Qt5 REQUIRED COMPONENTS Core Sql)

set(CORE_SOURCES
    src/models/object.cpp
    src/models/sensor.cpp
    src/models/sensorreading.cpp
    src/models/objecttype.cpp
    src/models/sensortype.cpp
    src/models/user.cpp
    src/database/dbmanager.cpp
    src/database/dbmigrations.cpp
    src/utils/config.cpp
    src/utils/helpers.cpp
    src/utils/logger.cpp          
)

set(CORE_HEADERS
    include/core_global.h
    include/models/object.h
    include/models/sensor.h
    include/models/sensorreading.h
    include/models/objecttype.h
    include/models/sensortype.h
    include/models/user.h
    include/database/dbmanager.h
    include/database/dbmigrations.h
    include/utils/config.h
    include/utils/helpers.h
    include/utils/logger.h       
)

add_library(core ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ПРАВИЛЬНО: добавляем Qt5::Sql в target_link_libraries
target_link_libraries(core
    Qt5::Core
    Qt5::Sql
)

install(TARGETS core LIBRARY DESTINATION lib)