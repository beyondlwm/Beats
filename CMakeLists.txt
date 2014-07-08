cmake_minimum_required(VERSION 2.8)

PROJECT(Beats_Test)

#1. Set Common Header.
INCLUDE(${CMAKE_SOURCE_DIR}/CMakeCommon.cmake)

#2. Set Include Directory
INCLUDE_DIRECTORIES(
		${CMAKE_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/Platform
		${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_FOLDER}
		${CMAKE_SOURCE_DIR}/Beats/Lib/boost_1_55_0
)

ADD_SUBDIRECTORY(Utility)
ADD_SUBDIRECTORY(Components)