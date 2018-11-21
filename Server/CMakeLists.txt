cmake_minimum_required(VERSION 3.5.0)
project ( CameraStreamingDemoServer )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pthread -fno-stack-protector")

#set(BOOST_ROOT /usr/include/boost)
#set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME OFF)
#find_package(Boost)


#set(V4L2_LIBRARIES v4l2)

include_directories(${CMAKE_SOURCE_DIR})

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/*.h)

##get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} )
