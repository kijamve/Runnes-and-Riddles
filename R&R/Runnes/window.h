#ifndef WINDOW_H
#define WINDOW_H

#include "headers.h"
#include "audir.h"
#include "skybox.h"
#include "HeightMap.h"
#include "ParticleSystem.h"
#include <QtGui>
#include <QGLWidget>

typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );

class Window : public QGLWidget
{
public:
	unsigned int g_Texture[MAX_TEXTURES];			// This holds the texture info, referenced by an ID
	CLoadObj g_LoadObj;								// This is OBJ class.  This should go in a good model class.
	t3DModel g_3DModel;								// This holds the 3D Model info that we load in
	float g_RotateX;								// This is the current value at which the model is rotated
	float g_RotationSpeed ;							// This is the speed that our model rotates.  (-speed rotates left)
	bool objetos[6];								//objetos cargados en el panel on screen
	GLuint panel[6],botonesP;//texturas para el panel on screen	
	vector<int> CombinacionRunas;
	float dk;
	float dl;
	float dk2;
	float dl2;
	//Puerta y Cofre
	bool intClose,intOpen,intCloseDoor,intOpenDoor;
	float angCof,tFI,angDoor,tFIDoor;

//Logica de Runas
	bool runas[5];								
	bool ActiveRunas;								
	bool intCloseR[5],intOpenR[5];
	float angCofR[5],tFIR[5];
	int initRunes;
	int soundStep;
	int noPint,pisoAje,room;
    GLuint primitiveList;
	int initCol;
	int cofre,door;
	vector<Audir*> sonidos;									//Audio Initialize
	SkyBox *sky,*skyDead;									//SkyBox Initialize
	int countCaidas;
	CXBOXController playerController;
bool dead;


	Window(QWidget *parent);						//Window Handler
	~Window();


	HeightMap *hp;	
	HeightMap *escalera1;
	HeightMap *escalera2;
	HeightMap *techoCas;
	HeightMap *ejedrez;	
	HeightMap *pisoIni;	


	CFrustum g_Frustum;								// esta es nuestra piramide truncada
	bool g_bIgnoreFrustum;							// boleano que me la activa o no

    QTime m_time;									//Gametime
	QCursor *mouse;									//Mouse instance

	//Detecta colisiones
	bool isColliding(t3DObject &A, t3DObject &B);
	void resizeGL(int width, int height);			//Window resize
	
	// PARTICLE'S VAGO
	CParticleSystem *g_pParticleSystems[15];
	void initParticles();
	double g_dCurTime;
	float g_fElpasedTime;
	double g_dLastTime;
	int g_nParticleSystems;
	
	bool kL,kD,kR,kU;
	int nrObjectDraw;
	void drawObj(int ID,CVector3 *Pos=NULL);
	float dt,deadTime;
	//VSync handler
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT;
	void setVSync(int interval=1)
	{
	  const char *extensions = (const char *)glGetString( GL_EXTENSIONS );
	  if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
	  {
		  printf("Failed WGL_EXT_swap_control not Found");
			return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n");
	  }
	  else
	  {
			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
			
			if( wglSwapIntervalEXT )
				wglSwapIntervalEXT(interval);
	  }
	}

	char *textFileRead(char *fn) 
		{
			FILE *fp;
			char *content = NULL;

			int count=0;

			if (fn != NULL) 
			{
				fp = fopen(fn,"r");

				if (fp != NULL) 
				{
					fseek(fp, 0, SEEK_END);
					count = ftell(fp);
					rewind(fp);

					if (count > 0) 
					{
						content = (char *)malloc(sizeof(char) * (count+1));
						count = fread(content,sizeof(char),count,fp);
						content[count] = '\0';
					}
					fclose(fp);
				}
			}
			return content;
		}

		int textFileWrite(char *fn, char *s) 
		{
			FILE *fp;
			int status = 0;

			if (fn != NULL) 
			{
				fp = fopen(fn,"w");

				if (fp != NULL) 
				{
					if (fwrite(s,sizeof(char),strlen(s),fp) == strlen(s))
						status = 1;
					fclose(fp);
				}else printf("filed open %s\n",fn);
			}
			return(status);
		}

		GLint getUniLoc(GLuint program, const GLchar *name)
		{
			GLint loc;
			loc = glGetUniformLocation(program, name);

			return loc;
		}

		void initShader(const char* vr,const char* fr, GLuint &pp)
		{
			char *vs = NULL,*fs = NULL,*fs2 = NULL;
			
			//creacion del shader
			v = glCreateShader(GL_VERTEX_SHADER);
			f = glCreateShader(GL_FRAGMENT_SHADER);
			
			char cwd[1000];
			strcpy(cwd,vr);

			char cwd2[1000];
			strcpy(cwd2,fr);

			vs =  textFileRead(cwd);
			fs = textFileRead(cwd2);

			const char * ff = fs;	
			const char * vv = vs;

			//carga el codigo al shader creado
			glShaderSource(v, 1, &vv,NULL);
			glShaderSource(f, 1, &ff,NULL);
			
			free(vs);
			free(fs);
			
			//compila el shader
			glCompileShader(v);
			glCompileShader(f);

			//crea el programa
			pp = glCreateProgram();

			//adjunta el shader al programa creado
			glAttachShader(pp,f);
			glAttachShader(pp,v);

			//se enlaza el programa
			glLinkProgram(pp);
		}

		void applyShader(GLuint &pp)
		{
			glUseProgram(pp);
		}

		void unapplyShader(void)
		{
			glUseProgram(0);
		}

	//void DrawBox(CVector3 Maximo,CVector3 Minimo);	

protected:
    QTimer *m_timer;
	QString debugDisplay;
    int sec;
	int fps;
	double ratio;
	int mX,mY, lastX, lastY;
	float posX, posZ;
	float viewX, viewY;
	CCamera camera;
	float hant;
	float blur_s[BLUR_STEP];
	int pas,initGem;
	float prom,initGame;
	GLuint v,f,f2,p,p2,normalMap;
	//motion blur
	void OrthoMode(int left, int top, int right, int bottom);
	void PerspectiveMode();
	void CreateRenderTexture(UINT textureArray[], int size, int channels, int type, int textureID);
	bool AnimateNextFrame(int desiredFrameRate);
	void RenderMotionBlur(int textureID);
	void suenaWav(int NroSound, float TimeSound);
	void repaint();
	//motion blur fin
	UINT g_Texture2[2];
int g_BlurRate;
float traslate_MB;
int g_Viewport;
bool montionBlur;
	
GLuint      g_colorMapTexture;
GLuint      g_normalMapTexture;

	void initializeGL();
	void paintGL();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent (QKeyEvent * event );
	void keyReleaseEvent(QKeyEvent * event );

	void orthogonalStart (void);
	void orthogonalEnd (void);
};


#endif // WINDOW_H
