
#include "GLBarebones.h"

using std::vector;

namespace GLGeneral
{
	glerror SetupGeometry()
	{
		glerror error = glError;

		if (!Load3DModel("NoiseIcoSphere",
			Vertices, Texcoords, Normals, Tangents, Bitangents,
			Indices, NrTriangles,
			DiffuseMap, DiffuseMap, NormalMap, SpecularMap))
		{
			DiffuseColor = vec3(0.8f, 0.0f, 0.8f);
			SpecularColor = vec3(1.0f, 1.0f, 1.0f);

			Shininess = 30.0f;

			DiffuseMap = ReadBMPSurface("C:\\Projects\\Textures\\Crystal_002_SD\\Crystal_002_Diffuse.bmp");
			NormalMap = ReadBMPSurface("C:\\Projects\\Textures\\Crystal_002_SD\\Crystal_002_Normals.bmp");
			SpecularMap = ReadBMPSurface("C:\\Projects\\Textures\\Crystal_002_SD\\Crystal_002_Specular.bmp");

			error = glOK;
		}

		return error;
	}

	glerror SetupShaders()
	{
		uint VertShaderID, FragShaderID;

		glerror error = glError;
		
		if (VertShaderID = LoadShader(GL_VERTEX_SHADER, "Default"))
		{
			if (FragShaderID = LoadShader(GL_FRAGMENT_SHADER, "Default"))
			{
				if (SpecularShader = LinkShaders(2, VertShaderID, FragShaderID))
				{
					//	Attributes
					GetAttribLocation(aPosition, SpecularShader, "Position");
					GetAttribLocation(aTexCoord, SpecularShader, "TexCoord");
					GetAttribLocation(aNormal, SpecularShader, "Normal");
					GetAttribLocation(aTangent, SpecularShader, "Tangent");
					GetAttribLocation(aBitangent, SpecularShader, "Bitangent");

					GetUniformLocation(uMVP, SpecularShader, "MVP");
					GetUniformLocation(uMV, SpecularShader, "MV");
					GetUniformLocation(uN, SpecularShader, "N");
					GetUniformLocation(uP, SpecularShader, "P");

					//	Texture uniforms
					GetUniformLocation(uDiffuseColor, SpecularShader, "DiffuseColor");
					GetUniformLocation(uAmbientColor, SpecularShader, "AmbientColor");
					GetUniformLocation(uSpecularColor, SpecularShader, "SpecularColor");
					GetUniformLocation(uDiffuseMap, SpecularShader, "DiffuseMap");
					GetUniformLocation(uNormalMap, SpecularShader, "NormalMap");
					GetUniformLocation(uSpecularMap, SpecularShader, "SpecularMap");
					GetUniformLocation(uShininess, SpecularShader, "Shininess");

					//	Light uniforms
					GetUniformLocation(ulPosition, SpecularShader, "lPosition");
					GetUniformLocation(ulAmbient, SpecularShader, "lAmbient");
					GetUniformLocation(ulDiffuse, SpecularShader, "lDiffuse");
					GetUniformLocation(ulSpecular, SpecularShader, "lSpecular");
					GetUniformLocation(ulCAttenuation, SpecularShader, "lCAttenuation");
					GetUniformLocation(ulLAttenuation, SpecularShader, "lLAttenuation");
					GetUniformLocation(ulQAttenuation, SpecularShader, "lQAttenuation");

					glBindFragDataLocation(SpecularShader, 0, "Color0");

					//	Relink for the output locations
					glLinkProgram(SpecularShader);

					glUseProgram(SpecularShader);
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
					glViewport(0, 0, HRES, VRES);

					error = glOK;
				}
			}
		}

		return error;
	}

	void SetupMatrices()
	{
		Proj = perspective(
			45.0f,																//	45 degree vision
			(float)VRES / HRES,													//	Aspect ratio
			0.1f,																//	range close
			200.0f);															//	range far

		glUniformMatrix4fv(uP, 1, GL_FALSE, &Proj[0][0]);

		//	Camera matrix
		vec3 camPos = vec3(0.0f, 0.0f, 4.0f);									//	front
		vec3 camLookAt = vec3(0.0f, 0.0f, 0.0f);

		vec3 camDir = normalize(camLookAt - camPos);
		vec3 camRight = normalize(cross(camDir, vec3(0.0f, 1.0f, 0.0f)));		//	No upfacing camera
		vec3 camUp = normalize(cross(camRight, camDir));

		View = lookAt(camPos, camLookAt, camUp);
	}

	void SetupLights()
	{
		vec4 Position = vec4(3.0f, 3.0f, 3.0f, 1.0f);							//	Lit from front
		vec3 Ambient = vec3(0.2f, 0.2f, 0.2f);
		vec3 Diffuse = vec3(1.0f, 1.0f, 1.0f);
		vec3 Specular = vec3(1.0f, 1.0f, 1.0f);

		float CAttenuation = 0.01f;
		float LAttenuation = 0.02f;
		float QAttenuation = 0.01f;

		//	Convert to Eyespace
		Position = View * Position;

		//	Upload to GPU
		SendVec3Uniform(ulPosition, Position);
		SendVec3Uniform(ulAmbient, Ambient);
		SendVec3Uniform(ulDiffuse, Diffuse);
		SendVec3Uniform(ulSpecular, Specular);
		Send1FloatUniform(ulCAttenuation, CAttenuation);
		Send1FloatUniform(ulLAttenuation, LAttenuation);
		Send1FloatUniform(ulQAttenuation, QAttenuation);
	}

	void HandleKeys()
	{

	}

	void Render()
	{
		//	Rotate cube
		float time = (float)glfwGetTime();
		Model = rotate(mat4(1.0f), time * 5, vec3(1, 0, 0)) *
			rotate(mat4(1.0f), time * 7, vec3(0, 1, 0)) *
			rotate(mat4(1.0f), time * 2, vec3(0, 0, 1));

		//	Set Matrices
		mat4 matMV = View * Model;
		mat4 matMVP = Proj * View * Model;
		mat4 matN = transpose(inverse(matMV));

		SendMat4Uniform(uMV, &matMV[0][0]);
		SendMat4Uniform(uMVP, &matMVP[0][0]);
		SendMat4Uniform(uN, &matN[0][0]);

		Bind3Float(aPosition, Vertices);
		Bind2Float(aTexCoord, Texcoords);
		Bind3Float(aNormal, Normals);
		Bind3Float(aTangent, Tangents);
		Bind3Float(aBitangent, Bitangents);

		SendVec3Uniform(uDiffuseColor, DiffuseColor);
		SendVec3Uniform(uAmbientColor, AmbientColor);
		SendVec3Uniform(uSpecularColor, SpecularColor);
		Send1FloatUniform(uShininess, Shininess);

		Send2DTexture(0, uDiffuseMap, DiffuseMap);
		Send2DTexture(1, uNormalMap, NormalMap);
		Send2DTexture(2, uSpecularMap, SpecularMap);

		//	Set Renderparameters
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);													GLERROR();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Indices);														GLERROR();
		glDrawElements(GL_TRIANGLES, NrTriangles * 3, GL_UNSIGNED_SHORT, nullptr);							GLERROR();

		glfwSwapBuffers(pGLWin);																			GLERROR();
		glfwPollEvents();																					GLERROR();
	}

	glerror MainLoop()
	{
		do
		{
			Render();

		} while (
			(glfwGetKey(pGLWin, GLFW_KEY_ESCAPE) != GLFW_PRESS) &&
			!glfwWindowShouldClose(pGLWin));

		return glOK;
	}

	void LogOpenGLInfo()
	{
		//	List Supported Extensions.
		LogMessage("Extensions		: %s", (char*)glGetString(GL_EXTENSIONS));
		LogMessage("Video driver		: %s", (char*)glGetString(GL_RENDERER));
		LogMessage("Video driver vendor	: %s", (char*)glGetString(GL_VENDOR));
		LogMessage("OpenGL Version		: %s", (char*)glGetString(GL_VERSION));
		LogMessage("Shading Language	: %s", (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
		LogMessage("GLFW Version		: %s", (char*)glfwGetVersionString());

		int size = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &size); // Get total videomem
		LogMessage("Total video memory	: %i MB", size / 1024);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &size); // Get available videomem
		LogMessage("Available memory	: %i MB", size / 1024);
	}

	glerror Initialize()
	{
		if (!glfwInit()) {
			MessageBox(NULL, "Unable to init GLFW", "Error", MB_OK);
			return false;
		}

		glfwWindowHint(GLFW_FOCUSED, 1);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLMAJORVERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLMINORVERSION);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		if (!(pGLWin = glfwCreateWindow(HRES, VRES, "GLEngine Barebones", NULL, NULL))) {
			MessageBox(NULL, "Failed to create OpenGL window.", "Error", MB_OK);
			glfwTerminate();
			return glError;
		}

		glfwMakeContextCurrent(pGLWin);

		glewExperimental = GL_TRUE;
		uint err = glewInit();
		if (err != GLEW_OK) {
			MessageBox(NULL, "Failed to init GLEW.", "Error", MB_OK);
			glfwTerminate();
			return glError;
		}

		LogOpenGLInfo();

		glfwSetWindowTitle(pGLWin, (char*)glGetString(GL_RENDERER));
		glfwPollEvents();

		//	Enable keyboard and mouse input
		glfwSetInputMode(pGLWin, GLFW_STICKY_KEYS, GL_TRUE);
		glfwGetInputMode(pGLWin, GLFW_STICKY_MOUSE_BUTTONS);

		//	Set the window color
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

		//	Clear the initial window, to make the app come off less buggy.
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(pGLWin);

		//	Enable Z-Buffering
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		//	Enable transparency
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);

		//	Enable backface culling
		glEnable(GL_CULL_FACE);
		glEnable(GL_BACK);
		
		glGetError();

		return glOK;
	}

	void Destroy()
	{
		glDisableVertexAttribArray(aPosition);
		glDisableVertexAttribArray(aTexCoord);
		glDisableVertexAttribArray(aNormal);
		glDisableVertexAttribArray(aTangent);
		glDisableVertexAttribArray(aBitangent);

		glDeleteBuffers(1, &Vertices);
		glDeleteBuffers(1, &Texcoords);
		glDeleteBuffers(1, &Normals);
		glDeleteBuffers(1, &Tangents);
		glDeleteBuffers(1, &Bitangents);
		glDeleteBuffers(1, &Indices);

		glDeleteProgram(SpecularShader);

		glfwTerminate();
	}

	glerror Run()
	{
		bufferid VAO = 0;
		HRESULT ret = 1;

		StartLogging();

		if ((Initialize() == glOK) && (SetupShaders() == glOK))
		{
			GenerateVAO(VAO);

			SetupMatrices();
			SetupLights();

			if (SetupGeometry() == glOK)
			{
				ret = MainLoop();
			}

			DeleteVAO(VAO);
		}

		Destroy();

		StopLogging();

		return ret;
	}
}

HRESULT main(int NrArgs, char* Args[])
{
	return GLGeneral::Run();
}

int __stdcall WinMain(
	HINSTANCE				hInstance,
	HINSTANCE				hPrevInstance,
	LPSTR					lpCmdLine,
	int						nCmdShow)
{
	return GLGeneral::Run();
}
