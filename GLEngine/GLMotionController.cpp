
#include "GLMotionController.h"

using glm::rotate;

//	Movement per second
#define Acceleration		5.0f

#define MouseSpeed			20.0f
#define MaxVelocity			0.2f

#define Force				2.0f	//	Translation Speed
#define RForce				0.1f
#define Friction			0.0f		
#define Gravity				9.80665l

namespace GLEngine
{
	//- Cameramove declarations ---------------------------------------------------

	vec3		GLCameraController::camPos = vec3(0.0f);
	vec3		GLCameraController::camDir = vec3(0.0f);
	vec3		GLCameraController::camUp = vec3(0.0f);
	vec3		GLCameraController::camRight = vec3(0.0f);

	vec3		GLCameraController::TAcceleration = vec3(0.0f);
	vec3		GLCameraController::TVelocity = vec3(0.0f);

	double		GLCameraController::LastTime = 0.0l;

	GLFWwindow*	GLCameraController::lpGLFWin = nullptr;
	double		GLCameraController::WindowHSize = 0;
	double		GLCameraController::WindowVSize = 0;

	//- Transformation Controllers ------------------------------------------------

	GLCameraController::GLCameraController(GLFWwindow* lpGLFWwin)
	{
		lpGLFWin = lpGLFWwin;

		int h = 0, v = 0;
		glfwGetWindowSize(lpGLFWin, &h, &v);

		WindowHSize = (double)h;
		WindowVSize = (double)v;

		glfwSetCursorPos(lpGLFWin, WindowHSize / 2.0, WindowVSize / 2.0);
	}

	const mat4 GLCameraController::SetViewMatrix(const vec3& Position, const vec3& Lookat)
	{
		camPos = Position;
		camDir = normalize(Lookat - Position);
		camRight = normalize(cross(camDir, YAxis3));
		camUp = normalize(cross(camRight, camDir));

		return lookAt(camPos, camPos + camDir, camUp);
	}
	
	void GLCameraController::RotateCameraWithMouse(const double& Time, mat4& Transform)
	{
		double timeDelta = Time - LastTime;

		double hh = WindowHSize / 2.0;
		double hv = (double)(uint)(WindowVSize / (uint)2.0);

		double lh = 0.0, lv = 0.0;
		glfwGetCursorPos(lpGLFWin, &lh, &lv);
		glfwSetCursorPos(lpGLFWin, hh, hv);

		double rx = MouseSpeed * timeDelta * (lh - hh);
		double ry = MouseSpeed * timeDelta * (lv - hv);

		mat4 RVert = rotate(mat4(1.0f), (float)ry, camRight);
		mat3 iView = (mat3)transpose(rotate(RVert, (float)rx, YAxis3));
		camRight = iView * camRight;
		camUp = iView * camUp;
		camDir = iView * camDir;
	}

	void GLCameraController::TranslateCamera(const double& Time, mat4& Transform)
	{
		double timeDelta = Time - LastTime;

		if (glfwGetKey(lpGLFWin, GLFW_KEY_D) == GLFW_PRESS) {				//	Move right
			TAcceleration += Acceleration * camRight;
		}

		if (glfwGetKey(lpGLFWin, GLFW_KEY_A) == GLFW_PRESS) {				//	Move left
			TAcceleration -= Acceleration * camRight;
		}

		if (glfwGetKey(lpGLFWin, GLFW_KEY_W) == GLFW_PRESS) {				//	Move forward
			TAcceleration += Acceleration * camDir;
		}

		if (glfwGetKey(lpGLFWin, GLFW_KEY_S) == GLFW_PRESS) {				//	Move backward
			TAcceleration -= Acceleration * camDir;
		}

		if (glfwGetKey(lpGLFWin, GLFW_KEY_R) == GLFW_PRESS) {				//	Move Upwards
			TAcceleration += Acceleration * vec3(0.0f, 1.0f, 0.0f);
		}

		if (glfwGetKey(lpGLFWin, GLFW_KEY_F) == GLFW_PRESS) {				//	Move Downwards
			TAcceleration -= Acceleration * vec3(0.0f, 1.0f, 0.0f);
		}

		TVelocity = ((float)timeDelta * TAcceleration);
		TAcceleration = vec3(0.0f);

		camPos += TVelocity;
	}

	void GLCameraController::MoveFirstPersonCamera(const double& Time, mat4& Transform)
	{
		RotateCameraWithMouse(Time, mat4(0.0));
		TranslateCamera(Time, mat4(0.0));

		Transform = lookAt(camPos, camPos + camDir, camUp);

		LastTime = Time;
	}

	void RotateLight(const double& Time, mat4& Transform)
	{
		float time = (float)Time * 50.0f;

		const mat4 r = rotate(mat4(1.0f), time, vec3(0.0f, 1.0f, 0.0f));
		const mat4 t = translate(mat4(1.0f), vec3(7.0f, 4.0f, 0.0f));

		Transform = r * t;
	}

	void RotateCube(const double& Time, mat4& Transform)
	{
		float time = (float)Time;

		Transform = rotate(mat4(1.0f), time * 20, vec3(0, 1, 0));
	}
}