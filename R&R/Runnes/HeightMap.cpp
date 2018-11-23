#include "HeightMap.h"

HeightMap::HeightMap(t3DObject *plane,int err)
{
	minX=plane->Min.x;
	minY=plane->Min.z;
	maxX=plane->Max.x;
	maxY=plane->Max.z;
	dX=maxX-minX;
	dY=maxY-minY;
	for(int i=minX;i<=maxX;++i){
		for(int j=minY;j<=maxY;++j){
			float dist=30000;
			float h;
			for(int v=0;v<plane->numOfVerts;++v){
				float a=i - plane->pVerts[v].x;
				float b=j - plane->pVerts[v].z;
				float d=a*a + b*b;
				if(d<dist){
					dist=d;
					h=plane->pVerts[v].y;
				}
			}
			if(dist>err*err) h=-200;
			mapa[i][j]=h; 
			mapaR[i][j]=rand()%2; 
		}
	}
}
HeightMap::HeightMap(void)
{
	
	
}

HeightMap::~HeightMap(void)
{
}

float HeightMap::Height(float X, float Y)
{
	if(X<minX || X>maxX || Y<minY || Y>maxY) return -200;
	int xMin=int(X);
	int yMin=int(Y);
	int xMax=xMin;
	float d1=X-(float)(xMin);
	float d2=Y-(float)(yMin);

	if(X>0)
		if(d1>0.0f) xMax++;
	if(X<0)
		if(d1<0.0f) xMax--;

	int yMax=yMin;
	if(Y>0)
		if(d2>0.0f) yMax++;
	if(Y<0)
		if(d2<0.0f) yMax--;


	int xA = xMin;					// Error check our x value
	int yA = yMin;					// Error check our y value
	int xB = xMax;					// Error check our x value
	int yB = yMax;					// Error check our y value

	float h1=mapa[xA][yA];
	float h2=mapa[xB][yA];
	float h3=mapa[xA][yB];
	float h4=mapa[xB][yB];

	float p1=fabs(X-(float)(xMin));
	float p2=1.0f-p1;
	float p3=fabs(Y-(float)(yMin));
	float p4=1.0f-p3;
	float hTotal=p2*p4*h1 + p1*p4*h2 + p3*p2*h3 + p1*p3*h4;
	return hTotal;	// Index into our height array and return the height
}
