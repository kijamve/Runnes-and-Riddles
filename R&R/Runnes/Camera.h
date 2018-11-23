#pragma once
#include "vector.h"
#include "headers.h"
#include "3d.h"

class CCamera
{
public:
	CCamera(void);
	~CCamera(void);

	void normalizeAngle(int *angle);
	void Rotate(int dx, int dy);
	void RotateView(float angle, float x, float y, float z);
	void MoveCamera(float speed);
	void StrafeCamera(float speed);
	float lastRotX;
	float currentRotX;

	CVector3 center;	//Camera's position
	CVector3 up;		//Camera's Up Vector
	CVector3 eye;		//Camera's target
	CVector3 m_vStrafe;
	
	int angleX;
	int angleY;
	int angleZ;
	float m_radius;								
	void Look();

	t3DObject box;	
	
	void PositionCamera(float positionX, float positionY, float positionZ,
				  				 float viewX,     float viewY,     float viewZ,
								 float upVectorX, float upVectorY, float upVectorZ)
	{
		CVector3 vPosition	= CVector3(positionX, positionY, positionZ);
		CVector3 vView		= CVector3(viewX, viewY, viewZ);
		CVector3 vUpVector	= CVector3(upVectorX, upVectorY, upVectorZ);

		// The code above just makes it cleaner to set the variables.
		// Otherwise we would have to set each variable x y and z.

		center = vPosition;					// Assign the position
		eye     = vView;						// Assign the view
		up = vUpVector;					// Assign the up vector
		box.Max=CVector3(0.5f,1.0f,0.5f)+center;
		box.Min=CVector3(-0.5f,0.0f,-0.5f)+center;
		box.center=center;
	}

	void CheckCameraCollision(CVector3 *pVertices, int numOfVerts);
};
