cmake_minimum_required(VERSION 3.6)
project(plantcontrol)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES plantcontrol.cpp)

add_executable(plantcontrol ${SOURCE_FILES})

find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(plantcontrol ${Boost_INCLUDE_DIRS})

link_directories(plantcontrol ${Boost_LIBRARY_DIRS})

target_link_libraries(plantcontrol ${Boost_LIBRARIES})

SET_TARGET_PROPERTIES(plantcontrol PROPERTIES LINK_FLAGS -Wl,-lboost_iostreams)