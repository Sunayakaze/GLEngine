
#pragma once

#include "GLEngine.h"

namespace GLEngine
{
	class GLEngineDemo : public GLEngine
	{

	public:

	private:
		glerror								SetupSimpleComputeshader();
		glerror								SetupComputeshader();
		glerror								SetupControllers();
		glerror								SetupSubjects();
		glerror								SetupScene();

	};
}