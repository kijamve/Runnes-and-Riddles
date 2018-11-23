uniform float time;
uniform float r1;
uniform float r2;
uniform float r22;
uniform float r3;

uniform int activeLight[5];
uniform int fogA;
uniform int text;

varying vec3 V;
varying vec3 L[5];
varying vec3 N;


void main()
{
	vec4 vertex = vec4(gl_Vertex);
	
	float rnd = cos(time/5);
	
	vertex.x =   (0.25f * ((vertex.y-r1)/3) * ((vertex.y-r1)/3) * abs(rnd * rnd + (vertex.z-r3)/5)) + vertex.x;  
	//if(vertex.x<r2) vertex.x=r2;
	//if(vertex.x>r22) vertex.x=r22;
	vec3 vertexPos = vec3(gl_ModelViewMatrix * vertex);
	
    V = -normalize(vertexPos);
    
	for(int i=0;i<5;++i)
	{
   		if(activeLight[i]==1) 
    		L[i] = normalize(vec3(gl_LightSource[i].position) - vertexPos);
    }
	
    N = (gl_NormalMatrix * gl_Normal);
	
    gl_Position = gl_ModelViewProjectionMatrix * vertex;
	
    gl_FogFragCoord = gl_Position.z;
	
    gl_TexCoord[0] = gl_MultiTexCoord0;
	
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}