#include "globals.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace globals {
	bool	g_visible = true;
	VK_KEY	g_showMenuKey = VK_OEM_3;
	float	g_screenWidth = 0;
    float	g_screenHeight = 0;
}