
#pragma once

#include "GLData.h"

namespace GLEngine
{
	//- Buffer operations ---------------------------------------------------------

	const uint LoadIndexData(const AttribData<GLFace16>&);

	//- Class definitions ---------------------------------------------------------

	//	All the keyframes for a given bone
	class GLKeyFrames
	{
		count								m_NrKeyFrames;
		float*								m_Time;
		mat4*								m_JointMatrix;

	public:

		GLKeyFrames()
		{
			m_NrKeyFrames = 0;
			m_Time = nullptr;
			m_JointMatrix = nullptr;
		}

		~GLKeyFrames()
		{
			m_NrKeyFrames = 0;

			if (m_Time)
			{
				delete[] m_Time;
				m_Time = NULL;
			}

			if (m_JointMatrix)
			{
				delete[] m_JointMatrix;
				m_JointMatrix = NULL;
			}
		}

		void								SetKeyFrameData(
			const Animations &					AnimSource)
		{
			m_NrKeyFrames = AnimSource.GetNrKeyFrames();
			m_Time = new float[m_NrKeyFrames];
			m_JointMatrix = new mat4[m_NrKeyFrames];

			memcpy(m_Time, AnimSource.GetKeyFrameTimes().GetBufferPointer(),
				m_NrKeyFrames * sizeof(float));

			memcpy(m_JointMatrix, AnimSource.GetKeyFrameMatrices().GetBufferPointer(),
				m_NrKeyFrames * sizeof(mat4));
		}

		const uint							GetNrKeyFrames()		const
		{
			return m_NrKeyFrames;
		}

		const mat4 GetJointMatrix(const float& Time) const;
	};

	class GLBones
	{

	private:

		ubyte								m_NrJoints;				//	total joints
		ubyte								m_NrSkinningJoints;		//	joints affecting the mesh
		mat4*								m_JointMatrix;
		mat4*								m_InverseBindMatrix;
		mat4*								m_WorldMatrix;
		mat4*								m_SkinningMatrix;
		uint*								m_Flags;
		ubyte*								m_ID;
		ubyte*								m_ParentID;
		GLKeyFrames*						m_KeyFrames;

	public:

		GLBones()
		{
			m_NrJoints = 0;
			m_NrSkinningJoints = 0;
			m_JointMatrix = nullptr;
			m_InverseBindMatrix = nullptr;
			m_WorldMatrix = nullptr;
			m_SkinningMatrix = nullptr;
			m_Flags = nullptr;
			m_ID = nullptr;
			m_ParentID = nullptr;
			m_KeyFrames = nullptr;
		}

		~GLBones()
		{
			Destroy();
		}

		GLBones&					operator=(
			GLBones						rhs)
		{
			Swap(m_KeyFrames, rhs.m_KeyFrames);
			Swap(m_NrJoints, rhs.m_NrJoints);
			Swap(m_NrSkinningJoints, rhs.m_NrSkinningJoints);
			Swap(m_JointMatrix, rhs.m_JointMatrix);
			Swap(m_InverseBindMatrix, rhs.m_InverseBindMatrix);
			Swap(m_WorldMatrix, rhs.m_WorldMatrix);
			Swap(m_SkinningMatrix, rhs.m_SkinningMatrix);
			Swap(m_Flags, rhs.m_Flags);
			Swap(m_ID, rhs.m_ID);
			Swap(m_ParentID, rhs.m_ParentID);
			return *this;
		}

		void Set(const Library<BoneData>& BoneLib)
		{
			m_NrJoints = BoneLib.Size();
			m_NrSkinningJoints = 0;						//	Set during matrix calculations
			m_JointMatrix = new mat4[m_NrJoints];
			m_InverseBindMatrix = new mat4[m_NrJoints];
			m_WorldMatrix = new mat4[m_NrJoints];
			m_SkinningMatrix = new mat4[m_NrJoints];
			m_Flags = new uint[m_NrJoints];
			m_ID = new ubyte[m_NrJoints];
			m_ParentID = new ubyte[m_NrJoints];
			m_KeyFrames = new GLKeyFrames[m_NrJoints];

			for (uint b = 0; b < m_NrJoints; b++) {
				const BoneData & Bones = BoneLib.GetItem(b);
				m_JointMatrix[b] = Bones.JointMatrix;
				m_InverseBindMatrix[b] = Bones.InverseBindMatrix;
				m_WorldMatrix[b] = mat4(1.0f);
				m_SkinningMatrix[b] = mat4(1.0f);
				m_Flags[b] = Bones.Flags;
				m_ID[b] = Bones.ID;
				m_ParentID[b] = Bones.ParentID;
			}
		}

		void Destroy()
		{
			m_NrJoints = 0;
			m_NrSkinningJoints = 0;
			delete[] m_JointMatrix;		m_JointMatrix = NULL;
			delete[] m_InverseBindMatrix;	m_InverseBindMatrix = NULL;
			delete[] m_WorldMatrix;		m_WorldMatrix = NULL;
			delete[] m_SkinningMatrix;		m_SkinningMatrix = NULL;
			delete[] m_Flags;				m_Flags = NULL;
			delete[] m_ID;					m_ID = NULL;
			delete[] m_ParentID;			m_ParentID = NULL;
			delete[] m_KeyFrames;			m_KeyFrames = NULL;
		}

		void LoadKeyFrames(const Library<Animations>& AnimLib);

		const ubyte&						GetNRJoints()				const
		{
			return this->m_NrJoints;
		}

		void								SetNrSkinningJoints(
			const ubyte&						NrSkinningJoints)
		{
			this->m_NrSkinningJoints = NrSkinningJoints;
		}

		const ubyte&						GetNrSkinningJoints()		const
		{
			return m_NrSkinningJoints;
		}

		//- Get matrix ------------------------------------------------------------

		const mat4&							GetInverseBindMatrix(
			const uint&							ID)						const
		{
			return this->m_InverseBindMatrix[ID - 1];
		}

		const mat4&							GetWorldMatrix(
			const uint&							ID)						const
		{
			return this->m_WorldMatrix[ID - 1];
		}

		const mat4&							GetSkinningMatrix(
			const uint&							ID)						const
		{
			return this->m_SkinningMatrix[ID - 1];
		}

		const mat4&							GetParentIBM(
			uint								ID)						const
		{
			return GetInverseBindMatrix(m_ParentID[ID - 1]);
		}

		const ubyte&						GetParentID(
			const ushort&						ID)						const
		{
			return m_ParentID[ID - 1];
		}

		const mat4*							GetParentJointPointer()		const
		{
			return m_SkinningMatrix;
		}

		const mat4*							GetSkinningMatrixPointer(
			const uint&							JointID)				const
		{
			if (JointID)
				return &m_SkinningMatrix[JointID - 1];
			else
				return NULL;
		}

		const mat4*							GetJointMatrixPointer(
			const uint&							JointID)				const
		{
			if (JointID)
				return &m_JointMatrix[JointID - 1];
			else
				return NULL;
		}

		const mat4*							GetIBMPointer(
			const uint&							JointID)				const
		{
			if (JointID)
				return &m_InverseBindMatrix[JointID - 1];
			else
				return NULL;
		}

		const mat4*							GetWorldMatrixPointer(
			const uint&							JointID)				const
		{
			if (JointID)
				return &m_WorldMatrix[JointID - 1];
			else
				return NULL;
		}

		void								DumpBoneData();

		void								IterateJointMatrices();
		void								UpdateJointMatrices(const float&);
	};

	class GLPolygons
	{

	public:

		GLPolygons();

		GLPolygons(count NrFaces)
		{
			this->NrFaces = NrFaces;
		}

		void								LoadMaterials(const GLMaterial&, const uint&);
		void								LoadIndices(const Polygons&);
		void								SetIndexBuffer(const bufferid&, const count&);

		const bufferid&						getAmbient() const;
		const bufferid&						getDiffuse() const;
		const bufferid&						getSpecular() const;
		const bufferid&						getBumpmap() const;
		const bufferid&						getScreen() const;
		const bufferid&						getHeightmap() const;

		const color4&						getAmbientColor() const;
		const color4&						getDiffuseColor() const;
		const color4&						getSpecularColor() const;

		const float&						getShininess() const;
		const flags&						getMaterialType() const;

		const size&							getNrFaces() const;
		const bufferid&						getIndexBuffer() const;

		void								Destroy();

	private:
		//	Material
		bufferid							Ambient;
		bufferid							Diffuse;
		bufferid							Specular;
		bufferid							Bumpmap;
		bufferid							Screen;
		bufferid							Heightmap;

		color4								AmbientColor;
		color4								DiffuseColor;
		color4								SpecularColor;
		float								Shininess;
		flags								MaterialType;

		//	Geometry
		count								NrFaces;
		bufferid							IndexBuffer;
	};

	class GLBuffers
	{

	public:

		GLBuffers();
		GLBuffers(const MeshData& Mesh);

		void								LoadMesh(const MeshData& Mesh);
		void								LoadMaterial(const index& PolygonListIndex, const GLMaterial& Material, const uint& SurfaceOffset);
		void								SetParentJoint(const mat4* const lpParentJoint);
		void								Destroy();
		void								Initialize();

		const bufferid&						getNrVertices() const;
		const bufferid&						getVertexBuffer() const;
		const bufferid&						getNormalBuffer() const;
		const bufferid&						getUVBuffer() const;
		const bufferid&						getColorBuffer() const;
		const bufferid&						getTangentBuffer() const;
		const bufferid&						getBitangentBuffer() const;
		const bufferid&						getJointBuffer() const;
		const bufferid&						getWeight1Buffer() const;
		const bufferid&						getWeight2Buffer() const;
		const bufferid&						getWeight3Buffer() const;
		const bufferid&						getWeight4Buffer() const;
		const mat4*							getParentJointPointer() const;
		const size&							getNrPolyLists() const;
		const AttribData<GLPolygons>&		getPolygons() const;

		void								SetBuffers(const count&, const count&, const bufferid&, const bufferid&, const bufferid&, const bufferid&, const bufferid&, const bufferid&);

	private:

		//	From the MeshData
		count								NrAttributes;

		//	Hardware buffers
		bufferid							VertexBuffer;
		bufferid							NormalBuffer;
		bufferid							UVBuffer;
		bufferid							ColorBuffer;
		bufferid							TangentBuffer;
		bufferid							BitangentBuffer;

		//	Skinning buffers
		bufferid							JointBuffer;
		bufferid							Weight1Buffer;
		bufferid							Weight2Buffer;
		bufferid							Weight3Buffer;
		bufferid							Weight4Buffer;

		mat4*								lpParentJoint;

		//	Polygonlists
		AttribData<GLPolygons>				Polygons;
	};

	class GLModel
	{

	public:

		GLModel()
		{
			m_nrMaterials = 0;
			m_nrMeshes = 0;
			m_nrBones = 0;
			m_nrMaxInfluences = 0;
		}

		~GLModel()
		{
			Destroy();
		}

		GLModel(const GLModel& Param)
			: m_Name(Param.m_Name)
			, m_nrMaterials(Param.m_nrMaterials)
			, m_nrMeshes(Param.m_nrMeshes)
			, m_nrBones(Param.m_nrBones)
			, m_nrMaxInfluences(Param.m_nrMaxInfluences)
			, m_BoneStructure(Param.m_BoneStructure)
			, m_ModelBuffers(Param.m_ModelBuffers)
		{

		}

		GLModel&					operator=(
			GLModel&				rhs)
		{
			Swap(m_Name, rhs.m_Name);
			Swap(m_ModelBuffers, rhs.m_ModelBuffers);
			Swap(m_BoneStructure, rhs.m_BoneStructure);
			Swap(m_nrMaterials, rhs.m_nrMaterials);
			Swap(m_nrMeshes, rhs.m_nrMeshes);
			Swap(m_nrBones, rhs.m_nrBones);
			Swap(m_nrMaxInfluences, rhs.m_nrMaxInfluences);
			return *this;
		}

		void								Destroy();
		void								UpdateJointMatrices(const float&);
		void								DrawSkeleton();
		void								DumpModelInfo();
		void								SetupBuffers(count NrAttribs, count NrFaces)
		{

		}

		const char*							GetName()	const
		{
			return m_Name.Val();
		}

		void								SetName(
			const char*							Name)
		{
			this->m_Name = Name;
		}

		void								SetModelBuffer(
			const index&						Index,
			const GLBuffers&					GLBuffer)
		{
			//	!!	Check if this is OK	!!
			if (Index + 1 > m_ModelBuffers.Size()) {
				m_nrMeshes = Index + 1;
				m_ModelBuffers.Resize(m_nrMeshes);
			}

			//	m_ModelBuffers.Set(Index, GLBuffer);
			this->m_ModelBuffers[Index] = GLBuffer;
		}

		void								LoadMaterial(
			const GLMaterial					Material)
		{
			this->m_ModelBuffers[0].LoadMaterial(0, Material, 0);
		}

		void								SetScreen(
			const uint&							ScreenTextureID);

		void								SetDiffuse(
			const index&						ModelBufferID,
			const index&						PolyListID,
			const index&						TextureID);

		void								SetBumpmap(
			const index&						ModelBufferID,
			const index&						PolyListID,
			const index&						TextureID);

		void								SetSpecular(
			const index&						ModelBufferID,
			const index&						PolyListID,
			const index&						TextureID);

		void								SetMaterialType(
			const index&						ModelBufferID,
			const index&						PolyListID,
			const index&						TypeFlags);

		const AttribData<GLBuffers>&		GetModelBuffers()	const
		{
			return m_ModelBuffers;
		}

		AttribData<GLBuffers>&				GetModelBufferRef()
		{
			return m_ModelBuffers;
		}

		const GLBones*						GetBoneStructure()	const
		{
			return &m_BoneStructure;
		}

		const ushort&						GetNrMeshes()		const
		{
			return m_nrMeshes;
		}

		void								SetBuffers(
			const count& NrAttribs,
			const count& NrFaces,
			const bufferid& Vertices,
			const bufferid& Normals,
			const bufferid& Tangents,
			const bufferid& Bitangents,
			const bufferid& UVCoords,
			const bufferid& Indices)
		{
			m_ModelBuffers.Resize(1);
			m_nrMeshes = 1;

			GLBuffers bufs;
			bufs.SetBuffers(NrAttribs, NrFaces, Vertices, Normals, Tangents, Bitangents, UVCoords, Indices);

			m_ModelBuffers[0] = bufs;
		}

		virtual void						LoadMesh(
			const uint&							Index,
			const MeshData&						Mesh);

		virtual void						LoadMeshLib(
			const Library<MeshData>&			MeshLib);

		void								LoadMesh(
			const MeshData&						Mesh);

		void								LoadSurfLib(
			uint&								SurfaceOffset,
			const Library<GLSurface>&			SurfLib);

		void								LoadMatLib(
			const uint&							SurfaceOffset,
			const Library<MeshData>&			MeshLib,
			const Library<GLMaterial>&		MatLib);

		void								LoadBoneLib(
			const Library<BoneData>&			BoneLib);

		void								LoadAnimLib(
			const Library<Animations>&			AnimLib);

	private:

		//	The name of the model on disk
		String								m_Name;

		//	Counters
		ushort								m_nrMaterials;
		ushort								m_nrMeshes;
		ushort								m_nrBones;
		ushort								m_nrMaxInfluences;

		GLBones								m_BoneStructure;

		//	Harware IDs for polygonlists and their materials
		AttribData<GLBuffers>				m_ModelBuffers;

	};	//	GLModelBase
}
