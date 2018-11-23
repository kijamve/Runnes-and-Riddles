#pragma once

#include "headers.h" 
//#include "include/gl_texture.h" 
// Creates an ARGB color
#define ARGB(A, R, G, B) ( (int)((A & 0xFF) << 24 | \
								 (R & 0xFF) << 16 | \
								 (G & 0xFF) << 8 | \
								 (B & 0xFF)) )
								 
// Gets the A, R, G, and B from an ARGB color								 
#define GET_A(c) ((c >> 24) & 0xFF)
#define GET_R(c) ((c >> 16) & 0xFF)
#define GET_G(c) ((c >> 8) & 0xFF)
#define GET_B(c) (c & 0xFF)

// Returns a random percent between 0 - 1
#define RAND_PERCENT() ((rand() & 0x7FFF) / ((float)0x7FFF))

// Returns a random number between (and including) "min" and "max"
// Assumes "min" < "max"
#define RAND(min, max) (min + (max - min) * RAND_PERCENT())

// The gravity that effects every particle
// This number was reached by the "what looks good" method %) 								 
const float kParticleGravity = -9.81f / 10.0f;

class Particles
{
public:
	Particles(void);
public:
	~Particles(void);

		// This initializes a particle
		bool init(const CVector3 &pos, 
			const CVector3 &vel, float lifeSpan, float size,
				  float angle = 0.0f, int color = 0xFFFFFFFF, int texName=0);
		
		void process(float dt); // Moves the particle each frame
		void render(); // Renders the particle to screen
		
		// If the a particle's life is greater than zero, it's considered active
		// otherwise it's deactivate
		bool isAlive() { return (life > 0.0f); }
	
		int texture; // The texture	
		

	private:
		CVector3 posIni;
		
		CVector3 pos; // World position
		CVector3 vel; // Velocity
		int color; // ARGB color of particle
		float size; // Width/Height dimensions of the particle
		float life; // Life span of particle in seconds
		float angle; // Angle, in degrees, to rotate the texture's UVs every second
				 
};
