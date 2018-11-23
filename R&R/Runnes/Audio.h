#pragma once

#include "headers.h"
#include "Framework/Framework.h"
	

class Audio
{
public:
	Audio(const int _buffAl,const char* dir);
	void Play();
public:
	~Audio(void);
	ALuint      uiBuffer;
	ALuint      uiSource;  
	ALint       iState;
	char *arch;
	bool load;
	float lastTime;
	int buffAl;
private:

};
