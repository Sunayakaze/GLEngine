
#include "GLEngineDemo.h"

//- Scene Parameters ------------------------------------------------------------------------------

#define CameraPosition	vec3(  0.0f,  5.0f,  10.0f )
#define CameraLookat	vec3(  0.0f,  5.0f,  0.0f  )
#define GridHSize		256

namespace GLEngine
{
	glerror GLEngineDemo::SetupSimpleComputeshader()
	{
		size HSize = 8192;
		count NrBuffers = 4;

		//-----------------------------------------------------------------------------------------------

		glfwSetTime(0.0);

		double ts = glfwGetTime();
		ComputeShader.SetupComputeshader("Adding");
		double te = glfwGetTime();

		LogMessage("\nCompiled shaderprogram in          : %.2f ms", (te - ts) * 1000.0);

		//-----------------------------------------------------------------------------------------------

		float* pBuffer = nullptr;
		size GridSize = HSize * HSize;

		ts = glfwGetTime();
				
		ComputeShader.GenerateFloatBuffers(NrBuffers, GridSize);

		te = glfwGetTime();

		LogMessage("Gridsize                           : %i x %i = %i", HSize, HSize, GridSize);
		LogMessage("Allocating %.2f MB in %i buffers   : %.2f ms",
			((double)GridSize * NrBuffers * sizeof(float)) / (1024 * 1024), NrBuffers, (te - ts) * 1000.0);

		//- Initialize ----------------------------------------------------------------------------------

		ts = glfwGetTime();

		pBuffer = ComputeShader.MapFloatBufferForWriting(0, GridSize);
		for (count i = 0; i < GridSize; i++) pBuffer[i] = 2.0f;
		ComputeShader.UnmapBuffer();
		
		pBuffer = ComputeShader.MapFloatBufferForWriting(1, GridSize);
		for (count i = 0; i < GridSize; i++) pBuffer[i] = 1.0f;
		ComputeShader.UnmapBuffer();

		te = glfwGetTime();

		LogMessage("Initializing                       : %.2f ms", (te - ts) * 1000.0);

		//- Generate Counters ---------------------------------------------------------------------------

		ComputeShader.GenerateCounterBuffers();

		//- Execute -------------------------------------------------------------------------------------

		ts = glfwGetTime();

		ComputeShader.Execute(512, 512, 1);

		te = glfwGetTime();

		LogMessage("Executing                          : %.2f ms", (te - ts) * 1000.0);

		//- Read back memory ----------------------------------------------------------------------------

		ts = glfwGetTime();

		count b3s = 0, b3e = 0;
		pBuffer = ComputeShader.MapFloatBufferForReading(2, GridSize);
		for (count i = 0; i < GridSize; i++)
		{
			if (pBuffer[i] == 3.0f) {
				b3s++;
			} else {
				b3e++;
			}
		}
		ComputeShader.UnmapBuffer();

		count b4s = 0, b4e = 0;
		pBuffer = ComputeShader.MapFloatBufferForReading(3, GridSize);
		for (count i = 0; i < GridSize; i++)
		{
			if (pBuffer[i] == 2.0f) {
				b4s++;
			} else {
				b4e++;
			}
		}
		ComputeShader.UnmapBuffer();

		te = glfwGetTime();

		LogMessage("CPU Compare                        : %.2f ms", (te - ts) * 1000.0);
		LogMessage("Add Total, Success, error          : %i, %i, %i", b3s + b3e, b3s, b3e);
		LogMessage("Mul Total, Success, error          : %i, %i, %i", b4s + b4e, b4s, b4e);

		//-----------------------------------------------------------------------------------------------

		uint counters[10] = { 0 };
		ComputeShader.GetCounterValues(&counters[0]);

		LogMessage("\n");
		LogMessage("Add Success                        : %i", counters[0]);
		LogMessage("Mul Success                        : %i", counters[1]);
		LogMessage("Total                              : %i", counters[2]);
		//	LogMessage("Workgroups                         : %i", counters[3]);
		//	LogMessage("Workgroupsize                      : %i", counters[4]);
		//	LogMessage("Highest index                      : %i", counters[5]);
		//	LogMessage("Highest gix                        : %i", counters[6]);
		//	LogMessage("Highest giy                        : %i", counters[7]);
		//	LogMessage("Highest lx                         : %i", counters[8]);
		//	LogMessage("Highest ly                         : %i", counters[9]);
		//	LogMessage("\n");

		return glError;
	}

	glerror GLEngineDemo::SetupControllers()
	{
		LogHeader("Setup controllers");

		GLCameraController::GLCameraController(pGLWin);

		//-	Setup compute shader ------------------------------------------------------------------

//		if (SetupSimpleComputeshader()) return glError;

		if (SetupComputeshader()) return glError;

		//-	Setup Scene shaders -------------------------------------------------------------------

		shdrid vert, frag1, frag2, geom;

		if (vert = LoadShader(GL_VERTEX_SHADER, "Default"))
		{
			if (frag1 = LoadShader(GL_FRAGMENT_SHADER, "Default")) {
				if (MaterialShader.SetupShader(vert, frag1, HRES, VRES)) {
					DeleteShader(vert);
					DeleteShader(frag1);
					return glError;
				}
			}

			if (frag2 = LoadShader(GL_FRAGMENT_SHADER, "TrongridMaterialShader")) {
				if (MaterialFragShader1.SetupShader(vert, frag2, HRES, VRES)) {
					DeleteShader(vert);
					DeleteShader(frag2);
					return glError;
				}
			}

			if (geom = LoadShader(GL_GEOMETRY_SHADER, "DefaultAttributes")) {
				if (WireframeShader.SetupShader(vert, geom, frag2, HRES, VRES)) {
					DeleteShader(geom);
					return glError;
				}
			}
		}

		return glOK;
	}

	glerror GLEngineDemo::SetupSubjects()
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

	glerror GLEngineDemo::SetupComputeshader()
	{
		LogHeader("Setup Computeshader");

		//-----------------------------------------------------------------------------------------

		ComputeShader.SetupComputeshader("TerrainInitializer");

		//-----------------------------------------------------------------------------------------

		const count Size  = 1 * (GridHSize - 0) * (GridHSize - 0);
		const count Faces = 2 * (GridHSize - 1) * (GridHSize - 1);

		ComputeShader.GenerateVec3Buffers(4, Size);	//	modelvectors
		ComputeShader.GenerateVec2Buffers(1, Size);	//	UVCoords
		ComputeShader.GenerateIndexBuffer(Faces);
		ComputeShader.GenerateCounterBuffers();

		//-----------------------------------------------------------------------------------------

		ComputeShader.Execute(256, 256, 1);

		//- Setup Validation shader ---------------------------------------------------------------

		ComputeShader.SetupComputeshader("TerrainGenerator");

		//		ComputeShader.Execute(256, 256, 1);

				//- Setup material ------------------------------------------------------------------------

		GLModel * pTerrain = NewModel();
		pTerrain->SetName("CSTerrain");
		pTerrain->SetBuffers(Size, Faces,
			ComputeShader.GetBufferID(0),	//	Vertices
			ComputeShader.GetBufferID(1),	//	Normals
			ComputeShader.GetBufferID(2),	//	Tangents
			ComputeShader.GetBufferID(3),	//	Bitangents 
			ComputeShader.GetBufferID(4),	//	UVCoords
			ComputeShader.GetBufferID(5));	//	Indices

		GLMaterial Material;
		Material.setShininess(25.0f);
		Material.setBumpmap(ReadBMPSurface("C:\\Projects\\Textures\\Lava 001\\Lava_001_Normals.bmp"));
		Material.setSpecular(ReadBMPSurface("C:\\Projects\\Textures\\Lava 001\\Lava_001_Specular.bmp"));

		pTerrain->LoadMaterial(Material);

		GLObject * const pShdObj = AddModel(
			pTerrain, scale(mat4(1.0), vec3(1.0)), (GLEntity*)nullptr,
			&MaterialFragShader1);

		return glOK;
	}

	glerror GLEngineDemo::SetupScene()
	{
		LogHeader("Setup scene");

		const color3& CA = color3(0.2f);
		const color3& CD = color3(0.6f, 0.6f, 0.6f);
		const color3& CS = CD; // color3(0.6f);

		GLOmniLight light = GLOmniLight(CA, CD, CS, 0.2f, 0.1f, 0.05f, mat4(1.0f), nullptr, RotateLight);

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

		//	Flash
		mat4 s = scale(mat4(1.0f), vec3(1.0f));
		GLObject* pFlashObj = AddModel(
			"flash", translate(mat4(1.0f), vec3(+0.f, .0f, .0f)) * s,
			(GLEntity*)nullptr, &MaterialShader);
		
		return glOK;
	}
}

int __stdcall WinMain(
	HINSTANCE				hInstance,
	HINSTANCE				hPrevInstance,
	LPSTR					lpCmdLine,
	int						nCmdShow)
{
	GLEngine::GLEngineDemo EngineDemo;

	return EngineDemo.Run();
}

int __cdecl main(void)
{
	GLEngine::GLEngineDemo EngineDemo;

	return EngineDemo.Run();
}
