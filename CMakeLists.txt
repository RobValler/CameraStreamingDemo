#/*****************************************************************
# * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
# *
# * This file is part of the project: Communication platform
# *
# * This project can not be copied and/or distributed
# * without the express permission of the copyright holder
# *****************************************************************/
cmake_minimum_required(VERSION 3.15.0)
project(Camera)
set (Wfloat-conversion)
set (Wfno-stack-protector)
set (Winconsistent-missing-override)

FIND_PACKAGE(Protobuf REQUIRED)

add_subdirectory(messages)
add_subdirectory(Server)
add_subdirectory(Client)
