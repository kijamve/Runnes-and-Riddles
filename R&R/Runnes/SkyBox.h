#pragma once
#include "headers.h"
#include <QGLWidget>
class SkyBox
{
public:
	SkyBox(QGLWidget *p,const char* c1,const char* c2,const char* c3,const char* c4,const char* c5,const char* c6);
public:
	~SkyBox(void);
	QGLWidget *parent;
	unsigned int g_TextureSkyBox[MAX_TEXTURES];		// This holds the texture info, referenced by an ID
	void CreateSkyBox(float x, float y, float z, float width, float height, float length);	//SkyBox
	void CreateSkyBoxTexture(const char* strFileName, int textureID);			//Create texture for skybox

};
