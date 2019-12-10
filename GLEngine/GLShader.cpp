
#include "GLShader.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

//- Generic Shader Functions ----------------------------------------------------------------------

namespace GLEngine
{
	//- GLFrameShader ------------------------------------------------------------------------------
	
	void GLFrameShader::enableShader()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
		glViewport(0, 0, HRes, VRes);
		UseProgram(ProgramID);
	}
	
	//- GL2DShader ----------------------------------------------------------------------------
	
	void GL2DShader::BindFragDataLocations()
	{
		BindFragDataLocation(0, ProgramID, "Color0");
		BindFragDataLocation(1, ProgramID, "Color1");
		BindFragDataLocation(2, ProgramID, "Color2");
		BindFragDataLocation(3, ProgramID, "Color3");
		BindFragDataLocation(4, ProgramID, "Color4");
		BindFragDataLocation(5, ProgramID, "Color5");
		BindFragDataLocation(6, ProgramID, "Color6");
		BindFragDataLocation(7, ProgramID, "Color7");
		BindFragDataLocation(8, ProgramID, "Color8");
		BindFragDataLocation(9, ProgramID, "Color9");
	}

	//- Set up individual shaders -----------------------------------------------------------------

	GL3DShader::GL3DShader() : GLFrameShader()
	{
		//	Matrices
		MatMVP = -1;
		MatDMVP = -1;
		MatMV = -1;
		MatN = -1;
		MatP = -1;

		//	Light
		lPosition = -1;
		lDiffuse = -1;
		lAmbient = -1;
		lSpecular = -1;
		lCAttenuation = -1;
		lLAttenuation = -1;
		lQAttenuation = -1;
		LightType = -1;

		//	Geometry IDs
		Vertex = -1;
		TexCoords = -1;
		Colors = -1;
		Normal = -1;
		Tangent = -1;
		Bitangent = -1;

		//	Skinning
		VertexWeight = -1;
		VertexJoint = -1;
		Bones = -1;

		Shadowmap = -1;
	}

	void GL3DShader::GetShaderLocations()
	{
		//	Attribute locations
		GetAttribLocation(Vertex, ProgramID, "Position");
		GetAttribLocation(TexCoords, ProgramID, "TexCoord");
		GetAttribLocation(Normal, ProgramID, "Normal");
		GetAttribLocation(Colors, ProgramID, "Colors");
		GetAttribLocation(Tangent, ProgramID, "Tangent");
		GetAttribLocation(Bitangent, ProgramID, "Bitangent");
		GetAttribLocation(VertexJoint, ProgramID, "Joints");
		GetAttribLocation(VertexWeight, ProgramID, "Weights");

		//	Get a handle to the the matrix uniforms
		GetUniformLocation(MatMVP, ProgramID, "MVP");
		GetUniformLocation(MatMV, ProgramID, "MV");
		GetUniformLocation(MatN, ProgramID, "N");
		GetUniformLocation(MatP, ProgramID, "P");
		GetUniformLocation(MatDMVP, ProgramID, "DMVP");
		GetUniformLocation(Bones, ProgramID, "Bones");

		//	Get the Light uniform handles
		GetUniformLocation(lPosition, ProgramID, "lPosition");
		GetUniformLocation(LightType, ProgramID, "LightType");
		GetUniformLocation(lDiffuse, ProgramID, "lDiffuse");
		GetUniformLocation(lAmbient, ProgramID, "lAmbient");
		GetUniformLocation(lSpecular, ProgramID, "lSpecular");
		GetUniformLocation(lCAttenuation, ProgramID, "lCAttenuation");
		GetUniformLocation(lLAttenuation, ProgramID, "lLAttenuation");
		GetUniformLocation(lQAttenuation, ProgramID, "lQAttenuation");

		GetUniformLocation(Shadowmap, ProgramID, "Shadowmap");

		BindFragDataLocation(0, ProgramID, "Color0");
	}

	void GL3DShader::SendAttributes(const GLBuffers& Buffers)
	{
		glGetError();

		Bind3Float(Vertex,				Buffers.getVertexBuffer());
		Bind2Float(TexCoords,			Buffers.getUVBuffer());
		Bind3Float(Tangent,				Buffers.getTangentBuffer());
		Bind3Float(Normal,				Buffers.getNormalBuffer());
		Bind3Float(Bitangent,			Buffers.getBitangentBuffer());
		Bind4Uint(VertexJoint,			Buffers.getJointBuffer());
		Bind4Float(VertexWeight + 0,	Buffers.getWeight1Buffer());
		Bind4Float(VertexWeight + 1,	Buffers.getWeight2Buffer());
		Bind4Float(VertexWeight + 2,	Buffers.getWeight3Buffer());
		Bind4Float(VertexWeight + 3,	Buffers.getWeight4Buffer());
	}

	void GL3DShader::SendPosition(const vec3& Position) {
		SendVec3Uniform(lPosition, Position);
	}

	void GL3DShader::SendDiffuse(const color3& Diffuse) {
		SendVec3Uniform(lDiffuse, Diffuse);
	}

	void GL3DShader::SendAmbient(const color3& Ambient) {
		SendVec3Uniform(lAmbient, Ambient);
	}

	void GL3DShader::SendSpecular(const color3& Specular) {
		SendVec3Uniform(lSpecular, Specular);
	}

	void GL3DShader::SendCAttenuation(const float& CAttenuation) {
		Send1FloatUniform(lCAttenuation, CAttenuation);
	}

	void GL3DShader::SendLAttenuation(const float& LAttenuation) {
		Send1FloatUniform(lLAttenuation, LAttenuation);
	}

	void GL3DShader::SendQAttenuation(const float& QAttenuation) {
		Send1FloatUniform(lQAttenuation, QAttenuation);
	}

	void GL3DShader::SendMVMatrix(const mat4& MVMatrix) {
		SendMat4Uniform(MatMV, &MVMatrix[0][0]);
	}

	void GL3DShader::SendMVPMatrix(const mat4& MVPMatrix) {
		SendMat4Uniform(MatMVP, &MVPMatrix[0][0]);
	}

	void GL3DShader::SendNMatrix(const mat4& NMatrix) {
		SendMat4Uniform(MatN, &NMatrix[0][0]);
	}

	void GL3DShader::DisableAttribArrays()
	{
		glGetError();

		Unbind(Vertex);
		Unbind(TexCoords);
		Unbind(Normal);
		Unbind(Tangent);
		Unbind(Bitangent);
		Unbind(VertexJoint);
		Unbind(VertexWeight + 0);
		Unbind(VertexWeight + 1);
		Unbind(VertexWeight + 2);
		Unbind(VertexWeight + 3);
	}

	void GL3DShader::SendBonestructure(
		const GLBones&						Bones)
	{
		if (Bones.GetNRJoints()) {
			glUniformMatrix4fv(this->Bones, Bones.GetNrSkinningJoints(),
				false, (float*)Bones.GetSkinningMatrixPointer(1));
		}
	}

	//-	GLMaterialShader --------------------------------------------------------------------------

	GLMaterialShader::GLMaterialShader() : GL3DShader()
	{
		//	Materials
		MaterialType = -1;

		AmbientTexture = -1;
		DiffuseTexture = -1;
		BumpmapTexture = -1;
		SpecularTexture = -1;
		RenderedTexture = -1;
		DepthTexture = -1;
		DiffuseColor = -1;
		AmbientColor = -1;
		SpecularColor = -1;
		Shininess = -1;
	}

	void GLMaterialShader::GetShaderLocations()
	{
		//	Materials
		GetUniformLocation(MaterialType,	ProgramID, "MaterialType");
		GetUniformLocation(DiffuseColor,	ProgramID, "DiffuseColor");
		GetUniformLocation(AmbientColor,	ProgramID, "AmbientColor");
		GetUniformLocation(SpecularColor,	ProgramID, "SpecularColor");
		GetUniformLocation(DiffuseTexture,	ProgramID, "DiffuseTexture");
		GetUniformLocation(BumpmapTexture,	ProgramID, "NormalTexture");
		GetUniformLocation(SpecularTexture,	ProgramID, "SpecularTexture");
		GetUniformLocation(RenderedTexture,	ProgramID, "RenderedTexture");
		GetUniformLocation(DepthTexture,	ProgramID, "DepthTexture");
		GetUniformLocation(Shininess,		ProgramID, "Shininess");

		GL3DShader::GetShaderLocations();
	}

	void GLMaterialShader::RenderPolygons(const GLPolygons& Polygons)
	{
		Send2DTexture(0, AmbientTexture,	Polygons.getAmbient());
		Send2DTexture(1, DiffuseTexture,	Polygons.getDiffuse());
		Send2DTexture(2, SpecularTexture,	Polygons.getSpecular());
		Send2DTexture(3, BumpmapTexture,	Polygons.getBumpmap());
		SendVec4Uniform(AmbientColor,		Polygons.getAmbientColor());
		SendVec4Uniform(DiffuseColor,		Polygons.getDiffuseColor());
		SendVec4Uniform(SpecularColor,		Polygons.getSpecularColor());
		Send1UintUniform(MaterialType,		Polygons.getMaterialType());
		Send1FloatUniform(Shininess,		Polygons.getShininess());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Polygons.getIndexBuffer());
		glDrawElements(GL_TRIANGLES, Polygons.getNrFaces() * 3, GL_UNSIGNED_SHORT, (void*)0);
	}

	//- Material Fragment Shader ---------------------------------------------------------------------

	GLMaterialFragShader::GLMaterialFragShader() : GL3DShader()
	{
		//	Materials
		GlobalTime		= -1;
		Projection		= -1;
		MaterialType	= -1;
		AmbientTexture	= -1;
		DiffuseTexture	= -1;
		NormalTexture	= -1;
		SpecularTexture	= -1;
		Shininess		= -1;
	}

	void GLMaterialFragShader::GetShaderLocations()
	{
		GL3DShader::GetShaderLocations();

		GetUniformLocation(MaterialType,	ProgramID, "MaterialType");
		GetUniformLocation(Projection,		ProgramID, "P");
		GetUniformLocation(Shininess,		ProgramID, "Shininess");
		GetUniformLocation(AmbientTexture,	ProgramID, "AmbientTexture");
		GetUniformLocation(DiffuseTexture,	ProgramID, "DiffuseTexture");
		GetUniformLocation(NormalTexture,	ProgramID, "NormalTexture");
		GetUniformLocation(SpecularTexture,	ProgramID, "SpecularTexture");
		GetUniformLocation(GlobalTime,		ProgramID, "iGlobalTime");
	}

	void GLMaterialFragShader::RenderPolygons(const GLPolygons&	Polygons)
	{
		Send2DTexture(0, AmbientTexture,	Polygons.getAmbient());
		Send2DTexture(1, DiffuseTexture,	Polygons.getDiffuse());
		Send2DTexture(2, SpecularTexture,	Polygons.getSpecular());
		Send2DTexture(3, NormalTexture,		Polygons.getBumpmap());
		Send1UintUniform(MaterialType,		Polygons.getMaterialType());
		Send1FloatUniform(Shininess,		Polygons.getShininess());

		Send1FloatUniform(GlobalTime, Time);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Polygons.getIndexBuffer());
		glDrawElements(GL_TRIANGLES, Polygons.getNrFaces() * 3, GL_UNSIGNED_SHORT, (void*)0);
	}
}