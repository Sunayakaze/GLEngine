
#include <iostream>

#include "GLModel.h"
#include "Logging.h"

//- Buffer operations ---------------------------------------------------------

namespace GLEngine
{
	template <class Attribs> const bufferid LoadAttribData(
		const AttribData<Attribs>&					Source)
	{
		bufferid Buffer = 0;
		LoadAtttribArrayBuffer(Buffer, Source.SizeInBytes(), Source.GetBufferPointer());

		return Buffer;
	}

	const bufferid LoadIndexData(
		const AttribData<GLFace16>&					Source)
	{
		bufferid Buffer = 0;
		LoadIndexArrayBuffer(Buffer, Source.SizeInBytes(), Source.GetBufferPointer());

		return Buffer;
	}

	//- GLBones -------------------------------------------------------------------

	void GLBones::LoadKeyFrames(const Library<Animations>& AnimLib)
	{
		const uint NrJoints = AnimLib.Size();

		if (NrJoints) {
			m_KeyFrames = new GLKeyFrames[NrJoints];
			for (uint j = 0; j < NrJoints; j++) {
				const Animations & Anims = AnimLib.GetItem(j);
				for (uint a = 0; a < Anims.GetNrKeyFrames(); a++) {
					m_KeyFrames[j].SetKeyFrameData(Anims);
				}	//	Animations a
			}	//	Joints j
		}
	}

	void GLBones::UpdateJointMatrices(const float& Time)
	{
		if (m_KeyFrames) {		//	If animated
			if (m_KeyFrames[0].GetNrKeyFrames()) {
				//	Put all the Jointmatrices from the keyframes into the bonestruct.
				for (uint i = 0; i < m_NrJoints; i++) {
					m_JointMatrix[i] = m_KeyFrames[i].GetJointMatrix(Time);
				}
			}
		}
	}

	void GLBones::IterateJointMatrices()
	{
		uint NrSkinningJoints = 0;

		if (m_JointMatrix)
		{
			//	Propagate through the hierarchy, multiply all jointmatrices
			//	with their parents and put the result in world.
			m_WorldMatrix[0] = m_JointMatrix[0];
			for (uint i = 1; i < m_NrJoints; i++)
				m_WorldMatrix[i] = m_JointMatrix[i] * m_WorldMatrix[m_ParentID[i] - 1];

			//	Multiply all resulting jointmatrices with their IBMatrices
			mat4* lpSkinningMatrix = (mat4*)m_SkinningMatrix;
			for (uint i = 1; i <= m_NrJoints; i++)
				if (m_Flags[i] && SkinningJoint)
					lpSkinningMatrix[NrSkinningJoints++] =
					m_InverseBindMatrix[i] * m_WorldMatrix[i];
		}

		m_NrSkinningJoints = NrSkinningJoints;
	}

	void GLBones::DumpBoneData()
	{
		LogMessage("<br><b>Dumping bonedata</b><br>");

		for (uint i = 0; i < m_NrJoints; i++) {

			const mat4* const ib = m_InverseBindMatrix;

			LogMessage("<b>ID</b>        : %u", m_ID[i]);
			LogMessage("<b>ParentID</b>  : %u", m_ParentID[i]);
			LogMessage("<b>IBM0</b>      : %4.4f, %4.4f, %4.4f, %4.4f", ib[i][0][0], ib[i][0][1], ib[i][0][2], ib[i][0][3]);
			LogMessage("<b>IBM1</b>      : %4.4f, %4.4f, %4.4f, %4.4f", ib[i][1][0], ib[i][1][1], ib[i][1][2], ib[i][1][3]);
			LogMessage("<b>IBM2</b>      : %4.4f, %4.4f, %4.4f, %4.4f", ib[i][2][0], ib[i][2][1], ib[i][2][2], ib[i][2][3]);
			LogMessage("<b>IBM3</b>      : %4.4f, %4.4f, %4.4f, %4.4f", ib[i][3][0], ib[i][3][1], ib[i][3][2], ib[i][3][3]);
			LogMessage("");
		}
	}

	//- GLPolygons ----------------------------------------------------------------

	GLPolygons::GLPolygons()
	{
		Ambient = 0;
		Diffuse = 0;
		Specular = 0;
		Bumpmap = 0;
		Screen = 0;
		Heightmap = 0;

		AmbientColor = color4(0.0f, 0.0f, 0.0f, 0.0f);
		DiffuseColor = color4(0.0f, 0.0f, 0.0f, 0.0f);
		SpecularColor = color4(0.0f, 0.0f, 0.0f, 0.0f);
		Shininess = 0;
		MaterialType = 0;

		NrFaces = 0;
		IndexBuffer = 0;
	}

	void GLPolygons::LoadMaterials(const GLMaterial& Material, const uint& SurfaceOffset)
	{
		Shininess = Material.getShininess();
		Ambient = Diffuse = Bumpmap = Specular = 0;

		AmbientColor = Material.getAmbient();
		DiffuseColor = Material.getDiffuse();
		SpecularColor = Material.getSpecular();

		if (Material.getMaterialType(HasAmbientMap)) {
			MaterialType |= HasAmbientMap;
			Ambient = SurfaceOffset + Material.getAmbientSurface(); 
		}

		if (Material.getMaterialType(HasDiffuseMap)) {
			MaterialType |= HasDiffuseMap;
			Diffuse = SurfaceOffset + Material.getDiffuseSurface();
		}

		if (Material.getMaterialType(HasSpecularMap)) {
			MaterialType |= HasSpecularMap;
			Specular = SurfaceOffset + Material.getSpecularSurface();
		}

		if (Material.getMaterialType(HasBumpMap)) {
			MaterialType |= HasBumpMap;
			Bumpmap = SurfaceOffset + Material.getBumpmapSurface();
		}
	}

	void GLPolygons::LoadIndices(const Polygons& Faces)
	{
		NrFaces = Faces.NrFaces();
		IndexBuffer = LoadIndexData(Faces.GetFaces());
	}

	void GLPolygons::SetIndexBuffer(const bufferid& BufferID, const count& NrFaces)
	{
		this->NrFaces = NrFaces;
		this->IndexBuffer = BufferID;
	}

	const bufferid& GLPolygons::getAmbient() const
	{
		return Ambient;
	}

	const bufferid& GLPolygons::getDiffuse() const
	{
		return Diffuse;
	}

	const bufferid& GLPolygons::getSpecular() const
	{
		return Specular;
	}

	const bufferid & GLPolygons::getBumpmap() const
	{
		return Bumpmap;
	}

	const bufferid & GLPolygons::getScreen() const
	{
		return Screen;
	}

	const bufferid & GLPolygons::getHeightmap() const
	{
		return Heightmap;
	}

	const color4& GLPolygons::getAmbientColor() const
	{
		return AmbientColor;
	}

	const color4& GLPolygons::getDiffuseColor() const
	{
		return DiffuseColor;
	}

	const color4& GLPolygons::getSpecularColor() const
	{
		return SpecularColor;
	}

	const float& GLPolygons::getShininess() const
	{
		return Shininess;
	}

	const flags& GLPolygons::getMaterialType() const
	{
		return MaterialType;
	}

	const size& GLPolygons::getNrFaces() const
	{
		return NrFaces;
	}

	const bufferid & GLPolygons::getIndexBuffer() const
	{
		return IndexBuffer;
	}

	void GLPolygons::Destroy()
	{
		MaterialType = 0;
		Shininess = 0.0f;
		NrFaces = 0;

		AmbientColor = color4(0.0f);
		DiffuseColor = color4(0.0f);
		SpecularColor = color4(0.0f);

		DeleteTexture(Diffuse);
		DeleteTexture(Ambient);
		DeleteTexture(Specular);
		DeleteTexture(Bumpmap);
		DeleteTexture(Screen);
		DeleteTexture(Heightmap);
		DeleteBuffer(IndexBuffer);
	}

	//- GLBuffers -------------------------------------------------------------------

	GLBuffers::GLBuffers()
	{
		Initialize();
	}

	GLBuffers::GLBuffers(const MeshData & Mesh)
	{
		Initialize();

		LoadMesh(Mesh);
	}

	void GLBuffers::LoadMesh(const MeshData& Mesh)
	{
		NrAttributes = Mesh.GetVertexArray().Size();

		VertexBuffer = LoadAttribData(Mesh.GetVertexArray());
		UVBuffer = LoadAttribData(Mesh.GetUVArray());
		NormalBuffer = LoadAttribData(Mesh.GetNormalArray());
		TangentBuffer = LoadAttribData(Mesh.GetTangentArray());
		BitangentBuffer = LoadAttribData(Mesh.GetBitangentArray());
		ColorBuffer = LoadAttribData(Mesh.GetColorBuffer());
		JointBuffer = LoadAttribData(Mesh.GetJointIndices());
		Weight1Buffer = LoadAttribData(Mesh.GetWeights1());
		Weight2Buffer = LoadAttribData(Mesh.GetWeights2());
		Weight3Buffer = LoadAttribData(Mesh.GetWeights3());
		Weight4Buffer = LoadAttribData(Mesh.GetWeights4());

		Polygons.Resize(Mesh.GetNrPolyLists());

		for (uint i = 0; i < Polygons.Size(); i++) {
			Polygons[i].LoadIndices(Mesh.GetPolygonList(i));
		}
	}

	void GLBuffers::LoadMaterial(
		const index&				PolygonListIndex,
		const GLMaterial&			Material,
		const uint&					SurfaceOffset)
	{
		Polygons[PolygonListIndex].LoadMaterials(Material, SurfaceOffset);
	}

	void GLBuffers::SetParentJoint(const mat4 * const lpParentJoint)
	{
		this->lpParentJoint = (mat4*)lpParentJoint;
	}

	void GLBuffers::SetBuffers(
		const count& NrAttribs,
		const count& NrFaces,
		const bufferid& Vertices,
		const bufferid& Normals,
		const bufferid& Tangents,
		const bufferid& Bitangents,
		const bufferid& UVCoords,
		const bufferid& Indices)
	{
		this->NrAttributes		= NrAttribs;
		this->VertexBuffer		= Vertices;
		this->NormalBuffer		= Normals;
		this->TangentBuffer		= Tangents;
		this->BitangentBuffer	= Bitangents;
		this->UVBuffer			= UVCoords;
		
		GLPolygons polys;
		polys.SetIndexBuffer(Indices, NrFaces);

		Polygons.Resize(1);
		Polygons[0] = polys;
	}

	void GLBuffers::Destroy()
	{
		NrAttributes = 0;

		DeleteBuffer(VertexBuffer);
		DeleteBuffer(NormalBuffer);
		DeleteBuffer(UVBuffer);
		DeleteBuffer(ColorBuffer);
		DeleteBuffer(TangentBuffer);
		DeleteBuffer(BitangentBuffer);
		DeleteBuffer(Weight1Buffer);
		DeleteBuffer(Weight2Buffer);
		DeleteBuffer(Weight3Buffer);
		DeleteBuffer(Weight4Buffer);
		DeleteBuffer(JointBuffer);

		for (uint p = 0; p < Polygons.Size(); p++) {
			Polygons[p].Destroy();
		}
	}

	void GLBuffers::Initialize()
	{
		NrAttributes = 0;
		VertexBuffer = 0;
		NormalBuffer = 0;
		UVBuffer = 0;
		ColorBuffer = 0;
		TangentBuffer = 0;
		BitangentBuffer = 0;
		JointBuffer = 0;
		Weight1Buffer = 0;
		Weight2Buffer = 0;
		Weight3Buffer = 0;
		Weight4Buffer = 0;
		lpParentJoint = nullptr;
	}

	const size& GLBuffers::getNrPolyLists() const
	{
		return Polygons.Size();
	}

	const bufferid& GLBuffers::getNrVertices() const
	{
		return NrAttributes;
	}

	const bufferid& GLBuffers::getVertexBuffer() const
	{
		return VertexBuffer;
	}

	const bufferid& GLBuffers::getNormalBuffer() const
	{
		return NormalBuffer;
	}

	const bufferid& GLBuffers::getUVBuffer() const
	{
		return UVBuffer;
	}

	const bufferid& GLBuffers::getColorBuffer() const
	{
		return ColorBuffer;
	}

	const bufferid& GLBuffers::getTangentBuffer() const
	{
		return TangentBuffer;
	}

	const bufferid& GLBuffers::getBitangentBuffer() const
	{
		return BitangentBuffer;
	}

	const bufferid& GLBuffers::getJointBuffer() const
	{
		return JointBuffer;
	}

	const bufferid& GLBuffers::getWeight1Buffer() const
	{
		return Weight1Buffer;
	}

	const bufferid& GLBuffers::getWeight2Buffer() const
	{
		return Weight2Buffer;
	}

	const bufferid& GLBuffers::getWeight3Buffer() const
	{
		return Weight3Buffer;
	}

	const bufferid& GLBuffers::getWeight4Buffer() const
	{
		return Weight4Buffer;
	}

	const mat4* GLBuffers::getParentJointPointer() const
	{
		return lpParentJoint;
	}

	const AttribData<GLPolygons>& GLBuffers::getPolygons() const
	{
		return Polygons;
	}

	//- GLKeyframes ---------------------------------------------------------------

	const mat4 GLKeyFrames::GetJointMatrix(
		const float&								Time) const
	{
		mat4 out;

		if (m_NrKeyFrames) {
			const float LastKeyFrameTime = m_Time[m_NrKeyFrames - 1];
			const float TimeDiff = fmod(Time, LastKeyFrameTime);

			//	find the correct keyframe
			uint keyframeindex = 0;
			while (!(TimeDiff < m_Time[keyframeindex]))
				keyframeindex++;

			const mat4 & jm0 = m_JointMatrix[keyframeindex];
			const mat4 & jm1 = m_JointMatrix[(keyframeindex + 1) % m_NrKeyFrames];

			uint lastkeyframe;
			if (keyframeindex == 0)
				lastkeyframe = m_NrKeyFrames - 1;
			else
				lastkeyframe = keyframeindex - 1;

			const float scalingfactor =
				(TimeDiff - m_Time[lastkeyframe]) /
				(m_Time[keyframeindex] - m_Time[lastkeyframe]);

			out = jm1 - jm0;
			out = jm0 + (scalingfactor * out);

		}
		else {
			out = mat4(1.0f);
		}

		return out;
	}

	//- GLModel -------------------------------------------------------------------

	//	The skinning calculation for each vertex v in a bind shape is
	//	for i to n
	//	     v += {[(v * BSM) * IBMi * JMi] * JW}
	//	
	//	• n: The number of joints that influence vertex v
	//	• BSM: Bind-shape matrix
	//	• IBMi: Inverse bind-pose matrix of joint i
	//	• JMi: Transformation matrix of joint i
	//	• JW: Weight of the influence of joint i on vertex v

	void GLModel::DrawSkeleton()
	{
		m_BoneStructure.IterateJointMatrices();

		const uint NrJoints = m_BoneStructure.GetNRJoints();

		vec4* Points = new vec4[NrJoints];

		const vec4 point = vec4(0.0f, 0.0f, 0.0f, 1.0f);

		for (uint i = 0; i < NrJoints; i++)
			Points[i] = point * m_BoneStructure.GetWorldMatrix(i + 1);

		glLineWidth(1.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);

		for (index i = 1; i < NrJoints; i++) {

			const uint& cid = i + 1;
			const uint& pid = m_BoneStructure.GetParentID(cid);
			const vec4& par = Points[cid - 1];
			const vec4& cur = Points[pid - 1];

			glVertex3fv((GLfloat*)&par);
			glVertex3fv((GLfloat*)&cur);
		}

		glEnd();

		if (Points) { delete[] Points; Points = NULL; }
	}

	void GLModel::UpdateJointMatrices(
		const float&								Time)
	{
		//	Send the Bonematrices
		m_BoneStructure.UpdateJointMatrices(Time);
		m_BoneStructure.IterateJointMatrices();
	}

	void GLModel::LoadMesh(
		const uint&									Index,
		const MeshData&								Mesh)
	{
		GLBuffers ModelBuffers = GLBuffers(Mesh);

		ModelBuffers.SetParentJoint(m_BoneStructure.GetParentJointPointer());

		SetModelBuffer(Index, ModelBuffers);
	}

	void GLModel::LoadMeshLib(
		const Library<MeshData>&					MeshLib)
	{
		m_nrMeshes = MeshLib.Size();
		m_ModelBuffers.Resize(m_nrMeshes);

		for (uint m = 0; m < m_nrMeshes; m++)
			LoadMesh(m, MeshLib.GetItem(m));
	}

	void GLModel::LoadMesh(
		const MeshData&								Mesh)
	{
		m_nrMeshes = 1;
		m_ModelBuffers.Resize(m_nrMeshes);

		LoadMesh(0, Mesh);
	}

	void GLModel::LoadSurfLib(
		uint&										SurfaceOffset,
		const Library<GLSurface>&					SurfLib)
	{
		const uint NrSurfaces = SurfLib.Size();

		//	Loading the first surface tells you how many there have been loaded already
		if (NrSurfaces)
			SurfaceOffset = ReadSurface(SurfLib[0]);

		//	Then load the rest
		if (NrSurfaces > 1)
			for (uint i = 1; i < NrSurfaces; i++)
				ReadSurface(SurfLib[i]);
	}

	void GLModel::LoadBoneLib(
		const Library<BoneData>&					BoneLib)
	{
		if (BoneLib.Size() != 0) {
			m_BoneStructure.Set(BoneLib);
			m_nrBones = BoneLib.Size();
			m_BoneStructure.IterateJointMatrices();
		}
	}

	void GLModel::LoadMatLib(
		const uint&									SurfaceOffset,
		const Library<MeshData>&					MeshLib,
		const Library<GLMaterial>&				MatLib)
	{
		const uint NrMaterials = MatLib.Size();
		const uint NrMeshes = MeshLib.Size();

		if (NrMaterials)
		{
			for (uint m = 0; m < NrMeshes; m++) {

				GLBuffers & modelbuffer = m_ModelBuffers.SetElem(m);

				const uint & NrPolyLists = modelbuffer.getNrPolyLists();
				for (uint j = 0; j < NrPolyLists; j++) {

					const uint& mat = MeshLib.GetItem(m).GetPolygonList(j).GetMaterial();

					modelbuffer.LoadMaterial(j, MatLib[mat], SurfaceOffset);

				}
			}
		}

		this->m_nrMaterials = NrMaterials;
	}

	void GLModel::LoadAnimLib(
		const Library<Animations>&					AnimLib)
	{
		m_BoneStructure.LoadKeyFrames(AnimLib);
	}

	void GLModel::Destroy()
	{
		for (uint m = 0; m < m_nrMeshes; m++) {
			m_ModelBuffers[m].Destroy();
		}

		m_BoneStructure.Destroy();
	}

	void GLModel::DumpModelInfo()
	{
		LogMessage("");
		LogMessage("<b>Skinning Matrices GLModel:</b>");
		DumpMatrices((float*)m_BoneStructure.GetSkinningMatrixPointer(0),
			m_BoneStructure.GetNrSkinningJoints());
	}
}
