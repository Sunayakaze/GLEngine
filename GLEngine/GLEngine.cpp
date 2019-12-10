
//	Source file for OpenGL Host App

#include "GLEngine.h"

namespace GLEngine
{
	void GLEngine::UpdateScene(
		const double						Time)
	{
		ListNode<GLObject>* pCurrentObject = Objects.GetFirstItem();
		while (pCurrentObject != nullptr) {
			pCurrentObject->GetNodeDataPointer()->Update(Time);
			pCurrentObject = pCurrentObject->getNext();
		}

		ListNode<GLSubject>* pCurrentSubject = Subjects.GetFirstItem();
		while (pCurrentSubject != nullptr) {
			pCurrentSubject->GetNodeDataPointer()->Update(Time);
			pCurrentSubject = pCurrentSubject->getNext();
		}

		//	Update skeletons per model
		ListNode<GLModel>* pCurrNode = Models.GetFirstItem();
		while (pCurrNode != nullptr) {
			pCurrNode->GetNodeDataPointer()->UpdateJointMatrices((float)Time);
			pCurrNode = pCurrNode->getNext();
		};
	}

	GLModel* const GLEngine::Load3DMModel(
		const char*							ModelName)
	{
		ModelData mesh;

		if (!mesh.ReadFromDisk(ModelName)) {
			return LoadModel(mesh);
		}

		LogError("Error reading '%s.3dm'.", ModelName);

		return nullptr;
	}

	GLModel* const GLEngine::LoadModel(
		const MeshData& Mesh,
		const GLMaterial& Material)
	{
		LogMessage("Loading model %s", Mesh.GetName());

		GLModel* pModel = NewModel();
		pModel->LoadMesh(Mesh);
		pModel->LoadMaterial(Material);

		return pModel;
	}

	GLModel* const GLEngine::LoadModel(
		const ModelData&					Model)
	{
		LogMessage("Loading model %s", Model.GetName());

		const Library<MeshData>& MeshLib = Model.GetMeshData();
		const Library<GLSurface>& SurfLib = Model.GetSurfaceData();
		const Library<GLMaterial>& MatLib = Model.GetMaterialData();
		const Library<BoneData>& BoneLib = Model.GetBoneData();
		const Library<Animations>& AnimLib = Model.GetAnimations();

		//	Set up new model
		uint SurfaceOffset = 0;
		GLModel* pModel = NewModel();
		pModel->SetName(Model.GetName());
		pModel->LoadBoneLib(BoneLib);
		pModel->LoadAnimLib(AnimLib);
		pModel->LoadMeshLib(MeshLib);					//	Has references to bones so order matters
		pModel->LoadSurfLib(SurfaceOffset, SurfLib);
		pModel->LoadMatLib(SurfaceOffset, MeshLib, MatLib);

		return pModel;
	}
	
	void GLEngine::SendLights()
	{
		ListNode<GLLight>* pCurrLight = Lights.GetFirstItem();
		while (pCurrLight != nullptr)
		{
			const GLLight* pLight = pCurrLight->GetNodeDataPointer();

			pLight->SendLightToShader();
			pCurrLight = pCurrLight->getNext();
		}
	}

	void GLEngine::Render()
	{
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ComputeShader.Execute(256, 256, 1);

		GLRenderController::setSubject(Subjects.GetFirstItem()->GetNodeDataPointer());

		ListNode<GLObject>* pCurrNode = Objects.GetFirstItem();
		while (pCurrNode != nullptr) {
			GLObject* pCurrObj = pCurrNode->GetNodeDataPointer();
			GLRenderController::setShader(pCurrObj->GetShaderPointer());

			SendLights();
			pCurrObj->Render();
			pCurrNode = pCurrNode->getNext();
		}
	}

	//- Main Run function ---------------------------------------------------------

	void GLEngine::MainLoop()
	{
		LogHeader("Render Start");

		glfwShowWindow(pGLWin);

		count Frame	= 0;
		double timeCurr = 0.0l, timeLast = 0.0l;

#ifdef	PROFILING

		char cap[120];
		count frameLast=0, frameCurr=0, deltaFrame=0;
		double deltaTime=0.0l;
		double interval = 0.25l;

#endif//PROFILING

		glfwSetTime(timeCurr);	//	Render start = T0;

		do
		{
			timeCurr = glfwGetTime();

			UpdateScene(timeCurr);

			Render();

			glfwSwapBuffers(pGLWin);
			glfwPollEvents();

#ifdef	PROFILING

			deltaTime = timeCurr - timeLast;
			if (deltaTime >= interval) {
				deltaFrame = Frame - frameLast;
				sprintf(cap, "%s : %.2f frames per second", GLEngine::Caption.Val(), (deltaFrame / deltaTime));
				glfwSetWindowTitle(pGLWin, cap);
				timeLast = timeCurr;
				frameLast = Frame;
			}

#endif//PROFILING

			Frame++;

			glDisable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(nullptr, 0);

		} while (
			(glfwGetKey(pGLWin, GLFW_KEY_ESCAPE) != GLFW_PRESS) &&
			!glfwWindowShouldClose(pGLWin));

		LogHeader("Render End");

		//	Log framerate data
		double FPS = (double)Frame / glfwGetTime();

		LogNothing();
		LogMessage("Number of frames drawn	: <b>%d</b>", Frame);
		LogMessage("Uptime					: <b>%f</b>", timeLast);
		LogMessage("Average FPS				: <b>%f</b>", FPS);
	}

	glerror GLEngine::Setup()
	{
		if (!SetupControllers() && !SetupSubjects() && !SetupScene())
			return glOK;

		return glError;
	}

	HRESULT GLEngine::Run()
	{
		bufferid VAO = 0;
		HRESULT ret = 0;

		StartLogging();

		if (!(ret = Initialize()))
		{
			GenerateVAO(VAO);

			if(!Setup())
				MainLoop();

			Terminate();
			DeleteVAO(VAO);

		}

		glfwTerminate();
		StopLogging();

		return ret;

	}

	void GLEngine::LogOpenGLInfo()
	{
		//	List Supported Extensions.
		LogMessage("Supported Extensions : %s", glGetString(GL_EXTENSIONS));
		LogMessage("GLFW Version         : %s", (char*)glfwGetVersionString());
		LogMessage("Video driver         : %s", Caption.Val());
		LogMessage("Video driver vendor  : %s", (char*)glGetString(GL_VENDOR));
		LogMessage("Shading Language     : %s", (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
		LogMessage("OpenGL Version       : %s", (char*)glGetString(GL_VERSION));

		int size = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &size); // Get total videomem
		LogMessage("Total video memory   : %i MB", size / 1024);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &size); // Get available videomem
		LogMessage("Available memory     : %i MB", size / 1024);

		int winWidth, winHeight;
		glfwGetWindowSize(pGLWin, &winWidth, &winHeight);
		LogMessage("\n%i x %i window created", winWidth, winHeight);
	}

	glerror GLEngine::Initialize()
	{
		if (glfwInit())
		{
			SetupGLFWWindow(4,6);
			CreateGLFWWindow("GLEngine");

			if (!pGLWin) {
				SetupGLFWWindow(4,5);
				CreateGLFWWindow("GLEngine");
			}

			if (pGLWin)
			{
				EnableAntiAliasing();
				FinalizeGLFWWindow();
				ConfigGLFWWindow();
				InitGLEW();
				LogOpenGLInfo();
				glGetError();
				return glOK;
			} else {
				MessageBox(NULL, "Failed to create OpenGL window.", "Error", MB_ICONEXCLAMATION | MB_OK);
			}
		}

		return glError;
	}
	
	void GLEngine::Terminate()
	{
		ComputeShader.Destroy();
		MaterialShader.Destroy();
		MaterialFragShader1.Destroy();

		ListNode<GLModel>* pCurrentModel = Models.GetFirstItem();
		while (pCurrentModel != nullptr) {
			pCurrentModel->GetNodeDataReference().Destroy();
			pCurrentModel->Destroy();
			pCurrentModel = pCurrentModel->getNext();
		};
	}

	//- Class Constructors and destructors ----------------------------------------

//	void GLEngine::Construct()
//	{
//		
//	}

	GLEngine::GLEngine()
	{
		pGLWin = nullptr;
	}

	//- Windows stuff -------------------------------------------------------------

	LRESULT CALLBACK GLEngine::GLWinProc(
		HWND								hwnd,
		UINT								msg,
		WPARAM								wparam,
		LPARAM								lparam)
	{
		switch (msg)
		{
		case WM_DESTROY: //afsluiten
		{
			PostQuitMessage(0);
			return 0;
		} break;

		default:
			break;
		}

		return DefWindowProc(hwnd, msg, wparam, lparam);
	};

	BOOL GLEngine::HandleMsg(
		MSG*								pMsg)
	{
		if (pMsg->message == WM_KEYDOWN &&
			pMsg->wParam == VK_ESCAPE)
			return FALSE;
		return TRUE;
	}
}