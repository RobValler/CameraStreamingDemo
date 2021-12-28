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

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}/../messages)
include_directories(${ProtobufIncludePath})
link_directories("/usr/local/lib/comms")


file(GLOB_RECURSE CSOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c )
file(GLOB_RECURSE CPPSOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${ProtobufIncludePath}/*.pb.cc)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${ProtobufIncludePath}/*.pb.h)

add_executable(${PROJECT_NAME} ${CSOURCES} ${CPPSOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} messages comms_server rt)
