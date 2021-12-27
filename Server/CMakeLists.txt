#/*****************************************************************
# * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
# *
# * This file is part of the project: Communication platform
# *
# * This project can not be copied and/or distributed
# * without the express permission of the copyright holder
# *****************************************************************/
cmake_minimum_required(VERSION 3.15.0)
project ( CameraStreamingDemoServer )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pthread -fno-stack-protector")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread -fno-stack-protector")

#set(BOOST_ROOT /usr/include/boost)
#set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME OFF)
#find_package(Boost)


#set(V4L2_LIBRARIES v4l2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}/../messages)
include_directories(${ProtobufIncludePath})

file(GLOB_RECURSE CSOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c )
file(GLOB_RECURSE CPPSOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${ProtobufIncludePath}/*.pb.cc)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${ProtobufIncludePath}/*.pb.h)

##get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
add_executable(${PROJECT_NAME} ${CSOURCES} ${CPPSOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} messages)
