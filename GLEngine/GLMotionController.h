
#pragma once

#include "GLData.h"

#define MOUSEENABLED

namespace GLEngine
{
	void RotateLight(const double& Time, mat4& Transform);
	void RotateCube(const double& Time, mat4& Transform);

	class GLCameraController
	{
	public:

		GLCameraController(GLFWwindow*);

		static void							MoveFirstPersonCamera(const double&, mat4&);
		static void							TranslateCamera(const double&, mat4&);
		static void							RotateCameraWithMouse(const double&, mat4&);
		static const mat4					SetViewMatrix(const vec3&, const vec3&);

	private:

		GLCameraController();		//	Prevent instantiation

		static vec3							camPos;
		static vec3							camRight;
		static vec3							camDir;
		static vec3							camUp;

		static vec3							TAcceleration;
		static vec3							TVelocity;

		static double						LastTime;

		static GLFWwindow*					lpGLFWin;			//	For getting keys
		static double						WindowHSize;
		static double						WindowVSize;
	};
}