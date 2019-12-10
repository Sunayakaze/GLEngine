#version 450
#pragma optimize(off)
#pragma debug(on)

//	Materials
layout(location = 10) uniform vec3			DiffuseColor;
layout(location = 11) uniform vec3			AmbientColor;
layout(location = 12) uniform vec3			SpecularColor;
layout(location = 13) uniform sampler2D		DiffuseMap;
layout(location = 14) uniform sampler2D		NormalMap;
layout(location = 15) uniform sampler2D		SpecularMap;
layout(location = 19) uniform float			Shininess;

//	Lights
layout(location = 16) uniform vec3			lAmbient;
layout(location = 17) uniform vec3			lDiffuse;
layout(location = 18) uniform vec3			lSpecular;
layout(location = 20) uniform float			lCAttenuation;
layout(location = 21) uniform float			lLAttenuation;
layout(location = 22) uniform float			lQAttenuation;

in VertexData {
	vec3									Position;
	smooth vec3								Normal;
	smooth vec3								Tangents;
	smooth vec3								Bitangents;
	smooth vec2								UV;
	smooth vec3								vLight;
	smooth vec3								vEye;
} vertexIn;

//	Output
layout(location = 0) out vec4				Color0;

void main()
{
	vec3 color					= texture2D(DiffuseMap, vertexIn.UV).rgb;
	vec3 spec					= texture2D(SpecularMap, vertexIn.UV).rgb;
	vec3 normal 				= normalize(texture2D(NormalMap, vertexIn.UV).rgb * 2.0 - 1.0);
	vec3 specular				= vec3(0.0);
	
//	normal = vec3(0.0, 0.0, 1.0);
	
	//- Diffuse calculation ---------------------------------------------------
	
	vec3	light				= normalize(vertexIn.vLight);
	float	arcLight			= max(0.0, dot(normal, light));
	vec3	diffuse				= color * arcLight;
	
	//- Attenuation calculation -----------------------------------------------
	
	float	dist				= length(vertexIn.vLight);
	float	Attenuation			= 1.0 /
								(	lCAttenuation	+
									lLAttenuation	* dist	+
									lQAttenuation	* dist	* dist	);
							
	//- Specular calculation --------------------------------------------------
	
	if(arcLight > 0) {
		vec3 v2e				= normalize(vertexIn.vEye);
		vec3 lrn				= normalize(reflect(-light, normal));
		float specfactor		= dot(lrn, v2e);
		if(specfactor > 0) {
			float SpecPow		= pow(specfactor, Shininess);
			specular			= spec * SpecPow * Attenuation;
		}
	}
	
	//- Fragment color --------------------------------------------------------
	
	vec3 out_color;
	out_color					= vec3(0.0);
	out_color				   += lAmbient * color;
	out_color           	   += lDiffuse * diffuse; 
	out_color           	   += lSpecular * specular;
	
	Color0						= vec4(out_color, 1.0);
	
}
