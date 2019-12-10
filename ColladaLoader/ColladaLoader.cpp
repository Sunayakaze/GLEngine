
#include "VModelData.h"
#include "ColladaLoader.h"

//- Scene Parameters ----------------------------------------------------------

#define CameraPosition	vec3(  0.0f,  4.0f,  7.0f )
#define CameraLookat	vec3( -0.0f,  1.0f,  0.0f)

//- Overridden ----------------------------------------------------------------
namespace GLEngine
{
	glerror ColladaLoader::SetupControllers()
	{
		LogHeader("Setup controllers");

		GLCameraController::GLCameraController(pGLWin);

		//-	Setup compute shader ------------------------------------------------------------------

//		if (SetupSimpleComputeshader()) return glError;

//		if (SetupComputeshader()) return glError;

		//-	Setup Scene shaders -------------------------------------------------------------------

		shdrid vert, frag1, frag2;

		if (vert = LoadShader(GL_VERTEX_SHADER, "Default"))
		{
			if (frag1 = LoadShader(GL_FRAGMENT_SHADER, "Default")) {
				if (MaterialShader.SetupShader(vert, frag1, HRES, VRES)) {
					DeleteShader(vert);
					DeleteShader(frag1);
					return glError;
				}
			}
		}
	}

	glerror ColladaLoader::SetupSubjects()
	{
		mat4 V = GLCameraController::SetViewMatrix(CameraPosition, CameraLookat);

		mat4 P = perspective(
			45.0f,															//	45 degree vision
			(float)HRES / (float)VRES,										//	Aspect ratio	
			0.1f,															//	range close
			500.0f);														//	range far

		GLSubject Camera = GLSubject(P, V, GLCameraController::MoveFirstPersonCamera, nullptr);

		Subjects.Add(Camera);

		return glOK;
	}

	glerror ColladaLoader::SetupScene()
	{
		LogHeader("Setup scene");

		const color3& CA = color3(0.2f);
		const color3& CD = color3(0.6f, 0.6f, 0.6f);
		const color3& CS = CD; // color3(0.6f);

		GLOmniLight light = GLOmniLight(CA, CD, CS, 0.3f, 0.2f, 0.1f, mat4(1.0f), nullptr, RotateLight);

		GLEntity* pLight = AddObject(light);

		//	Physical ligt object
		MeshData LightMesh;
		CreateDiamond(LightMesh, 0.25f);

		GLMaterial LightMaterial;
		LightMaterial.setAmbient(color4(CA, 1.0f));
		LightMaterial.setDiffuse(color4(CD, 1.0f));
		LightMaterial.setSpecular(color4(CS, 1.0f));
		LightMaterial.setShininess(50.0f);

		GLModel* pLightModel = LoadModel(LightMesh, LightMaterial);
		pLightModel->SetName("LightMesh");
		GLObject* const pLightObj = AddObject(GLObject(pLightModel, nullptr, pLight, &MaterialShader));

		GLModel* const pModel1 = LoadDAEModel("teapot");

		GLObject n1 = GLObject(pModel1, scale(mat4(1.0f), vec3(0.6f)), (GLEntity*)nullptr, &GLRenderController::MaterialShader);
		const GLObject* pEntity1 = AddObject(n1);

		return glOK;
	}

	//- ColladaLoader specific ----------------------------------------------------

	GLModel* const ColladaLoader::LoadDAEModel(
		const char*						ModelName)
	{
		ColladaDoc colladadoc;
		VModelData mesh;

		String ModelPath = FindDAEModelPath(ModelName);

		if (ModelPath.Size() > 1)
		{
			colladadoc.Read((ModelPath + ModelName + ".dae").Val());
			mesh.SetModelPath(ModelPath.Val());
			mesh.SetName(ModelName);
			mesh.LoadColladaDoc(colladadoc);

			return LoadModel(mesh);
		}

		return nullptr;
	}
}

HRESULT main()
{
	GLEngine::ColladaLoader EngineDemo;

	return EngineDemo.Run();
}
