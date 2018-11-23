#pragma once

#include "headers.h"

using namespace audiere;
 


class Audir
{
public:
	bool loop;
	AudioDevicePtr device;
	OutputStreamPtr sound;
	float vol;
	float lastTime;
	Audir(const char* _dir,bool _loop=false,float _vol=1.0f)
	{
		loop=_loop;
		vol=_vol;
		device=AudioDevicePtr(OpenDevice());
		sound=OutputStreamPtr(OpenSound(device, _dir, false));


	}
	void Play()
	{
		sound->play();
		sound->setRepeat(loop);
		sound->setVolume(vol);
	}
	void Stop(){
		sound->stop();
	}
};