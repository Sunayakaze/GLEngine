#version 450
#pragma optimize(off)
#pragma debug(on)
 
layout(location = 1) in vec3			Position;
layout(location = 2) in vec2			TexCoord;
layout(location = 3) in vec3			Normal;
layout(location = 4) in vec3			Tangent;
layout(location = 5) in vec3			Bitangent;

layout(location = 1) uniform mat4		MVP;
layout(location = 2) uniform mat4		MV;
layout(location = 3) uniform mat4		N;
layout(location = 4) uniform vec3 		lPosition;

out VertexData {
	vec3								Position;
	smooth vec3							Normal;
	smooth vec3							Tangents;
	smooth vec3							Bitangents;
	smooth vec2							UV;
	smooth vec3							vLight;
	smooth vec3							vEye;
} vertexOut;

void main()
{
	//	Initialize attributes in modelspace.
	vec4 msPos						=	vec4(Position, 1.0);
	vec4 msNor						=	vec4(Normal, 0.0);
	vec4 msTan						=	vec4(Tangent, 0.0);
	vec4 msBit						=	vec4(Bitangent, 0.0);
	
	//	Convert to EyeSpace
	vec3 esPos						=	(MV * msPos).xyz;
	vec3 esNor						=	normalize((N  * msNor).xyz);
	vec3 esTan						=	normalize((N  * msTan).xyz);
	vec3 esBit						=	normalize((N  * msBit).xyz);
	
	//	Lightposition in EyeSpace
	vec3 Vertex2Eye					=	vec3(0.0) - esPos;
	vec3 Vertex2Light 				=	lPosition - esPos;
	
	mat3 TBN						=	transpose(mat3(esTan, esBit, esNor));
	
	//	Send UV to fragment shader
	vertexOut.Position				=	esPos;
	vertexOut.Normal				=	esNor;
	vertexOut.Tangents				=	esTan;
	vertexOut.Bitangents			=	esBit;
	vertexOut.UV					=	TexCoord;
	vertexOut.vLight				=	TBN * Vertex2Light;
	vertexOut.vEye					=	TBN * Vertex2Eye;
	
	//	Transformed Position
	gl_Position						=	MVP * msPos;
}
