#/*****************************************************************
# * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
# *
# * This file is part of the project: Communication platform
# *
# * This project can not be copied and/or distributed
# * without the express permission of the copyright holder
# *****************************************************************/
cmake_minimum_required(VERSION 3.15.0)
project ( CameraStreamingDemoClient )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread -fno-stack-protector")

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Gui)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}/../messages)
include_directories(${ProtobufIncludePath})
link_directories("/usr/local/lib/comms")

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${ProtobufIncludePath}/*.pb.cc)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${ProtobufIncludePath}/*.pb.h)
file(GLOB_RECURSE UI ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)
file(GLOB_RECURSE RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)
QT5_WRAP_UI(FORMS ${UI})

get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCE})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets messages comms_client rt)


