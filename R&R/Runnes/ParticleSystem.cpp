#include "ParticleSystem.h"

#define DEGTORAD(degree) ((degree) * (3.141592654f / 180.0f))
#define RADTODEG(radian) ((radian) * (180.0f / 3.141592654f))

PFNGLPOINTPARAMETERFARBPROC  glPointParameterfARB  = NULL;
PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB = NULL;

CParticleSystem::CParticleSystem()
{
    m_pActiveList      = NULL; // Head node of point sprites that are active
    m_pFreeList        = NULL; // Head node of point sprites that are inactive and waiting to be recycled.
    m_pPlanes          = NULL;
	m_dwActiveCount    = 0;
	m_fCurrentTime     = 0.0f;
	m_fLastUpdate      = 0.0f;
    m_chTexFile        = NULL;
    m_textureID        = NULL;
    m_dwMaxParticles   = 1;
    m_dwNumToRelease   = 1;
    m_fReleaseInterval = 1.0f;
    m_fLifeCycle       = 1.0f;
    m_fSize            = 1.0f;
    m_clrColor         = CVector3(1.0f,1.0f,1.0f);
    m_vPosition        = CVector3(0.0f,0.0f,0.0f);
    m_vVelocity        = CVector3(0.0f,0.0f,0.0f);
    m_vGravity         = CVector3(0.0f,0.0f,0.0f);
    m_vWind            = CVector3(0.0f,0.0f,0.0f);
    m_bAirResistence   = true;
    m_fVelocityVar     = 1.0f;
    
    SetTexture("Textures/particle.bmp");
}
void CParticleSystem::SetTexture( char *chTexFile )
{
    // Deallocate the memory that was previously reserved for this string.
	if( m_chTexFile != NULL )
	{
		delete [] m_chTexFile;
		m_chTexFile = NULL;
	}
    
    // Dynamically allocate the correct amount of memory.
    m_chTexFile = new char[strlen( chTexFile ) + 1];

    // If the allocation succeeds, copy the initialization string.
    if( m_chTexFile != NULL )
		strcpy( m_chTexFile, chTexFile );
}
GLuint CParticleSystem::GetTextureID()
{
    return m_textureID;
}
//-----------------------------------------------------------------------------
// Name: getRandomMinMax()
// Desc: Gets a random number between min/max boundaries
//-----------------------------------------------------------------------------
float CParticleSystem::getRandomMinMax( float fMin, float fMax )
{
    float fRandNum = (float)rand () / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}

//-----------------------------------------------------------------------------
// Name: getRandomVector()
// Desc: Generates a random vector where X,Y, and Z components are between
//       -1.0 and 1.0
//-----------------------------------------------------------------------------
CVector3 CParticleSystem::getRandomVector( void )
{
	CVector3 vVector;

    // Pick a random Z between -1.0f and 1.0f.
    vVector.z = getRandomMinMax( -1.0f, 1.0f );
    
    // Get radius of this circle
    float radius = (float)sqrt(1 - vVector.z * vVector.z);
    
    // Pick a random point on a circle.
    float t = getRandomMinMax( -OGL_PI, OGL_PI );

    // Compute matching X and Y for our Z.
    vVector.x = (float)cosf(t) * radius;
    vVector.y = (float)sinf(t) * radius;

	return vVector;
}
//-----------------------------------------------------------------------------
// Name : classifyPoint()
// Desc : Classifies a point against the plane passed
//-----------------------------------------------------------------------------
int classifyPoint( CVector3 *vPoint, Plane *pPlane )
{
	CVector3 vDirection = pPlane->m_vPoint - *vPoint;

    float fResult = CVector3::Dot( vDirection,  pPlane->m_vNormal );

	if( fResult < -0.001f )
        return CP_FRONT;

	if( fResult >  0.001f )
        return CP_BACK;

	return CP_ONPLANE;
}

//-----------------------------------------------------------------------------
// Name: SetCollisionPlane()
// Desc: 
//-----------------------------------------------------------------------------
void CParticleSystem::SetCollisionPlane( CVector3 vPlaneNormal, CVector3 vPoint, 
                                         float fBounceFactor, int nCollisionResult )
{
    Plane *pPlane = new Plane;

    pPlane->m_vNormal          = vPlaneNormal;
    pPlane->m_vPoint           = vPoint;
    pPlane->m_fBounceFactor    = fBounceFactor;
    pPlane->m_nCollisionResult = nCollisionResult;

    pPlane->m_pNext = m_pPlanes; // Attach the curent list to it...
    m_pPlanes = pPlane;          // ... and make it the new head.
}

int CParticleSystem::Init(void)
{
	//
	// If the required extensions are present, get the addresses of thier 
	// functions that we wish to use...
	//

    char *ext = (char*)glGetString( GL_EXTENSIONS );

    if( strstr( ext, "GL_ARB_point_parameters" ) == NULL )
    {
        printf("GL_ARB_point_parameters extension was not found\n");
            
        return 0;
    }
    else
    {
        glPointParameterfARB  = (PFNGLPOINTPARAMETERFARBPROC)wglGetProcAddress("glPointParameterfARB");
        glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)wglGetProcAddress("glPointParameterfvARB");

        if( !glPointParameterfARB || !glPointParameterfvARB )
        {
            printf("One or more GL_ARB_point_parameters functions were not found");
            return 0;
        }
    }

	//
	// Load up the point sprite's texture...
	//

	AUX_RGBImageRec *pTextureImage = auxDIBImageLoadA( m_chTexFile );

    if( pTextureImage != NULL )
	{
        glGenTextures( 1, &m_textureID );

		glBindTexture(GL_TEXTURE_2D, m_textureID);

		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexImage2D( GL_TEXTURE_2D, 0, 3, pTextureImage->sizeX, pTextureImage->sizeY, 0,
				     GL_RGB, GL_UNSIGNED_BYTE, pTextureImage->data );
	}

	if( pTextureImage )
	{
		if( pTextureImage->data )
			free( pTextureImage->data );

		free( pTextureImage );
	}

	//
    // If you want to know the max size that a point sprite can be set 
    // to, do this.
	//

    glGetFloatv( GL_POINT_SIZE_MAX_ARB, &m_fMaxPointSize );
    glPointSize( m_fMaxPointSize );

    return 1;
}
//-----------------------------------------------------------------------------
// Name: Update()
// Desc:
//-----------------------------------------------------------------------------
int CParticleSystem::Update( FLOAT fElpasedTime )
{
    Particle  *pParticle;
    Particle **ppParticle;
    Plane     *pPlane;
    Plane    **ppPlane;
    CVector3 vOldPosition;

    m_fCurrentTime += fElpasedTime;     // Update our particle system timer...

    ppParticle = &m_pActiveList; // Start at the head of the active list

    while( *ppParticle )
    {
        pParticle = *ppParticle; // Set a pointer to the head

        // Calculate new position
        float fTimePassed  = m_fCurrentTime - pParticle->m_fInitTime;

        if( fTimePassed >= m_fLifeCycle )
        {
            // Time is up, put the particle back on the free list...
            *ppParticle = pParticle->m_pNext;
            pParticle->m_pNext = m_pFreeList;
            m_pFreeList = pParticle;

            --m_dwActiveCount;
        }
        else
        {
            // Update particle position and velocity

            // Update velocity with respect to Gravity (Constant Accelaration)
            pParticle->m_vCurVel += m_vGravity * fElpasedTime;

            // Update velocity with respect to Wind (Accelaration based on 
            // difference of vectors)
            if( m_bAirResistence == true )
                pParticle->m_vCurVel += (m_vWind - pParticle->m_vCurVel) * fElpasedTime;

            // Finally, update position with respect to velocity
            vOldPosition = pParticle->m_vCurPos;
            pParticle->m_vCurPos += pParticle->m_vCurVel * fElpasedTime;

            //-----------------------------------------------------------------
            // BEGIN Checking the particle against each plane that was set up

            ppPlane = &m_pPlanes; // Set a pointer to the head

            while( *ppPlane )
            {
                pPlane = *ppPlane;
                int result = classifyPoint( &pParticle->m_vCurPos, pPlane );

                if( result == CP_BACK /*|| result == CP_ONPLANE */ )
                {
                    if( pPlane->m_nCollisionResult == CR_BOUNCE )
                    {
                        pParticle->m_vCurPos = vOldPosition;

            //-----------------------------------------------------------------
            //
            // The new velocity vector of a particle that is bouncing off
            // a plane is computed as follows:
            //
            // Vn = (N.V) * N
            // Vt = V - Vn
            // Vp = Vt - Kr * Vn
            //
            // Where:
            // 
            // .  = Dot product operation
            // N  = The normal of the plane from which we bounced
            // V  = Velocity vector prior to bounce
            // Vn = Normal force
            // Kr = The coefficient of restitution ( Ex. 1 = Full Bounce, 
            //      0 = Particle Sticks )
            // Vp = New velocity vector after bounce
            //
            //-----------------------------------------------------------------

                        float Kr = pPlane->m_fBounceFactor;

                        CVector3 Vn = pPlane->m_vNormal * CVector3::Dot( pPlane->m_vNormal, pParticle->m_vCurVel );

                        CVector3 Vt = pParticle->m_vCurVel - Vn;
                        CVector3 Vp = Vt - Vn*Kr;

                        pParticle->m_vCurVel = Vp;
                    }
                    else if( pPlane->m_nCollisionResult == CR_RECYCLE )
                    {
                        pParticle->m_fInitTime -= m_fLifeCycle;
                    }

                    else if( pPlane->m_nCollisionResult == CR_STICK )
                    {
                        pParticle->m_vCurPos = vOldPosition;
                        pParticle->m_vCurVel = CVector3(0.0f,0.0f,0.0f);
                    }
                }

                ppPlane = &pPlane->m_pNext;
            }

            // END Plane Checking
            //-----------------------------------------------------------------

            ppParticle = &pParticle->m_pNext;
        }
    }

    //-------------------------------------------------------------------------
    // Emit new particles in accordance to the flow rate...
    // 
    // NOTE: The system operates with a finite number of particles.
    //       New particles will be created until the max amount has
    //       been reached, after that, only old particles that have
    //       been recycled can be reintialized and used again.
    //-------------------------------------------------------------------------

    if( m_fCurrentTime - m_fLastUpdate > m_fReleaseInterval )
    {
        // Reset update timing...
        m_fLastUpdate = m_fCurrentTime;
    
        // Emit new particles at specified flow rate...
        for( DWORD i = 0; i < m_dwNumToRelease; ++i )
        {
            // Do we have any free particles to put back to work?
            if( m_pFreeList )
            {
                // If so, hand over the first free one to be reused.
                pParticle = m_pFreeList;
                // Then make the next free particle in the list next to go!
                m_pFreeList = pParticle->m_pNext;
            }
            else
            {
                // There are no free particles to recycle...
                // We'll have to create a new one from scratch!
                if( m_dwActiveCount < m_dwMaxParticles )
                {
                    if( NULL == ( pParticle = new Particle ) )
                        return E_OUTOFMEMORY;
                }
            }

            if( m_dwActiveCount < m_dwMaxParticles )
            {
                pParticle->m_pNext = m_pActiveList; // Make it the new head...
                m_pActiveList = pParticle;
                
                // Set the attributes for our new particle...
                pParticle->m_vCurVel = m_vVelocity;

                if( m_fVelocityVar != 0.0f )
                {
                    CVector3 vRandomVec = getRandomVector();
                    pParticle->m_vCurVel += vRandomVec * m_fVelocityVar;
                }

                pParticle->m_fInitTime  = m_fCurrentTime;
                pParticle->m_vCurPos    = m_vPosition;
                
                ++m_dwActiveCount;
            }
        }
    }

	// Query for the max point size supported by the hardware
    float maxSize = 0.0f;
    glGetFloatv( GL_POINT_SIZE_MAX_ARB, &m_fMaxPointSize );
	
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RestartParticleSystem()
// Desc: 
//-----------------------------------------------------------------------------
void CParticleSystem::RestartParticleSystem( void )
{
	Particle  *pParticle;
	Particle **ppParticle;

	ppParticle = &m_pActiveList; // Start at the head of the active list

	while( *ppParticle )
	{
		pParticle = *ppParticle; // Set a pointer to the head

		// Put the particle back on the free list...
		*ppParticle = pParticle->m_pNext;
		pParticle->m_pNext = m_pFreeList;
		m_pFreeList = pParticle;

		--m_dwActiveCount;
	}
}
void CParticleSystem::Render2( void )
{
	glDisable(GL_TEXTURE_2D);
	Particle  *pParticle = m_pActiveList;

    //
	// Set up the OpenGL state machine for using point sprites...
	//

    // This is how will our point sprite's size will be modified by 
    // distance from the viewer.
    float quadratic[] =  { 1.0f, 0.0f, 0.01f };
    glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );

    // The alpha of a point is calculated to allow the fading of points 
    // instead of shrinking them past a defined threshold size. The threshold 
    // is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
    // the minimum and maximum point sizes.
    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f );

    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, m_fMaxPointSize);

    // Specify point sprite texture coordinate replacement mode for each texture unit
    glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );

	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    //
	// Render point sprites...
	//

    glEnable( GL_POINT_SPRITE_ARB );

    //glPointSize( m_fSize );
	
	glLineWidth(3.0f);
	glColor3f(m_clrColor.x,m_clrColor.y,m_clrColor.z);
	//float halfsize = m_fSize / 2.0f;

	glBegin( GL_LINES );
    {
		// Render each particle...
		while( pParticle )
		{
			
			glVertex3f(pParticle->m_vCurPos.x,pParticle->m_vCurPos.y,pParticle->m_vCurPos.z);

			glVertex3f(pParticle->m_vCurPos2.x,pParticle->m_vCurPos2.y,pParticle->m_vCurPos2.z);

			pParticle = pParticle->m_pNext;
		}
    }
	
	glColor3f(1.0f,1.0f,1.0f);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnd();

	glDisable( GL_POINT_SPRITE_ARB );
	glEnable(GL_TEXTURE_2D);
}
int CParticleSystem::Update2( float fElapsedTime )
{
	Particle  *pParticle;
    Particle **ppParticle;
    
    CVector3 vOldPosition;

    m_fCurrentTime += fElapsedTime;     // Update our particle system timer...

    ppParticle = &m_pActiveList; // Start at the head of the active list

    while( *ppParticle )
	{
		pParticle = *ppParticle; // Set a pointer to the head

		float fTimePassed  = m_fCurrentTime - pParticle->m_fInitTime;
		

		
		if (fTimePassed >= m_fLifeCycle) 
		{
			pParticle->m_fInitTime = m_fCurrentTime;

			float theta = DEGTORAD(rand()%180);
			float fi = DEGTORAD(rand()%360);

			CVector3 dir(m_radius * sinf(theta) * cosf(fi),
				m_radius * sinf(theta) * sinf(fi),
				m_radius * cosf(theta));

			dir.Normalize();
			pParticle->m_direction=dir;

			pParticle->m_vCurPos=CVector3(m_radius * sinf(theta) * cosf(fi),
				m_radius * sinf(theta) * sinf(fi),
				m_radius * cosf(theta)) + m_vPosition;

			float t=getRandomMinMax(m_minlength,m_maxlength);
			
			pParticle->m_vCurPos2= pParticle->m_vCurPos + dir * t;
		} else 
		{
			float u=m_fCurrentTime;
			float t=sinf(u);
			pParticle->m_vCurPos += pParticle->m_direction * t;
			pParticle->m_vCurPos2 += pParticle->m_direction * t;
		}
		
		
		ppParticle = &pParticle->m_pNext;
	}
	return 1;
}
int CParticleSystem::Init2( void )
{
	//
	// If the required extensions are present, get the addresses of thier 
	// functions that we wish to use...
	//

    char *ext = (char*)glGetString( GL_EXTENSIONS );

    if( strstr( ext, "GL_ARB_point_parameters" ) == NULL )
    {
        printf("GL_ARB_point_parameters extension was not found\n");
            
        return 0;
    }
    else
    {
        glPointParameterfARB  = (PFNGLPOINTPARAMETERFARBPROC)wglGetProcAddress("glPointParameterfARB");
        glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)wglGetProcAddress("glPointParameterfvARB");

        if( !glPointParameterfARB || !glPointParameterfvARB )
        {
            printf("One or more GL_ARB_point_parameters functions were not found");
            return 0;
        }
    }

	//x = r sen theta cos fi
	//y = r sen theta sen fi
	//z = r cos theta 

	// tetha 0 a OGL_PI
	// fi 0 a 2*OGL_PI
	m_pActiveList=NULL;
	Particle *w;
	for (int i = 0; i<m_dwMaxParticles; ++i)
	{
		Particle *p;
		float theta = rand()%180;
		float fi = rand()%360;

		p=new Particle();
		CVector3 dir(m_radius * sinf(theta) * cosf(fi),
			m_radius * sinf(theta) * sinf(fi),
			m_radius * cosf(theta));

		dir.Normalize();
		p->m_direction=dir;

		p->m_vCurPos=CVector3(m_radius * sinf(theta) * cosf(fi),
			m_radius * sinf(theta) * sinf(fi),
			m_radius * cosf(theta)) + m_vPosition;
		
		float t=getRandomMinMax(m_minlength,m_maxlength);
		p->m_fInitTime=0;
		p->m_vCurPos2= p->m_vCurPos + dir * t;
		if (m_pActiveList==NULL)
		{
			m_pActiveList=p; w=m_pActiveList;
			p->m_pNext=NULL;
		} else {
			w->m_pNext=p;
			p->m_pNext=NULL;
			w=p;
		}
	}	
	
	return 1;
}
//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
void CParticleSystem::Render( void )
{
    Particle  *pParticle = m_pActiveList;

    //
	// Set up the OpenGL state machine for using point sprites...
	//

    // This is how will our point sprite's size will be modified by 
    // distance from the viewer.
    float quadratic[] =  { 1.0f, 0.0f, 0.01f };
    glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );

    // The alpha of a point is calculated to allow the fading of points 
    // instead of shrinking them past a defined threshold size. The threshold 
    // is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
    // the minimum and maximum point sizes.
    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f );

    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, m_fMaxPointSize);

    // Specify point sprite texture coordinate replacement mode for each texture unit
    glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    //
	// Render point sprites...
	//

    glEnable( GL_POINT_SPRITE_ARB );

    //glPointSize( m_fSize );

	float mat[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, mat );
	
	glColor3f(m_clrColor.x,m_clrColor.y,m_clrColor.z);
	float halfsize = m_fSize / 2.0f;

	glBegin( GL_QUADS );
    {
		// Render each particle...
		while( pParticle )
		{
			CVector3 vPoint0;
			CVector3 vPoint1;
			CVector3 vPoint2;
			CVector3 vPoint3;

			CVector3 vRight( mat[0], mat[4], mat[8] );
			CVector3 vUp( mat[1], mat[5], mat[9] );
			CVector3 vCenter( pParticle->m_vCurPos.x, pParticle->m_vCurPos.y, pParticle->m_vCurPos.z);

			// Now, build a quad around the center point based on the vRight 
			// and vUp vectors. This will guarantee that the quad will be 
			// orthogonal to the view.
			vPoint0 = vCenter + ((-vRight - vUp) * m_fSize);
			vPoint1 = vCenter + (( vRight - vUp) * m_fSize);
			vPoint2 = vCenter + (( vRight + vUp) * m_fSize);
			vPoint3 = vCenter + ((-vRight + vUp) * m_fSize);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(vPoint0.x,vPoint0.y,vPoint0.z);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(vPoint1.x, vPoint1.y,vPoint1.z);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(vPoint2.x,vPoint2.y,vPoint2.z);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(vPoint3.x,vPoint3.y,vPoint3.z);

			pParticle = pParticle->m_pNext;
		}
    }
	glEnd();
	glColor3f(1.0f,1.0f,1.0f);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glDisable( GL_POINT_SPRITE_ARB );
}


CParticleSystem::~CParticleSystem(void)
{

}

