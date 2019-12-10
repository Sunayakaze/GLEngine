#version 450 core
#pragma optimize(off)
#pragma debug(on)

precision mediump float;

//	Light
layout(location = 13) uniform vec3					lDiffuse;
layout(location = 14) uniform vec3					lAmbient;
layout(location = 15) uniform vec3					lSpecular;
layout(location = 16) uniform float					lCAttenuation;
layout(location = 17) uniform float					lLAttenuation;
layout(location = 18) uniform float					lQAttenuation;

//	Materials
layout(location = 20) uniform int					MaterialType;
layout(location = 21) uniform sampler2D				AmbientTexture;
layout(location = 22) uniform sampler2D				DiffuseTexture;
layout(location = 23) uniform sampler2D				NormalTexture;
layout(location = 24) uniform sampler2D				SpecularTexture;
layout(location = 25) uniform float					Shininess;

layout(location = 32) uniform float     			iGlobalTime;                  // shader playback time (in seconds)

in VertexData {
	vec3											Position;
	vec3											Normal;
	vec3											Tangents;
	vec3											Bitangents;
	smooth vec2										UV;
	smooth vec3										vLight;
	smooth vec3										vEye;
} vertexIn;

//	Output
layout(location = 0) out vec4 						Color0;

//- Perlin Noise ----------------------------------------------------------------------------------

#define seed 20

float Noise(int x, int y)
{
	int n = x + y * 57 + seed;
    n = (n << 13) ^ n;
	uint t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x00ffffff;
	float f = t;
    f *= 5.9604644775390625e-08;
    
	return f;
}

float GetValue(vec2 i)
{
	int xi = int(floor(i.x));
	int yi = int(floor(i.y));
	float x0y0 = Noise(xi,   yi);
	float x1y0 = Noise(xi+1, yi);
	float x0y1 = Noise(xi,   yi+1);
	float x1y1 = Noise(xi+1, yi+1);
	
	float Xfrac = fract(i.x);
	float Yfrac = fract(i.y);
	float v1 = mix(x0y0, x1y0, Xfrac);
	float v2 = mix(x0y1, x1y1, Xfrac);
    return mix(v1, v2, Yfrac);
}

const float f0 = 2.00; const float a0 = 1.0/f0;
const float f1 = 2*f0; const float a1 = 1.0/f1;
const float f2 = 2*f1; const float a2 = 1.0/f2;
const float f3 = 2*f2; const float a3 = 1.0/f3;
const float f4 = 2*f3; const float a4 = 1.0/f4;
const float f5 = 2*f4; const float a5 = 1.0/f5;
const float f6 = 2*f5; const float a6 = 1.0/f6;
const float f7 = 2*f6; const float a7 = 1.0/f7;
const float f8 = 2*f7; const float a8 = 1.0/f8;
const float f9 = 2*f8; const float a9 = 1.0/f9;

float GetHeight(vec2 uv)
{
	float n0 = a0*GetValue(f0*uv);
	float n1 = a1*GetValue(f1*uv);
	float n2 = a2*GetValue(f2*uv);
	float n3 = a3*GetValue(f3*uv);
	float n4 = a4*GetValue(f4*uv);
	float n5 = a5*GetValue(f5*uv);
	float n6 = a6*GetValue(f6*uv);
	float n7 = a7*GetValue(f7*uv);
	float n8 = a8*GetValue(f8*uv);
	float n9 = a9*GetValue(f9*uv);

	return (n1 + n2 + n3 + n4 + n5 + n6 + n7 + n8 + n9);
}

void Noise( out vec3 color, out vec3 normal, in vec2 texuv )
{
	vec2 uv = texuv * 2.0 - 1.0;
//	uv.y -= (iGlobalTime * 0.2);
	
	float s		= 0.4;
    vec2 d		= vec2(s, 0.0);
    float h		= s*GetHeight((uv));
    float h1	= s*GetHeight((uv+d.xy));
    float h2	= s*GetHeight((uv+d.yx));

    vec3 n1		= vec3(d.y, d.y, h);
	vec3 n2		= vec3(d.x, d.y, h1);
	vec3 n3		= vec3(d.y, d.x, h2);

	normal		= normalize(cross(n2-n1, n3-n1));
	color		= vec3(h);
}

//- Materialshader --------------------------------------------------------------------------------

void main()
{
	vec3 color					= vec3(0.8);
	vec3 spec					= vec3(0.0);
	vec3 normal					= vec3(0.0, 0.0, 1.0);
	vec3 specular				= vec3(0.0);
	
	//	Debugging and checking
	vec2 TS	= vec2(64.0);
	
	Noise(spec, normal, vertexIn.UV*TS);
	//	color = normal;
	//	normal = vec3(0.0, 0.0, 1.0);

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
	
	vec3 out_color				= vec3(0.0);
	out_color				   += lAmbient * color;
	out_color           	   += lDiffuse * diffuse; 
	out_color           	   += lSpecular * specular;
	
	Color0						= vec4(out_color, 1.0);
}
