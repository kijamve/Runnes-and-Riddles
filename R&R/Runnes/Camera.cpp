#include "Camera.h"
#include "Matrix.h"
#include "stdio.h"

CCamera::CCamera(void)
{
	angleX=0;
	angleY=0;
	currentRotX=lastRotX=0.0f;
	CVector3 vZero = CVector3(0.0, 0.0, 0.0);		// Init a vVector to 0 0 0 for our position
	CVector3 vView = CVector3(0.0, 1.0, 0.5);		// Init a starting view vVector (looking up and out the screen) 
	CVector3 vUp   = CVector3(0.0, 0.0, 1.0);		// Init a standard up vVector (Rarely ever changes)

	center	= vZero;					// Init the position to zero
	eye		= vView;					// Init the view to a std starting view
	up	= vUp;						// Init the UpVector
	m_radius=1.0f;

	box.Max=CVector3(0.5f,1.0f,0.5f);
	box.Min=CVector3(-0.5f,0.0f,-0.5f);
}

CCamera::~CCamera(void)
{
}

void CCamera::Rotate(int dx, int dy)
{	
	// Normalize the strafe vector
	m_vStrafe = CVector3::Cross(eye - center, up);
	m_vStrafe.Normalize();
	
	float angleY = 0.0f;
	float angleZ = 0.0f;
	if( (dx==0) && (dy==0) ) return;
	angleY = (float)( dx ) / 500.0f;		
	angleZ = (float)( dy ) / 500.0f;	

	lastRotX = currentRotX;
	currentRotX += angleZ;
 
	CVector3 vAxis = CVector3::Cross(eye - center, up);
	vAxis.Normalize();

	if(currentRotX > 1.0f)     
	{
		currentRotX = 1.0f;
		if(lastRotX != 1.0f) 
			RotateView( 1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
	}
	else if(currentRotX < -1.0f)
	{
		currentRotX = -1.0f;
		if(lastRotX != -1.0f)
			RotateView( -1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		
	}
	else 
	{	
		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	}
	RotateView(angleY, 0, 1, 0);
}

void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVector3 vNewView;

	// Get the view vector (The direction we are facing)
	CVector3 vView = eye - center;		

	vView.Normalize();

	// Calculate the sine and cosine of the angle once
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);
	//printf("%.2f %.2f\n", cosTheta, sinTheta);

	// Find the new x position for the new rotated point
	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	// Find the new y position for the new rotated point
	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

	// Find the new z position for the new rotated point
	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

	// Now we just add the newly rotated vector to our position to set
	// our new rotated view of our camera.
	eye = center + vNewView;
}
void CCamera::MoveCamera(float speed)
{
	// Get the current view vector (the direction we are looking)
	CVector3 vVector = eye - center;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// I snuck this change in here!  We now normalize our view vector when
	// moving throughout the world.  This is a MUST that needs to be done.
	// That way you don't move faster than you strafe, since the strafe vector
	// is normalized too.
	vVector.Normalize();
	
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	center.x += vVector.x * speed;		// Add our acceleration to our position's X
	center.z += vVector.z * speed;		// Add our acceleration to our position's Z

	box.center=center;
	box.Max=CVector3(1.0f,0,1.0f)+center;
	box.Min=CVector3(-1.0f,-50.0f,-1.0f)+center;
	eye.x += vVector.x * speed;			// Add our acceleration to our view's X
	eye.z += vVector.z * speed;			// Add our acceleration to our view's Z
}

void CCamera::StrafeCamera(float speed)
{
	// Strafing is quite simple if you understand what the cross product is.
	// If you have 2 vectors (say the up vVector and the view vVector) you can
	// use the cross product formula to get a vVector that is 90 degrees from the 2 vectors.
	// For a better explanation on how this works, check out the OpenGL "Normals" tutorial at our site.
	// In our new Update() function, we set the strafing vector (m_vStrafe).  Due
	// to the fact that we need this vector for many things including the strafing
	// movement and camera rotation (up and down), we just calculate it once.
	//
	// Like our MoveCamera() function, we add the strafing vector to our current position 
	// and view.  It's as simple as that.  It has already been calculated in Update().
	
	// Add the strafe vector to our position
	center.x += m_vStrafe.x * speed;
	center.z += m_vStrafe.z * speed;

	box.center=center;
	box.Max=CVector3(1.0f,0,1.0f)+center;
	box.Min=CVector3(-1.0f,-50.0f,-1.0f)+center;

	// Add the strafe vector to our view
	eye.x += m_vStrafe.x * speed;
	eye.z += m_vStrafe.z * speed;
}

void CCamera::Look()
{
		// Give openGL our camera position, then camera view, then camera up vector
		gluLookAt(eye.x, eye.y, eye.z,	
				  center.x,	 center.y,     center.z,	
				  up.x, up.y, up.z);
}

void CCamera::CheckCameraCollision(CVector3 *pVertices, int numOfVerts)
{	
	// This function is pretty much a direct rip off of SpherePolygonCollision()
	// We needed to tweak it a bit though, to handle the collision detection once 
	// it was found, along with checking every triangle in the list if we collided.  
	// pVertices is the world data. If we have space partitioning, we would pass in 
	// the vertices that were closest to the camera. What happens in this function 
	// is that we go through every triangle in the list and check if the camera's 
	// sphere collided with it.  If it did, we don't stop there.  We can have 
	// multiple collisions so it's important to check them all.  One a collision 
	// is found, we calculate the offset to move the sphere off of the collided plane.

	// Go through all the triangles
	for(int i = 0; i < numOfVerts; i += 3)
	{
		// Store of the current triangle we testing
		CVector3 vTriangle[3] = { pVertices[i], pVertices[i+1], pVertices[i+2] };

		// 1) STEP ONE - Finding the sphere's classification
	
		// We want the normal to the current polygon being checked
		CVector3 vNormal = CVector3::Normal(vTriangle);

		// This will store the distance our sphere is from the plane
		float distance = 0.0f;

		// This is where we determine if the sphere is in FRONT, BEHIND, or INTERSECTS the plane
		int classification = CVector3::ClassifySphere(center, vNormal, vTriangle[0], m_radius, distance);

		// If the sphere intersects the polygon's plane, then we need to check further
		if(classification == INTERSECTS) 
		{
			// 2) STEP TWO - Finding the psuedo intersection point on the plane

			// Now we want to project the sphere's center onto the triangle's plane
			CVector3 vOffset = vNormal * distance;

			// Once we have the offset to the plane, we just subtract it from the center
			// of the sphere.  "vIntersection" is now a point that lies on the plane of the triangle.
			CVector3 vIntersection = center - vOffset;

			// 3) STEP THREE - Check if the intersection point is inside the triangles perimeter

			// We first check if our intersection point is inside the triangle, if not,
			// the algorithm goes to step 4 where we check the sphere again the polygon's edges.

			// We do one thing different in the parameters for EdgeSphereCollision though.
			// Since we have a bulky sphere for our camera, it makes it so that we have to 
			// go an extra distance to pass around a corner. This is because the edges of 
			// the polygons are colliding with our peripheral view (the sides of the sphere).  
			// So it looks likes we should be able to go forward, but we are stuck and considered 
			// to be colliding.  To fix this, we just pass in the radius / 2.  Remember, this
			// is only for the check of the polygon's edges.  It just makes it look a bit more
			// realistic when colliding around corners.  Ideally, if we were using bounding box 
			// collision, cylinder or ellipses, this wouldn't really be a problem.

			if(CVector3::InsidePolygon(vIntersection, vTriangle, 3) ||
			   CVector3::EdgeSphereCollision(center, vTriangle, 3, m_radius / 2))
			{
				// If we get here, we have collided!  To handle the collision detection
				// all it takes is to find how far we need to push the sphere back.
				// GetCollisionOffset() returns us that offset according to the normal,
				// radius, and current distance the center of the sphere is from the plane.
				vOffset = CVector3::GetCollisionOffset(vNormal, m_radius, distance);

				// Now that we have the offset, we want to ADD it to the position and
				// view vector in our camera.  This pushes us back off of the plane.  We
				// don't see this happening because we check collision before we render
				// the scene.
				center = center + vOffset;
				eye = eye + vOffset;
			}
		}
	}
}
