cmake_minimum_required(VERSION 3.16)
project(core_lib)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Sql)

set(SOURCES
    src/models/object.cpp
    src/models/sensor.cpp
    src/models/objecttype.cpp
    src/models/sensortype.cpp
    src/models/sensorreading.cpp
    src/database/dbmanager.cpp
    src/database/dbmigrations.cpp
    src/utils/logger.cpp
    src/utils/config.cpp
    src/utils/helpers.cpp
)

set(HEADERS
    include/core_global.h
    include/models/object.h
    include/models/sensor.h
    include/models/objecttype.h
    include/models/sensortype.h
    include/models/sensorreading.h
    include/database/dbmanager.h
    include/database/dbmigrations.h
    include/utils/logger.h
    include/utils/config.h
    include/utils/helpers.h
)

add_library(core_lib SHARED ${SOURCES} ${HEADERS})

target_include_directories(core_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(core_lib
    Qt5::Core
    Qt5::Sql
)

set_target_properties(core_lib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

install(TARGETS core_lib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)

install(DIRECTORY include/ DESTINATION include/core_lib)