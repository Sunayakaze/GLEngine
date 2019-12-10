#version 450 core
#pragma optimize(off)
#pragma debug(on)

#define MAX_LIGHTS 8

layout(location = 01) in vec3			Position;
layout(location = 02) in vec2			TexCoord;
layout(location = 03) in vec3			Normal;
layout(location = 04) in vec2			Colors;
layout(location = 05) in vec3			Tangent;
layout(location = 06) in vec3			Bitangent;
layout(location = 07) in uvec4			Joints;		// uint holds 4 ubytes x 4.
layout(location = 10) in vec4			Weights[4];

layout(location = 01) uniform mat4		MVP;
layout(location = 02) uniform mat4		MV;
layout(location = 03) uniform mat4		N;
layout(location = 11) uniform vec3		lPosition;
layout(location = 12) uniform uint		LightType;

layout(location = 500) uniform mat4		Bones[150];

out VertexData {
	vec3						Position;
	vec3						Normal;
	vec3						Tangents;
	vec3						Bitangents;
	smooth vec2					UV;
	smooth vec3					vLight;
	smooth vec3					vEye;
} vertexOut;

//	Initializes the attributes and animates the model, if applicable.
void AnimateModel(inout vec4 Pos, inout vec4 Nor, inout vec4 Tan, inout vec4 Bit)
{	
	vec4 oPos = Pos;
	vec4 oNrm = Nor;
	vec4 oTan = Tan;
	vec4 oBiT = Bit;
	
	if(Weights[0].x != 0.0) {
		uint index = Joints.x & 0x000000FF;
		Pos = oPos * Bones[index] * Weights[0].x;
		Nor = oNrm * Bones[index] * Weights[0].x;
		Tan = oTan * Bones[index] * Weights[0].x;
		Bit = oBiT * Bones[index] * Weights[0].x;
		if(Weights[0].y != 0.0) {
			index = (Joints.x & 0x0000FF00) >> 0x08;
			Pos += oPos * Bones[index] * Weights[0].y;
			Nor += oNrm * Bones[index] * Weights[0].y;
			Tan += oTan * Bones[index] * Weights[0].y;
			Bit += oBiT * Bones[index] * Weights[0].y;
			if(Weights[0].z != 0.0) {
				index = (Joints.x & 0x00FF0000) >> 0x10;
				Pos += oPos * Bones[index] * Weights[0].z;
				Nor += oNrm * Bones[index] * Weights[0].z;
				Tan += oTan * Bones[index] * Weights[0].z;
				Bit += oBiT * Bones[index] * Weights[0].z;
				if(Weights[0].w != 0.0) {
					index = (Joints.x & 0xFF000000) >> 0x18;
					Pos += oPos * Bones[index] * Weights[0].w;
					Nor += oNrm * Bones[index] * Weights[0].w;
					Tan += oTan * Bones[index] * Weights[0].w;
					Bit += oBiT * Bones[index] * Weights[0].w;
				}	//	Weight 4
			}	//	Weight 3
		}	//	Weight 2	
	}
}

void main()
{
	//	Initialize attributes in modelspace
	vec4 msPos				=	vec4(Position, 1.0);
	vec4 msNor				=	vec4(Normal, 0.0);
	vec4 msTan				=	vec4(Tangent, 0.0);
	vec4 msBit				=	vec4(Bitangent, 0.0);
	
	AnimateModel(msPos, msNor, msTan, msBit);
	
	//	Transformed Position
	gl_Position				=	MVP * msPos;
	
	//	Convert to EyeSpace
	mat3 Normal				=	mat3(N);
	vec3 esPos				=	(MV * msPos).xyz;
	vec3 esNor				=	normalize(Normal * msNor.xyz);
	vec3 esTan				=	normalize(Normal * msTan.xyz);
	vec3 esBit				=	normalize(Normal * msBit.xyz);
	
	//	Lightposition in EyeSpace
	vec3 Vertex2Eye			=	vec3(0.0) - esPos;
	vec3 Vertex2Light 		=	lPosition - esPos;
	
	mat3 TBN				=	transpose(mat3(esTan, esBit, esNor));
	
	//	Send UV to fragment shader
	vertexOut.Position		=	esPos;
	vertexOut.Normal		=	esNor;
	vertexOut.Tangents		=	esTan;
	vertexOut.Bitangents	=	esBit;
	vertexOut.UV			=	TexCoord;
	vertexOut.vLight		=	TBN * Vertex2Light;
	vertexOut.vEye			=	TBN * Vertex2Eye;
}
