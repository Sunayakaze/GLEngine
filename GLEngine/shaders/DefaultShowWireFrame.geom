#version 450
#pragma optimize(off)
//	#pragma debug(on)

layout(triangles) in;
layout(line_strip, max_vertices=4) out;

in VertexData {
	vec3											Position;
	vec3											Normal;
	vec3											Tangents;
	vec3											Bitangents;
	smooth vec2										UV;
	smooth vec3										vLight;
	smooth vec3										vEye;
} vertexIn[];

out VertexData {
	vec3											Position;
	vec3											Normal;
	vec3											Tangents;
	vec3											Bitangents;
	smooth vec2										UV;
	smooth vec3										vLight;
	smooth vec3										vEye;
} interpolators;

uniform mat4 P;

out vec4 tcolor;
out vec4 bcolor;
out vec4 ncolor;

float s = 1.0;

void main()
{
	for(int j=0; j<gl_in.length(); j=j+3) {
	
		gl_Position 			= gl_in[j].gl_Position;
		interpolators.Position	= vertexIn[j].Position;
		interpolators.UV		= vertexIn[j].UV;
		interpolators.vLight	= vertexIn[j].vLight;
		interpolators.Normal	= vertexIn[j].Normal;
		EmitVertex();
		
		gl_Position 			= gl_in[j+1].gl_Position;
		interpolators.Position	= vertexIn[j+1].Position;
		interpolators.UV		= vertexIn[j+1].UV;
		interpolators.vLight	= vertexIn[j+1].vLight;
		interpolators.Normal	= vertexIn[j+1].Normal;
		EmitVertex();
		
		gl_Position 			= gl_in[j+2].gl_Position;
		interpolators.Position	= vertexIn[j+2].Position;
		interpolators.UV		= vertexIn[j+2].UV;
		interpolators.vLight	= vertexIn[j+2].vLight;
		interpolators.Normal	= vertexIn[j+2].Normal;
		EmitVertex();
		
		gl_Position 			= gl_in[j].gl_Position;
		interpolators.Position	= vertexIn[j].Position;
		interpolators.UV		= vertexIn[j].UV;
		interpolators.vLight	= vertexIn[j].vLight;
		EmitVertex();
		
		EndPrimitive();
		
		//	//	Show lightvectors
		//	for(int i=0; i<3; i++) {
		//		mat3 iTBN = inverse(mat3(vertexIn[j+i].Tangents, vertexIn[j+i].Bitangents, vertexIn[j+i].Normal));
		//		vec4 pos = vec4(vertexIn[j+i].Position, 1.0);
		//		vec4 lgt = vec4(vertexIn[j+i].Position + (iTBN * vertexIn[j+i].vLight), 1.0);
		//		gl_Position			= P * pos;	EmitVertex();
		//		gl_Position			= P * lgt;	EmitVertex();
		//		EndPrimitive();
		//	}
		//	
		//	//	Show normals
		//	for(int i=0; i<3; i++) {
		//		vec4 pos = vec4(vertexIn[j+i].Position, 1.0);
		//		vec4 nor = vec4(vertexIn[j+i].Position + (vertexIn[j+i].Normal * s), 1.0);
		//		gl_Position			= P * pos;	EmitVertex();
		//		gl_Position			= P * nor;	EmitVertex();
		//		EndPrimitive();
		//	}
		//	
		//	//	Show tangent
		//	for(int i=0; i<3; i++) {
		//		vec4 pos = vec4(vertexIn[j+i].Position, 1.0);
		//		vec4 tan = vec4(vertexIn[j+i].Position + (vertexIn[j+i].Tangents * s), 1.0);
		//		gl_Position				= P * pos;	EmitVertex();
		//		gl_Position				= P * tan;	EmitVertex();
		//		EndPrimitive();		
		//	}
		//	
		//	//	Show bitangent
		//	for(int i=0; i<3; i++) {
		//		vec4 pos = vec4(vertexIn[j+i].Position, 1.0);
		//		vec4 bit = vec4(vertexIn[j+i].Position + (vertexIn[j+i].Bitangents * s), 1.0);
		//		gl_Position				= P * pos;	EmitVertex();
		//		gl_Position				= P * bit;	EmitVertex();
		//		bcolor					= vec4(0.0, 1.0, 0.0, 1.0);
		//		EndPrimitive();		
		//	}
	}
}
