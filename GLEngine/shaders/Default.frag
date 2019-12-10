#version 450 core
#pragma optimize(off)
#pragma debug(on)

#define MAX_LIGHTS 8

//	Light
layout(location = 13)uniform vec3					lDiffuse;
layout(location = 14)uniform vec3					lAmbient;
layout(location = 15)uniform vec3					lSpecular;
layout(location = 16)uniform float					lCAttenuation;
layout(location = 17)uniform float					lLAttenuation;
layout(location = 18)uniform float					lQAttenuation;

//	Materials
layout(location = 20) uniform int					MaterialType;
layout(location = 21) uniform vec4					DiffuseColor;
layout(location = 22) uniform vec4					AmbientColor;
layout(location = 23) uniform vec4					SpecularColor;
layout(location = 24) uniform sampler2D				DiffuseTexture;
layout(location = 25) uniform sampler2D				NormalTexture;
layout(location = 26) uniform sampler2D				SpecularTexture;
layout(location = 27) uniform sampler2D				RenderedTexture;
layout(location = 28) uniform float					Shininess;

layout(location = 29) uniform sampler2D				Shadowmap;

in VertexData {
	vec3						Position;
	vec3						Normal;
	vec3						Tangents;
	vec3						Bitangents;
	smooth vec2					UV;
	smooth vec3					vLight;
	smooth vec3					vEye;
} vertexIn;

//	Output
layout(location = 0) out vec4 	Color0;

void main()
{
	vec3 color					= DiffuseColor.rgb;
	vec3 spec					= SpecularColor.rgb;
	vec3 normal					= vec3(0.0, 0.0, 1.0);
	vec3 specular				= vec3(0.0);
	
//	float depth					= texture2D(Shadowmap, vertexIn.UV).x;
	
	//	Debugging and checking
	float TexHStretch = 1.0f;
	float TexVStretch = 1.0f;
	vec2 TS	= vec2(TexHStretch, TexVStretch);
	
	if((MaterialType & 0x00000002) == 0x00000002) {		//	HasDiffuseTexture
		color = texture(DiffuseTexture, vertexIn.UV*TS).rgb;
	}
	
	if((MaterialType & 0x00000004) == 0x00000004) {		//	HasBumpMap
		normal = texture(NormalTexture, vertexIn.UV*TS).rgb * 2.0 - 1.0;
	}
	
	if((MaterialType & 0x00000008) == 0x00000008) {		//	HasSpecularTexture
		spec = texture(SpecularTexture, vertexIn.UV*TS).rgb;
	}
	
	//- Attenuation calculation -----------------------------------------------
	
	float	dist				= length(vertexIn.vLight);
	float	Attenuation			= 1.0 /
								(	lCAttenuation	+
									lLAttenuation	* dist	+
									lQAttenuation	* dist	* dist	);
	
	//- Diffuse calculation ---------------------------------------------------
	
	vec3	light				= vertexIn.vLight / dist;
	float	arcLight			= max(0.0, dot(normal, light));
	vec3	diffuse				= color * arcLight * Attenuation;
	
	//- Specular calculation 1 ------------------------------------------------
	
	if(arcLight > 0.0) {
		vec3 v2e				= normalize(vertexIn.vEye);
		vec3 lrn				= reflect(-light, normal);
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
