
#pragma once

#include "GLModel.h"

//	Buffer macros ----------------------------------------------------------------------------------

namespace GLEngine
{
	//- 2D Shaders --------------------------------------------------------------------------------

	class GLShader
	{

	public:

		GLShader()
		{
			Time = 0.0l;
			ProgramID = 0;
		}

		GLShader(const GLShader& B)
		{
			Time = B.Time;
			ProgramID = B.ProgramID;
		}

		const glerror linkShaders(const count NrShaders, ...)
		{
			va_list args;

			va_start(args, NrShaders);
			ProgramID = LinkShaders(NrShaders, args);
			va_end(args);

			if (ProgramID != 0) {
				Shaders.Resize(NrShaders);
				va_start(args, NrShaders);
				for (count i = 0; i < NrShaders; i++) {
					Shaders[i] = va_arg(args, shdrid);
				}
				va_end(args);

				if (CheckShaderProgram(ProgramID) == glOK) {
					return glOK;
				}
			}

			DestroyShaders();

			return glError;
		}

		void DestroyShaders()
		{
			if (ProgramID != 0 && glIsProgram(ProgramID)) {
				for (count i = 0; i < Shaders.Size(); i++) {
					shdrid& sid = (shdrid&)Shaders.GetElem(i);
					DetachShader(ProgramID, sid);
					DeleteShader(sid);
				}
			}

			Shaders.Clear();
			DeleteProgram(ProgramID);
		}

		const progid GetProgramID() const
		{
			return ProgramID;
		}
		
	protected:

		//	Different for 2D shadertypes
		virtual void						GetShaderLocations() = 0;

		double								Time;
		progid								ProgramID;
		AttribData<shdrid>					Shaders;
	};

	class GLComputeShader : public GLShader
	{

	public:

		GLComputeShader() : GLShader()
		{
			ComputeShaderID = 0;;
		}

		GLComputeShader(const GLComputeShader& B)
		{
			ComputeShaderID = B.ComputeShaderID;
			Buffers = B.Buffers;
			Textures = B.Textures;
			uTime = B.uTime;
			CounterID = B.CounterID;
		}

		const glerror SetupComputeshader(const char* Shadername)
		{
			DestroyShaders();
			if (shdrid cs = LoadShader(GL_COMPUTE_SHADER, Shadername)) {
				if (SetupShader(cs)) {
					DeleteShader(cs);
					return glError;
				}

				return glOK;
			}
		}

		void GenerateFloatBuffers(const count NrBuffers, const size ItemCount)
		{
			Buffers.Resize(NrBuffers);

			size SizeInBytes = ItemCount * sizeof(float);
			for (count i = 0; i < NrBuffers; i++) {
				GenerateShaderStorageBuffer(Buffers[i], SizeInBytes);
			}
		}

		void GenerateVec4Buffers(const count NrBuffers, const size ItemCount)
		{
			count offset = Buffers.Size();
			Buffers.Resize(offset + NrBuffers);

			size SizeInBytes = ItemCount * sizeof(vec4);
			for (count i = 0; i < NrBuffers; i++) {
				GenerateShaderStorageBuffer(Buffers[offset+i], SizeInBytes);
			}
		}

		void GenerateVec2Buffers(const count NrBuffers, const size ItemCount)
		{
			count offset = Buffers.Size();
			Buffers.Resize(offset + NrBuffers);

			size SizeInBytes = ItemCount * sizeof(vec2);
			for (count i = 0; i < NrBuffers; i++) {
				GenerateShaderStorageBuffer(Buffers[offset+i], SizeInBytes);
			}
		}

		void GenerateVec3Buffers(const count NrBuffers, const size ItemCount)
		{
			count offset = Buffers.Size();
			Buffers.Resize(offset + NrBuffers);

			size SizeInBytes = ItemCount * sizeof(vec3);
			for (count i = 0; i < NrBuffers; i++) {
				GenerateShaderStorageBuffer(Buffers[offset+i], SizeInBytes);
			}
		}

		void GenerateIndexBuffer(const count& NrFaces)
		{
			count offset = Buffers.Size();
			Buffers.Resize(offset + 1);

			const size& SizeInBytes = 3 * sizeof(ushort) * NrFaces;
			GenerateShaderStorageBuffer(Buffers[offset], SizeInBytes);
		}

#define NrCounters 10

		void GenerateCounterBuffers()
		{
			uint zero[NrCounters] = { 0 };
			size s = NrCounters * sizeof(int);

			const unifloc loc = Buffers.Size() + 1;

			glGenBuffers(1, &CounterID);
			LogMessage("-- %i counterbuffer, location %i", CounterID, loc);
			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, loc, CounterID);
			glBufferData(GL_ATOMIC_COUNTER_BUFFER, s, NULL, GL_DYNAMIC_DRAW);
			glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &zero);
		}

		const uint GetCounterValues(uint* p1Vals)
		{
			uint val = 0;

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, CounterID);
			if (const uint * pCounter = (const uint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY)) {
				for (count c = 0; c < 10; c++) {
					const uint cnt = pCounter[c];
					*(&p1Vals[c]) = cnt;
				}
			}
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

			return val;
		}

		void Execute(uint gsx, uint gsy, uint gsz)
		{
			UseProgram(ProgramID);
			glClear(GL_COLOR_BUFFER_BIT);

			count i = 0;
			for (; i < Buffers.Size(); i++) {
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i+1, Buffers.GetElem(i));
			}

			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, i + 1, CounterID); i++;

			//	Send1FloatUniform(ComputeTime, (float)CurrTime);
			//	SendVec4Uniform(ComputeMouse, vec4(MouseLoc, vec2(0.0f)));
			
			glDispatchCompute(gsx, gsy, gsz);

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			//	Clear error buffer
			glGetError();

//			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}

		//	Retrieve GPUMEM pointer for writing, takes Bufferindex as parameter, Number of elements as Size
		vec4* MapVec4Buffer(index Index, size BufferSize)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, Buffers.GetElem(Index));

			return (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, BufferSize * sizeof(vec4),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		}

		float* MapFloatBufferForWriting(index Index, size BufferSize)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, Buffers.GetElem(Index));

			return (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, BufferSize * sizeof(float),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		}

		void* MapBufferForReading(index Index, size SizeInBytes)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, Buffers.GetElem(Index));

			return (void*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, SizeInBytes, GL_MAP_READ_BIT);
		}

		float* MapFloatBufferForReading(index Index, size BufferSize)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, Buffers.GetElem(Index));

			return (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, BufferSize * sizeof(float),
				GL_MAP_READ_BIT);
		}

		void UnmapBuffer()
		{
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		}

		virtual void Destroy()
		{
			for (count i = 0; i < Buffers.Size(); i++) {
				bufferid& id = (bufferid&)Buffers.GetElem(i);
				DeleteBuffer(id);
			}

			DeleteBuffer(CounterID);

			for (count i = 0; i < Textures.Size(); i++) {
				textureid& id = (textureid&)Textures.GetElem(i);
				DeleteTexture(id);
			}

			GLShader::DestroyShaders();
		}

		bufferid GetBufferID(index i)
		{
			return Buffers[i];
		}

		textureid GetTextureID(index i)
		{
			return Textures.GetElem(i);
		}

	protected:

		glerror SetupShader(const shdrid& ComputeShader)
		{
			if (ComputeShader != 0) {
				if (!(linkShaders(1, ComputeShader))) {
					GetShaderLocations();
					return glOK;
				}
			}

			return glError;
		}

		void								GetShaderLocations()
		{
			GetUniformLocation(uTime, ProgramID, "iGlobalTime");
		}

		shdrid								ComputeShaderID;

		unifloc								uTime;

		bufferid							CounterID;

		AttribData<bufferid>				Buffers;
		AttribData<textureid>				Textures;

	};

	class GLFrameShader : public GLShader
	{

	public:

		GLFrameShader::GLFrameShader() : GLShader()
		{
			FrameBuffer = 0;
			HRes = 0;
			VRes = 0;
		}

		GLFrameShader(const GLFrameShader& B)
		{
			FrameBuffer = B.FrameBuffer;
			HRes = B.HRes;
			VRes = B.VRes;
		}

		//- Shader program access -------------------------------------------------

		const progid&						getProgramID() const
		{
			return ProgramID;
		}

		virtual void						Update(const double& Time)
		{
			this->Time = Time;
		}

		void								enableShader();

		void								setResolution(
			const uint&							HRes,
			const uint&							VRes)
		{
			this->HRes = HRes;
			this->VRes = VRes;
		}

		void								Destroy()
		{
			GLShader::DestroyShaders();
		}

	protected:

		bufferid							FrameBuffer;
		uint								HRes;
		uint								VRes;
	};

	class GL2DShader : public GLFrameShader
	{

	public:

		GL2DShader::GL2DShader() : GLFrameShader()
		{
			PassThroughShader = 0;
			uResolution = 0;
			uGlobalTime = 0;
			DepthBuffer = 0;

			for (uint i = 0; i < MAXTEXTURES; i++) {
				uChannel[i] = 0;
				ChannelIn[i] = 0;
			}
		}

		GL2DShader(const GL2DShader& B) : GLFrameShader(B)
		{
			PassThroughShader = B.PassThroughShader;
			uResolution = B.uResolution;
			uGlobalTime = B.uGlobalTime;
			DepthBuffer = B.DepthBuffer;

			for (uint i = 0; i < MAXTEXTURES; i++) {
				uChannel[i] = B.uChannel[i];
				ChannelIn[i] = B.ChannelIn[i];
			}
		}

	protected:

		void								BindFragDataLocations();
		
		progid								PassThroughShader;

		//	Uniform locations
		unifloc								uResolution;
		unifloc								uGlobalTime;
		unifloc								uChannel[MAXTEXTURES];

		//	Resources
		textureid							ChannelIn[MAXTEXTURES];
		bufferid							DepthBuffer;
	};

	class GL3DShader : public GLFrameShader
	{

	public:

											GL3DShader();

		virtual void						GetShaderLocations();

		void								SendPosition(const vec3& Position);
		void								SendDiffuse(const color3&);
		void								SendAmbient(const color3&);
		void								SendSpecular(const color3&);
		void								SendCAttenuation(const float&);
		void								SendLAttenuation(const float&);
		void								SendQAttenuation(const float&);
//		void								SendLightType(const flags&);
		void								SendMVMatrix(const mat4&);
		void								SendMVPMatrix(const mat4&);
		void								SendNMatrix(const mat4&);
		void								SendBonestructure(const GLBones&);
		void								SendAttributes(const GLBuffers&);
		virtual void						RenderPolygons(const GLPolygons&) = 0;

		void								DisableAttribArrays();

		glerror								SetupShader(
			const shdrid						VertexShader,
			const shdrid						GeometryShader,
			const shdrid						FragmentShader,
			const uint&							HorizontalResolution,
			const uint&							VerticalResolution)
		{
			if (!(linkShaders(3, VertexShader, GeometryShader, FragmentShader))) {
				GetShaderLocations();
				setResolution(HorizontalResolution, VerticalResolution);
				return glOK;
			}
			
			return glError;
		}

		glerror								SetupShader(
			const shdrid						VertexShader,
			const shdrid						FragmentShader,
			const uint&							HorizontalResolution,
			const uint&							VerticalResolution)
		{
			if (!(linkShaders(2, VertexShader, FragmentShader))) {
				GetShaderLocations();
				setResolution(HorizontalResolution, VerticalResolution);
				return glOK;
			}

			return glError;
		}
				
	protected:

		//- 3D Shader locations --------------------------------------------------

		//	Matrices
		unifloc								MatDMVP;
		unifloc								MatMVP;
		unifloc								MatMV;
		unifloc								MatN;
		unifloc								MatP;

		//	Lights
		unifloc								lPosition;
		unifloc								lDiffuse;
		unifloc								lAmbient;
		unifloc								lSpecular;
		unifloc								lCAttenuation;
		unifloc								lLAttenuation;
		unifloc								lQAttenuation;
		unifloc								LightType;

		//	Geometry IDs
		unifloc								Vertex;
		unifloc								TexCoords;
		unifloc								Colors;
		unifloc								Normal;
		unifloc								Tangent;
		unifloc								Bitangent;

		//	Skinning
		unifloc								VertexWeight;
		unifloc								VertexJoint;
		unifloc								Bones;


		//	Shadow maps
		unifloc								Shadowmap;
	};

	class GLMaterialShader : public GL3DShader
	{

	public:

											GLMaterialShader();

		virtual void						GetShaderLocations();
		void								RenderPolygons(
			const GLPolygons&					Polygons);

	protected:

		//	Material
		unifloc								MaterialType;

		unifloc								AmbientTexture;
		unifloc								DiffuseTexture;
		unifloc								BumpmapTexture;
		unifloc								SpecularTexture;
		unifloc								RenderedTexture;
		unifloc								DepthTexture;
		unifloc								DiffuseColor;
		unifloc								AmbientColor;
		unifloc								SpecularColor;
		unifloc								Shininess;
	};

	class GLMaterialFragShader : public GL3DShader
	{

	public:

		GLMaterialFragShader();
		
		virtual void GetShaderLocations();
		void RenderPolygons(const GLPolygons& Polygons);
		void Update(const double& T) { Time = T; }

	protected:

		//	Material
		unifloc							GlobalTime;

		unifloc							Projection;
		unifloc							MaterialType;
		unifloc							AmbientTexture;
		unifloc							DiffuseTexture;
		unifloc							NormalTexture;
		unifloc							SpecularTexture;
		unifloc							Shininess;
	};
}