#include "window.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include "include\glaux.h"
#include <mmsystem.h>

//#define DIS_SHADER

Window::Window(QWidget *parent) : QGLWidget(parent),wglSwapIntervalEXT(0)
{
	setCursor(Qt::BlankCursor);
	setMinimumSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	setGeometry(50,50,SCREEN_WIDTH, SCREEN_HEIGHT);
	angCof=0;
	memset(blur_s,0,sizeof(blur_s));
	pas=0;
	dk=0.0f;
	dl=0.0f;
	dk2=0.0f;
	dl2=0.0f;
	m_timer = new QTimer(this);
	m_timer->setInterval(1);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	m_timer->start();

	// Inicializar Tiempo para FPS
	m_time=QTime::currentTime();
	sec=0;
	fps=0;

	//Inicializar camara
	lastX=0;
	lastY=0;

	// Inicializar carga de OBJs
	memset(g_Texture,0,sizeof(g_Texture));
	g_RotateX=0.0f;
	g_RotationSpeed=0.8f;
	/*camera.PositionCamera(	-830.65,181,198.173,
							38.31 , 54.21, -29.3,
							0 , 1   , 0);*/
	camera.PositionCamera(	37.65,54,-29.173,
							38.31 , 54.21, -29.3,
							0 , 1   , 0);
	
	g_dLastTime=0.0;
	kL=false;
	kD=false;
	kR=false;
	kU=false;
	montionBlur = false;
	cofre=-1;
	door=-1;
	angCof=0.0f;
	angDoor=0.0f;
	memset(angCofR,0,sizeof(angCofR));
	memset(tFIR,0,sizeof(tFIR));
	memset(objetos,false,sizeof(objetos));
	memset(runas,false,sizeof(runas));
	ActiveRunas=false;
}

Window::~Window()
{
	for( int i = 0; i < g_nParticleSystems; ++i )
	{
		if( g_pParticleSystems[i] != NULL )
		{
			delete g_pParticleSystems[i];
			g_pParticleSystems[i] = NULL;
		}
	}
}

void Window::resizeGL(int width, int height)
{
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)width/(float)height, 0.1f, 10000.0f);
}

bool Window::isColliding(t3DObject &A, t3DObject &B){
	for(int i=0;i<B.numOfFaces;i++){
		CVector3 N=B.pNormalsFaces[i];
		CVector3 q=A.center;
		int vertIndex = B.pFaces[i].vertIndex[0];
		CVector3 p=B.pVerts[vertIndex];
					
		if(N.x*q.x + N.y*q.y + N.z*q.z - (N.x*p.x + N.y*p.y+N.z*p.z) <= 0 ) return false;
	}
	return true;
}

void Window::initializeGL()
{
	primitiveList  = glGenLists(1000);
	setVSync(1);
	g_bIgnoreFrustum = false;
	printf("Load Model...");

	//Texturas del Panel On Screen
	panel[0]=bindTexture(QImage("Textures/gem1.jpg"), GL_TEXTURE_2D);
	panel[1]=bindTexture(QImage("Textures/gem2.jpg"), GL_TEXTURE_2D);
	panel[2]=bindTexture(QImage("Textures/gem3.jpg"), GL_TEXTURE_2D);
	panel[3]=bindTexture(QImage("Textures/gem4.jpg"), GL_TEXTURE_2D);
	panel[4]=bindTexture(QImage("Textures/gem5.jpg"), GL_TEXTURE_2D);
	panel[5]=bindTexture(QImage("Textures/key.jpg"),  GL_TEXTURE_2D);
	botonesP=bindTexture(QImage("Textures/runnesButtons.jpg"),  GL_TEXTURE_2D);


	//Models
	g_LoadObj.ImportObj(&g_3DModel, "Models/pisoTechoCastillo.obj");
	g_LoadObj.ImportObj(&g_3DModel, "Models/pisoIni.obj");
	g_LoadObj.ImportObj(&g_3DModel, "Models/foliage.obj",		bindTexture(QImage("Textures/foliageTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/stairLHM.obj");
	g_LoadObj.ImportObj(&g_3DModel, "Models/stairRHM.obj");
	g_LoadObj.ImportObj(&g_3DModel, "Models/tunnel2.obj");
	noPint=g_3DModel.numOfObjects;
	g_LoadObj.ImportObj(&g_3DModel, "Models/plane.obj",		bindTexture(QImage("Textures/planeTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/stairL.obj",		bindTexture(QImage("Textures/stairLTexture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/stairLTextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/stairR.obj",		bindTexture(QImage("Textures/stairRTexture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/stairRTextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/castle.obj",	bindTexture(QImage("Textures/castleTexture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/castleTextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/planeStart.obj",bindTexture(QImage("Textures/planeStartTexture.jpg"), GL_TEXTURE_2D));

	pisoAje=g_3DModel.numOfObjects;
	g_LoadObj.ImportObj(&g_3DModel, "Models/checker.obj",	bindTexture(QImage("Textures/checkerTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/trees.obj",		bindTexture(QImage("Textures/treesTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/table.obj",		bindTexture(QImage("Textures/tableTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/tower1.obj",	bindTexture(QImage("Textures/tower1Texture.jpg"), GL_TEXTURE_2D),	bindTexture(QImage("Textures/tower1TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/tower2.obj",	bindTexture(QImage("Textures/tower2Texture.jpg"), GL_TEXTURE_2D),	bindTexture(QImage("Textures/tower2TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/tower4.obj",	bindTexture(QImage("Textures/tower3Texture.jpg"), GL_TEXTURE_2D),	bindTexture(QImage("Textures/tower3TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/tower3.obj",	bindTexture(QImage("Textures/tower4Texture.jpg"), GL_TEXTURE_2D),	bindTexture(QImage("Textures/tower4TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/lamp.obj",		bindTexture(QImage("Textures/lampTexture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/lampTextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/indoorFloor.obj",		bindTexture(QImage("Textures/indoorFloorTexture.jpg"), GL_TEXTURE_2D));



	g_LoadObj.ImportObj(&g_3DModel, "Models/chest.obj",		bindTexture(QImage("Textures/chestTexture.jpg"), GL_TEXTURE_2D));
	g_3DModel.pObject[g_3DModel.numOfObjects-1].center-=CVector3(0,25,0);
	cofre=g_3DModel.numOfObjects;
	g_LoadObj.ImportObj(&g_3DModel, "Models/chestTop.obj",	bindTexture(QImage("Textures/chestTopTexture.jpg"), GL_TEXTURE_2D));
	g_3DModel.pObject[g_3DModel.numOfObjects-1].center-=CVector3(0,25,0);

	door=g_3DModel.numOfObjects;
	g_LoadObj.ImportObj(&g_3DModel, "Models/door.obj",		bindTexture(QImage("Textures/doorTexture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/doorTextureNormal.jpg"), GL_TEXTURE_2D));

	room=g_3DModel.numOfObjects;
	g_LoadObj.ImportObj(&g_3DModel, "Models/room.obj",		bindTexture(QImage("Textures/roomTexture.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/tunnel.obj",	bindTexture(QImage("Textures/tunnelTexture.jpg"), GL_TEXTURE_2D));

	initRunes=g_3DModel.numOfObjects;
	g_LoadObj.ImportObj(&g_3DModel, "Models/rune1.obj",		bindTexture(QImage("Textures/rune1Texture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/rune1TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/rune2.obj",		bindTexture(QImage("Textures/rune2Texture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/rune2TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/rune3.obj",		bindTexture(QImage("Textures/rune3Texture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/rune3TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/rune4.obj",		bindTexture(QImage("Textures/rune4Texture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/rune4TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/rune5.obj",		bindTexture(QImage("Textures/rune5Texture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/rune5TextureNormal.jpg"), GL_TEXTURE_2D));

	initGem=g_3DModel.numOfObjects;
	g_LoadObj.ImportObj(&g_3DModel, "Models/gem1.obj",		bindTexture(QImage("Textures/gem1Texture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/rune1TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/gem2.obj",		bindTexture(QImage("Textures/gem2Texture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/rune2TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/gem3.obj",		bindTexture(QImage("Textures/gem3Texture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/rune3TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/gem4.obj",		bindTexture(QImage("Textures/gem4Texture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/rune4TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/gem5.obj",		bindTexture(QImage("Textures/gem5Texture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/rune5TextureNormal.jpg"), GL_TEXTURE_2D));
	g_LoadObj.ImportObj(&g_3DModel, "Models/key.obj",		bindTexture(QImage("Textures/keyTexture.jpg"), GL_TEXTURE_2D),		bindTexture(QImage("Textures/keyNormal.jpg"), GL_TEXTURE_2D));
	initCol=g_3DModel.numOfObjects;
	g_LoadObj.ImportObj(&g_3DModel, "Models/colision.obj");
	g_LoadObj.ImportObj(&g_3DModel, "Models/doorColision.obj");
	printf(" End...\n");

	glEnable(GL_COLOR_MATERIAL);					// Allow color	
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

	if(playerController.IsConnected())
		printf("Player1 Controller XBOX is Connected...\n");
	else 
		printf("Player1 Controller XBOX not Connected...\n");
	setVisible(true);

	#ifndef DIS_SHADER
		glewInit();//shader
		if (glewIsSupported("GL_VERSION_2_0"))
			printf("Ready for OpenGL 2.0\n");
		else 
		{
			printf("OpenGL 2.0 not supported\n");
			//exit(1);
		}
	#endif;

	#ifndef DIS_SHADER
		initShader("./glsl/phong.vert","./glsl/phong.frag",p); 
		initShader("./glsl/morph.vert","./glsl/morph.frag",p2); 
		initShader("./glsl/normalMap.vert","./glsl/normalMap.frag",normalMap);
		//applyShader();	
	#endif

	printf("Calculate HeightMap... ");
	printf(" 1/6 -");
	hp=new HeightMap(&(g_3DModel.pObject[noPint]),150);
	printf(" 2/6 -");
	escalera1=new HeightMap(&(g_3DModel.pObject[3]),20);
	printf(" 3/6 -");
	escalera2=new HeightMap(&(g_3DModel.pObject[4]),20);
	printf(" 4/6 -");
	techoCas=new HeightMap(&(g_3DModel.pObject[0]),30);
	printf(" 5/6 -");
	ejedrez=new HeightMap(&(g_3DModel.pObject[pisoAje]),50);
	printf(" 6/6 -");
	pisoIni=new HeightMap(&(g_3DModel.pObject[1]),50);
	printf(" End...\n");

	initParticles();
	sky=new SkyBox(this,"Textures/SkyBox/Back.bmp","Textures/SkyBox/Front.bmp","Textures/SkyBox/Bottom.bmp","Textures/SkyBox/Top.bmp","Textures/SkyBox/Left.bmp","Textures/SkyBox/Right.bmp");
	skyDead=new SkyBox(this,"Textures/SkyBox/skull_back.jpeg","Textures/SkyBox/skull_back.jpeg","Textures/SkyBox/skull_back.jpeg","Textures/SkyBox/skull_back.jpeg","Textures/SkyBox/skull_back.jpeg","Textures/SkyBox/skull_back.jpeg");

	g_BlurRate = 100;
	g_Viewport = 512;
	CreateRenderTexture(g_Texture2, 512, 3, GL_RGB, 0);
	float dist=0.0f;
	soundStep=4;
	sonidos.push_back(new Audir("Sounds/ambienceMusic.wav",true));
	sonidos.push_back(new Audir("Sounds/doorOpen.wav"));
	sonidos.push_back(new Audir("Sounds/doorLock.wav"));
	sonidos.push_back(new Audir("Sounds/gemsDrag.wav"));
	sonidos.push_back(new Audir("Sounds/footStepGrass1.wav"));
	sonidos.push_back(new Audir("Sounds/footStepGrass2.wav"));
	sonidos.push_back(new Audir("Sounds/footStepGrass3.wav"));
	sonidos.push_back(new Audir("Sounds/footStepGrass4.wav"));
	sonidos.push_back(new Audir("Sounds/footStepGrass5.wav"));
	sonidos.push_back(new Audir("Sounds/footStepGrass6.wav"));
	sonidos.push_back(new Audir("Sounds/footStepGrass7.wav"));
	sonidos.push_back(new Audir("Sounds/footStepGrass8.wav"));
	sonidos.push_back(new Audir("Sounds/footStepFloor1.wav"));
	sonidos.push_back(new Audir("Sounds/footStepFloor2.wav"));
	sonidos.push_back(new Audir("Sounds/footStepFloor3.wav"));
	sonidos.push_back(new Audir("Sounds/footStepFloor4.wav"));
	sonidos.push_back(new Audir("Sounds/footStepFloor5.wav"));
	sonidos.push_back(new Audir("Sounds/footStepFloor6.wav"));
	sonidos.push_back(new Audir("Sounds/footStepFloor7.wav"));
	sonidos.push_back(new Audir("Sounds/footStepFloor8.wav"));
	sonidos.push_back(new Audir("Sounds/stone1.wav"));
	sonidos.push_back(new Audir("Sounds/stone2.wav"));
	sonidos.push_back(new Audir("Sounds/stone3.wav"));
	sonidos.push_back(new Audir("Sounds/stone4.wav"));
	sonidos.push_back(new Audir("Sounds/stone5.wav"));
	sonidos.push_back(new Audir("Sounds/Monster_Breaths.wav",true));
	dead=false;
	suenaWav(0,0);
	initGame=float(GAMETIME)/1000.0f+20.0f;
}
void Window::initParticles()
{
	g_nParticleSystems=16;

	//
    // Fire
	//

	g_pParticleSystems[0] = new CParticleSystem();

    g_pParticleSystems[0]->SetTexture( "Textures/fire.bmp" );
	g_pParticleSystems[0]->SetMaxParticles( 15 );
    g_pParticleSystems[0]->SetNumToRelease( 1 );
    g_pParticleSystems[0]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[0]->SetLifeCycle( 1.0f );
    g_pParticleSystems[0]->SetSize( 2.0f );
    g_pParticleSystems[0]->SetColor( CVector3( 0.5f, 0.5f, 0.5f ));
    g_pParticleSystems[0]->SetPosition( CVector3( -878.0f, 184.0f, 287.25f ) );
    g_pParticleSystems[0]->SetVelocity( CVector3( 0.0f, 5.0f, 0.0f ) );
    g_pParticleSystems[0]->SetGravity( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[0]->SetWind( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[0]->SetVelocityVar( 1.5f );

	g_pParticleSystems[0]->Init();

	//
    // Fire
	//

	g_pParticleSystems[1] = new CParticleSystem();

    g_pParticleSystems[1]->SetTexture( "Textures/fire.bmp" );
	g_pParticleSystems[1]->SetMaxParticles( 15 );
    g_pParticleSystems[1]->SetNumToRelease( 1 );
    g_pParticleSystems[1]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[1]->SetLifeCycle( 1.0f );
    g_pParticleSystems[1]->SetSize( 2.0f );
    g_pParticleSystems[1]->SetColor( CVector3( 0.5f, 0.5f, 0.5f ));
    g_pParticleSystems[1]->SetPosition( CVector3( -810.0f, 184.0f, 287.25f ) );
    g_pParticleSystems[1]->SetVelocity( CVector3( 0.0f, 5.0f, 0.0f ) );
    g_pParticleSystems[1]->SetGravity( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[1]->SetWind( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[1]->SetVelocityVar( 1.5f );

	g_pParticleSystems[1]->Init();

	//
    // Fire
	//

	g_pParticleSystems[2] = new CParticleSystem();

    g_pParticleSystems[2]->SetTexture( "Textures/fire.bmp" );
	g_pParticleSystems[2]->SetMaxParticles( 15 );
    g_pParticleSystems[2]->SetNumToRelease( 1 );
    g_pParticleSystems[2]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[2]->SetLifeCycle( 1.0f );
    g_pParticleSystems[2]->SetSize( 2.5f );
    g_pParticleSystems[2]->SetColor( CVector3( 0.5f, 0.5f, 0.5f ));
    g_pParticleSystems[2]->SetPosition( CVector3( -973.0f, 227.0f, 283.4f ) );
    g_pParticleSystems[2]->SetVelocity( CVector3( 0.0f, 5.0f, 0.0f ) );
    g_pParticleSystems[2]->SetGravity( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[2]->SetWind( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[2]->SetVelocityVar( 1.5f );

	g_pParticleSystems[2]->Init();

	//
    // Fire
	//

	g_pParticleSystems[3] = new CParticleSystem();

    g_pParticleSystems[3]->SetTexture( "Textures/fire.bmp" );
	g_pParticleSystems[3]->SetMaxParticles( 15 );
    g_pParticleSystems[3]->SetNumToRelease( 1 );
    g_pParticleSystems[3]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[3]->SetLifeCycle( 1.0f );
    g_pParticleSystems[3]->SetSize( 2.0f );
    g_pParticleSystems[3]->SetColor( CVector3( 0.5f, 0.5f, 0.5f ));
    g_pParticleSystems[3]->SetPosition( CVector3( -965.7f, 220.0f, 293.2f ) );
    g_pParticleSystems[3]->SetVelocity( CVector3( 0.0f, 5.0f, 0.0f ) );
    g_pParticleSystems[3]->SetGravity( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[3]->SetWind( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[3]->SetVelocityVar( 1.5f );

	g_pParticleSystems[3]->Init();

	//
    // Fire
	//

	g_pParticleSystems[4] = new CParticleSystem();

    g_pParticleSystems[4]->SetTexture( "Textures/fire.bmp" );
	g_pParticleSystems[4]->SetMaxParticles( 15 );
    g_pParticleSystems[4]->SetNumToRelease( 1 );
    g_pParticleSystems[4]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[4]->SetLifeCycle( 1.0f );
    g_pParticleSystems[4]->SetSize( 2.0f );
    g_pParticleSystems[4]->SetColor( CVector3( 0.5f, 0.5f, 0.5f ));
    g_pParticleSystems[4]->SetPosition( CVector3( -981.75f, 220.0f, 275.4f ) );
    g_pParticleSystems[4]->SetVelocity( CVector3( 0.0f, 5.0f, 0.0f ) );
    g_pParticleSystems[4]->SetGravity( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[4]->SetWind( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[4]->SetVelocityVar( 1.5f );

	g_pParticleSystems[4]->Init();

	//
    // Smooke
	//

	g_pParticleSystems[5] = new CParticleSystem();

    g_pParticleSystems[5]->SetTexture( "Textures/smoke.bmp" );
	g_pParticleSystems[5]->SetMaxParticles( 4 );
    g_pParticleSystems[5]->SetNumToRelease( 1 );
    g_pParticleSystems[5]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[5]->SetLifeCycle( 1.0f );
    g_pParticleSystems[5]->SetSize( 1.75f );
    g_pParticleSystems[5]->SetColor( CVector3( 0.5f, 0.5f, 0.5f ));
    g_pParticleSystems[5]->SetPosition( CVector3( -878.0f, 186.5f, 287.25f ) );
    g_pParticleSystems[5]->SetVelocity( CVector3( 1.0f, 4.0f, 0.0f ) );
    g_pParticleSystems[5]->SetGravity( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[5]->SetWind( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[5]->SetVelocityVar( 1.5f );

	g_pParticleSystems[5]->Init();

	//
    // Smooke
	//

	g_pParticleSystems[6] = new CParticleSystem();

    g_pParticleSystems[6]->SetTexture( "Textures/smoke.bmp" );
	g_pParticleSystems[6]->SetMaxParticles( 4 );
    g_pParticleSystems[6]->SetNumToRelease( 1 );
    g_pParticleSystems[6]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[6]->SetLifeCycle( 1.0f );
    g_pParticleSystems[6]->SetSize( 1.75f );
    g_pParticleSystems[6]->SetColor( CVector3( 0.5f, 0.5f, 0.5f ));
    g_pParticleSystems[6]->SetPosition( CVector3( -810.0f, 186.5f, 287.25f ) );
    g_pParticleSystems[6]->SetVelocity( CVector3( 1.0f, 4.0f, 0.0f ) );
    g_pParticleSystems[6]->SetGravity( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[6]->SetWind( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[6]->SetVelocityVar( 1.5f );

	g_pParticleSystems[6]->Init();

	//
    // Smooke
	//

	g_pParticleSystems[7] = new CParticleSystem();

    g_pParticleSystems[7]->SetTexture( "Textures/smoke.bmp" );
	g_pParticleSystems[7]->SetMaxParticles( 4 );
    g_pParticleSystems[7]->SetNumToRelease( 1 );
    g_pParticleSystems[7]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[7]->SetLifeCycle( 1.0f );
    g_pParticleSystems[7]->SetSize( 1.75f );
    g_pParticleSystems[7]->SetColor( CVector3( 0.5f, 0.5f, 0.5f ));
    g_pParticleSystems[7]->SetPosition( CVector3( -973.0f, 229.5f, 283.1f ) );
    g_pParticleSystems[7]->SetVelocity( CVector3( 1.0f, 4.0f, 0.0f ) );
    g_pParticleSystems[7]->SetGravity( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[7]->SetWind( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[7]->SetVelocityVar( 1.5f );

	g_pParticleSystems[7]->Init();

	//
    // Smooke
	//

	g_pParticleSystems[8] = new CParticleSystem();

    g_pParticleSystems[8]->SetTexture( "Textures/smoke.bmp" );
	g_pParticleSystems[8]->SetMaxParticles( 4 );
    g_pParticleSystems[8]->SetNumToRelease( 1 );
    g_pParticleSystems[8]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[8]->SetLifeCycle( 1.0f );
    g_pParticleSystems[8]->SetSize( 1.75f );
    g_pParticleSystems[8]->SetColor( CVector3( 0.5f, 0.5f, 0.5f ));
    g_pParticleSystems[8]->SetPosition( CVector3( -965.7f, 222.5f, 292.9f ) );
    g_pParticleSystems[8]->SetVelocity( CVector3( 1.0f, 4.0f, 0.0f ) );
    g_pParticleSystems[8]->SetGravity( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[8]->SetWind( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[8]->SetVelocityVar( 1.5f );

	g_pParticleSystems[8]->Init();

	//
    // Smooke
	//

	g_pParticleSystems[9] = new CParticleSystem();

    g_pParticleSystems[9]->SetTexture( "Textures/smoke.bmp" );
	g_pParticleSystems[9]->SetMaxParticles( 4 );
    g_pParticleSystems[9]->SetNumToRelease( 1 );
    g_pParticleSystems[9]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[9]->SetLifeCycle( 1.0f );
    g_pParticleSystems[9]->SetSize( 1.75f );
    g_pParticleSystems[9]->SetColor( CVector3( 0.5f, 0.5f, 0.5f ));
    g_pParticleSystems[9]->SetPosition( CVector3( -981.75f, 222.5f, 275.1f ) );
    g_pParticleSystems[9]->SetVelocity( CVector3( 1.0f, 4.0f, 0.0f ) );
    g_pParticleSystems[9]->SetGravity( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[9]->SetWind( CVector3( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[9]->SetVelocityVar( 1.5f );

	g_pParticleSystems[9]->Init();

	//
	// Spark
	//

	for (int gema=0; gema<6; ++gema)
	{
		g_pParticleSystems[10+gema] = new CParticleSystem();
		g_pParticleSystems[10+gema]->SetTexture( "Textures/particle.bmp" );
		g_pParticleSystems[10+gema]->SetMaxParticles( 9 );
		g_pParticleSystems[10+gema]->SetNumToRelease( 1 );
		g_pParticleSystems[10+gema]->SetReleaseInterval( 0.05f );
		g_pParticleSystems[10+gema]->SetLifeCycle( 1.0f );
		g_pParticleSystems[10+gema]->SetSize( 2.0f );
		g_pParticleSystems[10+gema]->SetColor( CVector3( 0.1f, 0.1f, 0.9f ));
		g_pParticleSystems[10+gema]->SetPosition( g_3DModel.pObject[initGem+gema].center );
		g_pParticleSystems[10+gema]->SetVelocity( CVector3( 0.0f, 5.0f, 0.0f ) );
		g_pParticleSystems[10+gema]->SetGravity( CVector3( 0.0f, 0.0f, 0.0f ) );
		g_pParticleSystems[10+gema]->SetWind( CVector3( 0.0f, 0.0f, 0.0f ) );
		g_pParticleSystems[10+gema]->SetVelocityVar( 1.5f );

		g_pParticleSystems[10+gema]->Init();
		/*g_pParticleSystems[10+gema]->SetMaxParticles( 45 );
		g_pParticleSystems[10+gema]->SetNumToRelease( 0 );
		g_pParticleSystems[10+gema]->SetReleaseInterval( 0.05f );
		g_pParticleSystems[10+gema]->SetLifeCycle( 4.0f );
		g_pParticleSystems[10+gema]->SetSize( 2.0f );
		g_pParticleSystems[10+gema]->SetColor( CVector3( 1.0f, 1.0f, 1.0f ));
		g_pParticleSystems[10+gema]->SetPosition( g_3DModel.pObject[initGem+gema].center );
		g_pParticleSystems[10+gema]->SetVelocity( CVector3( 0.0f, 5.0f, 0.0f ) );
		g_pParticleSystems[10+gema]->SetGravity( CVector3( 0.0f, 0.0f, 0.0f ) );
		g_pParticleSystems[10+gema]->SetWind( CVector3( 0.0f, 0.0f, 0.0f ) );
		g_pParticleSystems[10+gema]->SetVelocityVar( 1.5f );
		g_pParticleSystems[10+gema]->SetMaxLength(2.0f);
		g_pParticleSystems[10+gema]->SetMinLength(1.0f);
		g_pParticleSystems[10+gema]->SetRadius(0.25f);

		g_pParticleSystems[10+gema]->Init2();*/
	}
}




void Window::drawObj(int ID,CVector3 *Pos){
	//if(ID<noPint) return;
	if(!ActiveRunas &&  ID==initGem+4) return;
	if(g_3DModel.pObject.size() <= ID) return;
	if(ID>=initCol) return;
	t3DObject *pObject = &g_3DModel.pObject[ID];
	
	if(ID!=door && ((Pos==NULL && !g_bIgnoreFrustum && !g_Frustum.SphereInFrustum(pObject->center.x,pObject->center.y,pObject->center.z, pObject->radio)) || (Pos!=NULL && !g_bIgnoreFrustum && !g_Frustum.SphereInFrustum(pObject->center.x+Pos->x,pObject->center.y+Pos->y,pObject->center.z+Pos->z, pObject->radio)))) 
		return;
	
	nrObjectDraw++;
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	if(pObject->bHasTexture) {			
		if(pObject->materialID > 0 ){
			glEnable(GL_TEXTURE_2D);
			#ifndef DIS_SHADER
				if(pObject->normalID!=-1){
					glActiveTexture(GL_TEXTURE1);
					glUniform1i(getUniLoc(normalMap, "normalMap"), 1);
					glBindTexture(GL_TEXTURE_2D, pObject->normalID);
					glActiveTexture(GL_TEXTURE0);
					glEnable(GL_TEXTURE_2D);
					glUniform1i(getUniLoc(normalMap, "colorMap"), 0);
				}else{
					for(int i=0; i<5; i++)
						glUniform1i(getUniLoc(p, QString(QString("activeLight[")+QString::number(i)+QString("]")).toAscii()), 0);
					glUniform1i(getUniLoc(p, "text"), 1);
					glActiveTexture(GL_TEXTURE0);
					glUniform1i(getUniLoc(p, "texture"), 0);
				}
			#endif
			glBindTexture(GL_TEXTURE_2D, pObject->materialID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}else{
			#ifndef DIS_SHADER
				glUniform1i(getUniLoc(p, "text"), 0);
			#endif
			glDisable(GL_TEXTURE_2D);
		}
	} else {
		#ifndef DIS_SHADER
			glUniform1i(getUniLoc(p, "text"), 0);
		#endif
		glDisable(GL_TEXTURE_2D);
	}
	if(ID>=initRunes && ID<initRunes+5){
		int IdRune=ID-initRunes;
		if(runas[IdRune]){
			intCloseR[IdRune]=false;
			if(!intOpenR[IdRune]){
				intOpenR[IdRune]=true;
				tFIR[IdRune]=float(GAMETIME)/1000.0f;
			}
			if(angCofR[IdRune]<90){
				angCofR[IdRune]+=(float(GAMETIME)/1000.0f-tFIR[IdRune]) * 60.0f;
				tFIR[IdRune]=float(GAMETIME)/1000.0f;
			}
		}else{
			intOpenR[IdRune] = false;
			if(!intCloseR[IdRune]){
				intCloseR[IdRune]=true;
				tFIR[IdRune]=float(GAMETIME)/1000.0f;
			}
			if(angCofR[IdRune]>0){
				angCofR[IdRune]-=(float(GAMETIME)/1000.0f-tFIR[IdRune]) * 60.0f;
				if(angCofR[IdRune]<0)angCofR[IdRune]=0;
				tFIR[IdRune]=float(GAMETIME)/1000.0f;
			}
		}
		glTranslatef(pObject->center.x,pObject->center.y,pObject->center.z);
		glRotatef(angCofR[IdRune],0.0f,1.0f,0.0f);
		glTranslatef(-pObject->center.x,-pObject->center.y,-pObject->center.z);
	}
	if(ID<initCol){
		if(ID==cofre && objetos[0] && objetos[1] && objetos[2] && objetos[3] && objetos[4]){
			if(CVector3::Distance(pObject->center,camera.center)<=70){
				intClose=false;
				if(!intOpen){
					intOpen=true;
					tFI=float(GAMETIME)/1000.0f;
				}
				if(angCof<90){
					angCof+=(float(GAMETIME)/1000.0f-tFI) * 20.0f;
					tFI=float(GAMETIME)/1000.0f;
				}
			}else{
				intOpen = false;
				if(!intClose){
					intClose=true;
					tFI=float(GAMETIME)/1000.0f;
				}
				if(angCof>0){
					angCof-=(float(GAMETIME)/1000.0f-tFI) * 20.0f;
					if(angCof<0)angCof=0;
					tFI=float(GAMETIME)/1000.0f;
					
				}
			}
			
			glTranslatef(pObject->center.x,pObject->Min.y-25,pObject->Max.z);
			glRotatef(angCof,1.0f,0.0f,0.0f);
			glTranslatef(-pObject->center.x,-(pObject->Min.y-25),-pObject->Max.z);
		}
		if(ID==door && objetos[5]){
			if(CVector3::Distance(pObject->center,camera.center)<=100){
				intCloseDoor=false;
				if(!intOpenDoor){
					intOpenDoor=true;
					tFIDoor=float(GAMETIME)/1000.0f;
					suenaWav(1,0);
				}
				if(angDoor<70){
					angDoor+=(float(GAMETIME)/1000.0f-tFIDoor) * 50.0f;
					tFIDoor=float(GAMETIME)/1000.0f;
				}
			}else{
				intOpenDoor = false;
				if(!intCloseDoor){
					intCloseDoor=true;
					tFIDoor=float(GAMETIME)/1000.0f;
					suenaWav(2,0);
				}
				if(angDoor>0){
					angDoor-=(float(GAMETIME)/1000.0f-tFIDoor) * 50.0f;
					if(angDoor<0)angDoor=0;
					tFIDoor=float(GAMETIME)/1000.0f;
					
				}
			}
			
			glTranslatef(pObject->center.x,pObject->center.y,pObject->Max.z);
			glRotatef(angDoor,0.0f,1.0f,0.0f);
			glTranslatef(-pObject->center.x,-pObject->center.y,-pObject->Max.z);
		}
		if(ID==cofre-1 || ID==cofre)
			glTranslatef(0,-25,0);
	
		glBegin(GL_TRIANGLES);
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					int vertIndex = pObject->pFaces[j].vertIndex[whichVertex];
					glNormal3f(pObject->pNormals[ vertIndex ].x, pObject->pNormals[ vertIndex ].y, pObject->pNormals[ vertIndex ].z);
					
					if(pObject->bHasTexture) {
						if(pObject->pTexVerts) {
							int coordIndex = pObject->pFaces[j].coordIndex[whichVertex];
							glTexCoord2f(pObject->pTexVerts[ coordIndex ].x, pObject->pTexVerts[ coordIndex ].y);	
							#ifndef DIS_SHADER
								glMultiTexCoord2f(GL_TEXTURE0,pObject->pTexVerts[ coordIndex ].x, pObject->pTexVerts[ coordIndex ].y);
								glMultiTexCoord4f(GL_TEXTURE1,pObject->pTang[vertIndex].x,pObject->pTang[vertIndex].y,pObject->pTang[vertIndex].z,pObject->pTang[vertIndex].w);
							#endif
						}
					} 
					glVertex3f(pObject->pVerts[ vertIndex ].x, pObject->pVerts[ vertIndex ].y, pObject->pVerts[ vertIndex ].z);
				}
			}
		glEnd();
	}else{
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		if(!isColliding(camera.box,*pObject))
			glColor3ub(255, 255, 255);
		else
			glColor3ub(255, 0, 0);
		glBegin(GL_LINES);
		for(int j = 0; j < pObject->numOfVerts; j++)
		{	
			for(int i = 0; i < pObject->numOfVerts; i++)
			{
				if(i==j) continue;
					glVertex3f(pObject->pVerts[ j ].x, pObject->pVerts[ j ].y, pObject->pVerts[ j ].z);
					glVertex3f(pObject->pVerts[ i ].x, pObject->pVerts[ i ].y, pObject->pVerts[ i ].z);
			
			}
		}
	
		glEnd();
		
	}
}
void Window::paintGL()
{ 
	
	if(playerController.IsConnected())
	{
			//if(playerController.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
			//{
			//	playerController.Vibrate(65535, 0);
			//}
			//DPAD Move
			if(playerController.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			{
				camera.StrafeCamera(-0.8f);
			}

			if(playerController.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			{
				camera.StrafeCamera(0.8f);
			}

			if(playerController.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
			{
				camera.MoveCamera(-0.8f);
			}

			if(playerController.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			{
				camera.MoveCamera(0.8f);
			}

			//Analog Left Stick Move
			if(playerController.GetState().Gamepad.sThumbLX < -10000 || playerController.GetState().Gamepad.sThumbLX > 10000 || playerController.GetState().Gamepad.sThumbLY < -10000 || playerController.GetState().Gamepad.sThumbLY > 10000)
			{
				posX=(float)playerController.GetState().Gamepad.sThumbLX * 0.000027f;
				posZ=(float)playerController.GetState().Gamepad.sThumbLY * 0.000027f;
				
				camera.StrafeCamera(-posX);
				camera.MoveCamera(-posZ);
			}

			//Analog Right Stick Move
			if(playerController.GetState().Gamepad.sThumbRX < -10000 || playerController.GetState().Gamepad.sThumbRX > 10000 || playerController.GetState().Gamepad.sThumbRY < -10000 || playerController.GetState().Gamepad.sThumbRY > 10000)
			{
				viewX=(float)playerController.GetState().Gamepad.sThumbRX * 0.0006f;
				viewY=(float)playerController.GetState().Gamepad.sThumbRY * 0.0006f;
				camera.Rotate(-viewX, -viewY);
			}
	}

	//OpenGL Initialize
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Camara
	camera.Look();	
	glDisable(GL_LIGHTING);
	
	// Camara
	if(dead){
		skyDead->CreateSkyBox(0, 0, 0, 4096, 4096, 4096);
		camera.center=CVector3(37.65,54,-29.173);
		if(deadTime<float(GAMETIME)/1000.0f){
			memset(angCofR,0,sizeof(angCofR));
			memset(tFIR,0,sizeof(tFIR));
			memset(objetos,false,sizeof(objetos));
			memset(runas,false,sizeof(runas));
			ActiveRunas=false;
			dead=false;
			montionBlur=false;
			camera.PositionCamera(	37.65,54,-29.173,
							38.31 , 54.21, -29.3,
							0 , 1   , 0);
			initGame=float(GAMETIME)/1000.0f+20.0f;
		}
	}else{
		sky->CreateSkyBox(0, 0, 0, 4096, 4096, 4096);
	}
	


	if(montionBlur){
		if( AnimateNextFrame(g_BlurRate) )
		{
			glViewport(0, 0, g_Viewport, g_Viewport);								
			RenderMotionBlur(0);
			repaint();
			glBindTexture(GL_TEXTURE_2D,g_Texture2[0]);				
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, g_Viewport, g_Viewport, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
			glViewport(0, 0, width(), height());	
		}
		RenderMotionBlur(0);
	}

	

	repaint();

	// DRAW PARTICLE'S VAGO  << Jajajaj elll vagoooo!!!

	
	g_dCurTime     = timeGetTime();// GAMETIME;
	g_fElpasedTime = (float)((g_dCurTime - g_dLastTime) * 0.001);
	g_dLastTime    = g_dCurTime;

	glEnable(GL_TEXTURE_2D);
	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_FALSE );

	glEnable( GL_BLEND );
	glBlendFunc( GL_DST_ALPHA, GL_ONE );
	for (int p=0; p<g_nParticleSystems; ++p)
	{
		glPushMatrix();
		//
		// Render particle system
		//

		if (p>=10)
		{
			if (p==14) {
				if (!objetos[p-10] && ActiveRunas) {
					g_pParticleSystems[p]->Update( (float)g_fElpasedTime );

					glBindTexture( GL_TEXTURE_2D, g_pParticleSystems[p]->GetTextureID() );
					g_pParticleSystems[p]->Render();
				}
			}else if(!objetos[p-10]){

				g_pParticleSystems[p]->Update( (float)g_fElpasedTime );

				glBindTexture( GL_TEXTURE_2D, g_pParticleSystems[p]->GetTextureID() );
				g_pParticleSystems[p]->Render();
			}
		} else {
			g_pParticleSystems[p]->Update( (float)g_fElpasedTime );

			glBindTexture( GL_TEXTURE_2D, g_pParticleSystems[p]->GetTextureID() );
			g_pParticleSystems[p]->Render();
		}
		glPopMatrix();

	}

	//
	// Reset OpenGL states...
	//

	glDepthMask( GL_TRUE );
	glDisable( GL_BLEND );

	

	
// Calcular Posicion de heightMap
	CVector3 vPos		= camera.center;
	CVector3 vNewPos    = vPos;
	float h;
	float alturas[6];
	alturas[0]=hp->Height(vPos.x,vPos.z);
	alturas[1]=escalera1->Height(vPos.x,vPos.z);
	alturas[2]=escalera2->Height(vPos.x,vPos.z);
	alturas[3]=ejedrez->Height(vPos.x,vPos.z);
	alturas[4]=techoCas->Height(vPos.x,vPos.z);
	alturas[5]=pisoIni->Height(vPos.x,vPos.z);
	float difAltu[5];
	difAltu[0]=vPos.y-alturas[0];
	difAltu[1]=vPos.y-alturas[1];
	difAltu[2]=vPos.y-alturas[2];
	difAltu[3]=vPos.y-alturas[3];
	difAltu[4]=vPos.y-alturas[4];
	difAltu[5]=vPos.y-alturas[5];
	float hDif=fabs(difAltu[0]);
	int Hi=0;
	for(int i=1;i<6;++i){
		if(difAltu[i]<hDif && difAltu[i]>=0){
			Hi=i;
			hDif=difAltu[i];
		}
	}
	h=alturas[Hi];
	if(Hi==5 || Hi==0) soundStep=4;
	else soundStep=12;

	if(Hi==4) montionBlur=true;
	else  montionBlur=false;
	if(vPos.y < h + 40 || vPos.y > h + 40)
	{
		/*if(initGame<float(GAMETIME)/1000.0f && (fabs(vPos.y-h)<150 || h==-200) ){
			dead=true;		
			montionBlur=true;
			deadTime=float(GAMETIME)/1000.0f + 10.0f;
		}*/
		blur_s[pas]=h;
		pas=(pas+1)%BLUR_STEP;
		//if(Hi!=1 && Hi!=2){
			prom=0.0f;
			for(int i=0;i<BLUR_STEP;++i) prom+=blur_s[i];
			prom/=float(BLUR_STEP);
			h=prom;
		//}
		vNewPos.y = h + 40;
		float temp = vNewPos.y - vPos.y;
		CVector3 vView = camera.eye;
		vView.y += temp;
		camera.PositionCamera(vNewPos.x,  vNewPos.y,  vNewPos.z, vView.x,	vView.y,	vView.z,	0, 1, 0);								
	}

	#ifndef DIS_SHADER
		applyShader(p2);
		glUniform1i(getUniLoc(p2, "text"), 1);
		glUniform1i(getUniLoc(p2, "texture"), 0);
		glUniform1f(getUniLoc(p2, "time"), GAMETIME/1000.0f);
		glActiveTexture(GL_TEXTURE0);
	#endif

	//for(int i=hp->minX;i<=hp->maxX;i+=40){
	//	for(int j=hp->minY;j<=hp->maxY;j+=40){
	//		if(i>-454 && j>210) continue;
	//		if(j>40 && i>-996 && i<-648) continue;
	//		if(hp->mapaR[i][j]) continue;
	//		glPushMatrix();
	//		CVector3 v;
	//		v.x=i;
	//		v.y=hp->mapa[i][j]-6;
	//		v.z=j;
	//		#ifndef DIS_SHADER
	//		glUniform1i(getUniLoc(p2, "r1"),v.y);
	//		glUniform1i(getUniLoc(p2, "r2"),hp->minX+v.x);
	//		glUniform1i(getUniLoc(p2, "r22"),hp->maxX+v.x);
	//		glUniform1i(getUniLoc(p2, "r3"),v.z);
	//		#endif
	//
	//		glTranslatef(v.x,v.y,v.z);
	//		drawObj(2,&v);
	//		glPopMatrix();
	//	}
	//}

	//Panel On Screen
	#ifndef DIS_SHADER
		unapplyShader();
	#endif

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	orthogonalStart();
	int tU=0;


	for(int i=width()/3;i<=(width()*2)/3;i+=width()/15){

		if(objetos[tU]){
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_COLOR, GL_ONE);
			glDepthMask(false);
			glPushMatrix();
			//glColor4ub(0,0,0,255);
			glBindTexture(GL_TEXTURE_2D, panel[tU]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glBegin(GL_QUADS);
				glTexCoord2f(0.0f,0.0f);
				glVertex2f(i,height()-5);
				glTexCoord2f(0.0f,1.0f);
				glVertex2f(i, height()-width()/15 -10);
				glTexCoord2f(1.0f,1.0f);
				glVertex2f(i+width()/15-5, height()-width()/15 -10);
				glTexCoord2f(1.0f,0.0f);
				glVertex2f(i+width()/15-5, height()-5);
			glEnd();
			glPopMatrix();
			glDepthMask(true); // Put the Z-buffer back into it's normal "Z-read and Z-write" state
			glDisable(GL_BLEND);
		}
		tU++;	
	}
	if(isColliding(camera.box,g_3DModel.pObject[room])){
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		glDepthMask(false);
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, botonesP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2f(width()/2-327,height()-100);
			glTexCoord2f(0,1);
			glVertex2f(width()/2-327, height()-100-204);
			glTexCoord2f(1,1);
			glVertex2f(width()/2+327, height()-100-204);
			glTexCoord2f(1,0);
			glVertex2f(width()/2+327, height()-100);
		glEnd();
		glPopMatrix();
		glDepthMask(true); // Put the Z-buffer back into it's normal "Z-read and Z-write" state
		glDisable(GL_BLEND);
	}
	orthogonalEnd();

	glColor3f(1.0f,1.0f,1.0f);
	//FPS counter
	++fps;
	if(m_time.currentTime().second()!=sec && fps>0){
		ratio=double(fps)/double(abs(m_time.currentTime().second()-sec));
		sec=m_time.currentTime().second();
		setWindowTitle(QString("FPS: ")+QString::number(ratio));
		fps=0;
	}

	//Display FPS
	//debugDisplay=QString("FPS: ")+QString::number(ratio)+
		//QString(" Eye: ")+QString::number((double)camera.eye.x)+QString(" ")+QString::number((double)camera.eye.y)+QString(" ")+QString::number((double)camera.eye.z)+
		//QString(" Center: ")+QString::number((double)camera.center.x)+QString(" ")+QString::number((double)camera.center.y)+QString(" ")+QString::number((double)camera.center.z)+
		//QString(" Up: ")+QString::number((double)camera.up.x)+QString(" ")+QString::number((double)camera.up.y)+QString(" ")+QString::number((double)camera.up.z) +
		//QString(" GameTime: ")+QString::number(GAMETIME)+QString(" NRO objetos pintados: ")+QString::number(nrObjectDraw);
		
	//Abrir cofre
	
	

	//debugDisplay+=QString(" dist: ")+QString::number(CVector3::Distance(g_3DModel.pObject[initRunes].center,camera.center));
	//renderText(10,10,debugDisplay);

}
float* genF(float a,float b,float c,float d){
	float *r=new float[4];
	r[0]=a;
	r[1]=b;
	r[2]=c;
	r[3]=d;
	return r;
}
void Window::repaint()
{

	
	#ifndef DIS_SHADER
		applyShader(p);
	#endif

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT0);	
	glEnable(GL_LIGHT1);	
	glEnable(GL_LIGHT2);	
	glEnable(GL_LIGHT3);	
	glEnable(GL_LIGHT4);			
	glEnable(GL_LIGHTING);							

	glLightfv(GL_LIGHT0, GL_AMBIENT, genF(1.0f, 1.0f, 1.0f, 1.0f));
	glLightfv(GL_LIGHT0, GL_POSITION, genF(-856.0f+float(rand()%20)/9.0f ,182.0f+float(rand()%20)/9.0f  ,207.0f+float(rand()%20)/9.0f , 1.0f));
	glLightfv(GL_LIGHT0, GL_SPECULAR, genF(1.0f, 1.0f, 1.0f, 1.0f));
	glLightfv(GL_LIGHT0, GL_DIFFUSE, genF(0.6f, 0.2f, 0.4f, 1.0f));

	glLightfv(GL_LIGHT1, GL_AMBIENT, genF(1.0f, 1.0f, 1.0f, 1.0f));
	glLightfv(GL_LIGHT1, GL_POSITION, genF(49.0f, 1.50f, 38.0f, 1.0f));
	glLightfv(GL_LIGHT1, GL_SPECULAR, genF(1.0f, 1.0f, 1.0f, 1.0f));
	glLightfv(GL_LIGHT1, GL_DIFFUSE, genF(1.0f, 1.0f, 1.0f, 1.0f));

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, genF(1.0f, 1.0f, 1.0f, 1.0f));
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, genF(1.0f, 1.0f, 1.0f, 1.0f));
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, genF(1.0f, 1.0f, 1.0f, 1.0f));
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	
	//numeros de objetos pintados
	nrObjectDraw = 0;

	// Calcular frustum
	g_Frustum.CalculateFrustum();


	#ifndef DIS_SHADER
		glUniform1i(getUniLoc(p, "text"), 1);
		glUniform1i(getUniLoc(p, "texture"), 0);
		glUniform1f(getUniLoc(p, "time"), GAMETIME/1000.0f);
		glActiveTexture(GL_TEXTURE0);
	#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_2D);
	

	//Draw OBJ
	//if(!dead)
	for(int i = noPint; i < g_3DModel.numOfObjects; i++)
	{
		glPushMatrix();
		#ifndef DIS_SHADER
			if(g_3DModel.pObject[i].normalID!=-1){
				applyShader(normalMap);
			}else{
				applyShader(p);
			}
		#endif
		if(i>=initGem && i<initGem+6){
			if(!objetos[i-initGem]) drawObj(i);
		}else{
			drawObj(i);
		}
		glPopMatrix();
	}

	#ifndef DIS_SHADER
		unapplyShader();
	#endif


	
}


void Window::mousePressEvent(QMouseEvent *event)
{	
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
	
	mX=event->x();
	mY=event->y();

	QCursor::setPos(width()/2 + geometry().left(), height()/2 + geometry().top());
	
	lastX=width()/2 /* + geometry().left() */- mX;
	lastY=height()/2/* + geometry().top() */- mY;
	
	if(lastX==0 && lastY==0) 
		return; 

	//printf("%d %d\n",mX,mY);
	camera.Rotate(lastX,-lastY);
	
}

void Window::keyPressEvent(QKeyEvent *event)
{
	if(event->key()==Qt::Key_Escape){
		close();
	}
	if(event->key()==Qt::Key_A || event->key()==Qt::Key_Left){
		kL=true;suenaWav(soundStep,1);
	}
	if(event->key()==Qt::Key_S || event->key()==Qt::Key_Down){
		kD=true;suenaWav(soundStep,1);
	}
	if(event->key()==Qt::Key_D || event->key()==Qt::Key_Right){
		kR=true;suenaWav(soundStep,1);
	}
	if(event->key()==Qt::Key_W || event->key()==Qt::Key_Up){
		kU=true;suenaWav(soundStep,1);
	}

	
	if(event->key()==Qt::Key_Space){

		for(int i=initGem;i<6+initGem;++i){
			if(i==initGem+4 && ActiveRunas || i!=initGem+4)
				if(!objetos[i-initGem] && CVector3::Distance(g_3DModel.pObject[i].center,camera.center)<60){
					objetos[i-initGem]=true;
					suenaWav(3,0);
				}
		}
	}
	
	
	if(kL)	{ dk=1.8; dk2=-2.0f; }
	else	{ dk=0.0f; dk2=0.0f; }//camera.StrafeCamera(1.8f);

	if(kD)	{ dl=1.8f; dl2=-2.0f; }
	else	{ dl=0.0f; dl2=0.0f;  } //camera.MoveCamera(1.8f);

	if(kR)	{ dk=-1.8f; dk2=2.0f; }
	else if(!kL)	{ dk=0.0f;  dk2=0.0f; }//camera.StrafeCamera(-1.8f);

	if(kU)	{ dl=-1.8f; dl2=2.0f; }//camera.MoveCamera(-1.8f);
	else if(!kD)	{ dl=0.0f;  dl2=0.0f; }

	

	if(dl!=0.0f || dk!=0.0f){
		camera.StrafeCamera(dk);
		camera.MoveCamera(dl);
		int finCol=g_3DModel.numOfObjects;
		if(objetos[5]) finCol--;
		for(int i=initCol;i<finCol; i++)
			if(isColliding(camera.box,g_3DModel.pObject[i])){
				camera.StrafeCamera(dk2);
				camera.MoveCamera(dl2);
				break;
			}
		if(isColliding(camera.box,g_3DModel.pObject[noPint-1]) || isColliding(camera.box,g_3DModel.pObject[noPint-2])){
			if(!sonidos[25]->sound->isPlaying()) sonidos[25]->Play();
			float volV=min(1.0f,(CVector3::Distance(g_3DModel.pObject[initRunes].center,camera.center)-70.0f)/400);
			sonidos[25]->sound->setVolume(1.0f - volV);
			sonidos[0]->sound->setVolume(volV);
		}else {
			sonidos[25]->sound->setVolume(0);
			sonidos[0]->sound->setVolume(1.0f);
		}
	}

	if(event->key()==Qt::Key_R){
		memset(angCofR,0,sizeof(angCofR));
		memset(tFIR,0,sizeof(tFIR));
		memset(objetos,false,sizeof(objetos));
		memset(runas,false,sizeof(runas));
		ActiveRunas=false;
		dead=false;
		montionBlur=false;
		camera.PositionCamera(	37.65,54,-29.173,
						38.31 , 54.21, -29.3,
						0 , 1   , 0);
	}

	if(event->key()==Qt::Key_0)
	{
		if(this->isFullScreen())
		{
			showNormal();
			setCursor(Qt::BlankCursor);
		}
		else
		{
			showFullScreen();
			setCursor(Qt::BlankCursor);	
		}
	}

	if(event->key()==Qt::Key_1)
	{
		if(!ActiveRunas && !runas[0] && isColliding(camera.box,g_3DModel.pObject[room])){
			suenaWav(20,1);
			CombinacionRunas.push_back(0);
			runas[0]=true;

		}
	}
	if(event->key()==Qt::Key_2)
	{
		if(!ActiveRunas && !runas[1] && isColliding(camera.box,g_3DModel.pObject[room])){
			suenaWav(21,1);
			CombinacionRunas.push_back(1);
			runas[1]=true;
		}
	}
	if(event->key()==Qt::Key_3)
	{
		if(!ActiveRunas && !runas[2] && isColliding(camera.box,g_3DModel.pObject[room])){
			suenaWav(22,1);
			CombinacionRunas.push_back(2);
			runas[2]=true;
		}
	}
	if(event->key()==Qt::Key_4)
	{
		if(!ActiveRunas && !runas[3] && isColliding(camera.box,g_3DModel.pObject[room])){
			suenaWav(23,1);
			CombinacionRunas.push_back(3);
			runas[3]=true;
		}
	}
	if(event->key()==Qt::Key_5)
	{
		if(!ActiveRunas && !runas[4] && isColliding(camera.box,g_3DModel.pObject[room])){
			suenaWav(24,1);
			CombinacionRunas.push_back(4);
			runas[4]=true;
		}
	}
	if(!ActiveRunas && CombinacionRunas.size()==5){
	
		if(
	CombinacionRunas[0]!=1  || 
	CombinacionRunas[1]!=0  || 
	CombinacionRunas[2]!=4  || 
	CombinacionRunas[3]!=3  || 
	CombinacionRunas[4]!=2){
			memset(runas,false,sizeof(runas));
			CombinacionRunas.clear();
		}else{
			ActiveRunas=true;
		}
	}
}
void Window::keyReleaseEvent(QKeyEvent *event)
{

	if(event->key()==Qt::Key_A || event->key()==Qt::Key_Left){
		kL=false;
	}
	if(event->key()==Qt::Key_S || event->key()==Qt::Key_Down){
		kD=false;
	}
	if(event->key()==Qt::Key_D || event->key()==Qt::Key_Right){
		kR=false;
	}
	if(event->key()==Qt::Key_W || event->key()==Qt::Key_Up){
		kU=false;
	}

}
void Window::orthogonalStart (void) {
	GLint iViewport[4];

	// Get a copy of the viewport
	glGetIntegerv( GL_VIEWPORT, iViewport );

	// Save a copy of the projection matrix so that we can restore it 
	// when it's time to do 3D rendering again.
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	// Set up the orthographic projection
	glOrtho( iViewport[0], iViewport[0]+iViewport[2],
			 iViewport[1]+iViewport[3], iViewport[1], -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	// Make sure depth testing and lighting are disabled for 2D rendering until
	// we are finished rendering in 2D
	glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );

}
void Window::orthogonalEnd (void) {
	glPopAttrib();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

}
void Window::OrthoMode(int left, int top, int right, int bottom)
{
	// Switch to our projection matrix so that we can change it to ortho mode, not perspective.
	glMatrixMode(GL_PROJECTION);						

	// Push on a new matrix so that we can just pop it off to go back to perspective mode
	glPushMatrix();									
	
	// Reset the current matrix to our identify matrix
	glLoadIdentity();								

	//Pass in our 2D ortho screen coordinates like so (left, right, bottom, top).  The last
	// 2 parameters are the near and far planes.
	glOrtho( left, right, bottom, top, 0, 1 );	
	
	// Switch to model view so that we can render the scope image
	glMatrixMode(GL_MODELVIEW);								

	// Initialize the current model view matrix with the identity matrix
	glLoadIdentity();										
}
void Window::PerspectiveMode()										// Set Up A Perspective View
{
	// Enter into our projection matrix mode
	glMatrixMode( GL_PROJECTION );							

	// Pop off the last matrix pushed on when in projection mode (Get rid of ortho mode)
	glPopMatrix();											

	// Go back to our model view matrix like normal
	glMatrixMode( GL_MODELVIEW );							

	// We should be in the normal 3D perspective mode now
}

void Window::CreateRenderTexture(UINT textureArray[], int size, int channels, int type, int textureID)										
{
	// Create a pointer to store the blank image data
	unsigned int *pTexture = NULL;											

	// Allocate and init memory for the image array and point to it from pTexture
	pTexture = new unsigned int [size * size * channels];
	memset(pTexture, 0, size * size * channels * sizeof(unsigned int));	

	// Register the texture with OpenGL and bind it to the texture ID
	glGenTextures(1, &textureArray[textureID]);								
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);					
	
	// Create the texture and store it on the video card
	glTexImage2D(GL_TEXTURE_2D, 0, channels, size, size, 0, type, GL_UNSIGNED_INT, pTexture);						
	
	// Set the texture quality
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// Since we stored the texture space with OpenGL, we can delete the image data
	delete [] pTexture;																					
}


bool Window::AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = 0.0f;
	float elapsedTime = 0.0;

	// Get current time in seconds  (milliseconds * .001 = seconds)
	float currentTime = GetTickCount() * 0.001f; 

	// Get the elapsed time by subtracting the current time from the last time
	elapsedTime = currentTime - lastTime;

	// Check if the time since we last checked is over (1 second / framesPerSecond)
	if( elapsedTime > (1.0f / desiredFrameRate) )
	{
		// Reset the last time
		lastTime = currentTime;	

		// Return TRUE, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}
void Window::suenaWav(int NroSound, float TimeSound){
	//static float lastTime = 0.0f;
	float elapsedTime = 0.0;
	float currentTime = GAMETIME * 0.001f; 

	// Get the elapsed time by subtracting the current time from the last time
	elapsedTime = currentTime - sonidos[NroSound]->lastTime;

	// Check if the time since we last checked is over (1 second / framesPerSecond)
	if( elapsedTime > (TimeSound ) )
	{
		sonidos[NroSound]->lastTime = currentTime;
		if(NroSound==4 || NroSound==12)
		{
			NroSound+=rand()%8;
		}
		// Reset the last time
		sonidos[NroSound]->Play();	

		// Return TRUE, to animate the next frame of animation
	}

	// We don't animate right now.

}

void Window::RenderMotionBlur(int textureID2)
{
	// This function was created to blend the rendered texture over top of the screen
	// to create the recursive blur effect.  What happens is we start out by turning
	// off depth testing, setting our blending mode, then binding the texture of our 
	// rendered textured over the QUAD that is about to be created.  Next, we set our 
	// alpha level to %90 of full strength.  This makes it so it will slowly disappear.
	// Before rendering the QUAD, we want to go into ortho mode.  This makes it easier
	// to render a QUAD over the full screen without having to deal with vertices, but
	// 2D coordinates.  Once we render the QUAD, we want to go back to perspective mode.
	// We can then turn depth testing back on and turn off texturing.

	// Push on a new stack so that we do not interfere with the current matrix
	glPushMatrix();

		// Turn off depth testing so that we can blend over the screen
		glDisable(GL_DEPTH_TEST);			

		// Set our blend method and enable blending
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);	
		glEnable(GL_BLEND);					

		// Bind the rendered texture to our QUAD
		glBindTexture(GL_TEXTURE_2D, g_Texture2[textureID2]);			

		// Decrease the alpha value of the blend by %10 so it will fade nicely
		glColor4f(1, 1, 1, 0.6f);

		// Switch to 2D mode (Ortho mode)
		OrthoMode(0, 0, width(), height());

		// Display a 2D quad with our blended texture
		glBegin(GL_QUADS);

			// Display the top left point of the 2D image
			glTexCoord2f(0.0f, 1.0f);	glVertex2f(0, 0);

			// Display the bottom left point of the 2D image
			glTexCoord2f(0.0f, 0.0f);	glVertex2f(0, height());

			// Display the bottom right point of the 2D image
			glTexCoord2f(1.0f, 0.0f);	glVertex2f(width(), height());

			// Display the top right point of the 2D image
			glTexCoord2f(1.0f, 1.0f);	glVertex2f(width(), 0);

		// Stop drawing 
		glEnd();
		
		// Let's set our mode back to perspective 3D mode.
		PerspectiveMode();

		// Turn depth testing back on and texturing off.  If you do NOT do these 2 lines of 
		// code it produces a cool flash effect that might come in handy somewhere down the road.
		glEnable(GL_DEPTH_TEST);						
		glDisable(GL_BLEND);							

	// Go back to our original matrix
	glPopMatrix();
}
