cmake_minimum_required(VERSION 3.5.0)
project ( CameraStreamingDemoServer )
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pthread -fno-stack-protector")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-stack-protector")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


#set(V4L2_LIBRARIES v4l2)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)


include_directories(${CMAKE_SOURCE_DIR})
include_directories(${Protobuf_INCLUDE_DIRS})


file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/*.c
                          ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/*.h)

##get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARIES} Threads::Threads)
