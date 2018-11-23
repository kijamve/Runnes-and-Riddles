#pragma once
#include "headers.h"
#include "3d.h"
#include<map>
class HeightMap
{
public:
	map< int , map < int , float > > mapa;
	map< int , map < int , float > > mapaR;
	HeightMap(t3DObject *plane,int err);
	float Height(float X,float Y);
	HeightMap();
	int minX,minY;
	int maxX,maxY;

	int dX,dY;
public:
	~HeightMap(void);
};
