
#ifndef _VMODELDATA_H_
#define _VMODELDATA_H_

#include "..\GLEngine\GLData.h"
#include "ColladaDoc.h"

//	Debug switches
//	#define DumpBoneLibEnabled
//  #define VModelData_DisableInvertUVMaps

//- File Handling -------------------------------------------------------------

String FindDAEModelPath(const char *ModelName);

//- Collada specific datacontainers -------------------------------------------

//- Material Stuff ------------------------------------------------------------

namespace GLEngine
{
	struct VSurfaceData : public GLSurface
	{

	public:

		//	These are only used before the actual data is read in.
		String							Name;
		char							Type[4];

		VSurfaceData()
		{
			this->Name = String();
			memset(&this->Type, 0, 4);
		}

		VSurfaceData(
			const char* Name,
			const char* Type)
		{
			this->Name = Name;

			Strcpy(this->Type, Type, 4);
		}

		VSurfaceData(
			const VSurfaceData& rhs)
			: GLSurface(rhs)
		{
			this->Name = rhs.Name;
			Strcpy(this->Type, rhs.Type, 4);
		}

		const String& GetName()			const
		{
			return Name;
		}

		const char* GetType()			const
		{
			return Type;
		}

		uint							Load(
			const char* FilePath)
		{
			String Path = String(FilePath);
			String FileName = String(Name.Val());
			String FullPath = Path + FileName;

			FILE* pFile;
			if (pFile = fopen(FullPath.Val(), "rb")) {
				printf("\nOpening surface '%s'\n", FileName.Val());
				if (!GLSurface::Read(pFile)) {
					fclose(pFile);	//	Load failed
					return false;
				}
			}
			else {
				return false;
			}

			fclose(pFile);
			return true;
		}

		uint							ReadFromDisk(
			FILE* pFile)
		{
			uint Header = 0;
			fread(&Header, sizeof(Header), 1, pFile);
			if (Header == ChunkSurfaceData)
				if (!GLSurface::ReadFromDisk(pFile))
					return false;

			return true;
		}

		uint							WriteToDisk(
			FILE * pFile)
		{
			return GLSurface::WriteToDisk(pFile);
		}

	};

	struct VMaterialData : GLMaterial
	{
		String							Name;
		String							AmbientName;
		String							DiffuseName;
		String							SpecularName;
		String							BumpmapName;
	};

	struct VVertex
	{
		ushort							Position;
		ushort							Normal;
		ushort							Texcoord;
	};

	struct VFace
	{
		VVertex							Vertex[3];
	};

	class VPolygons
	{
		uint							m_Material;
		AttribData<VFace>				m_VFaces;	// Colladaindexed

	public:

		VPolygons()
		{
			m_Material = 0;
		}

		void							LoadVFaces(
			const Triangles& Polys);

		void							LoadVFaces(
			const AttribData<VFace>& Polys)
		{
			m_VFaces = Polys;
		}

		const uint						NrVFaces()				const
		{
			return m_VFaces.Size();
		}

		const AttribData<VFace>& GetVFaces()				const
		{
			return m_VFaces;
		}

		const uint& GetMaterial()			const
		{
			return m_Material;
		}

		AttribData<VFace>& SetVFaces()
		{
			return m_VFaces;
		}

		void							SetMaterial(
			const uint						 MaterialID)
		{
			this->m_Material = MaterialID;
		}

	};

	class VMeshData : public MeshData
	{

	private:

		AttribData<VPolygons>				m_VPolygons;
		AttribData<vinfs>					m_VertexInfs;

	public:

		//	Single list of polygons that share the same material
		const VPolygons& GetVPolygonList(
			uint								Index)				const
		{
			return m_VPolygons.GetElem(Index);
		}

		VPolygons& SetVPolygonList(
			uint								Index)
		{
			return m_VPolygons.SetElem(Index);
		}

		const uint							GetNrVPolyLists()		const
		{
			return m_VPolygons.Size();
		}

		const AttribData<vinfs>& GetVertexInfArray()		const
		{
			return m_VertexInfs;
		}

		void								SetVSkinData(
			const AttribData<vinfs>& Src)
		{
			m_VertexInfs.Set(Src);
		}

		void								SetVSkinData(
			const vinfs* Src,
			const count							NrItems)
		{
			m_VertexInfs.Load(Src, NrItems);
		}

		void								ResizeVPolyLists(
			uint								NrPolyLists)
		{
			this->m_VPolygons.Resize(NrPolyLists);
		}

		//- Set -------------------------------------------------------------------

		void								SetVPolygonList(
			const AttribData<VPolygons>& PolyList)
		{
			m_VPolygons = PolyList;
		}

		const AttribData<vinfs>& GetSkinData(
			const index							Index)				const
		{
			return m_VertexInfs;
		}

		void								SetSkinData(
			const AttribData<vinfs>& Src)
		{
			m_VertexInfs = Src;
		}
	};

	class VModelData : public ModelData
	{

	public:

		VModelData()
		{
			//	memset(this, 0, sizeof(ModelData));
			m_IsIndexed = false;
		}

		//	Creating a usable mesh for OpenGL
		void								SetBindShape();
		void								SetMeshBindLocation();
		void								SetupIndexedMesh();
		void								SetupIndexedSkinnedMesh();
		void								BindToSkeleton();
		void								ConvertSkinToShaderFormat();

		//	Logging and troubleshooting
		void								DumpSkinData();
		void								DumpVSkinData();

		const VMeshData& GetVMeshData(
			index								Mesh)
		{
			return m_VMeshData.GetItem(Mesh);
		}

		const size						GetVMeshDataSize()		const
		{
			return m_VMeshData.Size();
		}

		const uint							GetMaxWeights()			const
		{
			return ModelData::GetMaxWeights();
		}

		void								SetMaxWeights(
			const uint							MaxWeights)
		{
			ModelData::SetMaxWeights(MaxWeights);
		}

		void								SetNrMeshes(
			const uint							NrMeshes)
		{
			this->m_VMeshData.Resize(NrMeshes);
		}

		void								SetVSkinData(
			const index							Index,
			const AttribData<vinfs>& Source)
		{
			this->m_VMeshData.SetItem(Index).SetVSkinData(Source);
		}

		void								SetVSkinData(
			const index							Index,
			const vinfs* pSource,
			const count						NrItems)
		{
			this->m_VMeshData.SetItem(Index).SetVSkinData(pSource, NrItems);
		}

		const AttribData<vinfs>& GetVSkinData(
			const index							Index)				const
		{
			return m_VMeshData.GetItem(Index).GetVertexInfArray();
		}

		void								AddSurfaceData(
			VSurfaceData* pSource,
			count								 NrItems)
		{
			this->m_VSurfaceData.Append(pSource, NrItems);
		}

		void								AddMaterialData(
			VMaterialData* pSource,
			count								 NrItems)
		{
			this->m_VMaterialData.Append(pSource, NrItems);
		}

		void								SetGeometryInstance(
			const std::string& GeometryURL,
			const uint& JointID)
		{
			std::string Name = std::string(GeometryURL.c_str()).erase(0, 1);
			for (uint i = 0; i < this->m_VMeshData.Size(); i++) {
				if (std::string(m_VMeshData.GetItem(i).GetName()) == Name) {
					this->m_VMeshData.SetItem(i).SetParentJoint(JointID);
					break;
				}
			}
		}

		Library<VMeshData>& SetVMeshData()
		{
			return m_VMeshData;
		}

		void								SetVertexArray(
			index								Index,
			vec3* pSource,
			count								NrItems)
		{
			m_VMeshData.SetItem(Index).LoadVertexArray(pSource, NrItems);
		}

		void								SetNormalArray(
			index								Index,
			vec3* pSource,
			count								NrItems)
		{
			m_VMeshData.SetItem(Index).LoadNormalArray(pSource, NrItems);
		}

		void								LoadColladaDoc(
			const ColladaDoc& Doc);

		void								LoadSurfaces(
			const ColladaDoc& Doc);

		void								LoadMaterials(
			const ColladaDoc& Doc);

		void								LoadGeometries(
			const ColladaDoc& Doc);

		void								LoadBones(
			const ColladaDoc& Doc);

		void								LoadSkin(
			const ColladaDoc& Doc);

		void								LoadAnimations(
			const ColladaDoc& Doc);

		void								LoadUpAxis(
			const ColladaDoc& Doc);

	private:

		mat4								m_UpAxis;
		bool								m_IsIndexed;
		Library<VMeshData>					m_VMeshData;
		Library<VMaterialData>				m_VMaterialData;
		Library<VSurfaceData>				m_VSurfaceData;

	};
}

#endif	//	_VMODELDATA_H_