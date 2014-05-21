cmake_minimum_required(VERSION 2.8)

PROJECT(Beats_Test)


if(DEBUG_MODE)
  SET(CMAKE_BUILD_TYPE DEBUG)
else(DEBUG_MODE)
  SET(CMAKE_BUILD_TYPE RELEASE)
endif(DEBUG_MODE)

# architecture
if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	SET(ARCH_DIR "64-bit")
else()
	SET(ARCH_DIR "32-bit")
endif()

ADD_SUBDIRECTORY(Utility)
ADD_SUBDIRECTORY(Components)