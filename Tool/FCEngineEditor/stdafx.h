// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "PublicDef.h"
#include "wx/wx.h"

// TODO: 在此处引用程序需要的其他头文件

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here
// STL Headers
#include <vector>
#include <map>
#include <list>
#include <mutex>
#include <thread>

#include "GL/glew.h"

#include "math/kazmath/kazmath/GL/matrix.h"
#include "math/kazmath/kazmath/mat4.h"
#include "math/kazmath/kazmath/vec4.h"
#include "math/kazmath/kazmath/vec3.h"
#include "math/kazmath/kazmath/vec2.h"

#include "Resource/ResourcePublic.h"
#include "Render/RenderPublic.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "LanguageManager.h"

#define NULLDATA -1
#define DEFAULT_PGBUTTONWIDTH 20
#define DEFAULT_DEPTH 32
#define MINWINDOWSIZE 50

#define SHIFTMOUSESPEED
#define WHEELSPEED 3.0F * 0.016F
#define SHIFTWHEELSPEED 6.0F * 0.016F

#define MAXFPS 60

#define WARNING "ERROR : "