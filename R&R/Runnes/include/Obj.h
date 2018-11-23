#ifndef _OBJ_H
#define _OBJ_H

#define MINF 1.7e-300
#define INF 1.7e+300

#include "headers.h"
#include "3d.h"

using namespace std;

// This class can be used to load a .Obj file.
class CLoadObj {

public:
	CLoadObj():m_bObjectHasUV(false),m_bJustReadAFace(false){};
	// This will be the only function that needs to be called by you.
	// Just pass in your model that will hold the information and the file name.
	bool ImportObj(t3DModel *pModel, char *strFileName,unsigned int textureId=0,unsigned int mapId=0);

	// This is the main loading loop that gets called in ImportObj()
	void ReadObjFile(t3DModel *pModel,unsigned int textureId=0,unsigned int mapId=0);

	// This is called in ReadObjFile() if we find a line starting with 'v'
	void ReadVertexInfo();

	// This is called in ReadObjFile() if we find a line starting with 'f'
	void ReadFaceInfo();

	// This is called when we are finished loading in the face information
	void FillInObjectInfo(t3DModel *pModel,unsigned int textureId,unsigned int mapId);

	// This isn't necessary for the loader, but it's nice to have vertex normals for lighting
	void ComputeNormals(t3DModel *pModel);

	// Since .obj files don't have texture/material names, we create a function to set them manually.
	// The materialID is the index into the pMaterial array for our model.
	void SetObjectMaterial(t3DModel *pModel, int whichObject, int materialID);

	// To make it easier to assign a material to a .obj object we create a funtion to do so.
	// We can pass in the model, the material name, the texture file name and the RGB colors.
	// If we just want a color, pass in NULL for strFile.
	void AddMaterial(t3DModel *pModel, char *strName, char *strFile, 
					 int r = 255,      int g = 255,   int b = 255);

	//setea el maximo y el minimo para el box
	void SetMaxMin(t3DModel *pModel);
	// This tells us if the current object has texture coordinates
	bool m_bObjectHasUV;

	// This tells us if we just read in face data so we can keep track of multiple objects
	bool m_bJustReadAFace;

	//Calculo de tangente para el NormalMap
	void CalcTangentVector(const CVector3 pos1, const CVector3 pos2,
                       const CVector3 pos3, const CVector2 texCoord1,
                       const CVector2 texCoord2, const CVector2 texCoord3,
                       const CVector3 normal, float tangent[4]);

	void ComputeTang(t3DModel *pModel);

private:

	// This holds our file pointer for reading in the file
	FILE *m_FilePointer;

	//vector that holds a list of vertices
	vector<CVector3>  m_pVertices;

	//vector that holds a list of faces
	vector<tFace> m_pFaces;

	//vector that holds a list of UV Coordinates
	vector<CVector2>  m_pTextureCoords;
};

#endif
