#version 450 core
#pragma optimize(off)
#pragma debug(on)

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

//- Trongrid ------------------------------------------------------------------------------------

#define Octaves		6
#define Size		1.0

// rotate position around axis
vec2 rotate(vec2 p, float a)
{
	return vec2(p.x * cos(a) - p.y * sin(a), p.x * sin(a) + p.y * cos(a));
}

// 1D random numbers
float rand(float n)
{
    return fract(sin(n) * 43758.5453123);
}

// 2D random numbers
vec2 rand2(in vec2 p)
{
	return fract(vec2(sin(p.x * 591.32 + p.y * 154.077), cos(p.x * 391.32 + p.y * 49.077)));
}

// 1D noise
float noise1(float p)
{
	float fl = floor(p);
	float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

// voronoi distance noise, based on iq's articles
float voronoi(in vec2 x)
{
	vec2 p = floor(x);
	vec2 f = fract(x);
	
	vec2 res = vec2(4.0);
	for(int j = -1; j <= 1; j ++)
	{
		for(int i = -1; i <= 1; i ++)
		{
			vec2 b = vec2(i, j);
			vec2 r = vec2(b) - f + rand2(p + b);
			
			// chebyshev distance, one of many ways to do this
			float d = max(abs(r.x), abs(r.y));
			
			if(d < res.x)
			{
				res.y = res.x;
				res.x = d;
			}
			else if(d < res.y)
			{
				res.y = d;
			}
		}
	}
	return res.y - res.x;
}

void DigitalBrain( out vec3 fragColor, in vec2 texuv )
{
    float flicker = noise1(iGlobalTime * 2.0) * 0.8 + 0.4;

	vec2 uv = texuv * Size;
	
	vec2 suv = uv;
	float v = 0.0;
	
	// a bit of camera movement
	uv.y += 1.5; //(iGlobalTime * 0.02);
	
	// add some noise octaves
	float a = 0.6, f = 1.0;
	
	for(int i = 0; i < Octaves; i ++) // 4 octaves also look nice, its getting a bit slow though
	{
		float v1 = voronoi(uv * f + 5.0);
		float v2 = 0.0;
		
		// make the moving electrons-effect for higher octaves
		if(i > 0)
		{
			// of course everything based on voronoi
			v2 = voronoi(uv * f * 0.5 + 50.0 + iGlobalTime);
			
			float va = 0.0, vb = 0.0;
			va = 1.0 - smoothstep(0.0, 0.1, v1);
			vb = 1.0 - smoothstep(0.0, 0.08, v2);
			v += a * pow(va * (0.5 + vb), 2.0);
		}
		
		// make sharp edges
		v1 = 1.0 - smoothstep(0.0, 0.3, v1);
		
		// noise is used as intensity map
		v2 = a * (noise1(v1 * 5.5 + 0.1));
		
		// octave 0's intensity changes a bit
		if(i == 0)
			v += v2 * flicker;
		else
			v += v2;
		
		f *= 3.0;
		a *= 0.7;
	}

	// slight vignetting
	//	v *= exp(-0.6 * length(suv)) * 1.2;
	v *= exp(-0.6) * 1.2;
	
	vec3 cexp = vec3(0.5, 1.0, 0.5);
	cexp *= 4;
	
	fragColor = vec3(pow(v, cexp.x), pow(v, cexp.y), pow(v, cexp.z)) * 2.0;
}

//- Materialshader --------------------------------------------------------------------------------

void main()
{
	vec3 color					= vec3(0.0);
	vec3 spec					= vec3(0.0);
	vec3 normal					= vec3(0.0, 0.0, 1.0);
	vec3 specular				= vec3(0.0);
	
	//	Debugging and checking
	vec2 TS	= vec2(4.0);
	
	DigitalBrain(color, vertexIn.UV);
	
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
	
	vec3 out_color				= vec3(0.0);
	out_color				   += lAmbient * color;
	out_color           	   += lDiffuse * diffuse; 
	out_color           	   += lSpecular * specular;

	Color0						= vec4(out_color, 1.0);
}
