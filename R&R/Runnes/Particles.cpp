#include "Particles.h"

Particles::Particles()
{
	color = ARGB(255, 255, 255, 255);
	size = 0.0f;
	life = 0.0f;
	angle = 0.0f;
	
	// All other variables gets initialized with it's constructor
}

// Init the particle
bool Particles::init(const CVector3 &p, const CVector3 &v, float lifeSpan, float s, float a, int c,
					 int texName)
{
	pos = p; // Set the position
	vel = v; // Set the velocity
	posIni=p;
	// Don't allow initialization of a "dead" particle 
	if(lifeSpan <= 0.0f) 
		return false;
	
	life = lifeSpan; // Set the time, in seconds, the particle will remain alive
	
	// Size must be positive
	if(s <= 0.0f)
		return false;
		
	size = s; // Set the size
	angle = a; // Set the angle of texture UV rotation per second
	color = c; // Set the color
	texture=texName;

		return true;
}

void Particles::process(float dt)
{
	// If particle is dead, reset it's position
	if(isAlive() == false)
	{
		life = RAND(1.0f, 5.0f); // Make it alive again
		pos = posIni;
			return;
	}		

	// Apply velocity
	pos.x += vel.x * dt;
	pos.y += vel.y * dt;
	pos.z += vel.z * dt;
	
	// Apply gravity
	pos.y += kParticleGravity * dt;
		
	life -= dt; // Decrease particle's life span
	
	// Apply rotation by "angle" per second if applicable
	angle = angle + (angle * dt);
}

void Particles::render()
{
	if(isAlive() == false)
		return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_ONE);
	glDepthMask(false);
	glPushMatrix();
		glColor4ub(GET_R(color), GET_G(color), GET_B(color), GET_A(color));
		glTranslatef(pos.x, pos.y, pos.z);
		float halfSize = size * 0.5f;
		glTranslatef(0.5f, 0.5f, 0.0f); // Translate so center of texture is at the texture // coordinate system's origin
		glRotatef(angle, 0.0f, 0.0f, 1.0f); // Rotate around the Z-axis 
		glTranslatef(-0.5f, -0.5f, 0.0f); // Translate back
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-halfSize, halfSize, 0.0f); // Top left vertex
			
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-halfSize, -halfSize, 0.0f); // Bottom left vertex
			
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(halfSize, -halfSize, 0.0f); // Bottom right vertex
			
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(halfSize, halfSize, 0.0f); // Top right vertex
		glEnd();
		
	glPopMatrix();
	
	glDepthMask(true); // Put the Z-buffer back into it's normal "Z-read and Z-write" state

	glDisable(GL_BLEND);
}
	
	
	
		
	
	
	
	
	
	

Particles::~Particles(void)
{
}
