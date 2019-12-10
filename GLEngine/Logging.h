
#pragma once

void _StartLogging();
void _StopLogging();
void _LogHeader(const char* Message, ...);
void _LogError(const char* Message, ...);
void _LogWarning(const char* Message, ...);
void _LogMessage(const char* Message, ...);
void _LogStats(const char* Message, ...);
void _LogNothing();

void DumpMatrices(const float* pMatrices, int NrMatrices);

#ifndef NDEBUG

#define GLERROR()																		\
{																						\
	switch (glGetError())																\
	{																					\
	case GL_INVALID_OPERATION:															\
		LogError("Error : %s,%d : Invalid Operation",									\
			__FILE__, __LINE__);														\
		break;																			\
																						\
	case GL_INVALID_ENUM:																\
		LogError("Error : %s,%d : Invalid Enumeration",									\
			__FILE__, __LINE__);														\
		break;																			\
																						\
	case GL_INVALID_VALUE:																\
		LogError("Error : %s,%d : Invalid value",										\
			__FILE__, __LINE__);														\
		break;																			\
																						\
	case GL_OUT_OF_MEMORY:																\
		LogError("Error : %s,%d : Out of memory",										\
			__FILE__, __LINE__);														\
		break;																			\
																						\
	case GL_INVALID_FRAMEBUFFER_OPERATION:												\
		LogError("Error : %s,%d : Invalid framebuffer operation",						\
			__FILE__, __LINE__);														\
		break;																			\
	}																					\
}

#define StartLogging()	_StartLogging();
#define StopLogging()	_StopLogging();
#define LogHeader(...)	_LogHeader(__VA_ARGS__);
#define LogError(...)	_LogError(__VA_ARGS__);
#define LogWarning(...) _LogWarning(__VA_ARGS__);
#define LogMessage(...) _LogMessage(__VA_ARGS__);
#define LogStats(...)	_LogStats(__VA_ARGS__);
#define LogNothing()	_LogNothing();

#else

#define GLERROR()		{}
#define StartLogging()	{}
#define StopLogging()	{}
#define LogHeader(...)	{}
#define LogError(...)	{}
#define LogWarning(...)	{}
#define LogMessage(...)	{}
#define LogStats(...)	{}
#define LogNothing()	{}

#endif	// _DEBUG
