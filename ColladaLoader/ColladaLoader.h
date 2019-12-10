
#pragma once

#include "..\GLEngine\GLEngine.h"

namespace GLEngine
{
	class ColladaLoader : public GLEngine
	{

	public:

		GLModel* const						LoadDAEModel(
			const char*							ModelName);

		virtual glerror						SetupControllers();
		virtual glerror						SetupSubjects();
		virtual glerror						SetupScene();

		virtual void						SetupFragShader() {}

	};
}