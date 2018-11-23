//#include "main.h"
#include "Obj.h"

//This function loads a .obj file into a specified model by a .obj file name

bool closeEnough(float f1, float f2)
{
    // Determines whether the two floating-point values f1 and f2 are
    // close enough together that they can be considered equal.

    return fabsf((f1 - f2) / ((f2 == 0.0f) ? 1.0f : f2)) < 1e-6f;
}

bool CLoadObj::ImportObj(t3DModel *pModel, char *strFileName,unsigned int textureId,unsigned int mapId)
{
	char strMessage[255] = {0};				// This will be used for error messages

	// Make sure we have a valid model and file name
	if(!pModel || !strFileName) return false;

	// Here we open the desired file for read only and return the file pointer
	m_FilePointer = fopen(strFileName, "r");

	// Check to make sure we have a valid file pointer
	if(!m_FilePointer) {
		// Create an error message for the attempted file
		printf("Unable to find or open the file: %s", strFileName);
		return false;
	}

	// Now that we have a valid file and it's open, let's read in the info!
	ReadObjFile(pModel,textureId,mapId);

	// Now that we have the file read in, let's compute the vertex normals for lighting
	ComputeNormals(pModel);

	//Calcular las tangentes
	ComputeTang(pModel);


	// Close the .obj file that we opened
	fclose(m_FilePointer);

	// Return a success!
	return true;
}


//This function is the main loop for reading in the .obj file
void CLoadObj::ReadObjFile(t3DModel *pModel,unsigned int textureId,unsigned int mapId)
{
	char strLine[255]		= {0};
	char ch					= 0;

	while(!feof(m_FilePointer))
	{
		float x = 0.0f, y = 0.0f, z = 0.0f;

		// Get the beginning character of the current line in the .obj file
		ch = fgetc(m_FilePointer);

		switch(ch)
		{
		case 'v':						// Check if we just read in a 'v' (Could be a vertice/normal/textureCoord)
			
			// If we just read in a face line, then we have gone to another object,
			// so we need to save the last object's data before moving onto the next one.
			if(m_bJustReadAFace) {
				// Save the last object's info into our model structure
				FillInObjectInfo(pModel,textureId,mapId);
			}

			// Decipher this line to see if it's a vertex ("v"), normal ("vn"), or UV coordinate ("vt")
			ReadVertexInfo();
			break;

		case 'f':						// Check if we just read in a face header ('f')
			
			// If we get here we then we need to read in the face information.
			// The face line holds the vertex indices into our vertex array, or if
			// the object has texture coordinates then it holds those too.
			ReadFaceInfo();
			break;

		case '\n':

			// If we read in a newline character, we've encountered a blank line in
			// the .obj file.  We don't want to do the default case and skip another
			// line, so we just break and do nothing.
			break;

		default:

			// If we get here then we don't care about the line being read, so read past it.
			fgets(strLine, 100, m_FilePointer);
			break;
		}
	}

	// Now that we are done reading in the file, we have need to save the last object read.
	FillInObjectInfo(pModel,textureId,mapId);
}


//	This function reads in the vertex information ("v" vertex : "vt" UVCoord)
void CLoadObj::ReadVertexInfo()
{
	CVector3 vNewVertex;
	CVector2 vNewTexCoord;
	char strLine[255]		= {0};
	char ch = 0;

	// Read the next character in the file to see if it's a vertice/normal/UVCoord
	ch = fgetc(m_FilePointer);

	if(ch == ' ')				// If we get a space it must have been a vertex ("v")
	{
		// Here we read in a vertice.  The format is "v x y z"
		fscanf(m_FilePointer, "%f %f %f", &vNewVertex.x, &vNewVertex.y, &vNewVertex.z);

		// Read the rest of the line so the file pointer returns to the next line.
		fgets(strLine, 100, m_FilePointer);

		// Add a new vertice to our list
		m_pVertices.push_back(vNewVertex);
	}
	else if(ch == 't')			// If we get a 't' then it must be a texture coordinate ("vt")
	{
		// Here we read in a texture coordinate.  The format is "vt u v"
		fscanf(m_FilePointer, "%f %f", &vNewTexCoord.x, &vNewTexCoord.y);

		// Read the rest of the line so the file pointer returns to the next line.
		fgets(strLine, 100, m_FilePointer);

		// Add a new texture coordinate to our list
		m_pTextureCoords.push_back(vNewTexCoord);

		// Set the flag that tells us this object has texture coordinates.
		// Now we know that the face information will list the vertice AND UV index.
		// For example, ("f 1 3 2" verses "f 1/1 2/2 3/3")
		m_bObjectHasUV = true;
	}
	else						// Otherwise it's probably a normal so we don't care ("vn")
	{
		// We calculate our own normals at the end so we read past them.
		fgets(strLine, 100, m_FilePointer);
	}
}

//	This function reads in the face information ("f")
void CLoadObj::ReadFaceInfo()
{
	tFace newFace			= {0};
	char strLine[255]		= {0};

	// This function reads in the face information of the object.
	// A face is a polygon (a triangle in this case) that has information about it.
	// It has the 3D points that make up the polygon and may also have texture coordinates.
	// When reading in an .obj, objects don't have to have UV texture coordinates so we
	// need to read in the face information differently in that case.  If the object does have
	// UV coordinates, then the format will look like this:
	// "f vertexIndex1/coordIndex1 vertexIndex2/coordIndex2 vertexIndex3/coordIndex3"
	// otherwise the format will look like this:"f vertexIndex1 vertexIndex2 vertexIndex3"
	// The index values index into our vertice and texture coordinate arrays.  More explained in RenderScene().
	// *Note* Make sure if you cut this code out for your own use you minus 1 from the indices.
	// This is because arrays are zero based and the .obj indices start at 1.  Look at FillInObjectInfo().

	// Check if this object has texture coordinates before reading in the values
	if(m_bObjectHasUV )
	{
		// Here we read in the object's vertex and texture coordinate indices.
		// Here is the format: "f vertexIndex1/coordIndex1 vertexIndex2/coordIndex2 vertexIndex3/coordIndex3"
		fscanf(m_FilePointer, "%d/%d %d/%d %d/%d", &newFace.vertIndex[0], &newFace.coordIndex[0],
												   &newFace.vertIndex[1], &newFace.coordIndex[1],
												   &newFace.vertIndex[2], &newFace.coordIndex[2]);				
	}
	else										// The object does NOT have texture coordinates
	{
		// Here we read in just the object's vertex indices.
		// Here is the format: "f vertexIndex1 vertexIndex2 vertexIndex3"
		fscanf(m_FilePointer, "%d %d %d", &newFace.vertIndex[0],
										  &newFace.vertIndex[1],
										  &newFace.vertIndex[2]);				
	}

	// Read the rest of the line so the file pointer returns to the next line.
	fgets(strLine, 100, m_FilePointer);
				
	// Add the new face to our face list
	m_pFaces.push_back(newFace);

	// We set this flag to TRUE so we know that we just read in some face information.
	// Since face information is the last thing we read in for an object we will check
	// this flag when we read in a vertice.  If it's true, then we just finished reading
	// in an object and we need to save off the information before going to the next object.
	// Since there is no standard header for objects in a .obj file we do it this way.
	m_bJustReadAFace = true;
}

//	This function is called after an object is read in to fill in the model structure
void CLoadObj::FillInObjectInfo(t3DModel *pModel,unsigned int textureId,unsigned int mapId)
{
	t3DObject newObject = {0};
	int textureOffset = 0, vertexOffset = 0;
	int i = 0;
	
	// If we get here then we just finished reading in an object
	// and need to increase the object count.
	pModel->numOfObjects++;

	// Add a new object to the list of objects in our model
	pModel->pObject.push_back(newObject);

	// Get a pointer to the current object so our code is easier to read
	t3DObject *pObject = &(pModel->pObject[pModel->numOfObjects - 1]);

	// Now that we have our list's full of information, we can get the size
	// of these lists by calling size() from our vectors.  That is one of the
	// wonderful things that the Standard Template Library offers us.  Now you
	// never need to write a link list or constantly call malloc()/new.

	// Here we get the number of faces, vertices and texture coordinates
	pObject->numOfFaces   = m_pFaces.size();
	pObject->numOfVerts   = m_pVertices.size();
	pObject->numTexVertex = m_pTextureCoords.size();

	// If we read in any faces for this object (required)
	if(pObject->numOfFaces) {

		// Allocate enough memory to store all the faces in our object
		pObject->pFaces = new tFace [pObject->numOfFaces];
	}

	// If we read in any vertices for this object (required)
	if(pObject->numOfVerts) {

		// Allocate enough memory to store all the vertices in our object
		pObject->pVerts = new CVector3 [pObject->numOfVerts];
	}	

	// If we read in any texture coordinates for this object (optional)
	if(pObject->numTexVertex) {
		pObject->pTexVerts = new CVector2 [pObject->numTexVertex];
		pObject->bHasTexture = true;
	}	
	pObject->materialID = textureId;
	pObject->normalID = mapId;

	// Go through all of the faces in the object
	for(i = 0; i < pObject->numOfFaces; i++)
	{
		// Copy the current face from the temporary list to our Model list
		pObject->pFaces[i] = m_pFaces[i];

		// Because of the problem with .obj files not being very object friendly,
		// if a new object is found in the file, the face and texture indices start
		// from the last index that was used in the last object.  That means that if
		// the last one was 8, it would then go to 9 for the next object.  We need to
		// bring that back down to 1, so we just create an offset that we subtract from
		// the vertex and UV indices.

		// Check if this is the first face of the object
		if(i == 0) 
		{
			// If the first index is NOT 1, then we must be past the first object
			if(pObject->pFaces[0].vertIndex[0] != 1) {

				// To create the offset we take the current starting point and then minus 1.
				// Lets say the last object ended at 8.  Well we would then have 9 here.
				// We want to then subtract 8 from the 9 to get back to 1.
				vertexOffset = pObject->pFaces[0].vertIndex[0] - 1;

				// The same goes for texture coordinates, if we have them do the same
				if(pObject->numTexVertex > 0) {

					// Take the current index and minus 1 from it
					textureOffset = pObject->pFaces[0].coordIndex[0] - 1;
				}
			}					
		}

		// Because the face indices start at 1, we need to minus 1 from them due
		// to arrays being zero based.  This is VERY important!
		for(int j = 0; j < 3; j++)
		{
			// For each index, minus 1 to conform with zero based arrays.
			// We also need to add the vertex and texture offsets to subtract
			// the total amount necessary for this to work.  The first object
			// will have a offset of 0 for both since it starts at 1.
			pObject->pFaces[i].vertIndex[j]  -= 1 + vertexOffset;
			pObject->pFaces[i].coordIndex[j] -= 1 + textureOffset;
		}
	}

	CVector3 aux_max(-30000,-30000,-30000),aux_min(30000,30000,30000);
	//bool no_vacia = 0 < pObject->numOfVerts;
	// Go through all the vertices in the object
	for(i = 0; i < pObject->numOfVerts; i++)
	{
		// Copy the current vertice from the temporary list to our Model list
		pObject->pVerts[i] = m_pVertices[i];
		
		//obtengo el los vectores maximos y minimo para poder armar el box
		//if(no_vacia)
		//{
			//no_vacia = false;
			//aux_max = m_pVertices[0];
			//aux_min = m_pVertices[0];
		//}else{
			if(aux_max.x < m_pVertices[i].x) aux_max.x = m_pVertices[i].x;
			if(aux_max.y < m_pVertices[i].y) aux_max.y = m_pVertices[i].y;
			if(aux_max.z < m_pVertices[i].z) aux_max.z = m_pVertices[i].z;

			if(aux_min.x > m_pVertices[i].x) aux_min.x = m_pVertices[i].x;
			if(aux_min.y > m_pVertices[i].y) aux_min.y = m_pVertices[i].y;
			if(aux_min.z > m_pVertices[i].z) aux_min.z = m_pVertices[i].z;
		//}
	}
	//asigno el max y min al obj
	pObject->Max = aux_max;
	pObject->Min = aux_min;
	pObject->center = CVector3((aux_max.x + aux_min.x)/2.0f,(aux_max.y + aux_min.y)/2.0f,(aux_max.z + aux_min.z)/2.0f);
	pObject->radio = sqrt( (aux_max.x-aux_min.x)*(aux_max.x-aux_min.x) + (aux_max.y-aux_min.y)*(aux_max.y-aux_min.y) + (aux_max.z-aux_min.z)*(aux_max.z-aux_min.z))/2.0f;

	
	// Go through all of the texture coordinates in the object (if any)
	for(i = 0; i < pObject->numTexVertex; i++)
	{
		// Copy the current UV coordinate from the temporary list to our Model list
		pObject->pTexVerts[i] = m_pTextureCoords[i];
	}

	// Since .OBJ files don't have materials, we set the material ID to -1.
	// We need to manually give it a material using AddMaterial().
	//pObject->materialID = -1;

	// Now that we have all the information from out list's, we need to clear them
	// so we can be ready for the next object that we read in.
	m_pVertices.clear();
	m_pFaces.clear();
	m_pTextureCoords.clear();

	// Reset these booleans to be prepared for the next object
	m_bObjectHasUV   = false;
	m_bJustReadAFace = false;
}

//	This function assigns a material to a specific object in our array of objects
void CLoadObj::SetObjectMaterial(t3DModel *pModel, int whichObject, int materialID)
{
	// Make sure we have a valid model or else quit the function
	if(!pModel) return;

	// Make sure we don't index over the array of objects
	if(whichObject >= pModel->numOfObjects) return;

	// Here we set the material ID assigned to this object
	pModel->pObject[whichObject].materialID = materialID;
}


//	This function adds a material to our model manually since .obj has no such info
void CLoadObj::AddMaterial(t3DModel *pModel, char *strName, char *strFile, 
									int r,			  int g,		 int b)
{
	tMaterialInfo newMaterial = {0};

	// Set the RGB value for this material [0 - RED		1 - GREEN	2 - BLUE]
	newMaterial.color[0] = r; newMaterial.color[1] = g; newMaterial.color[2] = b;

	// If we have a file name passed in, let's copy it to our material structure
	if(strFile) {
		strcpy(newMaterial.strFile, strFile);
	}

	// If we have a material name passed in, let's copy it to our material structure
	if(strName) {
		strcpy(newMaterial.strName, strName);
	}

	// Now we add this material to model's list.  Once again we use the incredibly
	// helpfull STL vector functions for allocating dynamic memory.
	pModel->pMaterials.push_back(newMaterial);

	// Increase the material count
	pModel->numOfMaterials++;
}


// Below are some math functions for calculating vertex normals.  We want vertex normals
// because it makes the lighting look really smooth and life like.  You probably already
// have these functions in the rest of your engine, so you can delete these and call
// your own.  I wanted to add them so I could show how to calculate vertex normals.

//////////////////////////////	Math Functions  ////////////////////////////////*

// This computes the magnitude of a normal.   (magnitude = sqrt(x^2 + y^2 + z^2)
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

// This calculates a vector between 2 points and returns the result
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{
	CVector3 vVector;							// The variable to hold the resultant vector

	vVector.x = vPoint1.x - vPoint2.x;			// Subtract point1 and point2 x's
	vVector.y = vPoint1.y - vPoint2.y;			// Subtract point1 and point2 y's
	vVector.z = vPoint1.z - vPoint2.z;			// Subtract point1 and point2 z's

	return vVector;								// Return the resultant vector
}

// This adds 2 vectors together and returns the result
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vResult;							// The variable to hold the resultant vector
	
	vResult.x = vVector2.x + vVector1.x;		// Add Vector1 and Vector2 x's
	vResult.y = vVector2.y + vVector1.y;		// Add Vector1 and Vector2 y's
	vResult.z = vVector2.z + vVector1.z;		// Add Vector1 and Vector2 z's

	return vResult;								// Return the resultant vector
}

// This divides a vector by a single number (scalar) and returns the result
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler)
{
	CVector3 vResult;							// The variable to hold the resultant vector
	
	vResult.x = vVector1.x / Scaler;			// Divide Vector1's x value by the scaler
	vResult.y = vVector1.y / Scaler;			// Divide Vector1's y value by the scaler
	vResult.z = vVector1.z / Scaler;			// Divide Vector1's z value by the scaler

	return vResult;								// Return the resultant vector
}

// This returns the cross product between 2 vectors
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vCross;								// The vector to hold the cross product

												// Get the X value
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
												// Get the Y value
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
												// Get the Z value
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vCross;								// Return the cross product
}

// This returns the normal of a vector
CVector3 Normalize(CVector3 vNormal)
{
	double Magnitude;							// This holds the magnitude			

	Magnitude = Mag(vNormal);					// Get the magnitude

	vNormal.x /= (float)Magnitude;				// Divide the vector's X by the magnitude
	vNormal.y /= (float)Magnitude;				// Divide the vector's Y by the magnitude
	vNormal.z /= (float)Magnitude;				// Divide the vector's Z by the magnitude

	return vNormal;								// Return the normal
}

//	This function computes the normals and vertex normals of the objects
void CLoadObj::ComputeNormals(t3DModel *pModel)
{
	CVector3 vVector1, vVector2, vNormal, vPoly[3];

	// If there are no objects, we can skip this part
	if(pModel->numOfObjects <= 0)
		return;

	// What are vertex normals?  And how are they different from other normals?
	// Well, if you find the normal to a triangle, you are finding a "Face Normal".
	// If you give OpenGL a face normal for lighting, it will make your object look
	// really flat and not very round.  If we find the normal for each vertex, it makes
	// the smooth lighting look.  This also covers up blocky looking objects and they appear
	// to have more polygons than they do.    Basically, what you do is first
	// calculate the face normals, then you take the average of all the normals around each
	// vertex.  It's just averaging.  That way you get a better approximation for that vertex.

	// Go through each of the objects to calculate their normals
	for(int index = 0; index < pModel->numOfObjects; index++)
	{
		// Get the current object
		t3DObject *pObject = &(pModel->pObject[index]);

		// Here we allocate all the memory we need to calculate the normals
		CVector3 *pNormals		= new CVector3 [pObject->numOfFaces];
		CVector3 *pTempNormals	= new CVector3 [pObject->numOfFaces];
		pObject->pNormals		= new CVector3 [pObject->numOfVerts];

		// Go though all of the faces of this object
		for(int i=0; i < pObject->numOfFaces; i++)
		{												
			// To cut down LARGE code, we extract the 3 points of this face
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

			// Now let's calculate the face normals (Get 2 vectors and find the cross product of those 2)

			vVector1 = Vector(vPoly[0], vPoly[2]);		// Get the vector of the polygon (we just need 2 sides for the normal)
			vVector2 = Vector(vPoly[2], vPoly[1]);		// Get a second vector of the polygon

			vNormal  = Cross(vVector1, vVector2);		// Return the cross product of the 2 vectors (normalize vector, but not a unit vector)
			pTempNormals[i] = vNormal;					// Save the un-normalized normal for the vertex normals
			vNormal  = Normalize(vNormal);				// Normalize the cross product to give us the polygons normal

			pNormals[i] = vNormal;						// Assign the normal to the list of normals
		}
		pObject->pNormalsFaces=pNormals;
		//////////////// Now Get The Vertex Normals /////////////////

		CVector3 vSum;
		CVector3 vZero = vSum;
		int shared=0;

		for (int i = 0; i < pObject->numOfVerts; i++)			// Go through all of the vertices
		{
			for (int j = 0; j < pObject->numOfFaces; j++)	// Go through all of the triangles
			{												// Check if the vertex is shared by another face
				if (pObject->pFaces[j].vertIndex[0] == i || 
					pObject->pFaces[j].vertIndex[1] == i || 
					pObject->pFaces[j].vertIndex[2] == i)
				{
					vSum = AddVector(vSum, pTempNormals[j]);// Add the un-normalized normal of the shared face
					shared++;								// Increase the number of shared triangles
				}
			}      
			
			// Get the normal by dividing the sum by the shared.  We negate the shared so it has the normals pointing out.
			pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));

			// Normalize the normal for the final vertex normal
			pObject->pNormals[i] = Normalize(pObject->pNormals[i]);	

			vSum = vZero;									// Reset the sum
			shared = 0;										// Reset the shared
		}
	
		// Free our memory and start over on the next object
		delete [] pTempNormals;
	}
}

void CLoadObj::ComputeTang(t3DModel *pModel)
{
	CVector3 vVector1, vVector2, vNormal, vPoly[3];
	CVector2 vCoord[3];
	// If there are no objects, we can skip this part
	if(pModel->numOfObjects <= 0)
		return;

		float tangent[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	for(int index = 0; index < pModel->numOfObjects; index++)
	{
		// Get the current object
		t3DObject *pObject = &(pModel->pObject[index]);

		pObject->pTang		= new CVector4 [pObject->numOfVerts];

		// Go though all of the faces of this object
		for(int i=0; i < pObject->numOfFaces; i++)
		{												
			// To cut down LARGE code, we extract the 3 points of this face
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];
			vCoord[0]=pObject->pTexVerts[pObject->pFaces[i].coordIndex[0]];
			vCoord[1]=pObject->pTexVerts[pObject->pFaces[i].coordIndex[1]];
			vCoord[2]=pObject->pTexVerts[pObject->pFaces[i].coordIndex[2]];


			CalcTangentVector(
            vPoly[0], vPoly[1], vPoly[2],
            vCoord[0], vCoord[1], vCoord[2],
            pObject->pNormalsFaces[i], tangent);

			pObject->pTang[pObject->pFaces[i].vertIndex[0]]=CVector4(tangent[0],tangent[1],tangent[2],tangent[3]);
			pObject->pTang[pObject->pFaces[i].vertIndex[1]]=CVector4(tangent[0],tangent[1],tangent[2],tangent[3]);
			pObject->pTang[pObject->pFaces[i].vertIndex[2]]=CVector4(tangent[0],tangent[1],tangent[2],tangent[3]);

		}
	}

}



//setea el maximo y el minimo para el box
void SetMaxMin(t3DModel *pModel)
{
	//if(pModel->pObject.empty()) return;
	//CVector3 *aux_vertices;

	//for(int i = 0; i < pModel->pObject.size() ; i++ )
	//{
		//aux_vertices = pModel->pObject[i].pVerts;

		//for(int j = 0 ; j < pModel->pObject[i].pVerts->size() ; j++ )
		//{
		
		//}
	//}
	//pModel->pObject[]
}
/////////////////////////////////////////////////////////////////////////////////
// First, no .obj file format is going to be the same.  When you import/export
// .obj files anywhere, each application has their own way of saving it.  Some
// save normals, some save extra comments, some save object names, etc...  That is
// why I calculate my own normals because rarely are they including as "vn" in the file.
// The only thing you can depend on is the headers:
//
// "v"  - This is a line that contains a vertex (x y z)
//
//		IE:			v	-1	-1	0
//
// After the 'v' it will list the X Y and Z information for that vertex.
//
// "vt" - This is a line that contains a UV texture coordinate (U V)
//
//		IE:			vt	.99998	.99936
//
// After the "vt" it will list the U and V information for a vertex.
// Note that this will ONLY appear in the file if the object has a UVW map.
// Just dragging a texture onto an object doesn't create a UVW map.
//
// "f"	- This is a line that contains the vertex indices into the vertex array.
//        If there are UV coordinates for that object, it also contains the UV indices.
//
//		IE (Just vertices):			f	1	2	3
//		
//		IE (Verts and UV Indices)	f	1/1 2/2	3/3
//
// After the f it will list the vertice indices, or the vertex / UV indices.
//
// Those are the only 3 you can count on 99% of the time.  The other one that
// isn't always there is "vn".  That is a vertex normal:  vn -1 0 0
// There are some other lines that are sometimes used by they can be generally ignored.
//
// Once again, I personally would not use this format for anything I do because it's
// huge in file size and doesn't contain much information about the model, but I would
// recommend that you start here, and then add your own information to the file and
// modify this tutorial to read it in.  Eventually jump on board with .3DS because it
// has key frame information and such for animation, where .obj does NOT.
//
// I will eventually add this to a 3D Loading Library along with the other formats.
// Let us know if this helps you out!

void CLoadObj::CalcTangentVector(const CVector3 pos1, const CVector3 pos2,
                       const CVector3 pos3, const CVector2 texCoord1,
                       const CVector2 texCoord2, const CVector2 texCoord3,
                       const CVector3 normal, float tangent[4])
{
    // Given the 3 vertices (position and texture coordinates) of a triangle
    // calculate and return the triangle's tangent vector.

    // Create 2 vectors in object space.
    //
    // edge1 is the vector from vertex positions pos1 to pos2.
    // edge2 is the vector from vertex positions pos1 to pos3.
    CVector3 edge1(pos2.x - pos1.x, pos2.y - pos1.y, pos2.z - pos1.z);
    CVector3 edge2(pos3.x - pos1.x, pos3.y - pos1.y, pos3.z - pos1.z);

    edge1.Normalize();
    edge2.Normalize();

    // Create 2 vectors in tangent (texture) space that point in the same
    // direction as edge1 and edge2 (in object space).
    //
    // texEdge1 is the vector from texture coordinates texCoord1 to texCoord2.
    // texEdge2 is the vector from texture coordinates texCoord1 to texCoord3.
    CVector2 texEdge1(texCoord2.x - texCoord1.x, texCoord2.y - texCoord1.y);
    CVector2 texEdge2(texCoord3.x - texCoord1.x, texCoord3.y - texCoord1.y);

    texEdge1.Normalize();
    texEdge2.Normalize();

    // These 2 sets of vectors form the following system of equations:
    //
    //  edge1 = (texEdge1.x * tangent) + (texEdge1.y * bitangent)
    //  edge2 = (texEdge2.x * tangent) + (texEdge2.y * bitangent)
    //
    // Using matrix notation this system looks like:
    //
    //  [ edge1 ]     [ texEdge1.x  texEdge1.y ]  [ tangent   ]
    //  [       ]  =  [                        ]  [           ]
    //  [ edge2 ]     [ texEdge2.x  texEdge2.y ]  [ bitangent ]
    //
    // The solution is:
    //
    //  [ tangent   ]        1     [ texEdge2.y  -texEdge1.y ]  [ edge1 ]
    //  [           ]  =  -------  [                         ]  [       ]
    //  [ bitangent ]      det A   [-texEdge2.x   texEdge1.x ]  [ edge2 ]
    //
    //  where:
    //        [ texEdge1.x  texEdge1.y ]
    //    A = [                        ]
    //        [ texEdge2.x  texEdge2.y ]
    //
    //    det A = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x)
    //
    // From this solution the tangent space basis vectors are:
    //
    //    tangent = (1 / det A) * ( texEdge2.y * edge1 - texEdge1.y * edge2)
    //  bitangent = (1 / det A) * (-texEdge2.x * edge1 + texEdge1.x * edge2)
    //     normal = cross(tangent, bitangent)

    CVector3 t;
    CVector3 b;
    CVector3 n(normal.x, normal.y, normal.z);

    float det = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x);

    if (closeEnough(det, 0.0f))
    {
        t.set(1.0f, 0.0f, 0.0f);
        b.set(0.0f, 1.0f, 0.0f);
    }
    else
    {
        det = 1.0f / det;

        t.x = (texEdge2.y * edge1.x - texEdge1.y * edge2.x) * det;
        t.y = (texEdge2.y * edge1.y - texEdge1.y * edge2.y) * det;
        t.z = (texEdge2.y * edge1.z - texEdge1.y * edge2.z) * det;

        b.x = (-texEdge2.x * edge1.x + texEdge1.x * edge2.x) * det;
        b.y = (-texEdge2.x * edge1.y + texEdge1.x * edge2.y) * det;
        b.z = (-texEdge2.x * edge1.z + texEdge1.x * edge2.z) * det;

        t.Normalize();
        b.Normalize();
    }

    // Calculate the handedness of the local tangent space.
    // The bitangent vector is the cross product between the triangle face
    // normal vector and the calculated tangent vector. The resulting bitangent
    // vector should be the same as the bitangent vector calculated from the
    // set of linear equations above. If they point in different directions
    // then we need to invert the cross product calculated bitangent vector. We
    // store this scalar multiplier in the tangent vector's 'w' component so
    // that the correct bitangent vector can be generated in the normal mapping
    // shader's vertex shader.

    CVector3 bitangent = CVector3::Cross(n, t);
    float handedness = (CVector3::Dot(bitangent, b) < 0.0f) ? -1.0f : 1.0f;

    tangent[0] = t.x;
    tangent[1] = t.y;
    tangent[2] = t.z;
    tangent[3] = handedness;
}
