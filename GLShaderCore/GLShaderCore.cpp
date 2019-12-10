
#include "GLShaderCore.h"
#include <vector>

namespace GLGeneral
{
	//- Computeshaders -----------------------------------------------------------

	void GetWorkgroupLimits()
	{
		int x = 0;	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &x);
		int y = 0;	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &y);
		int z = 0;	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &z);
		LogMessage("Max total workgroups count x:%i y:%i z:%i\n", x, y, z);

		x = 0;		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &x);
		y = 0;		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &y);
		z = 0;		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &z);
		LogMessage("Max total workgroups size x:%i y:%i z:%i\n", x, y, z);

		int m = 0;	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &m);
		LogMessage("Max compute workgroup invocations : %i\n", m);
	}

	glerror SetupComputeShader()
	{
//		return glOK;	//	Uncomment to skip setting up computeshader

		LogHeader("Computeshader Setup");
		
		GetWorkgroupLimits();

		if (ComputeShader = LoadShader(GL_COMPUTE_SHADER, "NoiseFlow"))
		{
			if (ComputeProgram = LinkShaders(1, ComputeShader))
			{
				GetUniformLocation(ComputeTime, ComputeProgram, "iGlobalTime");
				GetUniformLocation(ComputeMouse, ComputeProgram, "iMouse");

				const count pc = 80*45*24*24;

				bufferid ppos  = 0;
				bufferid pvel  = 0;

				GenerateShaderStorageBuffer(ppos, pc * sizeof(vec2));
				GenerateShaderStorageBuffer(pvel, pc * sizeof(vec2));

				//	initialize particle locations
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ppos);
				vec2* pBuffer = (vec2*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pc * sizeof(vec2), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
				for (count i = 0; i < pc; i++) { pBuffer[i] = vec2(rand()%HRES, rand()%VRES); }
				glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ppos);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pvel);

				GenerateComputeShaderTextures(1, ComputeShaderTextures);

				MainShader.ShaderName	= "ComputeshaderTextureShader";
				MainShader.ChannelIn[0] = &ComputeShaderTextures[0];

				return glOK;
			}
		}

		return glError;
	}
	
	//- Fraghaders ---------------------------------------------------------------

	void ConfigureFragShader()
	{
		LogHeader("Fragshader Configuration");
		
//		Skybox = LoadBMPCubeMap(
//			"C:\\Projects\\Textures\\CubemapHighRes\\CubemapHighRes-right.bmp",
//			"C:\\Projects\\Textures\\CubemapHighRes\\CubemapHighRes-left.bmp",
//			"C:\\Projects\\Textures\\CubemapHighRes\\CubemapHighRes-top.bmp",
//			"C:\\Projects\\Textures\\CubemapHighRes\\CubemapHighRes-bottom.bmp",
//			"C:\\Projects\\Textures\\CubemapHighRes\\CubemapHighRes-back.bmp",
//			"C:\\Projects\\Textures\\CubemapHighRes\\CubemapHighRes-front.bmp");

//		Textures[0] = ReadBMPSurface("C:\\Projects\\Textures\\pebbles.bmp");
//		Textures[0] = ReadBMPSurface("C:\\Projects\\Textures\\Galaxy.bmp");
//		Textures[1] = ReadBMPSurface("C:\\Projects\\Textures\\Alien_Muscle_001_SD\\Alien_Muscle_Diffuse.bmp");
//		Textures[2] = ReadBMPSurface("C:\\Projects\\Textures\\LeopardSkin.bmp");
//		Textures[3] = ReadBMPSurface("C:\\Projects\\Textures\\pebbles.bmp");
//		Textures[4] = ReadBMPSurface("C:\\Projects\\Textures\\WaterStream.bmp");
//		Textures[5] = ReadBMPSurface("C:\\Projects\\Textures\\Wood.bmp");
//		Textures[6] = ReadBMPSurface("C:\\Projects\\Textures\\RobotLove.bmp");

//		Renderstates[0].ShaderName = "EnergyDecelerator";
//		Renderstates[1].ShaderName = "Generators";
//		Renderstates[2].ShaderName = "Voidtemple";
//		Renderstates[3].ShaderName = "Voxeltunnel";

//		MainShader.ShaderName = "SilverKleinianSponge";
//		MainShader.ShaderName = "MultiChannel";
//		MainShader.ChannelIn[0] = &Renderstates[0].ChannelOut;
//		MainShader.ChannelIn[1] = &Renderstates[1].ChannelOut;
//		MainShader.ChannelIn[2] = &Renderstates[2].ChannelOut;
//		MainShader.ChannelIn[3] = &Renderstates[3].ChannelOut;

	}

	//- Engine part --------------------------------------------------------------

	// Generate a Texture for every Shader
	glerror SetupFrameBuffer()
	{
		glerror ret = glError;

		LogHeader("Framebuffer Setup");

		GLERROR();

		GenerateFrameBuffer(FrameBuffer);
		GenerateDepthAttachment(DepthBuffer);

		//	Generate offscreen buffer for each ChannelShader
		RenderState* pCurrState = &Renderstates[0];
		for (count bi = 0; !pCurrState[bi].ShaderName.IsEmpty(); bi++) {
			GenerateColorAttachment(pCurrState[bi].ChannelOut, bi);
			pCurrState[bi].FrameBuffer = FrameBuffer;
			pCurrState[bi].FragDataLocation = bi;
		}

		CheckFrameBufferStatus(ret);

		return ret;
	}

	glerror SetupFragmentShaderProgram(RenderState& rs)
	{
		if (VertexShader && !rs.ShaderName.IsEmpty()) {
			if (rs.FragShader = LoadShader(GL_FRAGMENT_SHADER, rs.ShaderName.Val())) {
				if (rs.ShaderProgram = LinkShaders(2, rs.VertShader, rs.FragShader)) {
					return glOK;
				}
			}
		}

		return glError;
	}

	glerror SetupRenderPhases()
	{
		LogHeader("Shader Setup");

		glerror ret = glError;

		if (!MainShader.ShaderName.IsEmpty())
		{
			glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

			GenerateScreenQuadVertexShader(VertexShader);

			RenderState* pCurrState = &Renderstates[0];
			while (!pCurrState->ShaderName.IsEmpty()) {
				pCurrState->SetVertexShader(VertexShader);
				if (!SetupFragmentShaderProgram(*pCurrState)) {
					pCurrState->SetResolution(vec2((float)HTEX, (float)VTEX));
					pCurrState->GetShaderUniforms();
					pCurrState->BindShaderLocations();
				}

				pCurrState++;
			}

			*pCurrState = MainShader;
			pCurrState->SetVertexShader(VertexShader);
			if (!SetupFragmentShaderProgram(*pCurrState)) {
				pCurrState->SetResolution(vec2((float)HRES, (float)VRES));
				pCurrState->GetShaderUniforms();
				pCurrState->BindShaderLocations();
				ret = glOK;
			}
		}
		else {
			LogError("No shader set.");
		}

		return ret;
	}

	glerror InitializeResources()
	{
		glerror ret = glError;

		LogHeader("Initializing resources");

		double starttime = glfwGetTime();

		ClearRenderStates();
		ConfigureFragShader();
		if (!SetupFrameBuffer() && !SetupComputeShader() && !SetupRenderPhases())
			ret = glOK;

		double time = glfwGetTime() - starttime;

		LogHeader("Done loading shaders in %.3f ms.", 1000.0*time);

		return ret;
	}

	HRESULT Run()
	{
		StartLogging();

		if (!Initialize())
		{
			GenerateVAO(VAO);

			if (!InitializeResources())
				MainLoop();

			DeleteVAO(VAO);
		}

		TerminateApplication();
		
		StopLogging();

		return true;
	}

	void ClearRenderStates()
	{
		MainShader = RenderState();
		for (count r = 0; r < MAXRENDERTARGETS; r++) {
			Renderstates[r] = RenderState();
		}
	}

	void HandleKeys()
	{
		glfwGetCursorPos(pGLWin, &MouseLoc.x, &MouseLoc.y);

		//	if (glfwGetKey(pGLWin, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) { }

		if (glfwGetKey(pGLWin, GLFW_KEY_F5) == GLFW_PRESS) {
			if (KeyPressed != GLFW_KEY_F5) {
				LogHeader("Reloading shaders");
				DestroyResources();
				InitializeResources();
				KeyPressed = GLFW_KEY_F5;
			}
		}

		if (glfwGetKey(pGLWin, GLFW_KEY_F5) == GLFW_RELEASE) {
			KeyPressed = 0;
		}
	}

	void ProcessComputeShaders()
	{
		if (ComputeProgram) {
			glGetError();

			UseProgram(ComputeProgram);
			glClear(GL_COLOR_BUFFER_BIT);

//			BindTexture(GL_TEXTURE_2D, ComputeShaderTexture);
			Send1FloatUniform(ComputeTime, (float)CurrTime);
			SendVec4Uniform(ComputeMouse, vec4(MouseLoc, vec2(0.0f)));
			glDispatchCompute(Global_size_x, Global_size_y, Global_size_z);

//			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}
	}

	void Render()
	{
		//- Render Channels -------------------------------------------------------

		RenderState* pCurrState = &Renderstates[0];
		while (pCurrState->FrameBuffer != 0)
		{
			const RenderState& rs = *pCurrState;

			glViewport(0, 0, HTEX, VTEX);
			BindFrameBuffer(rs.FrameBuffer);
			UseProgram(rs.ShaderProgram);

			glClear(GL_DEPTH_BUFFER_BIT);

			glDrawBuffer(GL_CA + rs.FragDataLocation);

			SendVec4Uniform(rs.uMouse, vec4(MouseLoc, vec2(0.0f)));
			SendVec2Uniform(rs.uResolution, rs.Res);
			Send1FloatUniform(rs.uTime, (float)CurrTime);

			SendCubeMapTexture(0, rs.uChannel[0], Skybox);
			for (uint j = Skybox ? 1 : 0; rs.ChannelIn[j] != nullptr; j++) {
				Send2DTexture(j, rs.uChannel[j], *rs.ChannelIn[j]);
			}

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			pCurrState++;
			
		}

		//-	Render Main Image -----------------------------------------------------

		const RenderState& rs = *pCurrState;

		glViewport(0, 0, HRES, VRES);
		BindFrameBuffer(0);
		UseProgram(rs.ShaderProgram);

		glClear(GL_DEPTH_BUFFER_BIT);

		SendVec4Uniform(rs.uMouse, vec4(MouseLoc, vec2(0.0f)));
		SendVec2Uniform(rs.uResolution, rs.Res);
		Send1FloatUniform(rs.uTime, (float)CurrTime);

		SendCubeMapTexture(0, rs.uChannel[0], Skybox);
		for (uint j = Skybox ? 1 : 0; rs.ChannelIn[j] != nullptr; j++) {
			Send2DTexture(j, rs.uChannel[j], *rs.ChannelIn[j]);
		}

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	void MainLoop()
	{
		CurrTime = glfwGetTime();

		double TimerStart = CurrTime,
			AppStart = CurrTime;

		double timeDiff = 0.0f;

		char Caption[] = "FragShader: 99999.99 frames per second";

		LogHeader("Render Start");

		glfwShowWindow(pGLWin);

		do
		{
			HandleKeys();

			ProcessComputeShaders();

			Render();

			FrameCounter++;
			CurrTime = glfwGetTime();

			timeDiff = CurrTime - TimerStart;

			if (timeDiff >= 1.0f)
			{
				float fps = FrameCounter / timeDiff;
				sprintf(Caption, "ShaderCore : %.2f frames per second", fps);

				glfwSetWindowTitle(pGLWin, Caption);

				LogMessage("frames : %.2f/s, %0.2f ms/frame", fps, 1000.0f / fps);

				TimerStart = CurrTime;
				FrameCounter = 0;
			}

			glfwSwapBuffers(pGLWin);
			glfwPollEvents();

		} while (
			(glfwGetKey(pGLWin, GLFW_KEY_ESCAPE) != GLFW_PRESS) &&
			!glfwWindowShouldClose(pGLWin));

		LogHeader("Render End");

	}

	glerror Initialize()
	{
		if (glfwInit())
		{
			SetupGLFWWindow(4, 6);
			CreateGLFWWindow("ShaderCore");

			if (!pGLWin) {
				SetupGLFWWindow(4, 5);
				CreateGLFWWindow("ShaderCore");
			}

			if (pGLWin) {
				FinalizeGLFWWindow();
				ConfigGLFWWindow();
				InitGLEW();
				LogOpenGLInfo();
				return glOK;
			}
			else {
				MessageBox(NULL, "Failed to create OpenGL window.", "Error", MB_ICONEXCLAMATION | MB_OK);
			}
		}

		return glError;
	}

	void DestroyComputeShader()
	{
		DetachShader(ComputeProgram, ComputeShader);
		DeleteShader(ComputeShader);
		DeleteProgram(ComputeProgram);
		DeleteTextures(ComputeShaderTextures);
	}

	void DestroyShaders()
	{
		RenderState* pstate = &Renderstates[0];
		while (pstate->ShaderProgram != 0) {
			
			DetachShader(pstate->ShaderProgram, pstate->FragShader);
			DetachShader(pstate->ShaderProgram, pstate->VertShader);
			DeleteShader(pstate->VertShader);
			DeleteShader(pstate->FragShader);
			DeleteProgram(pstate->ShaderProgram);
			pstate++;
		}
	}

	void DestroyFrameBuffer()
	{
		DeleteTexture(Skybox);
		DeleteTextures(Textures);

		for (count c = 0; !Renderstates[c].ShaderName.IsEmpty(); c++)
			DeleteTexture(Renderstates[c].ChannelOut);

		DeleteRenderbuffer(DepthBuffer);
		DeleteFramebuffer(FrameBuffer);
	}

	void DestroyResources()
	{
		LogHeader("Destroying resources");

		DestroyComputeShader();
		DestroyShaders();
		DestroyFrameBuffer();
	}

	void TerminateApplication()
	{
		DestroyResources();
		
		StopLogging();

		glfwTerminate();
	}

	void LogOpenGLInfo()
	{
		LogMessage("GLFW Version		: %s", (char*)glfwGetVersionString());
		LogMessage("Video driver		: %s", (char*)glGetString(GL_RENDERER));
		LogMessage("Video driver vendor	: %s", (char*)glGetString(GL_VENDOR));
		LogMessage("Shading Language	: %s", (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
		LogMessage("OpenGL Version		: %s", (char*)glGetString(GL_VERSION));

		int size = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &size);
		LogMessage("Total video memory	: %.2f MB", (size / 1024.0f));
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &size);
		LogMessage("Available memory	: %.2f MB", (size / 1024.0f));
		LogNothing();
	}

	//- Test stuff --------------------------------------------------------------------------------

	void TestBindTimes()
	{
		double start, end;

		//-	glBindFramebuffer ---------------------------------------------------------------------

		start = glfwGetTime();

		for (uint fb = 0; fb < 1000000; fb++) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 1);
			glBindFramebuffer(GL_FRAMEBUFFER, 2);
			glBindFramebuffer(GL_FRAMEBUFFER, 3);
			glBindFramebuffer(GL_FRAMEBUFFER, 4);
			glBindFramebuffer(GL_FRAMEBUFFER, 5);
			glBindFramebuffer(GL_FRAMEBUFFER, 6);
			glBindFramebuffer(GL_FRAMEBUFFER, 7);
			glBindFramebuffer(GL_FRAMEBUFFER, 8);
			glBindFramebuffer(GL_FRAMEBUFFER, 9);
		}

		end = (glfwGetTime() - start) / 10.0l;

		LogMessage("<br><b>BindFramebuffer times : %lf</b> MicroSeconds", end);

		//- glBindTexture -------------------------------------------------------------------------

		start = glfwGetTime();

		for (uint fb = 0; fb < 1000000; fb++) {
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, 1);
			glBindTexture(GL_TEXTURE_2D, 2);
			glBindTexture(GL_TEXTURE_2D, 3);
			glBindTexture(GL_TEXTURE_2D, 4);
			glBindTexture(GL_TEXTURE_2D, 5);
			glBindTexture(GL_TEXTURE_2D, 6);
			glBindTexture(GL_TEXTURE_2D, 7);
			glBindTexture(GL_TEXTURE_2D, 8);
			glBindTexture(GL_TEXTURE_2D, 9);
		}

		end = (glfwGetTime() - start) / 10.0l;

		LogMessage("<br><b>glBindTexture times : %lf</b> MicroSeconds", end);

		//-----------------------------------------------------------------------------------------
	}
}

int __stdcall WinMain(
	HINSTANCE				hInstance,
	HINSTANCE				hPrevInstance,
	LPSTR					lpCmdLine,
	int						nCmdShow)
{
	return GLGeneral::Run();
}

HRESULT main(int NrArgs, char* Args[])
{
	if (NrArgs > 1)
		GLGeneral::Renderstates[0].ShaderName = Args[1];

	return GLGeneral::Run();
}
