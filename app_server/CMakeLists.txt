cmake_minimum_required(VERSION 3.16)
project(app_server)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Network)

set(SOURCES
    src/main.cpp
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
)

set(HEADERS
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
)

add_executable(app_server ${SOURCES} ${HEADERS})

target_include_directories(app_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/core/include
)

target_link_libraries(app_server
    core_lib
    Qt5::Core
    Qt5::Network
)

install(TARGETS app_server RUNTIME DESTINATION bin)