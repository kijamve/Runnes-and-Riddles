#pragma once
#include "vector.h"
#include "headers.h"
#include "include\glaux.h"

//-----------------------------------------------------------------------------
// SYMBOLIC CONSTANTS
//-----------------------------------------------------------------------------

// Classify Point
const int CP_FRONT   = 0;
const int CP_BACK    = 1;
const int CP_ONPLANE = 2;

// Collision Results
const int CR_BOUNCE  = 0;
const int CR_STICK   = 1;
const int CR_RECYCLE = 2;

const float OGL_PI = 3.141592654f;

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------

struct Plane
{
    CVector3 m_vNormal;           // The plane's normal
    CVector3 m_vPoint;            // A coplanar point within the plane
    float    m_fBounceFactor;     // Coefficient of restitution (or how bouncy the plane is)
    int      m_nCollisionResult;  // What will particles do when they strike the plane

    Plane   *m_pNext;             // Next plane in list
};

struct Particle
{
    CVector3  m_vCurPos;    // Current position of particle
	CVector3  m_vCurPos2;    
    CVector3  m_vCurVel;    // Current velocity of particle
	CVector3  m_direction;
    float     m_fInitTime;  // Time of creation of particle

    Particle *m_pNext;      // Next particle in list
};

struct PointVertex
{
    CVector3 posit;
    CVector3 color;
};

class CParticleSystem
{
public:
	CParticleSystem();
	~CParticleSystem(void);

    void SetMaxParticles( DWORD dwMaxParticles ) { m_dwMaxParticles = dwMaxParticles; }
	DWORD GetMaxParticles( void ) { return m_dwMaxParticles; }

    void SetNumToRelease( DWORD dwNumToRelease ) { m_dwNumToRelease = dwNumToRelease; }
	DWORD GetNumToRelease( void ) { return m_dwNumToRelease; }

    void SetReleaseInterval( float fReleaseInterval ) { m_fReleaseInterval = fReleaseInterval; }
    float GetReleaseInterval( void ) { return m_fReleaseInterval; }

    void SetLifeCycle( float fLifeCycle ) { m_fLifeCycle = fLifeCycle; }
	float GetLifeCycle( void ) { return m_fLifeCycle; }

    void SetSize( float fSize ) { m_fSize = fSize; }
	float GetSize( void ) { return m_fSize; }
	float GetMaxPointSize( void ) { return m_fMaxPointSize; }

    void SetColor( CVector3 clrColor ) { m_clrColor = clrColor; }
	CVector3 GetColor( void ) { return m_clrColor; }

    void SetPosition( CVector3 vPosition ) { m_vPosition = vPosition; }
	CVector3 GetPosition( void ) { return m_vPosition; }

    void SetVelocity( CVector3 vVelocity ) { m_vVelocity = vVelocity; }
	CVector3 GetVelocity( void ) { return m_vVelocity; }

    void SetGravity( CVector3 vGravity ) { m_vGravity = vGravity; }
	CVector3 GetGravity( void ) { return m_vGravity; }

    void SetWind( CVector3 vWind ) { m_vWind = vWind; }
	CVector3 GetWind( void ) { return m_vWind; }

    void SetAirResistence( bool bAirResistence ) { m_bAirResistence = bAirResistence; }
	bool GetAirResistence( void ) { return m_bAirResistence; }

    void SetVelocityVar( float fVelocityVar ) { m_fVelocityVar = fVelocityVar; }
	float GetVelocityVar( void ) { return m_fVelocityVar; }

    void SetCollisionPlane( CVector3 vPlaneNormal, CVector3 vPoint, 
                            float fBounceFactor = 1.0f, int nCollisionResult = CR_BOUNCE );
	
	void SetMaxLength( float fLength ) { m_maxlength = fLength; }
	float GetMaxLength( void ) { return m_maxlength; }

	void SetMinLength( float fLength ) { m_minlength = fLength; }
	float GetMinLength( void ) { return m_minlength; }

	void SetRadius( float fRadius) { m_radius = fRadius; }
	float GetRadius( void ) { return m_radius; }

    int Init( void );
    int Update( float fElapsedTime );
    void Render( void );

	int Init2( void );
    int Update2( float fElapsedTime );
    void Render2( void );

	float getRandomMinMax( float fMin, float fMax );
	CVector3 getRandomVector( void );

    void SetTexture( char *chTexFile );
    GLuint GetTextureID(void);

	void RestartParticleSystem(void);

protected:
	float m_maxlength,m_minlength;
	float m_radius;
    Particle   *m_pActiveList;
    Particle   *m_pFreeList;
    Plane      *m_pPlanes;
	DWORD       m_dwActiveCount;
	float       m_fCurrentTime;
	float       m_fLastUpdate;
    float       m_fMaxPointSize;
    bool        m_bDeviceSupportsPSIZE;
    GLuint      m_textureID;
    
    // Particle Attributes
    DWORD       m_dwMaxParticles;
    DWORD       m_dwNumToRelease;
    float       m_fReleaseInterval;
    float       m_fLifeCycle;
    float       m_fSize;
    CVector3    m_clrColor;
    CVector3    m_vPosition;
    CVector3    m_vVelocity;
    CVector3    m_vGravity;
    CVector3    m_vWind;
    bool        m_bAirResistence;
    float       m_fVelocityVar;
    char       *m_chTexFile;
};
