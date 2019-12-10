#version 450
#pragma optimize(off)
//	#pragma debug(on)

//-----------------------------------------------------------------------------
//- Specular Shader -----------------------------------------------------------
//-----------------------------------------------------------------------------

layout(triangles) in;
layout(line_strip, max_vertices=32) out;

in VertexData {
	vec3						Position;
	smooth vec3					Normal;
	smooth vec3					Tangents;
	smooth vec3					Bitangents;
	smooth vec2					UV;
	smooth vec3					vLight;
	smooth vec3					vEye;
} vertexIn[];

out VertexData {
	vec3						Position;
	smooth vec3					Normal;
	smooth vec3					Tangents;
	smooth vec3					Bitangents;
	smooth vec2					UV;
	smooth vec3					vLight;
	smooth vec3					vEye;
} interpolators;

uniform mat4					P;

out vec4						color;

void main()
{
	float s = 0.2;
	
	for(int j=0; j<gl_in.length(); j=j+3) {

		gl_Position 			= gl_in[j].gl_Position;
		interpolators.Position	= vertexIn[j].Position;
		interpolators.UV		= vertexIn[j].UV;
		interpolators.vLight	= vertexIn[j].vLight;
		interpolators.Normal	= vertexIn[j].Normal;
		EmitVertex();
	
		gl_Position = gl_in[j+0].gl_Position;	color = vec4(1.0, 0.0, 1.0, 1.0);	EmitVertex();
		gl_Position = gl_in[j+1].gl_Position;	color = vec4(1.0, 0.0, 1.0, 1.0);	EmitVertex();
		gl_Position = gl_in[j+2].gl_Position;	color = vec4(1.0, 0.0, 1.0, 1.0);	EmitVertex();
		gl_Position = gl_in[j+0].gl_Position;	color = vec4(1.0, 0.0, 1.0, 1.0);	EmitVertex();
		EndPrimitive();
		
		//	Show tangent
		for(int i=0; i<3; i++) {
			vec4 pos = vec4(vertexIn[j+i].Position, 1.0);
			vec4 tan = vec4(vertexIn[j+i].Position + (vertexIn[j+i].Tangents * s), 1.0);
			gl_Position	= P * pos;				color = vec4(1.0, 0.0, 0.0, 1.0);	EmitVertex();
			gl_Position	= P * tan;				color = vec4(1.0, 0.0, 0.0, 1.0);	EmitVertex();
			EndPrimitive();
		}
		
		//	Show bitangent
		for(int i=0; i<3; i++) {
			vec4 pos = vec4(vertexIn[j+i].Position, 1.0);
			vec4 bit = vec4(vertexIn[j+i].Position + (vertexIn[j+i].Bitangents * s), 1.0);
			gl_Position	= P * pos;				color = vec4(0.0, 1.0, 0.0, 1.0);	EmitVertex();
			gl_Position	= P * bit;				color = vec4(0.0, 1.0, 0.0, 1.0);	EmitVertex();
			EndPrimitive();		
		}
		
		//	Show normals
		for(int i=0; i<3; i++) {
			vec4 pos = vec4(vertexIn[j+i].Position, 1.0);
			vec4 nor = vec4(vertexIn[j+i].Position + (vertexIn[j+i].Normal * s), 1.0);
			gl_Position	= P * pos;				color = vec4(0.0, 0.0, 1.0, 1.0);	EmitVertex();
			gl_Position	= P * nor;				color = vec4(0.0, 0.0, 1.0, 1.0);	EmitVertex();
			EndPrimitive();
		}
	}
}
