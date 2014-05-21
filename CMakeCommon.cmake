#1. Set platform folder.

if(WIN32)	
	set(PLATFORM_FOLDER win32)
elseif(APPLE)
	set(PLATFORM_FOLDER ios)
elseif(ANDROID)
	set(PLATFORM_FOLDER android)
else()
	set(PLATFORM_FOLDER linux)
endif()