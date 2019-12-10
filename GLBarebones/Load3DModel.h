#pragma once

#include "..\GLEngine\General.h"

namespace GLGeneral
{
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
			: m_EdgeTop(0.0f)
			, m_EdgeRight(0.0f)
			, m_EdgeBottom(0.0f)
			, m_EdgeLeft(0.0f)
			, m_EdgeFront(0.0f)
			, m_EdgeBack(0.0f)
			, m_SphereCenter(0.0f)
			, m_SphereRadius(0.0f)
		{

		}
	};

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

	class Polygons
	{
		uint								m_Material;
		AttribData<GLFace16>				m_Faces;		// Indexed

	public:

		//- Disk operations -------------------------------------------------------

		const uint							ReadFromDisk(
			FILE*								pFile)
		{
			uint Header = 0, Size = 0;
			fread(&m_Material, sizeof(m_Material), 1, pFile);
			fread(&Header, sizeof(Header), 1, pFile);
			fread(&Size, sizeof(Size), 1, pFile);
			if (Header == ChunkMeshDataPolyList) {
				if (!m_Faces.ReadFromDisk(Size, pFile))
					return false;
			}

			return true;
		}

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

	template <class Attribs>
	const uint									LoadAttribData(
		const AttribData<Attribs>&					Source)
	{
		uint tBuffer = 0;

		Source.GetElemPointer(0);

		if (Source.Size()) {
			glGenBuffers(1, &tBuffer);													GLERROR();
			glBindBuffer(GL_ARRAY_BUFFER, tBuffer);										GLERROR();
			glBufferData(GL_ARRAY_BUFFER,
				Source.Size() * sizeof(Attribs),
				Source.GetBufferPointer(),
				GL_STATIC_DRAW);														GLERROR();
		}

		return tBuffer;
	}

	const uint									LoadIndexData(
		const AttribData<GLFace16>&					Source)
	{
		uint tBuffer = 0;

		void* pbuffer = (void*)Source.GetBufferPointer();
		const uint size = Source.Size();

		if (size) {
			glGenBuffers(1, &tBuffer);													GLERROR();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tBuffer);								GLERROR();
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				size * sizeof(GLFace16),
				pbuffer,
				GL_STATIC_DRAW);														GLERROR();
		}

		return tBuffer;
	}

	//template <class Attribs>
	const uint								LoadIndexData(
		const AttribData<poly>&					Source)
	{
		uint tBuffer = 0;

		void* pBuffer = (void*)Source.GetElem(0).GetFaces().GetBufferPointer();
		uint size = Source.GetElem(0).GetFaces().Size();

		if (Source.Size()) {
			glGenBuffers(1, &tBuffer);													GLERROR();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tBuffer);								GLERROR();
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				size * sizeof(GLFace16),
				pBuffer,
				GL_STATIC_DRAW);														GLERROR();
		}

		return tBuffer;
	}

	glerror Load3DModel(
		const char*				ModelName,
		bufferid&				Vertices,
		bufferid&				TexCoords,
		bufferid&				Normals,
		bufferid&				Tangents,
		bufferid&				Bitangents,
		bufferid&				Indices,
		uint&					NrPolys,
		bufferid&				AmbientMap,
		bufferid&				DiffuseMap,
		bufferid&				NormalMap,
		bufferid&				SpecularMap)
	{
		glerror error = glError;

		const String ResourcePath = FindResourcePath("models");
		const String FilePath = ResourcePath + "\\" + ModelName + "\\" + ModelName + ".3dm";

		if (FILE* pFile = fopen(FilePath.Val(), "rb"))
		{
			MeshBounds							Bounds;
			AttribData<vec3>					VertexArray;
			AttribData<vec3>					NormalArray;
			AttribData<vec2>					UVArray;
			AttribData<vec3>					ColorArray;
			AttribData<vec3>					TangentArray;
			AttribData<vec3>					BitangentArray;
			AttribData<poly>					Polygons;

			AttribData<uvec4>					JointIndices;
			AttribData<vec4>					Weights1;
			AttribData<vec4>					Weights2;
			AttribData<vec4>					Weights3;
			AttribData<vec4>					Weights4;

			uint Header = 0, Size = 0;
			while (fread(&Header, sizeof(Header), 1, pFile) == 1)
			{
				switch(Header)
				{
				case LibraryMeshData:
					while (fread(&Header, 1, 4, pFile) == 4)
					{
						switch (Header)
						{
						case ChunkMeshDataVertices:
							fread(&Size, sizeof(Size), 1, pFile);
							if (!VertexArray.ReadFromDisk(Size, pFile))
								return error;
							break;

						case ChunkMeshDataNormals:
							fread(&Size, sizeof(Size), 1, pFile);
							if (!NormalArray.ReadFromDisk(Size, pFile))
								return error;
							break;

						case ChunkMeshDataTexCoords:
							fread(&Size, sizeof(Size), 1, pFile);
							if (!UVArray.ReadFromDisk(Size, pFile))
								return error;
							break;

						case ChunkMeshDataTangents:
							fread(&Size, sizeof(Size), 1, pFile);
							if (!TangentArray.ReadFromDisk(Size, pFile))
								return error;
							break;

						case ChunkMeshDataBitangents:
							fread(&Size, sizeof(Size), 1, pFile);
							if (!BitangentArray.ReadFromDisk(Size, pFile))
								return error;
							break;

						case ChunkMeshDataPolygons:
							fread(&Size, sizeof(Size), 1, pFile);
							Polygons.Resize(Size);
							for (uint p = 0; p < Size; p++)
								if (!Polygons.SetElem(p).ReadFromDisk(pFile))
									return error;

							NrPolys = Polygons.GetElem(0).GetFaces().Size();
							break;

						case ChunkMeshDataIndices:
							fread(&Size, sizeof(Size), 1, pFile);
							if (!JointIndices.ReadFromDisk(Size, pFile))
								return error;
							break;

						case ChunkMeshDataWeights1:
							fread(&Size, sizeof(Size), 1, pFile);
							if (!Weights1.ReadFromDisk(Size, pFile))
								return error;
							break;

						case ChunkMeshDataWeights2:
							fread(&Size, sizeof(Size), 1, pFile);
							if (!Weights1.ReadFromDisk(Size, pFile))
								return error;
							break;

						case ChunkMeshDataWeights3:
							fread(&Size, sizeof(Size), 1, pFile);
							if (!Weights3.ReadFromDisk(Size, pFile))
								return error;
							break;

						case ChunkMeshDataWeights4:
							fread(&Size, sizeof(Size), 1, pFile);
							if (!Weights4.ReadFromDisk(Size, pFile))
								return error;
							break;

						case ChunkMeshDataBounds:
							fread(&Bounds, sizeof(Bounds), 1, pFile);
							break;

						case ChunkMeshDataEnd:
							Vertices = LoadAttribData(VertexArray);
							TexCoords = LoadAttribData(UVArray);
							Normals = LoadAttribData(NormalArray);
							Tangents = LoadAttribData(TangentArray);
							Bitangents = LoadAttribData(BitangentArray);
							Indices = LoadIndexData(Polygons);

							break;
						}
					}

					error = glOK;
					break;

				case LibrarySurfaceData:
					while (fread(&Header, 1, 4, pFile) == 4);
					switch (Header)
					{
					case ChunkSurfaceData:

						fread(&Size, sizeof(Size), 1, pFile);
						for (uint i = 0; i < Size; i++)
						{
							char DDSHeader[4];
							fread(&DDSHeader, 1, 4, pFile);
							if (strncmp(DDSHeader, "DDS ", 4) == 0) {
								LoadDDSurfaceFileContents(pFile);
							}
							else {
								printf("not ok\n");
								fclose(pFile);
								return error;
							}
						}

					}

					error = glOK;
					break;
				}
			}
		}

		return error;
	}
}