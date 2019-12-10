
#include "Load3DModel.h"

GLFWwindow*					pGLWin;

//- Engine Defines ------------------------------------------------------------

#define HRES				960		// 1280 / 1366
#define VRES				1080

namespace GLGeneral
{

	//- CPU SIDE Variables -------------------------------------------------------

	//	Shader var
	shdrid						SpecularShader;
	shdrid						AttribShader;

	//	Matrices
	mat4						Model;
	mat4						View;
	mat4						Proj;

	vec3						AmbientColor;
	vec3						DiffuseColor;
	vec3						SpecularColor;
	float						Shininess;

	//	Geometry handles
	bufferid					Vertices;
	bufferid					Texcoords;
	bufferid					Normals;
	bufferid					Tangents;
	bufferid					Bitangents;
	bufferid					Indices;

	// Material handles
	bufferid					DiffuseMap;
	bufferid					NormalMap;
	bufferid					SpecularMap;

	uint						NrTriangles;

	//- GPU SIDE Variables --------------------------------------------------------

	//	Matrix Shader Locations
	unifloc						uP;
	unifloc						uMVP;
	unifloc						uMV;
	unifloc						uN;

	//	Geometry Shader Locations
	unifloc						aPosition;
	unifloc						aTexCoord;
	unifloc						aNormal;
	unifloc						aTangent;
	unifloc						aBitangent;

	//	Texture Shader Locations
	unifloc						uDiffuseMap;
	unifloc						uNormalMap;
	unifloc						uSpecularMap;
	unifloc						uDiffuseColor;
	unifloc						uAmbientColor;
	unifloc						uSpecularColor;
	unifloc						uShininess;

	//	Light Shader Locations
	unifloc						ulPosition;
	unifloc						ulAmbient;
	unifloc						ulDiffuse;
	unifloc						ulSpecular;
	unifloc						ulCAttenuation;
	unifloc						ulLAttenuation;
	unifloc						ulQAttenuation;

	glerror						Initialize();
	glerror						Run();
}