cmake_minimum_required(VERSION 3.16)
project(MonitoringSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Настройки для Windows
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Настройки компиляции
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG")
endif()

# Поиск Qt
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Svg Sql SerialPort Concurrent)

# Подключение подпроектов
add_subdirectory(core)
add_subdirectory(monitoring_server)
add_subdirectory(app_server)
add_subdirectory(gui_client)

# Ресурсы
qt5_add_resources(RESOURCES resources.qrc)

# Тесты (опционально)
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Установка
install(TARGETS monitoring_server app_server gui_client
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)

install(DIRECTORY resources/
    DESTINATION share/monitoring_system/resources
    PATTERN "*.svg" EXCLUDE
)

install(FILES resources/maps/russia.svg resources/schemes/plan_mo.svg
    DESTINATION share/monitoring_system/resources
)