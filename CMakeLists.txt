cmake_minimum_required(VERSION 3.21)
project(RainHub LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 COMPONENTS Core Widgets Network Sql REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/client)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/control)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rgear_icons)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/model)

# Simple static library to validate Qt linkage with existing model code.
add_library(rainhub_common STATIC
    model/GlobalEnum.hpp
    model/StationUtils.cpp
    model/RainGear.h
    model/RainGear.cpp
    model/RainGearFactory.h
    model/Stationlocal.h
    model/Stationlocal.cpp
    model/User.h
)
target_link_libraries(rainhub_common PRIVATE Qt6::Core)
target_include_directories(rainhub_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/model)

# Client executable (MVC View/Control entrypoint)
add_executable(rainhub_client
    client/main.cpp
    client/MainWindow.h
    client/MainWindow.cpp
    client/SlotItem.h
    client/SlotItem.cpp
    control/DatabaseManager.h
    control/DatabaseManager.cpp
    control/RainHubController.hpp
    control/OperationResult.h
    model/GlobalEnum.hpp
    model/RainGear_subclasses.hpp
    model/Stationlocal.h
    model/Stationlocal.cpp
    rgear_icons/icons.qrc
)
target_include_directories(rainhub_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/client
    ${CMAKE_CURRENT_SOURCE_DIR}/control
    ${CMAKE_CURRENT_SOURCE_DIR}/model
)
target_link_libraries(rainhub_client PRIVATE Qt6::Core Qt6::Widgets Qt6::Sql rainhub_common)

# Placeholder: add executables / further targets under client/server later.

