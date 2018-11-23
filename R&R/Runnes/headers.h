#ifndef _MY_HEADER
#define _MY_HEADER

#include <iostream>	
#include <fstream>
#include <string>
#include <vector>	
#include <queue>
#include <math.h>
#include <algorithm>
#include <float.h>

#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4305)
#pragma warning(disable:4056)
#pragma warning(disable:4756)

using namespace std;

#include "vector.h"
#include "include/3d.h"
#include "include/obj.h"
#include "camera.h"
#include "Frustum.h"

typedef unsigned char	byte;
#define SCREEN_WIDTH	1024								//Window width
#define SCREEN_HEIGHT	700									//Window height
#define SCREEN_DEPTH	32									//Color Depth
#define MAX_PATH		260
#define MAX_TEXTURES	100									// The maximum amount of textures to load
#define BACK_ID			0									// The texture ID for the back side of the cube
#define FRONT_ID		1									// The texture ID for the front side of the cube
#define BOTTOM_ID		2									// The texture ID for the bottom side of the cube
#define TOP_ID			3									// The texture ID for the top side of the cube
#define LEFT_ID			4									// The texture ID for the left side of the cube
#define RIGHT_ID		5									// The texture ID for the right side of the cube
#define GAMETIME		m_time.msecsTo(QTime::currentTime())
#define PI				3.1415926535897932					// This is our famous PI
#define BEHIND			0
#define INTERSECTS		1
#define FRONT			2
#define MAP_SIZE		4096								// This is the size of our .raw height map
#define STEP_SIZE		64  								    // This is width and height of each QUAD
#define HEIGHT_RATIO	1.5f								// This is the ratio that the Y is scaled according to the X and Z
#define MAX_PARTICLES	1024 // Maximum number of particles
#define BLUR_STEP		15

// Include OpenGL Library
#include "include/glew.h"
#include "include/gl.h"
#include "include/glu.h"
#include "include/al.h"
#include "include/alc.h"
#include "include/efx.h"
#include "include/efx-creative.h"
#include "include/EFX-Util.h"
#include "include/xram.h"
#include "include/Framework/Framework.h"
#include "include/Framework/Vorbis/vorbisfile.h"
#include "include/XInput.h"
#include "include/CXBOXController.h"
#include "include/audiere.h"
//#include "include/3DMath.h"
// Now, the XInput Library
// NOTE: COMMENT THIS OUT IF YOU ARE NOT USING A COMPILER THAT SUPPORTS THIS METHOD OF LINKING LIBRARIES

//#include "GL/glut.h"
//#pragma comment(lib, "lib/glut32.lib")

#pragma comment(lib, "lib/opengl32.lib")
#pragma comment(lib, "lib/glew32.lib")
#pragma comment(lib, "lib/OpenAL32.lib")
#pragma comment(lib, "lib/EFX-Util.lib")
#pragma comment(lib, "lib/XInput.lib")
#pragma comment(lib, "lib/glaux.lib")
#pragma comment(lib, "lib/audiere.lib")



#endif