
#include <stdio.h>
#include <stdlib.h>

#include "GLData.h"

#pragma warning (disable : 4996)	//	Disable fopen warning.

//- Debugging -----------------------------------------------------------------

namespace GLEngine
{
	//- Loading Non-Mesh Stuff ----------------------------------------------------

	const uint ReadBMPSurface(const char* FileName)
	{
		uint SurfaceID = 0;
		GLSurface BMPSurface;

		if (BMPSurface.ReadBMPSurface(FileName)) {
			SurfaceID = ReadBMPSurface(BMPSurface);
		}

		return SurfaceID;
	}

	const uint ReadDDSSurface(const GLSurface& Surf)
	{
		uint height				= Surf.GetHeight();
		uint width				= Surf.GetWidth();
		const uint& linearSize	= Surf.GetLinearSize();
		const uint& mipMapCount	= Surf.GetMipMapCount();
		const uint& fourCC		= Surf.GetFourCC();
		const uint& components	= (fourCC == FOURCC_DXT1) ? 3 : 4;

		uint format;

		format = (fourCC == FOURCC_DXT1) ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : 0;
		format = (fourCC == FOURCC_DXT3) ? GL_COMPRESSED_RGBA_S3TC_DXT3_EXT : 0;
		format = (fourCC == FOURCC_DXT5) ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : 0;

//		switch(fourCC)
//		{ 
//		case FOURCC_DXT1:
//			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
//			break;
//		case FOURCC_DXT3:
//			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
//			break;
//		case FOURCC_DXT5:
//			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
//			break;
//		default:
//			return 0; 
//		}
		
		// Create one OpenGL texture
		uint TextureID = 0;
		glGenTextures(1, &TextureID);																	GLERROR();
		glBindTexture(GL_TEXTURE_2D, TextureID);														GLERROR();
		
		uint blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
		uint offset = 0;

		/* load the mipmaps */
		for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
		{
			unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
				0, size, Surf.GetBufferPointer() + offset);

			offset += size;
			width  /= 2;
			height /= 2;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);								GLERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);								GLERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);									GLERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);									GLERROR();

		return TextureID;
	}

	const uint ReadBMPSurface(const GLSurface& Surf)
	{
		textureid Texture = 0;
		LoadImageBMP(Texture, Surf.GetWidth(), Surf.GetHeight(), Surf.GetBufferPointer());

		return Texture;
	}

	const uint ReadSurface(const GLSurface& Surf)
	{
		if(strncmp(Surf.GetHeader(), "DDS ", 4) == 0)
			return ReadDDSSurface(Surf);
		else if(strncmp(Surf.GetHeader(), "BM", 2) == 0)
			return ReadBMPSurface(Surf);

		return false;
	}

	//- Surfaces ------------------------------------------------------------------

	uint GLSurface::ReadDDSSurface(const char* pFileName)
	{
		//	First clear the original image, if present
		if(m_Buffer) {
			delete [] m_Buffer;
			memset(this, 0, sizeof(*this));
		}

		FILE* pFile = fopen(pFileName, "rb");
		if(!pFile)
			return false;

		fread(m_Header, 1, 4, pFile);
		if (strncmp(m_Header, "DDS ", 4) == 0)
			ReadDDSSurface(pFile);
		else
			return false;

		fclose(pFile);

		return true;
	}

	uint GLSurface::ReadDDSSurface(FILE* pFile)
	{
		//	Read the header
		unsigned char header[124];
		if(fread(&header, 1, 124, pFile) != 124) {
			LogError("DDS file header incomplete.");
			return false;
		}

		m_Height		= *(unsigned int*)&(header[8 ]);	printf("-Height      : %i\n", m_Height);
		m_Width			= *(unsigned int*)&(header[12]);	printf("-Width       : %i\n", m_Width);
		m_LinearSize	= *(unsigned int*)&(header[16]);
		m_MipMapCount	= *(unsigned int*)&(header[24]);	printf("-Mipmapcount : %i\n", m_MipMapCount);
		m_BufferSize	= *(unsigned int*)&(header[28]);	//	Reserved header field.
		m_FourCC		= *(unsigned int*)&(header[80]);

		if (m_BufferSize == 0 || m_BufferSize > 2 * m_LinearSize) {
			//	Set max buffer for temp storage
			m_BufferSize = m_MipMapCount > 1 ? m_LinearSize * 2 : m_LinearSize;
			m_Buffer = new ubyte[m_BufferSize];
			if (m_BufferSize = fread(m_Buffer, 1, m_BufferSize, pFile))
				return true;
		}
		else {
			m_Buffer = new ubyte[m_BufferSize];
			if (fread(m_Buffer, 1, m_BufferSize, pFile) == m_BufferSize)
			return true;
		}

		return false;
	}

	uint GLSurface::ReadBMPSurface(const char* pFileName)
	{
		uint Error = 0;
		FILE* pFile = fopen(pFileName, "rb");
		if (pFile) {
			//	First clear the original image, if present
			if (m_Buffer) {
				delete[] m_Buffer;
				memset(this, 0, sizeof(*this));
				m_Buffer = nullptr;
			}

			fread(m_Header, 1, 4, pFile);
			if (strncmp(m_Header, "BM", 2) == 0) {
				Error = ReadBMPSurface(pFile);
			}

			fclose(pFile);
		}

		return Error;
	}

	uint GLSurface::ReadBMPSurface(FILE* pFile)
	{
		unsigned char header[50];		//	BMP File header is 54 of which we already read 4.

		if(!(fread(header, 1, 50, pFile) == 50)) {
			LogError("Not a valid BMP File.");
			return false;
		}

		m_Height		= *(uint*)&(header[14]);
		m_Width			= *(uint*)&(header[18]);
		m_MipMapCount	= 0;
		m_LinearSize	= 3 * m_Height * m_Width;
		m_BufferSize	= m_LinearSize;
		m_FourCC		= 0;

		m_Buffer = new ubyte[m_BufferSize];

		//	Bitmaps stored upside down
		uint LineWidth = 3 * m_Width;
		for(int i=m_Height-1; i>=0; --i)
			if(fread(m_Buffer+(i*LineWidth), 1, LineWidth, pFile) != LineWidth)
				return false;

		return true;
	}

	uint GLSurface::ReadBMPSurface(const uint Height, const uint Width, const ubyte* pBuffer)
	{
		m_Header[0]		= 'B';
		m_Header[1]		= 'M';
		m_Height		= Height;
		m_Width			= Width;
		m_MipMapCount	= 0;
		m_LinearSize	= 3 * m_Height * m_Width;
		m_BufferSize	= m_LinearSize;
		m_FourCC		= 0;

		printf("-Height      : %i\n", m_Height);
		printf("-Width       : %i\n", m_Width);
		printf("-Mipmapcount : %i\n", m_MipMapCount);

		m_Buffer = new ubyte[m_BufferSize];

		memcpy(m_Buffer, pBuffer, m_LinearSize);

		return true;
	}

	uint GLSurface::Read(FILE* pFile)
	{		
		//	Is DDS file?
		fread(m_Header, 1, 4, pFile);
		if (strncmp(m_Header, "DDS ", 4) == 0) {
			ReadDDSSurface(pFile);
		} else if(strncmp(m_Header, "BM", 2) == 0) {
			ReadBMPSurface(pFile);
		} else {
			printf("not ok\n");
			fclose(pFile);
			return false;
		}

		return true;
	}

	uint GLSurface::ReadFromDisk(FILE* pFile)
	{
		uint Header=0;
		fread(&Header, sizeof(Header), 1, pFile);
		if(Header == ChunkSurfaceData)
			if(!this->Read(pFile))
				return false;

		return true;
	}

	uint GLSurface::WriteToDisk(FILE* pFile)
	{
		uint Header = ChunkSurfaceData;
		fwrite(&Header, sizeof(uint), 1, pFile);

		if(m_Header[0] == 'B' && m_Header[1] == 'M')
			return WriteBMPSurface(pFile);
		else if(m_Header[0] == 'D' && m_Header[1] == 'D' && m_Header[2] == 'S')
			return WriteDDSurface(pFile);

		return false;
	}

	uint GLSurface::WriteDDSurface(FILE* pFile)
	{
		size_t bufsize = m_MipMapCount > 1 ?
			m_LinearSize * 2 : m_LinearSize;

		fwrite(m_Header, sizeof(m_Header), 1, pFile);

		//	Setup DDS file header
		unsigned char header[124];
		memset(header, 0, 124);
		*(unsigned int*)&(header[8 ]) = m_Height;
		*(unsigned int*)&(header[12]) = m_Width;
		*(unsigned int*)&(header[16]) = m_LinearSize;
		*(unsigned int*)&(header[24]) = m_MipMapCount;
		*(unsigned int*)&(header[28]) = m_BufferSize;		//	Use unused field
		*(unsigned int*)&(header[80]) = m_FourCC;

		if(fwrite(&header, sizeof(char), 124, pFile) != 124)
			return false;

		if(fwrite(m_Buffer, sizeof(ubyte), m_BufferSize, pFile) != m_BufferSize)
			return false;

		return true;
	}

	uint GLSurface::WriteBMPSurface(FILE* pFile)
	{
		unsigned char header[54];
		memset(header, 0, 54);

		*(ushort*)&(header[00])	= 0x4d42;
		*(uint*)&(header[14])	= 40;
		*(uint*)&(header[18])	= m_Height;
		*(uint*)&(header[22])	= m_Width;
		*(ushort*)&(header[26])	= 1;
		*(ushort*)&(header[28])	= 24;
		*(uint*)&(header[30])	= 0;
		*(uint*)&(header[34])	= 0;

		if(fwrite(header, 1, 54, pFile) != 54)
			return false;

		//	Bitmaps stored upside down
		uint LineWidth = 3 * m_Width;
		for(int i=m_Height-1; i>=0; --i)
			if(fwrite(m_Buffer+(i*LineWidth), 1, LineWidth, pFile) != LineWidth)
				return false;

		return true;
	}

	//- Polygons ------------------------------------------------------------------

	const uint Polygons::ReadFromDisk(FILE* pFile)
	{
		uint Header=0, Size=0;
		fread(&m_Material, sizeof(m_Material), 1, pFile);
		fread(&Header, sizeof(Header), 1, pFile);
		fread(&Size, sizeof(Size), 1, pFile);
		if(Header == ChunkMeshDataPolyList) {			
			if(!m_Faces.ReadFromDisk(Size, pFile))
				return false;
		}

		return true;
	}

	const uint Polygons::WriteToDisk(FILE* pFile) const
	{
		LogMessage("Writing Polygons, Material %i", m_Material);
		fwrite(&m_Material, sizeof(m_Material), 1, pFile);
		if(!m_Faces.WriteToDisk(ChunkMeshDataPolyList, pFile))
			return false;

		return true;
	}

	//- Mesh stuff ----------------------------------------------------------------

	const uint MeshData::ReadFromDisk(FILE* pFile)
	{
		uint Header=0, Size=0;

		while(fread(&Header, 1, 4, pFile) == 4)
		{
			if(Header == ChunkMeshDataVertices) {
				fread(&Size, sizeof(Size), 1, pFile);
				if(!m_VertexArray.ReadFromDisk(Size, pFile))
					return false;
			} else if(Header == ChunkMeshDataNormals) {
				fread(&Size, sizeof(Size), 1, pFile);
				if(!m_NormalArray.ReadFromDisk(Size, pFile))
					return false;
			} else if(Header == ChunkMeshDataTexCoords) {
				fread(&Size, sizeof(Size), 1, pFile);
				if(!m_UVArray.ReadFromDisk(Size, pFile))
					return false;
			} else if(Header == ChunkMeshDataTangents) {
				fread(&Size, sizeof(Size), 1, pFile);
				if(!m_TangentArray.ReadFromDisk(Size, pFile))
					return false;
			} else if(Header == ChunkMeshDataBitangents) {
				fread(&Size, sizeof(Size), 1, pFile);
				if(!m_BitangentArray.ReadFromDisk(Size, pFile))
					return false;
			} else if(Header == ChunkMeshDataPolygons) {

				fread(&Size, sizeof(Size), 1, pFile);
				m_Polygons.Resize(Size);
				for(uint p=0; p<Size; p++) {
					if(!m_Polygons.SetElem(p).ReadFromDisk(pFile))
						return false;
				}

			} else if(Header == ChunkMeshDataIndices) {
				fread(&Size, sizeof(Size), 1, pFile);
				if(!m_JointIndices.ReadFromDisk(Size, pFile))
					return false;
			} else if(Header == ChunkMeshDataWeights1) {
				fread(&Size, sizeof(Size), 1, pFile);
				if(!m_Weights1.ReadFromDisk(Size, pFile))
					return false;
			} else if(Header == ChunkMeshDataWeights2) {
				fread(&Size, sizeof(Size), 1, pFile);
				if(!m_Weights2.ReadFromDisk(Size, pFile))
					return false;
			} else if(Header == ChunkMeshDataWeights3) {
				fread(&Size, sizeof(Size), 1, pFile);
				if(!m_Weights3.ReadFromDisk(Size, pFile))
					return false;
			} else if(Header == ChunkMeshDataWeights4) {
				fread(&Size, sizeof(Size), 1, pFile);
				if(!m_Weights4.ReadFromDisk(Size, pFile))
					return false;
			} else if (Header == ChunkMeshDataBounds) {
 				fread(&m_Bounds, sizeof(m_Bounds), 1, pFile);
			} else if(Header == ChunkMeshDataEnd)
				return true;
		}

		return true;
	}

	const uint MeshData::WriteToDisk(FILE* pFile) const
	{
		uint Header=0, Size=0;

		Header		= ChunkMeshDataBounds;
		fwrite(&Header, sizeof(Header), 1, pFile);
		fwrite(&m_Bounds, sizeof(m_Bounds), 1, pFile);

		Header = ChunkMeshDataPolygons;
		Size	= m_Polygons.Size();
		fwrite(&Header, sizeof(Header), 1, pFile);
		fwrite(&Size, sizeof(Size), 1, pFile);
		for(uint p=0; p<m_Polygons.Size(); p++) {
			if(!m_Polygons.GetElem(p).WriteToDisk(pFile))
				return false;
		}

		if(!m_VertexArray.WriteToDisk(ChunkMeshDataVertices, pFile))
			return false;

		if(!m_NormalArray.WriteToDisk(ChunkMeshDataNormals, pFile))
			return false;

		if(!m_UVArray.WriteToDisk(ChunkMeshDataTexCoords, pFile))
			return false;

		if(!m_TangentArray.WriteToDisk(ChunkMeshDataTangents, pFile))
			return false;

		if(!m_BitangentArray.WriteToDisk(ChunkMeshDataBitangents, pFile))
			return false;

		if(!m_JointIndices.WriteToDisk(ChunkMeshDataIndices, pFile))
			return false;

		if(!m_Weights1.WriteToDisk(ChunkMeshDataWeights1, pFile))
			return false;

		if(!m_Weights2.WriteToDisk(ChunkMeshDataWeights2, pFile))
			return false;

		if(!m_Weights3.WriteToDisk(ChunkMeshDataWeights3, pFile))
			return false;

		if(!m_Weights4.WriteToDisk(ChunkMeshDataWeights4, pFile))
			return false;

		//	Tail
		uint Tail = ChunkMeshDataEnd;
		fwrite(&Tail, sizeof(Tail), 1, pFile);

		return true;
	}

	void MeshData::GenerateNormals(const bool& InvertNormals)
	{
		//	Sources
		const AttribData<vec3> & Vertices	= m_VertexArray;
		const AttribData<poly> & Faces		= m_Polygons;

		vec3* pNormals = new vec3[Vertices.Size()];
		memset(pNormals, 0, Vertices.Size() * sizeof(vec3));

		//	Go through the polylists
		const uint NrPolyLists = Faces.Size();
		for(uint p=0; p<NrPolyLists; p++) {

			const poly Polylist = Faces.GetElem(p);
			const uint NrFaces = Polylist.NrFaces();
			for(uint f=0; f<NrFaces; f++)
			{
				//	Get vertices
				const uint i1 = Polylist.GetFaces().GetElem(f).v1;
				const uint i2 = Polylist.GetFaces().GetElem(f).v2;
				const uint i3 = Polylist.GetFaces().GetElem(f).v3;

				const vec3 v1 = Vertices.GetElem(i1);
				const vec3 v2 = Vertices.GetElem(i2);
				const vec3 v3 = Vertices.GetElem(i3);

				//	Set Crossproduct
				vec3 facenormal;
				GenerateNormal(v1, v2, v3, facenormal);

				//	Add vertexnormals
				pNormals[i1]+=facenormal;
				pNormals[i2]+=facenormal;
				pNormals[i3]+=facenormal;
			}
		}

		//	Normalize
		vec3* pN = pNormals;
		for(uint n=0; n<Vertices.Size(); n++, pN++)
			*pN = normalize(*pN);

		MeshData::SetNormalArray().Load(pNormals, Vertices.Size());

		delete [] pNormals;
	}

	void MeshData::GenerateTangents()
	{
		//	Create references to attribute data for reading
		const AttribData<vec3>	&Vertices	= m_VertexArray;
		const AttribData<vec3>	&Normals	= m_NormalArray;
		const AttribData<vec2>	&TexCoord	= m_UVArray;;

		//	Temp Storage
		AttribData<vec3> Tangents;		Tangents.Resize(Vertices.Size());
		AttribData<vec3> Bitangents;	Bitangents.Resize(Vertices.Size());

		const uint NrPolylists = m_Polygons.Size();
		for(uint pl=0; pl<NrPolylists; pl++) {

			const Polygons & polylist	= m_Polygons.GetElem(pl);
			const AttribData<GLFace16>	&Faces	=  polylist.GetFaces();

			//	Run through each face
			uint NrFaces = Faces.Size();
			uint Face10p = (NrFaces > 10) ? NrFaces / 10 : 1;
			for(unsigned int face=0; face<NrFaces; face++) {

				//	Get the indices
				const ushort &i1 = Faces.GetElem(face).v1;
				const ushort &i2 = Faces.GetElem(face).v2;
				const ushort &i3 = Faces.GetElem(face).v3;

				//	Indices used to get the vertices...
				const vec3 &v1 = Vertices.GetElem(i1);
				const vec3 &v2 = Vertices.GetElem(i2);
				const vec3 &v3 = Vertices.GetElem(i3);

				//	... and the UVs
				const vec2 &uv1 = TexCoord.GetElem(i1);
				const vec2 &uv2 = TexCoord.GetElem(i2);
				const vec2 &uv3 = TexCoord.GetElem(i3);

				vec3 tangent, bitangent;
				GenerateTangent(v1, v2, v3, uv1, uv2, uv3, tangent, bitangent);

				//	Add all the tangents that share a vertex
				Tangents.Set(i1, tangent);	Bitangents.Set(i1, bitangent);
				Tangents.Set(i2, tangent);	Bitangents.Set(i2, bitangent);
				Tangents.Set(i3, tangent);	Bitangents.Set(i3, bitangent);

				if(face%Face10p == Face10p-1) printf(".");
			}

			printf("done\n");
		}

		//	Normalize the added tangents
		Tangents.Normalize();
		Bitangents.Normalize();

		m_TangentArray.Set(Tangents);
		m_BitangentArray.Set(Bitangents);
	}

	void MeshData::GetAttributeSet(
		ushort								Index,
		const vec3&							Vertex,
		const vec3&							Normal,
		const vec2&							Coords)
	{
		uint i=m_VertexArray.Size();

		for(uint t=i-1; i>0; i--, t=i-1)
			if(m_VertexArray.GetElem(t) == Vertex)
				if(m_NormalArray.GetElem(t) == Normal)
					if(m_UVArray.GetElem(t) == Coords)
						Index = t;
	}

	void MeshData::SetBounds()
	{
		//	Go through the vertices
		float left		= m_VertexArray.GetElem(0).x;
		float right		= m_VertexArray.GetElem(0).x;
		float front		= m_VertexArray.GetElem(0).y;
		float back		= m_VertexArray.GetElem(0).y;
		float top		= m_VertexArray.GetElem(0).z;
		float bottom	= m_VertexArray.GetElem(0).z;

		for(uint v=1; v<m_VertexArray.Size(); ++v) {
			const vec3 Vertex = m_VertexArray.GetElem(v);
			if(Vertex.x < left)		left	= Vertex.x;
			if(Vertex.x > right)	right	= Vertex.x;
			if(Vertex.y < front)	front	= Vertex.y;
			if(Vertex.y > back)		back	= Vertex.y;
			if(Vertex.z < bottom)	bottom	= Vertex.z;
			if(Vertex.z > top)		top		= Vertex.z;
		}

		float center = ((top + bottom) / 2.0f);
		float radius = ((top - bottom) / 2.0f);

		m_Bounds.m_EdgeLeft		= left;
		m_Bounds.m_EdgeRight	= right;
		m_Bounds.m_EdgeFront	= front;
		m_Bounds.m_EdgeBack		= back;
		m_Bounds.m_EdgeBottom	= bottom;
		m_Bounds.m_EdgeTop		= top;
		m_Bounds.m_SphereCenter	= center;
		m_Bounds.m_SphereRadius	= radius;
	}

	//- Bonedata ------------------------------------------------------------------

	const uint							BoneData::WriteToDisk(
		FILE*								pFile) const
	{
		uint ret=0;
		ret += fwrite(&ID, sizeof(ID), 1, pFile);
		ret += fwrite(&ParentID, sizeof(ParentID), 1, pFile);
		ret += fwrite(&JointMatrix, sizeof(JointMatrix), 1, pFile);
		ret += fwrite(&InverseBindMatrix, sizeof(InverseBindMatrix), 1, pFile);
		ret += fwrite(&NrKeyFrames, sizeof(NrKeyFrames), 1, pFile);
		ret += fwrite(&Flags, sizeof(Flags), 1, pFile);

		if(ret != 6)
			return false;

		return true;
	}

	const uint							BoneData::ReadFromDisk(
		FILE*								pFile)
	{
		uint ret=0;
		memset(&Name, 0, sizeof(Name));
		ret += fread(&ID, sizeof(ID), 1, pFile);
		ret += fread(&ParentID, sizeof(ParentID), 1, pFile);
		ret += fread(&JointMatrix, sizeof(JointMatrix), 1, pFile);
		ret += fread(&InverseBindMatrix, sizeof(InverseBindMatrix), 1, pFile);
		ret += fread(&NrKeyFrames, sizeof(NrKeyFrames), 1, pFile);
		ret += fread(&Flags, sizeof(Flags), 1, pFile);

		if(ret != 6)
			return false;

		return true;
	}

	//- Modeldata Class -----------------------------------------------------------

	void ModelData::GenerateTangents()
	{
		printf("Calculating tangents");

		for(uint m=0; m<m_MeshData.Size(); m++)
			m_MeshData.SetItem(m).GenerateTangents();
	}

	//-----------------------------------------------------------------------------
	//- Testing and generating meshes ---------------------------------------------
	//-----------------------------------------------------------------------------

	void CreateDiamond(MeshData& Diamond, const float& Size)
	{
		Diamond.SetName("Diamond");

		AttribData<vec3> Vertices;	Vertices.Resize(6);
		Vertices.Set(0, vec3( 0.0f, 1.0f, 0.0f ) * Size );	//	top
		Vertices.Set(1, vec3( 1.0f, 0.0f, 0.0f ) * Size );	//	right
		Vertices.Set(2, vec3( 0.0f, 0.0f,-1.0f ) * Size );	//	back
		Vertices.Set(3, vec3(-1.0f, 0.0f, 0.0f ) * Size );	//	left
		Vertices.Set(4, vec3( 0.0f, 0.0f, 1.0f ) * Size );	//	front
		Vertices.Set(5, vec3( 0.0f,-1.0f, 0.0f ) * Size );	//	bottom

		vec3 normals[] = {
			vec3(0.0f) - vec3(0.0f,  1.0f,  0.0f),
			vec3(0.0f) - vec3(1.0f,  0.0f,  0.0f),
			vec3(0.0f) - vec3(0.0f,  0.0f, -1.0f),
			vec3(0.0f) - vec3(-1.0f, 0.0f,  0.0f),
			vec3(0.0f) - vec3(0.0f,  0.0f,  1.0f),
			vec3(0.0f) - vec3(0.0f, -1.0f,  0.0f)
		};

		AttribData<vec3> Normals(normals, sizeof(normals));

		AttribData<vec2> TexCoords;	TexCoords.Resize(6);
		TexCoords.Set(0, vec2(0.5f,		1.0f) );
		TexCoords.Set(1, vec2(0.0f,		0.5f) );
		TexCoords.Set(2, vec2(0.25f,	0.5f) );
		TexCoords.Set(3, vec2(0.5f,		0.5f) );
		TexCoords.Set(4, vec2(0.75f,	0.5f) );
		TexCoords.Set(5, vec2(0.5f,		0.0f) );

		GLFace16 faces[] = {
			GLFace16(0, 1, 2),
			GLFace16(0, 2, 3),
			GLFace16(0, 3, 4),
			GLFace16(0, 4, 1),
			GLFace16(5, 2, 1),
			GLFace16(5, 3, 2),
			GLFace16(5, 4, 3),
			GLFace16(5, 1, 4)
		};

		AttribData<GLFace16> Faces(faces, sizeof(faces));

		Polygons Polylist1;
		Polylist1.SetMaterial(0);
		Polylist1.SetFaces(Faces);

		AttribData<poly> Polygons;
		Polygons.Resize(1);
		Polygons.Set(0, Polylist1);

		Diamond.SetVertexArray(Vertices);
		Diamond.SetUVArray(TexCoords);
		Diamond.GenerateNormals(true);
		Diamond.GenerateTangents();
		Diamond.SetPolygonList(Polygons);
	}

	void CreatePatch(MeshData& Mesh)
	{
		Mesh.SetName("Patch");

		AttribData<vec3> aVertex;// = MeshArray.SetVertexArray();
		aVertex.Resize(4);
		aVertex.Set(0, vec3(-10.0f, 0.0f,-10.0f));
		aVertex.Set(1, vec3(-10.0f, 0.0f, 10.0f));
		aVertex.Set(2, vec3( 10.0f, 0.0f,-10.0f));
		aVertex.Set(3, vec3( 10.0f, 0.0f, 10.0f));
		Mesh.SetVertexArray(aVertex);
	
		AttribData<vec2> aTexCoords;// = MeshArray.SetUVArray();
		aTexCoords.Resize(4);
		aTexCoords.Set(0, glm::vec2( 0.0f, 0.0f));
		aTexCoords.Set(1, glm::vec2( 0.0f, 1.0f));
		aTexCoords.Set(2, glm::vec2( 1.0f, 0.0f));
		aTexCoords.Set(3, glm::vec2( 1.0f, 1.0f));
		Mesh.SetUVArray(aTexCoords);

		AttribData<vec3> aNormal;// = MeshArray.SetNormalArray();
		aNormal.Resize(4);
		aNormal.Set(0, vec3(0.0f, 1.0f, 0.0f));
		aNormal.Set(1, vec3(0.0f, 1.0f, 0.0f));
		aNormal.Set(2, vec3(0.0f, 1.0f, 0.0f));
		aNormal.Set(3, vec3(0.0f, 1.0f, 0.0f));
		Mesh.SetNormalArray(aNormal);

		AttribData<vec3> aTangent;// = MeshArray.SetTangentArray();
		aTangent.Resize(4);
		aTangent.Set(0, vec3(1.0f, 0.0f, 0.0f));
		aTangent.Set(1, vec3(1.0f, 0.0f, 0.0f));
		aTangent.Set(2, vec3(1.0f, 0.0f, 0.0f));
		aTangent.Set(3, vec3(1.0f, 0.0f, 0.0f));
		Mesh.SetTangentArray(aTangent.GetBufferPointer(), aTangent.Size());

		AttribData<vec3> aBitangent;// = MeshArray.SetBitangentArray();
		aBitangent.Resize(4);
		aBitangent.Set(0, vec3(0.0f, 0.0f, 1.0f));
		aBitangent.Set(1, vec3(0.0f, 0.0f, 1.0f));
		aBitangent.Set(2, vec3(0.0f, 0.0f, 1.0f));
		aBitangent.Set(3, vec3(0.0f, 0.0f, 1.0f));
		Mesh.SetBitangentArray(aBitangent);

		AttribData<Polygons> aFaces;// = MeshArray.SetPolygonArray();

		Polygons polys;
		polys.SetMaterial(0);
		polys.SetFaces().Resize(2);
		polys.SetFaces().Set(0, GLFace16(0, 1, 2));
		polys.SetFaces().Set(1, GLFace16(0, 2, 3));

		aFaces.Resize(1);
		aFaces.Set(0, polys);
		Mesh.SetPolygonList(aFaces);
	}

	void ModelData::DumpSkinData() const
	{
		LogMessage("");
		LogMessage("Dumping skindata for %s <br>", this->GetName());

		//	Get source references
		const AttribData<vec3>	& Vertices = this->GetMeshData(0).GetVertexArray();
		const AttribData<uvec4>	& Joints = this->GetMeshData(0).GetJointIndices();
		const AttribData<vec4>	& Wghts1 = this->GetMeshData(0).GetWeights1();
		const uint NrVertices = Vertices.Size();

		for(uint i=0; i<NrVertices; i++)
			LogMessage("<b>%.5i:</b> | "
					"<b>vx:</b>%4.4f <b>vy:</b>%4.4f <b>vz:</b>%4.4f | "
					"<b>sw:</b>%.8X | "
					"<b>w1:</b>%4.4f <b>w2:</b>%4.4f <b>w3:</b>%4.4f <b>w4:</b>%4.4f", i,
				Vertices.GetElem(i).x, Vertices.GetElem(i).y, Vertices.GetElem(i).z, 
				Joints.GetElem(i).x,
				Wghts1.GetElem(i).x, Wghts1.GetElem(i).y, Wghts1.GetElem(i).z, Wghts1.GetElem(i).w);
	}
/*
	void ModelData::DumpModelData() const
	{

		const Library<MeshData>&		DMesh		= m_MeshData;
		const Library<GLSurface>&		DSurfaces	= m_SurfaceData;
		const Library<GLMaterial>&	DMaterials	= m_MaterialData;
		const Library<BoneData>&		DBones		= m_BoneData;
		const Library<Animations>&		DAnims		= m_Animations;

		LogHeader("<br>Modeldata for <b>%s</b>", m_Name);
		LogMessage("Number of Surfaces : <b>%i</b>", DSurfaces.Size());
		LogMessage("Number of materials : <b>%i</b>", DMaterials.Size());
		LogMessage("Number of meshes : <b>%i</b>", DMesh.Size());
		LogMessage("Number of bones : <b>%i</b>", DBones.Size());

		//- Log Surfaces ----------------------------------------------------------

		for(uint i=0; i<DSurfaces.Size(); ++i) {
			const GLSurface& Sur = DSurfaces.GetItem(i);
			LogHeader("Surface <b>%i</b> : ", i);
			LogMessage("- Surfacetype : <b>%s</b>", Sur.GetHeader());
			LogMessage("- Width : <b>%i</b>", Sur.GetWidth());
			LogMessage("- Height : <b>%i</b>", Sur.GetHeight());
			LogMessage("- Mipmapcount : <b>%i</b>", Sur.GetMipMapCount());
		}

		//- Log Materials ---------------------------------------------------------

		for(uint i=0; i<DMaterials.Size(); ++i) {

			const GLMaterial& Mat = DMaterials.GetItem(i);
			LogHeader("Material <b>%i</b> : ", i);
			LogMessage("- Diffuse : <b>R:%.2f G:%.2f B:%.2f A:%.2f</b>",
				Mat.Diffuse.r, Mat.Diffuse.g, Mat.Diffuse.b, Mat.Diffuse.a);
			LogMessage("- Ambient : <b>R:%.2f G:%.2f B:%.2f A:%.2f</b>",
				Mat.Ambient.r, Mat.Ambient.g, Mat.Ambient.b, Mat.Ambient.a);
			LogMessage("- Specular : <b>R:%.2f G:%.2f B:%.2f A:%.2f</b>",
				Mat.Specular.r, Mat.Specular.g, Mat.Specular.b, Mat.Specular.a);
			LogMessage("- Shininess : <b>%.2f</b>",
				Mat.Shininess);

			//	Get surfaces
			char *strDiffuse = new char[200];	memset(strDiffuse, 0, 200);
			char *strAmbient = new char[200];	memset(strAmbient, 0, 200);
			char *strSpecular = new char[200];	memset(strSpecular, 0, 200);
			char *strBump = new char[200];		memset(strBump, 0, 200);

			if(Mat.MaterialType & HasDiffuseMap)
				sprintf(strDiffuse, "- DiffuseSurface : <b>%i</b><br>", Mat.DiffuseSurface);
			if(Mat.MaterialType & HasAmbientMap)
				sprintf(strAmbient, "- AmbientSurface : <b>%i</b><br>", Mat.AmbientSurface);
			if(Mat.MaterialType & HasBumpMap)
				sprintf(strBump, "- BumpMapSurface : <b>%i</b><br>", Mat.BumpmapSurface);
			if(Mat.MaterialType & HasSpecularMap)
				sprintf(strSpecular, "- SpecularSurface : <b>%i</b><br>", Mat.SpecularSurface);
			if(Mat.MaterialType)
				LogMessage("%s %s %s %s", strDiffuse, strAmbient, strBump, strSpecular);

			delete [] strDiffuse;
			delete [] strAmbient;
			delete [] strSpecular;
			delete [] strBump;
		}

		//- Log Meshes --------------------------------------------------------
		
		for(uint i=0; i<DMesh.Size(); ++i) {
			const MeshData& mesh = DMesh.GetItem(i);
			LogHeader("Mesh <b>%i</b> : ", i);
			LogMessage("- Vertices : <b>%i</b>", mesh.GetVertexArray().Size());
			LogMessage("- Normals : <b>%i</b>", mesh.GetNormalArray().Size());
			LogMessage("- Texcoords : <b>%i</b>", mesh.GetVertexArray().Size());
			LogMessage("- Tangents : <b>%i</b>", mesh.GetTangentArray().Size());
			LogMessage("- Bitangents : <b>%i</b>", mesh.GetBitangentArray().Size());
			LogMessage("- Indices : <b>%i</b>", mesh.GetJointIndices().Size());
			LogMessage("- Weights : <b>%i</b>", mesh.GetWeights1().Size());
			LogMessage("- Polygonlists : <b>%i</b>", mesh.GetNrPolyLists());

			for(uint j=0; j<mesh.GetNrPolyLists(); ++j) {
				const Polygons& polys = mesh.GetPolygonArray().GetElem(j);
				LogMessage("-- List : <b>%i</b>", j);
				LogMessage("-- Material : <b>%i</b>", polys.GetMaterial());
				LogMessage("-- Faces : <b>%i</b>", polys.NrFaces());
			}
		}
	}
*/
	void ModelData::DumpIndexedAttribData() const
	{
		LogMessage("<br>Dumping attribdata for <b>%s</b><br>", this->GetName());

		const uint NrMeshes = this->GetMeshData().Size();
	
		for(uint m=0; m<NrMeshes; m++)
		{
			const AttribData<vec3>	& Vertices = this->GetMeshData(m).GetVertexArray();
			const AttribData<vec3>	& Normals  = this->GetMeshData(m).GetNormalArray();
			const AttribData<vec3>	& Tangents = this->GetMeshData(m).GetTangentArray();
			const AttribData<vec3>	& Bitangents = this->GetMeshData(m).GetBitangentArray();
			const AttribData<vec2>	& Coords =  this->GetMeshData(m).GetUVArray();
			const uint NrVertices = Vertices.Size();

			LogMessage("<cr><b>Dumping Mesh %i, %i vertices</b>", m, NrVertices);

			for(uint i=0; i<NrVertices; i++)
				LogMessage("<b>%.5i:</b> | "
						"<b>vx:</b>%4.4f <b>vy:</b>%4.4f <b>vz:</b>%4.4f | "
						"<b>nx:</b>%4.4f <b>ny:</b>%4.4f <b>nz:</b>%4.4f | "
						"<b>tx:</b>%4.4f <b>ty:</b>%4.4f <b>tz:</b>%4.4f | "
						"<b>bx:</b>%4.4f <b>by:</b>%4.4f <b>bz:</b>%4.4f | "
						"<b>u:</b>%4.4f <b>v:</b>%4.4f", i,
					Vertices.GetElem(i).x, Vertices.GetElem(i).y, Vertices.GetElem(i).z,
					Normals.GetElem(i).x, Normals.GetElem(i).y, Normals.GetElem(i).z,
					Tangents.GetElem(i).x, Tangents.GetElem(i).y, Tangents.GetElem(i).z,
					Bitangents.GetElem(i).x, Bitangents.GetElem(i).y, Bitangents.GetElem(i).z,
					Coords.GetElem(i).x, Coords.GetElem(i).y);
		}
	}

	//- Disk operations -----------------------------------------------------------

	glerror ModelData::ReadFromDisk(const String& ModelName)
	{
		String Folder = FindResourcePath("models");
		m_ModelPath = Folder + "\\" + ModelName + "\\";
		String FilePath = m_ModelPath + ModelName + ".3dm";

		FILE* pFile = fopen(FilePath.Val(), "rb");

		if(pFile)
		{
			m_Name = ModelName;
			m_ModelPath = FilePath;

			uint Header=0, Size=0;
			while(fread(&Header, sizeof(Header), 1, pFile) == 1)
			{
				if(Header == LibraryMeshData) {
					m_MeshData.ReadFromDisk(pFile);
				} else if(Header == LibrarySurfaceData) {
					m_SurfaceData.ReadFromDisk(pFile);
				} else if(Header == LibraryMaterialData) {
					m_MaterialData.ReadFromDisk(pFile);
				} else if(Header == LibraryBoneData) {
					m_BoneData.ReadFromDisk(pFile);
				} else if(Header == LibraryAnimations) {
					m_Animations.ReadFromDisk(pFile);
				}
			}

			return glOK;
		}	else
			return glError;
	}

	uint ModelData::WriteToDisk()
	{
		String FilePath = m_ModelPath + m_Name;
		FilePath = FilePath + ".3dm";
		FILE* pFile = fopen(FilePath.Val(), "wb");

		LogMessage("");
		LogMessage("<B>Writing Model %s</b>", m_Name);
	
		if(!this->m_MeshData.WriteToDisk(LibraryMeshData, pFile)) {
			fclose(pFile);
			return false;
		}

		if(!this->m_SurfaceData.WriteToDisk(LibrarySurfaceData, pFile)) {
			fclose(pFile);
			return false;
		}

		if(!this->m_MaterialData.WriteToDisk(LibraryMaterialData, pFile)) {
			fclose(pFile);
			return false;
		}

		if(!this->m_BoneData.WriteToDisk(LibraryBoneData, pFile)) {
			fclose(pFile);
			return false;
		}
	
		if(!this->m_Animations.WriteToDisk(LibraryAnimations, pFile)) {
			fclose(pFile);
			return false;
		}
	
		fclose(pFile);

		return true;
	}
}