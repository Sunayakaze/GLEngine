
#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>

#include "Library.h"
#include "Logging.h"
#include "String.h"

//	Disable fopen warning.
#pragma warning (disable : 4996)

//	Texture stuff
#define	MAXRENDERTARGETS				10
#define MAXTEXTURES						16
#define MAXPROGRAMS						2
#define MAXSHADERS						20
#define MAXBUFFERS						20

#define GLMAJORVERSION					4
#define GLMINORVERSION					5

#define MAXPATHDEPTH					4

//	BMP File errors
#define glBMPFileOpenError				0x00001001
#define glBMPFileHeaderError			0x00001002
#define glBMPFileHeaderSizeError		0x00001003
#define glBMPFileContentError			0x00001004

//	Material Types
#define HasAmbientMap					0x00000001		//	2^0
#define HasDiffuseMap					0x00000002		//	2^1
#define HasBumpMap						0x00000004		//	2^2
#define HasSpecularMap					0x00000008		//	2^3
#define HasRenderedTexture				0x00000010		//	2^4

#define HasSurface0						0x00000001
#define HasSurface1						0x00000002
#define HasSurface2						0x00000004
#define HasSurface3						0x00000008
#define HasSurface4						0x00000010
#define HasSurface5						0x00000020
#define HasSurface6						0x00000040
#define HasSurface7						0x00000080
#define HasSurface8						0x00000100
#define HasSurface9						0x00000200

//	Light Types
#define Light_Point						0x00000001
#define Light_Directional				0X00000002

//	Disk Operation IDs - Libraries
#define LibraryMeshData					0x44454D4C		//	LMED
#define LibrarySurfaceData				0x4455534C		//	LSUD
#define LibraryMaterialData				0x44414D4C		//	LMAD
#define LibraryBoneData					0x444F424C		//	LBOD
#define LibraryAnimations				0x494E414C		//	LANI

//	Disk Operation IDs - Chunks
#define ChunkMeshDataBounds				0x3042444D		//	MDB0
#define ChunkMeshDataVertices			0x3356444D		//	MDV3
#define ChunkMeshDataNormals			0x334E444D		//	MDN3
#define ChunkMeshDataTexCoords			0x3255444D		//	MDU2
#define ChunkMeshDataTangents			0x3354444D		//	MDT3
#define ChunkMeshDataBitangents			0x3342444D		//	MDB3
#define ChunkMeshDataPolygons			0x3350444D		//	MDP3
#define ChunkMeshDataPolyList			0x4C50444D		//	MDPL
#define ChunkMeshDataIndices			0x3449444D		//	MDI4
#define ChunkMeshDataWeights1			0x3157444D		//	MDW1
#define ChunkMeshDataWeights2			0x3257444D		//	MDW2
#define ChunkMeshDataWeights3			0x3357444D		//	MDW3
#define ChunkMeshDataWeights4			0x3457444D		//	MDW4
#define ChunkMeshDataEnd				0x3045444D		//	MDE0
#define ChunkSurfaceData				0x31304453		//	SD01
#define ChunkMaterialData				0x3130444D		//	MD01
#define ChunkBoneData					0x31304442		//	BD01
#define ChunkAnimationData				0x31304441		//	AD01
#define ChunkAnimationInput				0x50494441		//	ADIP
#define ChuckAnimationOutput			0x504F4441		//	ADOP
#define ChunkAnimationInterpolation		0x4E494441		//	ADIN

#define	GL_CA							GL_COLOR_ATTACHMENT0
#define DL_DA							GL_DEPTH_ATTACHMENT
#define GL_UBYTE						GL_UNSIGNED_BYTE
#define GL_USHORT						GL_UNSIGNED_SHORT
#define GL_UINT							GL_UNSIGNED_INT

//	Using glm
using									glm::vec4;
using									glm::vec3;
using									glm::vec2;
using									glm::mat4;
using									glm::mat3;
using									glm::mat2;
using									glm::uvec4;
using									glm::dvec2;

//	Transformations
using									glm::translate;
using									glm::rotate;
using									glm::scale;
using									glm::transpose;
using									glm::inverse;
using									glm::perspective;
using									glm::lookAt;
using									glm::ortho;

//	Vector operations
using									glm::length;
using									glm::normalize;
using									glm::cross;
using									glm::dot;

//	Unsigned datatypes
typedef unsigned char					ubyte;
typedef unsigned short					ushort;
typedef unsigned int					uint;
typedef unsigned long					ulong;
typedef unsigned int					flags;
typedef unsigned long					lflags;
typedef unsigned int					glenum;

//-	Specific Typedefs -----------------------------------------------------------------------------

//	Buffers
typedef unsigned int					shdrid;
typedef unsigned int					progid;
typedef unsigned int					bufferid;
typedef unsigned int					textureid;
typedef unsigned int					cubemapid;
typedef unsigned int					transformid;
typedef signed int						unifloc;

//	Colors
typedef vec3							color3;
typedef vec4							color4;

//- Error handling --------------------------------------------------------------------------------

typedef unsigned int					glerror;

#define glError							0xFFFFFFFF		//	GLEngine Error
#define glOK							0x00000000		//	No Error

#define glShaderLinkError				0x00002001		//	Currently unused
#define glShaderLoadError				0x00002002		//	Currently unused

//- DDS Surfaces ----------------------------------------------------------------------------------

#define FOURCC_DXT1						0x31545844		// "DXT1" in ASCII
#define FOURCC_DXT3						0x33545844		// "DXT3" in ASCII
#define FOURCC_DXT5						0x35545844		// "DXT5" in ASCII

//- BMP Surfaces ----------------------------------------------------------------------------------

#define IPF								GL_RGB8
#define PF								GL_BGR

#define CIPF							GL_RGBA32F		//	Compute internal pixel format (GL_RGBA16F)
#define CPF								GL_RGBA			//	Compute shader pixel format

//- Math Defines ----------------------------------------------------------------------------------

#define pi								3.1415926535897932384626433832795l

#define	XAxis3							vec3(1.0f, 0.0f, 0.0f)
#define	YAxis3							vec3(0.0f, 1.0f, 0.0f)
#define	ZAxis3							vec3(0.0f, 0.0f, 1.0f)
#define	XAxis4							vec4(1.0f, 0.0f, 0.0f, 0.0f)
#define	YAxis4							vec4(0.0f, 1.0f, 0.0f, 0.0f)
#define	ZAxis4							vec4(0.0f, 0.0f, 1.0f, 0.0f)

#define BiasMatrix						mat4(														\
											0.5, 0.0, 0.0, 0.0,										\
											0.0, 0.5, 0.0, 0.0,										\
											0.0, 0.0, 0.5, 0.0,										\
											0.5, 0.5, 0.5, 1.0)

//- Setting up ------------------------------------------------------------------------------------

#define ConfigGLFWWindow()																			\
{																									\
	/*	Enable Z-Buffering */																		\
	glEnable(GL_DEPTH_TEST);																		\
	glDepthFunc(GL_LESS);																			\
																									\
	/*	Enable keyboard and mouse input	*/															\
	glfwSetInputMode(pGLWin, GLFW_STICKY_KEYS, GL_TRUE);											\
	glfwGetInputMode(pGLWin, GLFW_STICKY_MOUSE_BUTTONS);											\
	glfwSetInputMode(pGLWin, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);										\
																									\
	/*	Enable transparency */																		\
	glEnable(GL_ALPHA_TEST);																		\
	glEnable(GL_BLEND);																				\
																									\
	/*	Enable backface culling */																	\
	glEnable(GL_CULL_FACE);																			\
	glEnable(GL_BACK);																				\
}

#define SetupGLFWWindow(vmajor, vminor) {															\
	glfwWindowHint(GLFW_FOCUSED, 1);																\
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, vmajor);												\
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, vminor);												\
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);									\
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE); 														\
}

#define EnableAntiAliasing() {																		\
	glfwWindowHint(GLFW_SAMPLES, 4);																\
	glEnable(GL_MULTISAMPLE);																		\
}

#define InitGLEW() {																				\
	glewExperimental = true;																		\
	if (glewInit() != GLEW_OK) {																	\
		MessageBox(NULL, "Failed to init GLEW.", "Error", MB_OK);									\
		return glError;																				\
	}																								\
}

#define FinalizeGLFWWindow() {																		\
	if(pGLWin) {																					\
		glfwMakeContextCurrent(pGLWin);																\
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);														\
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);											\
		glfwSwapBuffers(pGLWin);																	\
	}																								\
}

//- Getting Shaderlocations -----------------------------------------------------------------------

#define	GetAttribLocation(AttribHandle, ProgramID, Name) {											\
	AttribHandle = glGetAttribLocation(ProgramID, Name);								GLERROR();	\
	if(AttribHandle == -1 ) {																		\
	/*	LogError("Attribhandle %s : %i", Name, AttribHandle);	*/									\
	} else {																						\
		LogMessage("---- a%.2i %s", AttribHandle, Name);											\
	}																								\
}

#define	GetUniformLocation(UniformHandle, ProgramID, Name) {										\
	UniformHandle = glGetUniformLocation(ProgramID, Name);								GLERROR();	\
	if(UniformHandle == -1 ) {																		\
	/*	LogError("Uniformhandle %s : %i", Name, UniformHandle);	*/									\
	} else{																							\
		LogMessage("---- u%.2i %s", UniformHandle, Name);											\
	}																								\
}

#define UseProgram(Program) {																		\
	if ((Program != 0) && glIsProgram(Program)) {													\
		glUseProgram(Program);																		\
	} else {																						\
		LogError("i is not a valid Program");														\
	}																								\
}

//- Binding Resources -----------------------------------------------------------------------------

#define Bind2Float(location, ID) {																	\
	if(ID != 0) {																					\
		glEnableVertexAttribArray(location);											GLERROR();	\
		glBindBuffer(GL_ARRAY_BUFFER, ID);												GLERROR();	\
		glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 0, nullptr);				GLERROR();	\
	}																								\
}

#define Bind3Float(location, ID) {																	\
	if(ID != 0) {																					\
		glEnableVertexAttribArray(location);											GLERROR();	\
		glBindBuffer(GL_ARRAY_BUFFER, ID);												GLERROR();	\
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, nullptr);				GLERROR();	\
	}																								\
}

#define Bind4Float(location, ID) {																	\
	if(ID != 0) {																					\
		glEnableVertexAttribArray(location);											GLERROR();	\
		glBindBuffer(GL_ARRAY_BUFFER, ID);												GLERROR();	\
		glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, 0, nullptr);				GLERROR();	\
	}																								\
}

#define Bind4Uint(location, ID)	{																	\
	if(ID != 0) {																					\
		glEnableVertexAttribArray(location);											GLERROR();	\
		glBindBuffer(GL_ARRAY_BUFFER, ID);												GLERROR();	\
		glVertexAttribIPointer(location, 4, GL_UNSIGNED_INT, 0, nullptr);				GLERROR();	\
	}																								\
}

#define BindFragDataLocation(mCA, mProgramID, mName) {												\
	glUseProgram(mProgramID);															GLERROR();	\
	glBindFragDataLocation(mProgramID, mCA, mName);										GLERROR();	\
	glLinkProgram(mProgramID);															GLERROR();	\
}

#define BindFrameBuffer(FrameBuffer) {																\
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);										GLERROR();	\
}

//- Checking --------------------------------------------------------------------------------------

#define CheckFrameBufferStatus(error) {																\
	if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {					\
		LogError("Framebuffer status error.");														\
		error = glError;																			\
	} else {																						\
		error = glOK;																				\
	}																								\
}

//- Deleting OpenGL Resources ---------------------------------------------------------------------

#define DetachShader(Program, Shader) {																\
	if (Program != 0 && glIsProgram(Program)) {														\
		if (Shader != 0 && glIsShader(Shader)) {													\
			LogMessage("-- %i shader detached from program %i", Shader, Program);					\
			glDetachShader(Program, Shader);														\
		}																							\
	}																								\
}

#define AttachShader(Program, Shader) {																\
	if(Program != 0 && glIsProgram(Program)) {														\
		if (Shader != 0 && glIsShader(Shader)) {													\
			LogMessage("-- %i shader attached to program %i", Shader, Program);						\
			glAttachShader(Program, Shader);														\
		}																							\
	}																								\
}

#define DeleteShader(Shader) {																		\
	if(Shader != 0 && glIsShader(Shader)) {															\
		LogMessage("-- %i shader deleted", Shader);													\
		glDeleteShader(Shader); Shader = 0;															\
	}																								\
}

#define DeleteProgram(Program) {																	\
	if(Program != 0 && glIsProgram(Program)) {														\
		LogMessage("-- %i program deleted", Program);												\
		glDeleteProgram(Program); Program = 0;														\
	}																								\
}

#define DeleteTexture(Texture) {																	\
	if (Texture != 0 && glIsTexture(Texture)) {														\
		LogMessage("-- %i texture deleted", Texture);												\
		glDeleteTextures(1, &Texture); Texture = 0;													\
	}																								\
}

#define DeleteTextures(Texture) {																	\
	for(count c = 0; Texture[c] != 0; c++) {														\
		if (Texture[c] != 0 && glIsTexture(Texture[c])) {											\
			LogMessage("-- %i texture deleted", Texture[c]);										\
			glDeleteTextures(1, &Texture[c]); Texture[c] = 0;										\
		}																							\
	}																								\
}

#define DeleteBuffer(Buffer) {																		\
	if (glIsBuffer(Buffer)) {																		\
		LogMessage("-- %i buffer deleted", Buffer);													\
		glDeleteBuffers(1, &Buffer); Buffer = 0;													\
	}																								\
}

#define DeleteBuffers(Buffer) {																		\
	for(count c = 0; Buffer[c] != 0; c++) {															\
		if (glIsBuffer(Buffer[c])) {																\
			LogMessage("-- %i bufferi deleted", Buffer[c]);											\
			glDeleteBuffers(1, &Buffer[c]); Buffer[c] = 0;											\
		}																							\
	}																								\
}

#define DeleteRenderbuffer(Buffer) {																\
	if (glIsRenderbuffer(Buffer)) {																	\
		LogMessage("-- %i renderbuffer deleted", Buffer);											\
		glDeleteRenderbuffers(1, &Buffer); Buffer = 0;												\
	}																								\
}

#define DeleteFramebuffer(Buffer) {																	\
	if (glIsFramebuffer(Buffer)) {																	\
		LogMessage("-- %i framebuffer deleted", Buffer);											\
		glDeleteFramebuffers(1, &Buffer); Buffer = 0;												\
	}																								\
}

#define DeleteVAO(vao) {																			\
	LogMessage("%i VAO Deleted", vao);																\
	glDeleteVertexArrays(1, &vao);	vao = 0;														\
}

//- Generate OpenGL Resources handles -------------------------------------------------------------

#define GenerateShaderStorageBuffer(Buffer, SizeInByte)	{											\
	glGenBuffers(1, &Buffer);															GLERROR();	\
	LogMessage("-- % i storagebuffer, size %i", Buffer, SizeInByte);								\
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Buffer);										GLERROR();	\
	glBufferData(GL_SHADER_STORAGE_BUFFER, SizeInByte, NULL, GL_STATIC_DRAW);			GLERROR();	\
}

#define GenerateComputeShaderTextures(nrtexs, textures)	{											\
	glCreateTextures(GL_TEXTURE_2D, nrtexs, textures);								/*GLERROR();*/	\
	for(uint i=0; i<nrtexs; i++) {																	\
		LogMessage("-- %i cstexture %ix%i ", textures[i], HRES, VRES);								\
		glBindTexture(GL_TEXTURE_2D, textures[i]);										GLERROR();	\
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);			GLERROR();	\
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);			GLERROR();	\
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);				GLERROR();	\
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);				GLERROR();	\
		glTexImage2D(GL_TEXTURE_2D, 0, CIPF, HRES, VRES, 0, CPF, GL_FLOAT, NULL);		GLERROR();	\
		glBindImageTexture(i, textures[i], 0, 0, 0, GL_READ_WRITE, CIPF);				GLERROR();	\
	}																								\
}

#define Generate2DTexture(texture) {																\
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);									/*GLERROR();*/	\
	LogMessage("-- %i 2dtexture", texture);															\
	glBindTexture(GL_TEXTURE_2D, texture);												GLERROR();	\
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);				GLERROR();	\
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);				GLERROR();	\
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);					GLERROR();	\
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					GLERROR();	\
}

#define Generate3DTexture(texture) {																\
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texture);								/*GLERROR();*/	\
	LogMessage("-- %i 3dtexture", texture);															\
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);										GLERROR();	\
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);				GLERROR();	\
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);				GLERROR();	\
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);					GLERROR();	\
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					GLERROR();	\
}

#define GenerateBuffers	{																			\
	glCreateBuffers(1, &tBuffer);													/*GLERROR();*/	\
	LogMessage("-- %i buffer", tBuffer);															\
	glBindBuffer(GL_ARRAY_BUFFER, tBuffer);												GLERROR();	\
}

#define GenerateColorAttachment(texture, ca) {														\
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);									/*GLERROR();*/	\
	LogMessage("-- %i colorattachment%i %ix%i", texture, ca, HTEX, VTEX);							\
	glBindTexture(GL_TEXTURE_2D, texture);												GLERROR();	\
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);					GLERROR();	\
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					GLERROR();	\
	glTexImage2D(GL_TEXTURE_2D, 0, IPF, HTEX, VTEX, 0, PF, GL_UBYTE, NULL);				GLERROR();	\
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_CA+ca, GL_TEXTURE_2D, texture, 0);		GLERROR();	\
}

#define GenerateDepthAttachment(dac) {																\
	glCreateRenderbuffers(1, &dac);													/*GLERROR();*/	\
	LogMessage("-- %i depthattachment %ix%i", dac, HTEX, VTEX);										\
	glBindRenderbuffer(GL_RENDERBUFFER, dac);											GLERROR();	\
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, HTEX, VTEX);			GLERROR();	\
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, DL_DA, GL_RENDERBUFFER, dac);				GLERROR();	\
}

#define GenerateFrameBuffer(fb) {																	\
	glCreateFramebuffers(1, &fb);													/*GLERROR();*/	\
	LogMessage("-- %i framebuffer", fb);															\
	glBindFramebuffer(GL_FRAMEBUFFER, fb);												GLERROR();	\
}

#define GenerateScreenQuadVertexShader(ShaderID)													\
{																									\
	const char* ScreenQuadVertexShader = "#version 450\n"											\
		"const vec2[4] V={vec2(-1.0,-1.0),vec2(1.0,-1.0),vec2(-1.0,1.0),vec2(1.0,1.0)};\n"			\
		"void main(){gl_Position=vec4(V[gl_VertexID],0.0,1.0);}\n";									\
																									\
	GLERROR();																						\
																									\
	ShaderID = glCreateShader(GL_VERTEX_SHADER);										GLERROR();	\
	glShaderSource(ShaderID, 1, &ScreenQuadVertexShader, NULL);							GLERROR();	\
	glCompileShader(ShaderID);															GLERROR();	\
	LogMessage("-- %i vertexshader", ShaderID);														\
}

#define GenerateVAO(vao)	{																		\
	glCreateVertexArrays(1, &vao);													/*GLERROR();*/	\
	LogMessage("%i VAO Generated", vao);															\
	glBindVertexArray(vao);																GLERROR();	\
}

//- Loading data into buffers ---------------------------------------------------------------------

#define LoadAtttribArrayBuffer(Buffer, SizeInBytes, Pointer) {										\
	if (SizeInBytes && Pointer) {																	\
		glCreateBuffers(1, &Buffer);																\
		LogMessage("-- %i attribarray of size %i", Buffer, SizeInBytes);							\
		glBindBuffer(GL_ARRAY_BUFFER, Buffer);														\
		glBufferData(GL_ARRAY_BUFFER, SizeInBytes, Pointer, GL_STATIC_DRAW);			GLERROR();	\
		glBindBuffer(GL_ARRAY_BUFFER, 0);															\
	}																								\
}

#define LoadIndexArrayBuffer(Buffer, SizeInBytes, Pointer) {										\
	if (SizeInBytes && Pointer) {																	\
		glCreateBuffers(1, &Buffer);																\
		LogMessage("-- %i indexarray of size %i", Buffer, SizeInBytes);								\
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer);												\
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, SizeInBytes, Pointer, GL_STATIC_DRAW);	GLERROR();	\
		glBindBuffer(GL_ARRAY_BUFFER, 0);															\
	}																								\
}

#define LoadImageBMP(texture, width, height, pointer) {												\
	if(width && (width == height) && pointer)	{													\
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);												\
		LogMessage("-- %i texture BMP %ix%i", texture, width, height);								\
		glBindTexture(GL_TEXTURE_2D, texture);														\
																									\
		glTexImage2D(GL_TEXTURE_2D, 0, IPF,	width, height, 0, PF, GL_UBYTE, pointer);	GLERROR();	\
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);				GLERROR();	\
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);				GLERROR();	\
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);					GLERROR();	\
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);					GLERROR();	\
		glGenerateMipmap(GL_TEXTURE_2D);												GLERROR();	\
		glBindTexture(GL_TEXTURE_2D, 0);															\
	}																								\
}

//- Sending data ----------------------------------------------------------------------------------

#define Send2DTexture(TextureSlot, location, ID) {													\
	if(ID != 0) {																					\
		glActiveTexture(GL_TEXTURE0 + TextureSlot);										GLERROR();	\
		glBindTexture(GL_TEXTURE_2D, ID);												GLERROR();	\
		glUniform1i(location, TextureSlot);												GLERROR();	\
	}																								\
}

#define SendCubeMapTexture(TextureSlot, location, ID) {												\
	if(ID != 0) {																					\
		glActiveTexture(GL_TEXTURE0 + TextureSlot);										GLERROR();	\
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);											GLERROR();	\
		glUniform1i(location, TextureSlot);												GLERROR();	\
	}																								\
}

#define SendMat4Uniform(location, mat4n) {															\
	if(location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, mat4n);				GLERROR();	\
}

#define SendMat3Uniform(location, mat3n) {															\
	if(location != -1) glUniformMatrix3fv(location, 1, GL_FALSE, mat3n);				GLERROR();	\
}

#define SendVec3Uniform(location, vec3n) {															\
	if (location != -1) glUniform3f(location, vec3n.x, vec3n.y, vec3n.z);				GLERROR();	\
}

#define SendVec4Uniform(location, vec4n)	{														\
	if (location != -1) glUniform4f(location, vec4n.x, vec4n.y, vec4n.z, vec4n.w);		GLERROR();	\
}

#define Send1FloatUniform(location, floatn) {														\
	if (location != -1) glUniform1f(location, floatn);									GLERROR();	\
}

#define Send2FloatUniform(location, f1, f2) {														\
	if (location != -1) glUniform2f(location, f1, f2);									GLERROR();	\
}

#define Send4FloatUniform(location, f1, f2, f3, f4) {												\
	if (location != -1) glUniform4f(location, f1, f2, f3, f4);							GLERROR();	\
}

#define SendVec2Uniform(location, v2) {																\
	if (location != -1) glUniform2f(location, v2.x, v2.y);								GLERROR();	\
}

#define SendVec4Uniform(location, v4) {																\
	if (location != -1) glUniform4f(location, v4.x, v4.y, v4.z, v4.w);					GLERROR();	\
}

#define Send1UintUniform(location, n)	{															\
	if (location != -1) glUniform1i(location, n);										GLERROR();	\
}

//- Unbinding -------------------------------------------------------------------------------------

#define Unbind(location) { if(location != 0) glDisableVertexAttribArray(location);		GLERROR();	}

//- Non OpenGL Macros -----------------------------------------------------------------------------

namespace GLGeneral
{
	//	Using std
	using std::string;
	using std::ifstream;
	using std::ios;
	using std::vector;
	using std::string;

	//- Files and folders -------------------------------------------------------------------------

	inline bool CheckFileExists(const char* Filname)
	{
		FILE* pFile = NULL;
		if (pFile = fopen(Filname, "rb")) {
			fclose(pFile);
			return true;
		}
		else {
			return false;
		}
	}

	//	true exists, false not exists
	inline bool CheckFolderExist(const char* Foldername)
	{
		struct stat info;

		if (stat(Foldername, &info)) {
			return false;
		}
		else if (info.st_mode & S_IFDIR) {
			return true;
		}

		return false;
	}

	inline String FindResourcePath(const char* Folder)
	{
		String CurrentFolder = Folder;

		for (uint i = 0; i < MAXPATHDEPTH; i++) {
			if (CheckFolderExist(CurrentFolder.Val())) {
				break;
			} else {
				CurrentFolder = "..\\" + CurrentFolder;
			}
		}

		return CurrentFolder;
	}

	//- Reading shaders from disk and compiling ---------------------------------------------------

	inline const shdrid LoadShader(
		const uint				ShaderType,
		const char*				ShaderName)
	{
		if ((ShaderName != nullptr) && (ShaderName[0] != '\0'))
		{
			string Folder = FindResourcePath("shaders").Val();
			string Path = Folder + "\\" + ShaderName;
			char* Type;

			string FullPath;
			switch (ShaderType)
			{
			case GL_VERTEX_SHADER:
				FullPath = Path + ".vert"; Type = "vertexshader";  break;
			case GL_GEOMETRY_SHADER:
				FullPath = Path + ".geom"; Type = "geometryshader"; break;
			case GL_FRAGMENT_SHADER:
				FullPath = Path + ".frag"; Type = "fragmentshader"; break;
			case GL_COMPUTE_SHADER:
				FullPath = Path + ".comp"; Type = "computeshader"; break;
			}

			//- Read the Shader code from the file ------------------------------------------------

			shdrid ShaderID = glCreateShader(ShaderType);

			string ShaderCode;
			ifstream ShaderStream(&FullPath[0], ios::in);
			if (ShaderStream.is_open()) {
				LogMessage("-- %i %s %s", ShaderID, Type, ShaderName);
				string Line = "";
				while (getline(ShaderStream, Line))
					ShaderCode += Line + "\n";
				ShaderStream.close();
			}
			else {
				LogError("Error opening '%s'", &FullPath[0]);
				return 0;
			}

			//- Compile Vertex Shader -------------------------------------------------------------

			char const * ShaderSourcePointer = ShaderCode.c_str();
			glShaderSource(ShaderID, 1, &ShaderSourcePointer, NULL);
			glCompileShader(ShaderID);

			// Check Vertex Shader
			int InfoLogLength = 0, Result = 0;
			glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if (InfoLogLength > 0) {
				vector<char> ErrorMessage(InfoLogLength + 1);
				glGetShaderInfoLog(ShaderID, InfoLogLength,
					NULL, &ErrorMessage[0]);
				LogMessage("%s", &ErrorMessage[0]);
				printf("%s\n", &ErrorMessage[0]);
				glDeleteShader(ShaderID);
				return 0;
			}

			return ShaderID;
		}

		return 0;
	}

	inline const glerror CheckShaderProgram(const progid& Program)
	{
		int Result = GL_FALSE;
		int InfoLogLength = 0;

		glGetProgramiv(Program, GL_LINK_STATUS, &Result);
		glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &InfoLogLength);

		if (InfoLogLength > 0) {
			std::vector<char> ProgError(InfoLogLength + 1);
			glGetProgramInfoLog(Program, InfoLogLength, NULL, &ProgError[0]);
			LogError("%s", &ProgError[0]);

			return glError;
		}

		LogMessage("-- %i program linked", Program);

		return glOK;
	}

	inline const progid LinkShaders(const count NrShaders, va_list args)
	{
		progid Program = glCreateProgram();

		for (uint s = 0; s < NrShaders; s++) {
			const shdrid sid = va_arg(args, shdrid);
			AttachShader(Program, sid);
		}

		glLinkProgram(Program);

		return Program;
	}

	inline void DeleteShaders(const progid Program, const count NrShaders, va_list args)
	{
		if (glIsProgram(Program)) {
			for (uint s = 0; s < NrShaders; s++) {
				shdrid sid = va_arg(args, shdrid);
				if (glIsShader(sid)) {
					glDetachShader(Program, sid);
				}
			}
		}
	}

	inline const progid LinkShaders(uint NrShaders, ...)
	{
		va_list args;
		progid Program;

		if (NrShaders) {
			va_start(args, NrShaders);
			Program = LinkShaders(NrShaders, args);
			va_end(args);

			if (CheckShaderProgram(Program)) {
				va_start(args, NrShaders);
				DeleteShaders(Program, NrShaders, args);
				va_end(args);
				return 0;
			}
		}

		return Program;
	}

	inline void GLAPIENTRY GLErrorCallBack(
		GLenum								Source,
		GLenum								Type,
		GLuint								ID,
		GLenum								Severity,
		GLsizei								Length,
		const GLchar*						Message,
		const void*							UserParam)
	{
		/*	Sources		:	GL_DEBUG_SOURCE_API
							GL_DEBUG_SOURCE_WINDOW_SYSTEM
							GL_DEBUG_SOURCE_SHADER_COMPILER
							GL_DEBUG_SOURCE_THIRD_PARTY
							GL_DEBUG_SOURCE_APPLICATION
							GL_DEBUG_SOURCE_OTHER

			type		:	GL_DEBUG_TYPE_ERROR
							GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR
							GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
							GL_DEBUG_TYPE_PORTABILITY
							GL_DEBUG_TYPE_PERFORMANCE
							GL_DEBUG_TYPE_OTHER
							GL_DEBUG_TYPE_MARKER
							GL_DEBUG_TYPE_PUSH_GROUP
							GL_DEBUG_TYPE_POP_GROUP

			severity	:	GL_DEBUG_SEVERITY_LOW
							GL_DEBUG_SEVERITY_MEDIUM
							GL_DEBUG_SEVERITY_HIGH
							GL_DEBUG_SEVERITY_NOTIFICATION
		*/

		switch (Type)
		{
		case GL_DEBUG_TYPE_PORTABILITY:
			return;//LogWarning("Portability : %s", Message);
			break;
		case GL_DEBUG_TYPE_OTHER:
			return;//LogWarning("%s", Message);
			break;
		case GL_DEBUG_TYPE_MARKER:
			return;//LogWarning("Marker : %s", Message);
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			return;//LogWarning("Push group : %s", Message);
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			return;//LogWarning("Pop group : %s", Message);
			break;
		default:
			return;//LogError("Unspecified : %s", Message);
			break;

		case GL_DEBUG_TYPE_ERROR:
			LogError("Error : %s", Message);
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			LogWarning("Deprecated Behavior : %s", Message);
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			LogWarning("Undefined behavior: %s", Message);
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			LogWarning("Performance : %s", Message);
			break;
		}

	}

	//- Math Stuffs -------------------------------------------------------------------------------

	inline void GenerateNormal(
		const vec3&				Vertex1,
		const vec3&				Vertex2,
		const vec3&				Vertex3,
		vec3&					Normal)
	{
		const vec3 edge1 = Vertex2 - Vertex1;
		const vec3 edge2 = Vertex3 - Vertex1;

		Normal = normalize(cross(edge1, edge2));
	}

	inline void GenerateNormals(
		const vec3*				pVertices,
		const ushort*			pIndices,
		vec3*					pNormals,
		const bool&				InvertNormals)
	{
		const uint NrFaces = sizeof(pIndices) / 3;
		for (uint f = 0; f < NrFaces; f = f + 3)
		{
			//	Get vertices
			const uint i1 = pIndices[f + 0];
			const uint i2 = pIndices[f + 1];
			const uint i3 = pIndices[f + 2];

			const vec3 v1 = pVertices[i1];
			const vec3 v2 = pVertices[i2];
			const vec3 v3 = pVertices[i3];

			vec3 Normal;
			GenerateNormal(v1, v2, v3, Normal);

			if (InvertNormals) Normal = Normal * -1.0f;

			//	Add vertexnormals
			pNormals[i1] += Normal;
			pNormals[i2] += Normal;
			pNormals[i3] += Normal;
		}

		//	Normalize
		for (uint n = 0; n < sizeof(pNormals); n++)
			pNormals[n] = normalize(pNormals[n]);
	}

	inline void GenerateTangent(
		const vec3&				Vertex1,
		const vec3&				Vertex2,
		const vec3&				Vertex3,
		const vec2&				TexCoord1,
		const vec2&				TexCoord2,
		const vec2&				TexCoord3,
		vec3&					Tangent,
		vec3&					Bitangent)
	{
		//	Calculate the delta's
		const float& x1 = Vertex2.x - Vertex1.x;
		const float& x2 = Vertex3.x - Vertex1.x;
		const float& y1 = Vertex2.y - Vertex1.y;
		const float& y2 = Vertex3.y - Vertex1.y;
		const float& z1 = Vertex2.z - Vertex1.z;
		const float& z2 = Vertex3.z - Vertex1.z;

		const float& s1 = TexCoord2.x - TexCoord1.x;
		const float& s2 = TexCoord3.x - TexCoord1.x;
		const float& t1 = TexCoord2.y - TexCoord1.y;
		const float& t2 = TexCoord3.y - TexCoord1.y;

		//	Calculate the tangents
		float r, t = s1 * t2 - s2 * t1;
		if (t) {			//	Check for 0
			r = 1.0f / t;
		}
		else {
			r = 50000.0f;
		}

		const float& tx = (t2 * x1 - t1 * x2) * r;
		const float& ty = (t2 * y1 - t1 * y2) * r;
		const float& tz = (t2 * z1 - t1 * z2) * r;
		const float& bx = (s1 * x2 - s2 * x1) * r;
		const float& by = (s1 * y2 - s2 * y1) * r;
		const float& bz = (s1 * z2 - s2 * z1) * r;

		Tangent = vec3(bx, by, bz); //	vec3(tx, ty, tz);
		Bitangent = -1.0f * vec3(tx, ty, tz);		//	vec3(bx, by, bz);
	}

	//- Surface / Image stuffs --------------------------------------------------------------------

	inline const glerror LoadBMPSurface(const char* BMPFile, uint& Width, uint& Height, ubyte** ppBuffer)
	{
		//-	Read the file from disk ---------------------------------------------------------------

		FILE* pFile = fopen(BMPFile, "rb");

		char Header[4];
		if (pFile) {
			fread(Header, 1, 4, pFile);
			if (!strncmp(Header, "BM", 2) == 0) {
				LogError("BMP File header error");
				fclose(pFile);

				return glBMPFileHeaderError;
			}
		}
		else {
			LogError("BMP File not found", MB_OK);
			return glBMPFileOpenError;
		}

		unsigned char bmphdr[50];		//	BMP File header is 54 of which we already read 4.
		if (!(fread(bmphdr, 1, 50, pFile) == 50)) {
			LogError("BMP File info error");
			fclose(pFile);

			return glBMPFileHeaderSizeError;
		}

		Height = *(uint*)&(bmphdr[14]);
		Width = *(uint*)&(bmphdr[18]);
		uint BufferSize = Height * Width * 3;

		ubyte* nBuffer = new ubyte[BufferSize];
		memset(nBuffer, 0, BufferSize);

		//	Bitmaps stored upside down
		uint LineWidth = 3 * Width;
		for (int i = Height - 1; i >= 0; --i) {
			if (fread(nBuffer + (i*LineWidth), 1, LineWidth, pFile) != LineWidth) {
				LogError("BMP content read error");
				delete [] nBuffer;
				return glBMPFileContentError;
			}
		}

		*ppBuffer = nBuffer;

		fclose(pFile);

		return glOK;
	}

	inline const textureid ReadBMPSurface(const char* BMPFile)
	{
		textureid TextureID = 0;;
		uint Width, Height;
		ubyte* Buffer;

		if (!LoadBMPSurface(BMPFile, Height, Width, &Buffer) && Buffer) {

			Generate2DTexture(TextureID);

			glTexImage2D(GL_TEXTURE_2D, 0, IPF,
				Width, Height, 0, PF, GL_UNSIGNED_BYTE, Buffer);						GLERROR();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);			GLERROR();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);			GLERROR();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);				GLERROR();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);				GLERROR();
			glGenerateMipmap(GL_TEXTURE_2D);											GLERROR();

			delete[] Buffer;	Buffer = nullptr;
		}

		return TextureID;
	}

	inline const textureid LoadDDSurfaceFileContents(FILE* pFile)
	{
		//	Read the rest of the header
		unsigned char header[124];
		if (!(fread(&header, 1, 124, pFile) == 124)) {
			MessageBox(NULL, "Error reading DDS File header", "Error", MB_OK);
			fclose(pFile);
			return 0;
		}

		uint Height = *(uint*)&(header[8]);
		uint Width = *(uint*)&(header[12]);
		const uint& LinearSize = *(uint*)&(header[16]);
		const uint& MipMapCount = *(uint*)&(header[24]);
		const uint& BufferSize = *(uint*)&(header[28]);
		const uint& FourCC = *(uint*)&(header[80]);
		const uint& Components = (FourCC == FOURCC_DXT1) ? 3 : 4;

		uint Format;
		switch (FourCC)
		{
		case FOURCC_DXT1:
			Format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case FOURCC_DXT3:
			Format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			Format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			return 0;
		}

		//	Read the file
		size_t bufsize = MipMapCount > 1 ? LinearSize * 2 : LinearSize;

		ubyte* Buffer = new ubyte[bufsize];
		fread(Buffer, 1, bufsize, pFile);
		fclose(pFile);

		// Create one OpenGL texture
		textureid TextureID;
		Generate2DTexture(TextureID);

		uint Offset = 0;
		const uint& BlockSize = (Format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
		for (uint level = 0; level < MipMapCount && (Width || Height); ++level) {
			uint size = ((Width + 3) / 4)*((Height + 3) / 4)*BlockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, Format,
				Width, Height, 0, size, Buffer + Offset);							GLERROR();

			Offset += size;
			Width /= 2;
			Height /= 2;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);			GLERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);			GLERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);				GLERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);				GLERROR();

		return TextureID;
	}

	inline const textureid ReadDDSSurface(const char* DDSFile)
	{
		FILE* pFile = fopen(DDSFile, "rb");

		if (pFile) {
			//	Check DDS Header
			char DDSHeader[4];
			fread(DDSHeader, 1, 4, pFile);
			if (!strncmp(DDSHeader, "DDS ", 4) == 0) {
				MessageBox(NULL, "Not a valid DDS File", "Error", MB_OK);
				fclose(pFile);
				return 0;
			}
		}
		else {
			MessageBox(NULL, "Could not open DDS file", "Error", MB_OK);
			return 0;
		}

		textureid tid = LoadDDSurfaceFileContents(pFile);

		fclose(pFile);

		return tid;
	}

	inline const cubemapid			LoadBMPCubeMap(
		const char*						BMPCubeRight,
		const char*						BMPCubeLeft,
		const char*						BMPCubeTop,
		const char*						BMPCubeBottom,
		const char*						BMPCubeBack,
		const char*						BMPCubeFront)
	{
		cubemapid cid = 0;
		Generate3DTexture(cid);

		const char* faces[] = {
			BMPCubeRight, BMPCubeLeft,
			BMPCubeTop, BMPCubeBottom,
			BMPCubeBack, BMPCubeFront
		};

		uint Width, Height;
		for (uint f = 0; f < 6; f++)
		{
			ubyte* pBuffer = NULL;
			if (LoadBMPSurface(faces[f], Width, Height, &pBuffer) == 0)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0, IPF,
					Width, Height, 0, PF, GL_UNSIGNED_BYTE, pBuffer);

				delete [] pBuffer;
			}
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		GLERROR();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		GLERROR();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);	GLERROR();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	GLERROR();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	GLERROR();

		return cid;
	}
}
