# Generated by VisualGDB

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libboost_filesystem
LOCAL_SRC_FILES := C:\Project\FCEngine\FCEngine\Lib\Beats\libboost_filesystem.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libboost_system
LOCAL_SRC_FILES := C:\Project\FCEngine\FCEngine\Lib\Beats\libboost_system.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Utility-static
#VisualGDBAndroid: AutoUpdateSourcesInNextLine
LOCAL_SRC_FILES := ../../../../Utility/EnumStrGenerator/EnumStrGenerator.cpp ../../../../Utility/IdManager/IdManager.cpp ../../../../Utility/Math/Matrix3.cpp ../../../../Utility/Math/Matrix4.cpp ../../../../Utility/Math/Vector2.cpp ../../../../Utility/Math/Vector3.cpp ../../../../Utility/Math/Vector4.cpp ../../../../Utility/Serializer/Serializer.cpp ../../../../Utility/stdafx.cpp ../../../../Utility/StringHelper/StringHelper.cpp ../../../../Utility/TinyXML/tinystr.cpp ../../../../Utility/TinyXML/tinyxml.cpp ../../../../Utility/TinyXML/tinyxmlerror.cpp ../../../../Utility/TinyXML/tinyxmlparser.cpp ../../../../Utility/UtilityManager.cpp
LOCAL_C_INCLUDES := C:\Project\Beats_GitHub\Utility C:\Project\Beats_GitHub\Platform\Android C:\Project\Beats_GitHub\Platform C:\Project\Beats_GitHub C:\Project\Beats_GitHub\Beats\Lib\Boost_1_55_0
LOCAL_STATIC_LIBRARIES := libboost_filesystem libboost_system
LOCAL_SHARED_LIBRARIES := 
LOCAL_LDLIBS := 
LOCAL_CFLAGS := -D_DEBUG -DDEBUG
LOCAL_CPPFLAGS := -D_DEBUG -DDEBUG
LOCAL_LDFLAGS := 
LOCAL_LDFLAGS := 
include $(BUILD_STATIC_LIBRARY)
