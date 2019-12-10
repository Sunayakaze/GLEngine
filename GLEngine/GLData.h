
#pragma once

#include "General.h"

namespace GLEngine
{
	using GLGeneral::GenerateNormal;
	using GLGeneral::GenerateTangent;
	using GLGeneral::GenerateNormals;
	using GLGeneral::FindResourcePath;
	using GLGeneral::CheckFileExists;
	
	using GLGeneral::LinkShaders;
	using GLGeneral::LoadShader;
	using GLGeneral::CheckShaderProgram;
	using GLGeneral::DeleteShaders;
	
	using GLGeneral::GLErrorCallBack;

	//- Color ---------------------------------------------------------------------

	struct BitmapColor
	{
		ubyte								G;
		ubyte								R;
		ubyte								B;

		BitmapColor()
		{
			R = G = B = 0;
		}
	};

	//- Surface Stuff -------------------------------------------------------------

	class GLSurface
	{
		char								m_Header[4];
		uint								m_Height;
		uint								m_Width;
		uint								m_LinearSize;
		uint								m_MipMapCount;
		uint								m_FourCC;
		uint								m_BufferSize;
		ubyte*								m_Buffer;

	public:

		GLSurface()
			:	m_Height(0)
			,	m_Width(0)
			,	m_LinearSize(0)
			,	m_MipMapCount(0)
			,	m_FourCC(0)
			,	m_BufferSize(0)
			,	m_Buffer(NULL)
		{
			m_Header[0] = 0;
			m_Header[1] = 0;
			m_Header[2] = 0;
			m_Header[3] = 0;
		}

		GLSurface(
			const uint							Height,
			const uint							Width,
			const ubyte*						pBuffer)
		{
			ReadBMPSurface(Height, Width, pBuffer);
		}

		~GLSurface()
		{
			if(this->m_Buffer) {
				delete [] m_Buffer;	m_Buffer = NULL;
				memset(this, 0, sizeof(*this));
			}
		}

		GLSurface(
			const GLSurface&					rhs)
			:	m_Height(rhs.m_Height)
			,	m_Width(rhs.m_Width)
			,	m_LinearSize(rhs.m_LinearSize)
			,	m_MipMapCount(rhs.m_MipMapCount)
			,	m_FourCC(rhs.m_FourCC)
			,	m_BufferSize(rhs.m_BufferSize)
			,	m_Buffer(NULL)
		{
			Strcpy(m_Header, rhs.m_Header, 4);

			//	The buffer itself
			if(m_BufferSize) {
				this->m_Buffer = new ubyte[m_BufferSize];
				memset(m_Buffer, 0, sizeof(m_BufferSize));
				memcpy(m_Buffer, rhs.GetBufferPointer(), m_BufferSize);
			}
		}

		GLSurface&						operator=(
			GLSurface							rhs)
		{
			Swap(m_Buffer,		rhs.m_Buffer);
			Swap(m_Header[0],	rhs.m_Header[0]);
			Swap(m_Header[1],	rhs.m_Header[1]);
			Swap(m_Header[2],	rhs.m_Header[2]);
			Swap(m_Header[3],	rhs.m_Header[3]);
			Swap(m_Height,		rhs.m_Height);
			Swap(m_Width,		rhs.m_Width);
			Swap(m_LinearSize,	rhs.m_LinearSize);
			Swap(m_MipMapCount,	rhs.m_MipMapCount);
			Swap(m_FourCC,		rhs.m_FourCC);
			Swap(m_BufferSize,	rhs.m_BufferSize);
			return *this;
		}

		//- Disk Operations-----------------------------------------------------------

		uint								ReadFromDisk(
			FILE*								pFile);

		uint								WriteToDisk(
			FILE*								pFile);

		uint								WriteDDSurface(
			FILE*								pFile);

		uint								WriteBMPSurface(
			FILE*								pFile);

		uint								ReadBMPSurface(
			const char*							pFileName);
	
		uint								ReadBMPSurface(
			FILE*								pFile);

		uint								ReadBMPSurface(
			const uint							Height,
			const uint							Width,
			const ubyte*						pBuffer);

		uint								ReadDDSSurface(
			const char*							pFileName);

		uint								ReadDDSSurface(
			FILE*								pFile);

		uint								Read(
			FILE*								pFile);

		//- Member Access ---------------------------------------------------------
	
		const char*							GetHeader()					const
		{
			return m_Header;
		}

		const uint&							GetHeight()					const
		{
			return m_Height;
		}

		const uint&							GetWidth()					const
		{
			return m_Width;
		}
	
		const uint&							GetLinearSize()				const
		{
			return m_LinearSize;
		}

		const uint&							GetMipMapCount()			const
		{
			return m_MipMapCount;
		}

		const uint&							GetFourCC()					const
		{
			return m_FourCC;
		}

		const ubyte*						GetBufferPointer()			const
		{
			return m_Buffer;
		}
	};

	//- Materials -----------------------------------------------------------------

	struct GLMaterial
	{

	public:

		//- Constructor -----------------------------------------------------------

		GLMaterial()
		{
			//	Set default colors
			this->Ambient			= color4(color3(0.1f), 1.0f);
			this->Diffuse			= color4(color3(0.5f), 1.0f);
			this->Specular			= color4(color3(0.5f), 1.0f);

			this->Shininess			= 0.0f;
			this->MaterialType		= 0;
			this->Surface0			= 0;
			this->Surface1			= 0;
			this->Surface2			= 0;
			this->Surface3			= 0;
			this->Surface4			= 0;
			this->Surface5			= 0;
			this->Surface6			= 0;
			this->Surface7			= 0;
		}

		// -Disk operations -------------------------------------------------------

		uint ReadFromDisk(FILE* pFile) {
			if(fread(this, sizeof(*this), 1, pFile) != 1)
				return false;

			return true;
		}

		uint WriteToDisk(FILE* pFile) {
			if(fwrite(this, sizeof(*this), 1, pFile) != 1)
				return false;

			return true;
		}

		void setAmbient(const textureid& tex) {
			this->AmbientSurface = tex;
			this->MaterialType |= HasAmbientMap;
		}

		void setDiffuse(const textureid& tex) {
			this->DiffuseSurface = tex;
			this->MaterialType |= HasDiffuseMap;
		}

		void setBumpmap(const textureid& tex) {
			this->BumpmapSurface = tex;
			this->MaterialType |= HasBumpMap;
		}

		void setSpecular(const textureid& tex) {
			this->SpecularSurface = tex;
			this->MaterialType |= HasSpecularMap;
		}

		const bool getMaterialType(const flags& m) const {
			return (this->MaterialType && m);
		}

		void setAmbient(const color4& c)				{ this->Ambient = c; }
		void setDiffuse(const color4& c)				{ this->Diffuse = c; }
		void setSpecular(const color4& c)				{ this->Specular = c; }
		void setShininess(const float& s)				{ this->Shininess = s; }

		void setColors(const color4& Ambient, const color4& Diffuse, const color4& Specular, const float& Shininess) {
			this->Ambient = Ambient;
			this->Diffuse = Diffuse;
			this->Specular = Specular;
		}

		const color4	getAmbient()			const	{ return this->Ambient; }
		const color4	getDiffuse()			const	{ return this->Diffuse; }
		const color4	getSpecular()			const	{ return this->Specular; }
		const float		getShininess()			const	{ return this->Shininess; }
		const textureid getAmbientSurface()		const	{ return this->AmbientSurface; }
		const textureid getDiffuseSurface()		const	{ return this->DiffuseSurface; }
		const textureid getBumpmapSurface()		const	{ return this->BumpmapSurface; }
		const textureid getSpecularSurface()	const	{ return this->SpecularSurface; }
		
	protected:

		color4								Ambient;
		color4								Diffuse;
		color4								Specular;

		float								Shininess;
		flags								MaterialType;

		union {
			textureid						AmbientSurface;
			textureid						Surface0;
		};

		union {
			textureid						DiffuseSurface;
			textureid						Surface1;
		};

		union {
			textureid						BumpmapSurface;
			textureid						Surface2;
		};

		union {
			textureid						SpecularSurface;
			textureid						Surface3;
		};

		union {
			textureid						Surface4;
			textureid						HeightMap;
		};

		textureid							Surface5;
		textureid							Surface6;
		textureid							Surface7;

	};

	//- Polygons ------------------------------------------------------------------

	struct GLFace16
	{
		ushort v1;
		ushort v2;
		ushort v3;

		GLFace16()
		{
			this->v1 = 0;
			this->v2 = 0;
			this->v3 = 0;
		}

		GLFace16(
			ushort							v1,
			ushort							v2,
			ushort							v3)
		{
			this->v1 = v1;
			this->v2 = v2;
			this->v3 = v3;
		}
	};

	struct GLFace32
	{
		uint v1;
		uint v2;
		uint v3;

		GLFace32()
		{
			this->v1 = 0;
			this->v2 = 0;
			this->v3 = 0;
		}

		GLFace32(
			uint							v1,
			uint							v2,
			uint							v3)
		{
			this->v1 = v1;
			this->v2 = v2;
			this->v3 = v3;
		}
	};

	class Polygons
	{	
		uint								m_Material;
		AttribData<GLFace16>				m_Faces;		// Indexed

	public:
	
		//- Disk operations -------------------------------------------------------

		const uint							ReadFromDisk(
			FILE*								pFile);

		const uint							WriteToDisk(
			FILE*								pFile)				const;

		//- Setting ---------------------------------------------------------------

		void								SetFaces(
			const AttribData<GLFace16>&			Src)
		{
			m_Faces = Src;
		}

		void								SetFaces(
			GLFace16*							pFaces,
			uint								NrFaces)
		{
			this->m_Faces.Load(pFaces, NrFaces);
		}

		AttribData<GLFace16>&				SetFaces()
		{
			return m_Faces;
		}

		void								SetMaterial(
			uint								MaterialID)
		{
			m_Material = MaterialID;
		}

		//- Getting ---------------------------------------------------------------

		const uint&							NrFaces()				const
		{
			return m_Faces.Size();
		}

		const uint&							GetMaterial()			const
		{
			return m_Material;
		}

		const AttribData<GLFace16>&			GetFaces()				const
		{
			return m_Faces;
		}

	};

	typedef Polygons poly;

	//- Skinning Stuff ------------------------------------------------------------

	struct VertexInf
	{
		float								Weight;
		ubyte								Joint;

		VertexInf()
		{
			this->Weight = 0.0f;
			this->Joint = 0;
		}

		VertexInf(const ubyte Joint, const float Weight)
		{
			this->Weight = Weight;
			this->Joint = Joint;
		}
	};

	typedef VertexInf vinf;
	typedef AttribData<vinf> vinfs;

	//- Helpers -------------------------------------------------------------------

	struct MeshBounds
	{
		float								m_EdgeTop;
		float								m_EdgeRight;
		float								m_EdgeBottom;
		float								m_EdgeLeft;
		float								m_EdgeFront;
		float								m_EdgeBack;
		float								m_SphereCenter;	//	Average between top and bottom
		float								m_SphereRadius;	//	Distance from center to top

		MeshBounds()
			:	m_EdgeTop(0.0f)
			,	m_EdgeRight(0.0f)
			,	m_EdgeBottom(0.0f)
			,	m_EdgeLeft(0.0f)
			,	m_EdgeFront(0.0f)
			,	m_EdgeBack(0.0f)
			,	m_SphereCenter(0.0f)
			,	m_SphereRadius(0.0f) { }
	};

	//- Meshdata ------------------------------------------------------------------

	class MeshData
	{
		String								m_Name;
		uint								m_ParentJoint;
		MeshBounds							m_Bounds;
		AttribData<vec3>					m_VertexArray;
		AttribData<vec3>					m_NormalArray;
		AttribData<vec2>					m_UVArray;
		AttribData<vec3>					m_ColorArray;
		AttribData<vec3>					m_TangentArray;
		AttribData<vec3>					m_BitangentArray;
		AttribData<poly>					m_Polygons;

		//	Skinning stuff
		AttribData<uvec4>					m_JointIndices;
		AttribData<vec4>					m_Weights1;
		AttribData<vec4>					m_Weights2;
		AttribData<vec4>					m_Weights3;
		AttribData<vec4>					m_Weights4;

	public:

		MeshData()
			:	m_ParentJoint(0)
		{
		
		}

		~MeshData()
		{
			m_VertexArray.Clear();
			m_NormalArray.Clear();
			m_UVArray.Clear();
			m_ColorArray.Clear();
			m_TangentArray.Clear();
			m_BitangentArray.Clear();
			m_Polygons.Clear();
			m_JointIndices.Clear();
			m_Weights1.Clear();
			m_Weights2.Clear();
			m_Weights3.Clear();
			m_Weights4.Clear();
		}

		MeshData(const MeshData& rhs)
			:	m_Name(rhs.m_Name)
			,	m_Bounds(rhs.m_Bounds)
			,	m_ParentJoint(rhs.m_ParentJoint)
			,	m_VertexArray(rhs.m_VertexArray)
			,	m_NormalArray(rhs.m_NormalArray)
			,	m_UVArray(rhs.m_UVArray)
			,	m_TangentArray(rhs.m_TangentArray)
			,	m_BitangentArray(rhs.m_BitangentArray)
			,	m_Polygons(rhs.m_Polygons)
			,	m_JointIndices(rhs.m_JointIndices)
			,	m_ColorArray(rhs.m_ColorArray)
		{}

		void								SetBounds();

		const MeshBounds&					GetBounds()					const
		{
			return m_Bounds;
		}

		void								SetParentJoint(
			const uint&							ParentJoint)
		{
			m_ParentJoint = ParentJoint+1;
		}

		const uint&							GetParentJoint()			const
		{
			return m_ParentJoint;
		}

		void								ResizePolyLists(
			uint								NrPolyLists)
		{
			this->m_Polygons.Resize(NrPolyLists);
		}

		//-	Transformations -------------------------------------------------------

	//	void								SetTransform(
	//		const mat4&							Model,
	//		const mat4&							View)
	//	{
	//		const mat4 MV	= View * Model;
	//
	//		m_VertexArray.TransformPositions(MV);
	//		m_NormalArray.TransformDirections(MV);
	//		m_TangentArray.TransformDirections(MV);
	//		m_BitangentArray.TransformDirections(MV);
	//	}

		//- Disk operations -------------------------------------------------------
	
		const uint							ReadFromDisk(
			FILE*								pFile);

		const uint							WriteToDisk(
			FILE*								pFile) const;

		//- Calculate stuff -------------------------------------------------------

		void								GenerateNormals(
			const bool&							InvertNormals);
		void								GenerateTangents();

		//- Set -------------------------------------------------------------------
	
		void								SetJointIndices(
			const uvec4*						Src,
			const uint							NrItems)
		{
			this->m_JointIndices.Load(Src, NrItems);
		}
	
		void								SetWeights1(
			const vec4*							Src,
			const uint							NrItems)
		{
			m_Weights1.Load(Src, NrItems);
		}

		void								SetWeights2(
			const vec4*							Src,
			const uint							NrItems)
		{
			m_Weights2.Load(Src, NrItems);
		}

		void								SetWeights3(
			const vec4*							Src,
			const uint							NrItems)
		{
			m_Weights3.Load(Src, NrItems);
		}

		void								SetWeights4(
			const vec4*							Src,
			const uint							NrItems)
		{
			m_Weights4.Load(Src, NrItems);
		}

		void								SetVertexArray(
			const AttribData<vec3>&					Src)
		{
			m_VertexArray.Set(Src);
		}

		void								SetNormalArray(
			const AttribData<vec3>&				Src)
		{
			m_NormalArray.Set(Src);
		}
	
		void								SetUVArray(
			const AttribData<vec2>&				Src)
		{
			m_UVArray.Set(Src);
		}

		void								SetColorArray(
			const AttribData<vec3>				Src)
		{
			m_ColorArray.Set(Src);
		}

		void								SetTangentArray(
			const vec3*							pSrc,
			count								NrItems)
		{
			m_TangentArray.Load(pSrc, NrItems);
		}

		void								SetBitangentArray(
			const AttribData<vec3>&				Src)
		{
			m_BitangentArray.Set(Src);
		}

		void								SetPolygonList(
			const AttribData<Polygons>&			Src)
		{
			m_Polygons = Src;
		}

		void								SetPolygonList(
			uint								PolyListIndex,
			GLFace16*							pSource,
			uint								NrItems)
		{
			this->SetPolygonList(PolyListIndex).SetFaces().Load(pSource, NrItems);
		}

		void								SetMaterial(
			uint								PolyListIndex,
			uint								MaterialID)
		{
			this->SetPolygonList(PolyListIndex).SetMaterial(MaterialID);
		}

		//- Change content by return reference (BAD) ------------------------------

		AttribData<vec3>&					SetVertexArray()
		{									
			return m_VertexArray;			
		}

		AttribData<vec3>&					SetNormalArray()
		{									
			return m_NormalArray;			
		}

		AttribData<vec2>&					SetUVArray()
		{									
			return m_UVArray;				
		}

		AttribData<vec3>&					SetTangentArray()
		{									
			return m_TangentArray;
		}

		AttribData<vec3>&					SetBitangentArray()
		{									
			return m_BitangentArray;		
		}

		AttribData<poly>&					SetPolygonArray()
		{									
			return this->m_Polygons;		
		}

		poly&								SetPolygonList(
			count								 Index)
		{
			return m_Polygons.SetElem(Index);
		}

		//- Load ------------------------------------------------------------------

		void								SetName(
			const char*							Name)
		{
			m_Name = Name;
		}

		void								LoadVertexArray(
			vec3*								pVertices,
			count								NrVertices)
		{
			m_VertexArray.Load(pVertices, NrVertices);
		}

		void								LoadVertexArray(
			AttribData<vec3>&					Vertices)
		{
			this->m_VertexArray = Vertices;
		}

		void								LoadNormalArray(
			vec3*								pSource,
			count								NrItems)
		{
			m_NormalArray.Load(pSource, NrItems);
		}

		void								LoadTangentArray(
			const vec3*							pSource,
			count								NrItems)
		{
			m_TangentArray.Load(pSource, NrItems);
		}

		void								LoadBitangentArray(
			const vec3*							pSource,
			count								NrItems)
		{
			m_BitangentArray.Load(pSource, NrItems);
		}

		void								LoadTexCoordsArray(
			vec2								*pTexCoords,
			count								 NrTexCoords)
		{
			m_UVArray.Load(pTexCoords, NrTexCoords);
		}

		//- Get -------------------------------------------------------------------

		const char*							GetName()				const
		{
			return m_Name.Val();
		}

		const AttribData<vec3>&				GetVertexArray()		const
		{
			return m_VertexArray;
		}
	
		const AttribData<vec3>&				GetNormalArray()		const
		{
			return m_NormalArray;
		}
	
		const AttribData<vec2>&				GetUVArray()			const
		{
			return m_UVArray;
		}
	
		const AttribData<vec3>&				GetColorBuffer()		const
		{
			return m_ColorArray;
		}

		const AttribData<vec3>&				GetTangentArray()		const
		{
			return m_TangentArray;
		}
	
		const AttribData<vec3>&				GetBitangentArray()		const
		{
			return m_BitangentArray;
		}

		const AttribData<uvec4>&			GetJointIndices()		const
		{
			return m_JointIndices;
		}
	
		const AttribData<vec4>&				GetWeights1()			const
		{
			return m_Weights1;
		}

		const AttribData<vec4>&				GetWeights2()			const
		{
			return m_Weights2;
		}

		const AttribData<vec4>&				GetWeights3()			const
		{
			return m_Weights3;
		}

		const AttribData<vec4>&				GetWeights4()			const
		{
			return m_Weights4;
		}

		//	Can hold more than one polygonlist
		const AttribData<poly>&				GetPolygonArray()		const
		{
			return this->m_Polygons;
		}
	
		//	Single list of polygons that share the same material
		const Polygons&						GetPolygonList(
			uint								 Index)				const
		{
			return m_Polygons[Index];
		}

		const uint							GetNrPolyLists()		const
		{
			return m_Polygons.Size();
		}

		void								GetAttributeSet(
			ushort								Index,
			const vec3&							Vertex,
			const vec3&							Normal,
			const vec2&							Coords);

	};

	//- Skeleton ------------------------------------------------------------------

	#define	SkinningJoint	0x00000001

	struct BoneData
	{
		String								Name;
		String								InstanceGeometryURL;
		ubyte								ID;						//	1 or higher
		ubyte								ParentID;				//	0 is no parent or root
		mat4								JointMatrix;
		mat4								InverseBindMatrix;
		ushort								NrKeyFrames;
		uint								Flags;
	
		BoneData()
		{
			memset(this, 0, sizeof(BoneData));
		}

		BoneData(const char *Name)
			:	ID(0)
			,	ParentID(0)
			,	NrKeyFrames(0)
			,	Flags(0)
		{
			this->Name = Name;
		}

		BoneData(BoneData const &newBone)
			:	Name(newBone.Name)
			,	InstanceGeometryURL(newBone.InstanceGeometryURL)
			,	ID(newBone.ID)
			,	InverseBindMatrix(newBone.InverseBindMatrix)
			,	JointMatrix(newBone.JointMatrix)
			,	NrKeyFrames(newBone.NrKeyFrames)
			,	ParentID(newBone.ParentID)
			,	Flags(newBone.Flags)
		{ }

		//	Disk Operations -------------------------------------------------------

		const uint							WriteToDisk(
			FILE*								pFile) const;

		const uint							ReadFromDisk(
			FILE*								pFile);

	};

	//- Animations ----------------------------------------------------------------

	class Animations
	{
		AttribData<float>					m_input;
		AttribData<mat4>					m_output;
		AttribData<String>					m_interpolation;
		count								m_NrKeyFrames;

	public:

		Animations()
		{
			memset(this, 0, sizeof(*this));
		}

		Animations(const Animations & rhs)
		{
			memset(this, 0, sizeof(*this));
			m_input			= rhs.m_input;
			m_output		= rhs.m_output;
			m_NrKeyFrames	= rhs.m_NrKeyFrames;
		}

		void								SetNrKeyFrames(
			const count &						NrKeyFrames)
		{
			m_NrKeyFrames = NrKeyFrames;
		}

		const count&						GetNrKeyFrames()		const
		{
			return m_NrKeyFrames;
		}

		void								SetInput(
			float*								Source,
			uint								NrInputs)
		{
			m_input.Load(Source, NrInputs);
		}

		void								SetOutput(
			float*								Source,
			uint								NrOutputs)
		{
			m_output.Load((mat4*)Source, NrOutputs/16);
		}

		const AttribData<float>&			GetKeyFrameTimes()		const
		{
			return m_input;
		}

		const AttribData<mat4>&				GetKeyFrameMatrices()	const
		{
			return m_output;
		}
	
		uint								ReadFromDisk(
			FILE*								pFile)
		{
			uint Header=0, Size = 0;

			for(uint i=0; i<3; i++) {

				fread(&Header, sizeof(Header), 1, pFile);
				if(Header == ChunkAnimationInput) {
					fread(&Size, sizeof(Size), 1, pFile);
					if(!m_input.ReadFromDisk(Size, pFile))
						return false;
				} else if (Header == ChuckAnimationOutput) {
					fread(&Size, sizeof(Size), 1, pFile);
					if(!m_output.ReadFromDisk(Size, pFile))
						return false;
				} else if(Header == ChunkAnimationInterpolation) {
					fread(&Size, sizeof(Size), 1, pFile);
					if(!m_interpolation.ReadFromDisk(Size, pFile))
						return false;
				}
			}

			m_NrKeyFrames = m_input.Size();

			return true;
		}

		uint								WriteToDisk(
			FILE*								pFile)
		{
			if(!m_input.WriteToDisk(ChunkAnimationInput, pFile))
				return 0;

			if(!m_output.WriteToDisk(ChuckAnimationOutput, pFile))
				return 0;

			if(!m_interpolation.WriteToDisk(ChunkAnimationInterpolation, pFile))
				return 0;

			return 1;
		}
	};

	//- Modeldata Class -----------------------------------------------------------

	class ModelData
	{

	private:

		String								m_Name;
		String								m_ModelPath;
		mat4								m_BindShapeMatrix;
		uint								m_MaxSkinWeights;

		Library<MeshData>					m_MeshData;
		Library<GLSurface>					m_SurfaceData;
		Library<GLMaterial>					m_MaterialData;
		Library<BoneData>					m_BoneData;
		Library<Animations>					m_Animations;

	public:

		ModelData()
		{
			memset(this, 0, sizeof(ModelData));
		}

		//	Optimize mesh
		void SortIndexedMesh();
		void GenerateTangents();
		void SortSurfaces();

		void RecalculateNormals() {
			for(uint i=0; i<m_MeshData.Size(); i++)
				m_MeshData.SetItem(i).GenerateNormals(false);
		}

		//- Troubleshooting -------------------------------------------------------

		void DumpSkinData() const;
		void DumpModelData() const;
		void DumpIndexedAttribData() const;

		//- Safe Meshdata to disk -------------------------------------------------

		uint WriteToDisk();
		glerror ReadFromDisk(const String & ModelName);

		//- Set -------------------------------------------------------------------

		void								SetName(
			const char*							Name)
		{
			m_Name = Name;
		}

		void								SetBounds()
		{
			for(uint i=0; i<m_MeshData.Size(); i++)
				m_MeshData.SetItem(i).SetBounds();
		}

		void								SetParentJoint(
			const uint&							MeshIndex,
			const uint&							JointIndex)
		{
			MeshData& mesh = this->m_MeshData.SetItem(MeshIndex);

			mesh.SetParentJoint(JointIndex);
		}

		const uint&							GetParentJoint(
			const uint&							MeshIndex,
			const uint&							PolyListIndex)			const
		{
			return this->m_MeshData.GetItem(MeshIndex).GetParentJoint();
		}

		void								ResizeMeshData(
			const uint							NrMeshes)
		{
			this->m_MeshData.Resize(NrMeshes);
		}

		void								ResizeMaterialData(
			const uint							NrMaterials)
		{
			this->m_MaterialData.Resize(NrMaterials);
		}

		void								ResizeSurfaceData(
			const uint							NrSurfaces)
		{
			this->m_SurfaceData.Resize(NrSurfaces);
		}

		void								SetModelPath(
			const char*							Path)
		{
			m_ModelPath = Path;
		}

		void								SetMeshData(
			MeshData*							pSource,
			count								NrItems)
		{
			this->m_MeshData.Load(pSource, NrItems);
		}

		void								SetMeshData(
			index								i,
			const MeshData&						Item)
		{
			this->m_MeshData.Set(i, Item);
		}

		void								SetMeshData(
			const Library<MeshData>&			Src)
		{
			m_MeshData = Src;
		}

		void								SetSurfaceData(
			const Library<GLSurface>&			Src)
		{
			m_SurfaceData = Src;
		}

		void								SetSurfaceData(
			const uint							Item,
			const GLSurface&					Surface)
		{
			this->m_SurfaceData.Set(Item, Surface);
		}

		void								SetSurfaceData(
			GLSurface*						pSource,
			count								NrItems)
		{
			this->m_SurfaceData.Load(pSource, NrItems);
		}

		void								SetMaterialData(
			index								i,
			const GLMaterial&					Item)
		{
			this->m_MaterialData.Set(i, Item);
		}

		void								SetMaterialData(
			const Library<GLMaterial>&		Src)
		{
			m_MaterialData = Src;
		}

		void								SetMaterialData(
			GLMaterial*						pSource,
			count								NrItems)
		{
			this->m_MaterialData.Load(pSource, NrItems);
		}

		void								SetBoneData(
			BoneData*							pSource,
			count								NrItems)
		{
			this->m_BoneData.Load(pSource, NrItems);
		}

		void								SetBindShapeMatrix(
			const mat4&							Source)
		{
			this->m_BindShapeMatrix = Source;
		}

		void								SetMaxWeights(
			const uint							MaxWeights)
		{
			this->m_MaxSkinWeights = MaxWeights;
		}

		//- Setting Subcontainers -------------------------------------------------

		void								SetJointIndices(
			const index							Index,
			const uvec4*						Src,
			const uint							NrItems)
		{
			m_MeshData.SetItem(Index).SetJointIndices(Src, NrItems);
		}

		void								SetWeights1(
			const index							Index,
			const vec4*							Src,
			const uint							NrItems)
		{
			m_MeshData.SetItem(Index).SetWeights1(Src, NrItems);
		}

		void								SetWeights2(
			const index							Index,
			const vec4*							Src,
			const uint							NrItems)
		{
			m_MeshData.SetItem(Index).SetWeights2(Src, NrItems);
		}

		void								SetWeights3(
			const index							Index,
			const vec4*							Src,
			const uint							NrItems)
		{
			m_MeshData.SetItem(Index).SetWeights3(Src, NrItems);
		}

		void								SetWeights4(
			const index							Index,
			const vec4*							Src,
			const uint							NrItems)
		{
			m_MeshData.SetItem(Index).SetWeights4(Src, NrItems);
		}

		void								SetVertexArray(
			const index							Index,
			vec3*								pSource,
			count								NrItems)
		{
			m_MeshData.SetItem(Index).LoadVertexArray(pSource, NrItems);
		}

		void								SetNormalArray(
			index								Index,
			vec3*								pSource,
			count								NrItems)
		{
			m_MeshData.SetItem(Index).LoadNormalArray(pSource, NrItems);
		}

		void								SetTexCoordsArray(
			index								Index,
			vec2*								pSource,
			count								NrItems)
		{
			m_MeshData.SetItem(Index).LoadTexCoordsArray(pSource, NrItems);
		}

		void								SetTangentArray(
			index								Index,
			const vec3*							pSource,
			count								NrItems)
		{
			m_MeshData.SetItem(Index).LoadTangentArray(pSource, NrItems);
		}

		void								SetBitangentArray(
			index								Index,
			const vec3*							pSource,
			count								NrItems)
		{
			m_MeshData.SetItem(Index).LoadBitangentArray(pSource, NrItems);
		}

		void								SetPolygonList(
			index								MeshIndex,
			index								PolyListIndex,
			GLFace16*							pSource,
			count								NrItems)
		{
			m_MeshData.SetItem(MeshIndex).SetPolygonList(PolyListIndex).SetFaces().Load(pSource, NrItems);
		}

		void								SetAnimations(
			Animations*							Source,
			count								NrItems)
		{
			m_Animations.Load(Source, NrItems);
		}

		//- Add -------------------------------------------------------------------

		void								 AddMeshData(
			MeshData							*pSource,
			count								 NrItems)
		{
			this->m_MeshData.Append(pSource, NrItems);
		}

		void								 AddSurfaceData(
			GLSurface							*pSource,
			count								 NrItems)
		{
			this->m_SurfaceData.Append(pSource, NrItems);
		}

		void								 AddMaterialData(
			GLMaterial						*pSource,
			count								 NrItems)
		{
			this->m_MaterialData.Append(pSource, NrItems);
		}

		void								 AddBoneData(
			BoneData							*pSource,
			count								 NrItems)
		{
			this->m_BoneData.Append(pSource, NrItems);
		}

		//- Get -------------------------------------------------------------------

		const char*							GetName()				const
		{
			return m_Name.Val();
		}

		const uint							GetMaxWeights()			const
		{
			return m_MaxSkinWeights;
		}

		const char*							GetModelPath()			const
		{
			return m_ModelPath.Val();
		}

		const Library<GLSurface>&			GetSurfaceData()		const
		{
			return m_SurfaceData;
		}

		const Library<GLMaterial>&			GetMaterialData()		const
		{
			return m_MaterialData;
		}

		const Library<MeshData>&			GetMeshData()			const
		{
			return m_MeshData;
		}

		const MeshData&						GetMeshData(
			index								Index)				const
		{
			return m_MeshData.GetItem(Index);
		}

		const Library<BoneData>&			GetBoneData()			const
		{
			return m_BoneData;
		}

		const Library<Animations>&			GetAnimations()			const
		{
			return m_Animations;
		}

		const mat4&							GetBSM()				const
		{
			return m_BindShapeMatrix;
		}
	};

	//- Create primitives ---------------------------------------------------------

	void CreateDiamond(MeshData&, const float&);
	void CreateScreenQuad(MeshData&);

	//- Prototypes ----------------------------------------------------------------

	const uint ReadBMPSurface(const char*);

	const uint ReadDDSSurface(const GLSurface&);
	const uint ReadBMPSurface(const GLSurface&);
	const uint ReadSurface(const GLSurface&);

	//- Generic Functions ---------------------------------------------------------

//	const String FindModelPath(const String&);
//	const String FindShaderPath(const String&);
//	const String FindTexturePath(const String&);

}
