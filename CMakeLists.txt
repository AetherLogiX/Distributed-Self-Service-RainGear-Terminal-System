cmake_minimum_required(VERSION 3.21)
project(RainHub LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 COMPONENTS Core Widgets Network Sql REQUIRED)

# Simple static library to validate Qt linkage with existing model code.
add_library(rainhub_common STATIC
    model/GlobalDefs.h
    model/StationUtils.cpp
    model/RainGear.h
    model/RainGear.cpp
    model/RainGearFactory.h
    model/Station.h
    model/Station.cpp
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
)
target_link_libraries(rainhub_client PRIVATE Qt6::Core Qt6::Widgets rainhub_common)

# Placeholder: add executables / further targets under client/server later.

