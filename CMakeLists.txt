cmake_minimum_required(VERSION 2.8)

PROJECT(Beats_Test)

# architecture
if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	SET(ARCH_DIR "64-bit")
else()
	SET(ARCH_DIR "32-bit")
endif()

ADD_SUBDIRECTORY(Utility)
ADD_SUBDIRECTORY(Components)