cmake_minimum_required(VERSION 3.16)
project(app_server)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Добавьте ВСЕ необходимые модули Qt
find_package(Qt5 REQUIRED COMPONENTS Core Network Sql Svg Widgets)

set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../core)

set(SOURCES
    src/main.cpp
    src/traymanager.cpp
    src/httpserver.cpp
    src/httprouter.cpp
    src/httprequest.cpp
    src/httpresponse.cpp
    src/controllers/objectcontroller.cpp
    src/controllers/sensorcontroller.cpp
    src/controllers/authcontroller.cpp
    src/controllers/mapcontroller.cpp
    src/middleware/authmiddleware.cpp
    src/middleware/corsmiddleware.cpp
    ${CORE_DIR}/src/models/object.cpp
    ${CORE_DIR}/src/models/sensor.cpp
    ${CORE_DIR}/src/models/objecttype.cpp
    ${CORE_DIR}/src/models/sensortype.cpp
    ${CORE_DIR}/src/models/sensorreading.cpp
    ${CORE_DIR}/src/database/dbmanager.cpp
    ${CORE_DIR}/src/database/dbmigrations.cpp
    ${CORE_DIR}/src/utils/config.cpp
    ${CORE_DIR}/src/utils/helpers.cpp
    ${CORE_DIR}/src/utils/logger.cpp          
)

set(HEADERS
    include/traymanager.h 
    include/httpserver.h
    include/httprouter.h
    include/httprequest.h
    include/httpresponse.h
    include/controllers/objectcontroller.h
    include/controllers/sensorcontroller.h
    include/controllers/authcontroller.h
    include/controllers/mapcontroller.h
    include/middleware/authmiddleware.h
    include/middleware/corsmiddleware.h
    ${CORE_DIR}/include/core_global.h
    ${CORE_DIR}/include/models/object.h
    ${CORE_DIR}/include/models/sensor.h
    ${CORE_DIR}/include/models/objecttype.h
    ${CORE_DIR}/include/models/sensortype.h
    ${CORE_DIR}/include/models/sensorreading.h
    ${CORE_DIR}/include/database/dbmanager.h
    ${CORE_DIR}/include/database/dbmigrations.h
    ${CORE_DIR}/include/utils/config.h
    ${CORE_DIR}/include/utils/helpers.h
    ${CORE_DIR}/include/utils/logger.h          
)

add_executable(app_server ${SOURCES} ${HEADERS})

target_include_directories(app_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/controllers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/middleware
        ${CMAKE_SOURCE_DIR}
        ${CORE_DIR}/include
        ${CORE_DIR}/include/models
        ${CORE_DIR}/include/database
        ${CORE_DIR}/include/utils
)

# Подключаем ВСЕ необходимые библиотеки Qt
target_link_libraries(app_server
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    Qt5::Svg
    Qt5::Widgets
)

install(TARGETS app_server RUNTIME DESTINATION bin)