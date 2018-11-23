#include "headers.h"
#include "Particles.h"
class SystemParticle
{
public:
	Particles gParticles[MAX_PARTICLES];
	SystemParticle(int TextureID)
	{
		for(int i = 0; i < MAX_PARTICLES; ++i)
			gParticles[i].init(
			CVector3(70.0f,10.0f,70.0f),	// Posicion
			CVector3( RAND(-0.25f, 0.25f), // Direccion
					  RAND(0.5f, 1.0f),   
					  RAND(-0.25f, 0.25f)
					 ),
			RAND(1.0f, 5.0f),				 // Tiempo de Vida
			1.75f,						     // Tamaño
			30.0f,							 // Angulo
			ARGB(255, 215, 115, 40),		 // Color
			TextureID						 // Textura
			);
	}

	void render(float Time){
		for(int i = 0; i < MAX_PARTICLES; ++i)
		{
			gParticles[i].render();
			gParticles[i].process(Time);
		}
	}

};