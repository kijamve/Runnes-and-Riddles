#include "SkyBox.h"
SkyBox::SkyBox(QGLWidget *p,const char* c1,const char* c2,const char* c3,const char* c4,const char* c5,const char* c6) : parent(p)
{
	
	memset(g_TextureSkyBox,0,sizeof(g_TextureSkyBox));
	//Skybox
	CreateSkyBoxTexture(c1,		BACK_ID	 );
	CreateSkyBoxTexture(c2,		FRONT_ID );
	CreateSkyBoxTexture(c3,		BOTTOM_ID);
	CreateSkyBoxTexture(c4,		TOP_ID	 );
	CreateSkyBoxTexture(c5,		LEFT_ID	 );
	CreateSkyBoxTexture(c6,		RIGHT_ID );
}

SkyBox::~SkyBox(void)
{
}

void SkyBox::CreateSkyBoxTexture(const char* strFileName, int textureID)
{
	QImage img(strFileName);			// Load the bitmap and store the data

	g_TextureSkyBox[textureID]=parent->bindTexture(img, GL_TEXTURE_2D);
	// Initialize Texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


}

void SkyBox::CreateSkyBox(float x, float y, float z, float width, float height, float length)
{
	glEnable(GL_TEXTURE_2D);
	// Bind the BACK texture of the sky map to the BACK side of the cube
	glBindTexture(GL_TEXTURE_2D, g_TextureSkyBox[BACK_ID]);

	// This centers the sky box around (x, y, z)
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the BACK Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
		
	glEnd();

	// Bind the FRONT texture of the sky map to the FRONT side of the box
	glBindTexture(GL_TEXTURE_2D, g_TextureSkyBox[FRONT_ID]);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);	
		glNormal3f(0.0,0.0,-1.0);
		// Assign the texture coordinates and vertices for the FRONT Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height, z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z + length);
	glEnd();

	// Bind the BOTTOM texture of the sky map to the BOTTOM side of the box
	glBindTexture(GL_TEXTURE_2D, g_TextureSkyBox[BOTTOM_ID]);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);				
		glNormal3f(0.0,1.0,0.0);
		// Assign the texture coordinates and vertices for the BOTTOM Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y,			z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
	glEnd();

	// Bind the TOP texture of the sky map to the TOP side of the box
	glBindTexture(GL_TEXTURE_2D, g_TextureSkyBox[TOP_ID]);
	
	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);				
		glNormal3f(0.0,-1.0,0.0);		
		// Assign the texture coordinates and vertices for the TOP Side
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + height,	z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);
		
	glEnd();

	// Bind the LEFT texture of the sky map to the LEFT side of the box
	glBindTexture(GL_TEXTURE_2D, g_TextureSkyBox[LEFT_ID]);
	
	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		glNormal3f(1.0,0.0,0.0);
		// Assign the texture coordinates and vertices for the LEFT Side
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height,	z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);		
		
	glEnd();

	// Bind the RIGHT texture of the sky map to the RIGHT side of the box
	glBindTexture(GL_TEXTURE_2D, g_TextureSkyBox[RIGHT_ID]);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		glNormal3f(-1.0,0.0,0.0);
		// Assign the texture coordinates and vertices for the RIGHT Side
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height,	z + length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,	z);
	glEnd();
}