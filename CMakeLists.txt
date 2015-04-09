cmake_minimum_required(VERSION 2.8)

PROJECT(Beats)

#1. Set Common Header.
INCLUDE(${CMAKE_SOURCE_DIR}/CMakeCommon.cmake)

#2. Set Include Directory
INCLUDE_DIRECTORIES(
		${CMAKE_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/Platform
		${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_FOLDER}
		${ANDROID_NDK}/platforms/android-${ANDROID_API_LEVEL}/arch-arm/usr/include)

ADD_SUBDIRECTORY(Utility)
ADD_SUBDIRECTORY(Components)