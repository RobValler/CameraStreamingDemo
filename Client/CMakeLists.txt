cmake_minimum_required(VERSION 3.5.0)
project ( CameraStreamingDemoClient )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread -fno-stack-protector")

#set(BOOST_ROOT /usr/include/boost)
#set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME OFF)
#find_package(Boost)

find_package(Protobuf REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${Protobuf_INCLUDE_DIRS})


file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/*.h)
file(GLOB_RECURSE UI ${CMAKE_SOURCE_DIR}/*.ui)
file(GLOB_RECURSE RESOURCE ${CMAKE_SOURCE_DIR}/*.qrc)
QT5_WRAP_UI(FORMS ${UI})

get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCE})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets ${Protobuf_LIBRARIES})

