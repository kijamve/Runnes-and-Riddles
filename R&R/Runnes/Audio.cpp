#include "Audio.h"

Audio::Audio(const int _buffAl,const char* dir): buffAl(_buffAl)
{
	load=false;
	ALFWInit();
	// Initialize Framework
	if (!ALFWInitOpenAL())
	{
		if (!ALFWInitOpenAL())
		{
			ALFWprintf("Failed to initialize OpenAL\n");
			ALFWShutdown();
			return;
		}
	}
	// Generate an AL Buffer
	alGenBuffers(buffAl, &uiBuffer );
	if (!ALFWLoadWaveToBuffer((char*)ALFWaddMediaPath(dir), uiBuffer))
	{
		ALFWprintf("Failed to load %s\n", ALFWaddMediaPath(dir));
		return;
	}
	load=true;
	lastTime = 0;

}
Audio::~Audio(void)
{
	alSourceStop(uiSource);
    alDeleteSources(buffAl, &uiSource);
	alDeleteBuffers(buffAl, &uiBuffer);
	ALFWShutdownOpenAL();
	ALFWShutdown();
}
void Audio::Play(){	
	if(load){
		// Generate a Source to playback the Buffer
		alGenSources( buffAl, &uiSource );

		// Attach Source to Buffer
		alSourcei( uiSource, AL_BUFFER, uiBuffer );

		// Play Source
		alSourcePlay( uiSource );
		ALFWprintf("Play: %d\n", buffAl);
	}
}