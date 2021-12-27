#/*****************************************************************
# * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
# *
# * This file is part of the project: StarterApp
# *
# * This project can not be copied and/or distributed
# * without the express permission of the copyright holder
# *****************************************************************/
cmake_minimum_required(VERSION 3.15.0)
project(messages)

FIND_PACKAGE(Protobuf REQUIRED)

set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL /messages)
include_directories(${ProtobufIncludePath})

file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/src/*.proto")
PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})
add_library(${PROJECT_NAME} STATIC ${ProtoSources} ${ProtoHeaders} ${ProtoFiles})
target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARY})
