uniform int activeLight[5];
uniform int fogA;
uniform int text;
uniform sampler2D texture;

varying vec3 V;
varying vec3 L[5];
varying vec3 N;

void main()
{
	vec3 R;
	vec4 amb;
	vec4 diff;
	vec4 spec;
	vec4 sum;
	for(int i=0;i<5;++i){
            if(activeLight[i]==1)
			{
				R=-normalize(reflect(L[i],N));
				amb = gl_FrontLightProduct[i].ambient;
		   		diff= gl_FrontLightProduct[i].diffuse * max(dot(N,L[i]),0.0);
		   		spec= gl_FrontLightProduct[i].specular * pow(max(dot(R,V),0.0),gl_FrontMaterial.shininess);
				sum=sum+amb+diff+spec;
            }
	}
    if(text==1) 
		sum=texture2D(texture,gl_TexCoord[0].st)*sum;
 	float fog = 1.0;
    if(fogA==1)
	{
    	fog = ((gl_Fog.end - gl_FogFragCoord)+0.5) * gl_Fog.scale;
        fog = clamp(fog, 0.0, 1.0);
        if (fog >= 0.0)
        	gl_FragColor = mix(gl_Fog.color,sum, fog);
        else
        	gl_FragColor = gl_Fog.color;
    }else{
    	gl_FragColor=sum;
    }

}
