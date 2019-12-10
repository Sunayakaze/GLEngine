
#include "..\GLEngine\General.h"

//#define HALFTEXRES
//#define WINDOWED


#ifndef WINDOWED

#define HRES						1920
#define VRES						1080

#define Global_size_x				80
#define Global_size_y				45
#define Global_size_z				1

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

#define HRES						(1920/2)
#define VRES						(1080/2)

#define Global_size_x				80
#define Global_size_y				45
#define Global_size_z				1

#define CreateGLFWWindow(Name)  {																\
	int count = 0;																				\
	GLFWmonitor** ppMonitor = glfwGetMonitors(&count);											\
	GLFWmonitor* pMonitor = ppMonitor[0];														\
	const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);										\
																								\
	pGLWin = glfwCreateWindow(HRES, VRES, Name, NULL, NULL);									\
}

#endif // WINDOWED

#ifdef HALFTEXRES
#define HTEX						(HRES/2)	//1024
#define VTEX						(VRES/2)	//1024
#else
#define HTEX						HRES
#define VTEX						VRES
#endif

//- Renderstate object --------------------------------------------------------

namespace GLGeneral
{
	struct RenderState
	{
		String					ShaderName;
		bufferid				FrameBuffer;
		uint					FragDataLocation;

		shdrid					VertShader;
		shdrid					FragShader;
		progid					ShaderProgram;
		
		vec2					Res;

		//	Output channels
		textureid				ChannelOut;

		//	Uniforms
		unifloc					uResolution;
		unifloc					uMouse;
		unifloc					uTime;

		unifloc					uChannel[MAXRENDERTARGETS];
		bufferid*				ChannelIn[MAXTEXTURES];

		RenderState()
			: FrameBuffer(0)
			, VertShader(0)
			, FragShader(0)
			, ShaderProgram(0)
			, Res(vec2(0.0))
			, ChannelOut(0)
			, uResolution(0)
			, uMouse(0)
			, uTime(0)
		{
			ShaderName.Clear();
			memset(&uChannel[0], 0, sizeof(uChannel));
			memset(&ChannelIn[0], 0, sizeof(ChannelIn));
		}

		void SetResolution(vec2& res)
		{
			this->Res = res;
		}

		void SetVertexShader(shdrid& id)
		{
			this->VertShader = id;
		}

		void GetShaderUniforms()
		{
			if (ShaderProgram)
			{
				glGetError(); //	Clear error stack

				GetUniformLocation(uResolution, ShaderProgram, "iResolution");
				GetUniformLocation(uMouse,		ShaderProgram, "iMouse");
				GetUniformLocation(uTime,		ShaderProgram, "iGlobalTime");

				GetUniformLocation(uChannel[0], ShaderProgram, "iChannel0");
				GetUniformLocation(uChannel[1], ShaderProgram, "iChannel1");
				GetUniformLocation(uChannel[2], ShaderProgram, "iChannel2");
				GetUniformLocation(uChannel[3], ShaderProgram, "iChannel3");
				GetUniformLocation(uChannel[4], ShaderProgram, "iChannel4");
				GetUniformLocation(uChannel[5], ShaderProgram, "iChannel5");
				GetUniformLocation(uChannel[6], ShaderProgram, "iChannel6");
				GetUniformLocation(uChannel[7], ShaderProgram, "iChannel7");
				GetUniformLocation(uChannel[8], ShaderProgram, "iChannel8");
				GetUniformLocation(uChannel[9], ShaderProgram, "iChannel9");
			} else	{
				LogError("No Program to get shaderlocations from.");
			}
		}

		void BindShaderLocations()
		{
			if (ShaderProgram) {
				BindFragDataLocation(0, ShaderProgram, "Color0");
			} else {
				LogError("No Program to bind fragdata locations to.");
			}
		}
	};

	GLFWwindow*					pGLWin;

	dvec2						MouseLoc;
	double						CurrTime;
	count						FrameCounter;
	count						NrDrawBuffers;
	uint						KeyPressed = 0;

	//	shaderid					MainShader;
	shdrid						VertexShader;

	//	Texture Channels
	cubemapid					Skybox;
	textureid					Textures[MAXTEXTURES] = { 0 };
	textureid					ChannelIn[MAXTEXTURES] = { 0 };

	//	FBO ID's
	bufferid					VAO;
	bufferid					FrameBuffer;
	bufferid					DepthBuffer;

	//	ChannelShaders used for setting up the Renderstates
	RenderState					Renderstates[MAXRENDERTARGETS + 1];
	RenderState					MainShader;

	//- Compute Shader Stuff -----------------------------------------------------

	textureid					ComputeShaderTextures[MAXTEXTURES];
	shdrid						ComputeShader;
	progid						ComputeProgram;
	unifloc						ComputeTime;
	unifloc						ComputeMouse;

	//- Functions and function prototypes ----------------------------------------

	glerror						SetupComputeShader();
	glerror						Initialize();
	glerror						InitializeResources();

	void						ClearRenderStates();
	void						ConfigureFragShader();
	glerror						SetupFrameBuffer();
	glerror						SetupRenderPhases();
	progid						SetupFragmentShaderProgram(RenderState&);

	void						HandleKeys();
	void						ProcessComputeShaders();
	void						Render();
	void						MainLoop();

	void						DestroyComputeShader();
	void						DestroyShaders();
	void						DestroyFrameBuffer();
	void						DestroyResources();
	void						TerminateApplication();

	void						GetWorkgroupLimits();
	void						LogOpenGLInfo();
}