
#pragma once

#include "GLEntity.h"
#include "GLMotionController.h"

//-----------------------------------------------------------------------------
//- Debug switches ------------------------------------------------------------
//-----------------------------------------------------------------------------

//	Disable all for normal operation

#define GLEngine_DisableShadowmaps
#define GLEngine_DisableRenderedTextures
#define GLEngine_DisableHUD

#define FULLSCREEN
#define WINDOWED

#ifndef WINDOWED

#define HRES	1920
#define VRES	1080

#define CreateGLFWWindow(Name) {																\
	int count = 0;																				\
	GLFWmonitor** ppMonitor = glfwGetMonitors(&count);											\
	GLFWmonitor* pMonitor = ppMonitor[0];														\
	const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);										\
																								\
	glfwWindowHint(GLFW_RED_BITS, pMode->redBits);												\
	glfwWindowHint(GLFW_GREEN_BITS, pMode->greenBits);											\
	glfwWindowHint(GLFW_BLUE_BITS, pMode->blueBits);											\
	glfwWindowHint(GLFW_REFRESH_RATE, pMode->refreshRate);										\
	pGLWin = glfwCreateWindow(HRES, VRES, Name, pMonitor, NULL);								\
}

#else

#define HRES	(1920/2)
#define VRES	(1080/2)
#define CreateGLFWWindow(Name)  {																\
	int count = 0;																				\
	GLFWmonitor** ppMonitor = glfwGetMonitors(&count);											\
	GLFWmonitor* pMonitor = ppMonitor[0];														\
	const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);										\
																								\
	pGLWin = glfwCreateWindow(HRES, VRES, Name, NULL, NULL);									\
}

#endif // WINDOWED

#define HTEX						1024
#define VTEX						1024

//-----------------------------------------------------------------------------
//- Enable switches -----------------------------------------------------------
//-----------------------------------------------------------------------------

namespace GLEngine
{
	class GLEngine
	{

	public:

		GLEngine();

		LRESULT WINAPI						GLWinProc(
			HWND								hWnd,
			UINT								msg,
			WPARAM								wParam,
			LPARAM								lParam);

		virtual HRESULT						Run();

	protected:

		GLModel* const						Load3DMModel(
			const char*							ModelName);

		GLModel* const						LoadModel(
			const ModelData&					Model);

		GLModel* const						LoadModel(
			const MeshData&						Mesh,
			const GLMaterial&					Material);

		void								UpdateScene(
			const double						Time);

		void								SendLights();
		void								Render();

		//- Adding objects and subjects to Rendercontroller ---------------------------------------

		GLOmniLight* const AddObject(const GLOmniLight&	Light) {
			return (GLOmniLight*)Lights.Add(Light);
		}

		GLObject* const AddObject(const GLObject& Object) {
			return Objects.Add(Object);
		}

		template<class Sub> Sub* const AddSubject(const Sub& Shader) {
			return (Sub*)Subjects.Add(Shader);
		}

		GLModel* const NewModel()
		{
			return Models.Add(GLModel());
		}

		GLObject* const AddModel(
				GLModel* const					pModel,
				const mat4&						Transform,
				GLEntity*						pParentEntity,
				GL3DShader* const				pShader)
		{
			return AddObject(GLObject(pModel, Transform, pParentEntity, pShader));
		}

		GLObject* const AddModel(
			const char*							ModelName,
			const mat4&							Transform,
			GLEntity*							pParentEntity,
			GL3DShader* const					pShader)
		{
			if (GLModel * pModel = Load3DMModel(ModelName))
				return AddObject(GLObject(pModel, Transform, pParentEntity, pShader));

			return nullptr;
		}

/*		----- Obsolete, maybe -------------------------------------------------------------------
		
		template<class Obj> Obj* const AddObjectFront(const Obj& Object) {
			return (Obj*)GLRenderController::Objects.AddFront(Object);
		}

		template<class Sub>	Sub* const AddSubjectFront(const Sub& Shader) {
			return (Sub*)GLRenderController::Subjects.AddFront(Shader);
		}
*/
		//	Placeholders

		void								MainLoop();
		virtual glerror						Initialize();
		virtual void						Terminate();

		//	Pure virual setup functions
		virtual glerror						Setup();
		virtual glerror						SetupSubjects() = 0;
		virtual glerror						SetupScene() = 0;
		virtual glerror						SetupControllers() = 0;
		
		//	Message handler
		virtual BOOL						HandleMsg(MSG* pMsg);
		virtual void						LogOpenGLInfo();

		//- Variables -----------------------------------------------------------------------------

		GLFWwindow*							pGLWin;
		String								Caption;

		//	Shaders
		GLComputeShader						ComputeShader;
		GLMaterialShader					MaterialShader;
		GLMaterialFragShader				WireframeShader;
		GLMaterialFragShader				MaterialFragShader1;
		
		List<GLModel>						Models;
		List<GLSubject>						Subjects;
		List<GLObject>						Objects;
		List<GLLight>						Lights;

	};
}