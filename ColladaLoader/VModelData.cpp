
#include "VModelData.h"
#include "..\GLEngine\Logging.h"

//	Used with FindModelPath for maximum search paths downward
#define MaxPathDepth 4

namespace GLEngine
{

	template <class T>
	void FindUniqueItems(
		std::vector<T>&								outVector,
		const std::vector<T>&						inVector)
	{
		for(unsigned int in=0; in<inVector.size(); in++)
			FindUniqueItem(outVector, inVector[in]);
	}

	template <class T>
	void FindUniqueItem(
		std::vector<T>&								outVector,
		const T&									inVector)
	{
		if(outVector.size() == 0)
			outVector.push_back(inVector);
		else {
			for(unsigned int out = 0; out<outVector.size(); out++) {
	
				if(inVector == outVector[out])
					break;

				if(out+1==outVector.size()) {
					outVector.push_back(inVector);	//	Not found so unique
					break;
				}
			}
		}
	}

	//- Logging -------------------------------------------------------------------

	void DumpBoneHierarchy(
		const std::vector<BoneData>&				BoneLib)
	{
		LogMessage("");
		LogMessage("Dumping bone hierarchy:");

		for(uint i=0; i<BoneLib.size(); i++) {


			if(i==0) {
				LogMessage("<b>Bone: %i:</b> %s", i+1, BoneLib[0].Name.Val());
			} else {
				LogMessage("<b>Bone: %i:</b> %s, <b>Parent:</b> %s", i+1,
					BoneLib[i].Name.Val(),
					BoneLib[BoneLib[i].ParentID-1].Name.Val());
			}
		}
	}

	void DumpBoneLib(
		const std::vector<BoneData>&				BoneLib)
	{
		LogMessage("");
		LogMessage("Dumping bone Library:");

		for(uint i=0; i<BoneLib.size(); i++) {

			LogMessage("");
			LogMessage("<b>Bone:</b> %.2i:", i+1);
			LogMessage("-<B>Name:</b> %s", BoneLib[i].Name.Val());
			LogMessage("-<b>ID:</b> %.2i", BoneLib[i].ID);
			LogMessage("-<b>ParentID:</b> %.2i", BoneLib[i].ParentID);

			//	Jointmatrix
			LogMessage("-<b>JointMatrix:</b>");
			LogMessage("--<b>x1:</b>%.5f <b>y1:</b>%.5f <b>z1:</b>%.5f <b>w1:</b>%.5f",
				BoneLib[i].JointMatrix[0].x,
				BoneLib[i].JointMatrix[0].y,
				BoneLib[i].JointMatrix[0].z,
				BoneLib[i].JointMatrix[0].w);
			LogMessage("--<b>x2:</b>%.5f <b>y2:</b>%.5f <b>z2:</b>%.5f <b>w2:</b>%.5f",
				BoneLib[i].JointMatrix[1].x,
				BoneLib[i].JointMatrix[1].y,
				BoneLib[i].JointMatrix[1].z,
				BoneLib[i].JointMatrix[1].w);
			LogMessage("--<b>x3:</b>%.5f <b>y3:</b>%.5f <b>z3:</b>%.5f <b>w3:</b>%.5f",
				BoneLib[i].JointMatrix[2].x,
				BoneLib[i].JointMatrix[2].y,
				BoneLib[i].JointMatrix[2].z,
				BoneLib[i].JointMatrix[2].w);
			LogMessage("--<b>x4:</b>%.5f <b>y4:</b>%.5f <b>z4:</b>%.5f <b>w4:</b>%.5f",
				BoneLib[i].JointMatrix[3].x,
				BoneLib[i].JointMatrix[3].y,
				BoneLib[i].JointMatrix[3].z,
				BoneLib[i].JointMatrix[3].w);

			//	Inverse Bind Matrix
			LogMessage("-<b>Inverse Bind Matrix:</b>");
			LogMessage("--<b>x1:</b>%.5f <b>y1:</b>%.5f <b>z1:</b>%.5f <b>w1:</b>%.5f",
				BoneLib[i].InverseBindMatrix[0].x,
				BoneLib[i].InverseBindMatrix[0].y,
				BoneLib[i].InverseBindMatrix[0].z,
				BoneLib[i].InverseBindMatrix[0].w);
			LogMessage("--<b>x2:</b>%.5f <b>y2:</b>%.5f <b>z2:</b>%.5f <b>w2:</b>%.5f",
				BoneLib[i].InverseBindMatrix[1].x,
				BoneLib[i].InverseBindMatrix[1].y,
				BoneLib[i].InverseBindMatrix[1].z,
				BoneLib[i].InverseBindMatrix[1].w);
			LogMessage("--<b>x3:</b>%.5f <b>y3:</b>%.5f <b>z3:</b>%.5f <b>w3:</b>%.5f",
				BoneLib[i].InverseBindMatrix[2].x,
				BoneLib[i].InverseBindMatrix[2].y,
				BoneLib[i].InverseBindMatrix[2].z,
				BoneLib[i].InverseBindMatrix[2].w);
			LogMessage("--<b>x4:</b>%.5f <b>y4:</b>%.5f <b>z4:</b>%.5f <b>w4:</b>%.5f",
				BoneLib[i].InverseBindMatrix[3].x,
				BoneLib[i].InverseBindMatrix[3].y,
				BoneLib[i].InverseBindMatrix[3].z,
				BoneLib[i].InverseBindMatrix[3].w);
		}
	}

	//- File Handling -------------------------------------------------------------

	String FindDAEModelPath(
		const char*									ModelName)
	{
		String FileName, CurrentPath, ModelPath, Name;

		Name = String(ModelName);
		FileName = Name + ".dae";
		CurrentPath = "Models\\" + Name + "\\";
		for(uint i=0; i<MaxPathDepth; i++) {
			ModelPath = CurrentPath + FileName;
			if (FILE *file = fopen(ModelPath.Val(), "rb")) {
				fclose(file);
				return CurrentPath;
			} else
				CurrentPath = "..\\" + CurrentPath;
		}

		return "";
	}

	//- Helper functions ----------------------------------------------------------

	std::string DropTrailingTag(
		const std::string&							Name)
	{
		std::string ret = Name;

		int size=Name.size();
		do {
			ret.pop_back();
		}	while(ret.back() != '-');

		ret.pop_back();

		return ret;
	}

	std::string GetExtention(
		const std::string&							FileName)
	{
		std::string::size_type idx = FileName.rfind(".");
		std::string name = FileName;

		if(std::string::npos != idx)
			return name.substr(idx+1);
	
		return std::string();
	}

	std::string DropExtention(
		const std::string&							Name)
	{
		std::string ret = Name;
		std::string ext = GetExtention(Name);

		do {
			ret.pop_back();
		}	while(ret.back() != '.');

		ret.pop_back();

		return ret;
	}

	//	Single digit serialize only. Its only used for texture filenames thus far.
	std::string SerializeFileName(
		const std::string&							Name)
	{
		if(Name.size()) {
			std::string ret = Name;
			std::string ext = GetExtention(Name);

			ret = DropExtention(ret);
			char back = ret.back();
			ret.pop_back();
			ret.push_back(back+1);
			ret += ".";
			ret += ext;

			return ret;
		}

		return Name;
	}

	void AddUniqueSurface(
		std::vector<VSurfaceData>&					SurfaceLib,
		const char*									Name,
		const char*									Type)
	{
		uint NrSurfaces = SurfaceLib.size();
		
		if(strlen(Name) != 0) {

			if(NrSurfaces ==0) {
				SurfaceLib.push_back(VSurfaceData(Name, Type));
			} else
			{
				for(uint i=0; i<NrSurfaces; i++) {

					if(SurfaceLib[i].GetName().Equals(Name))
						break;
					if(i+1==NrSurfaces)
						SurfaceLib.push_back(VSurfaceData(Name, Type));
				}
			}
		}
	}

	//- Loading from Colladadoc ---------------------------------------------------

	void VPolygons::LoadVFaces(
		const Triangles&								pPolys)
	{
		const ulong NrFloats	= pPolys.GetSize();
		const uint NrFaces		= pPolys.count();
		const uint fpv			= pPolys.GetNrInputs();		//	Floats per vertex
		const uint fpf			= fpv * 3;					//	Floats per face

		//	Find semantics
		uint sV=0, sN=0, sT=0, sC=0;
		for(uint j=0; j<fpv; j++) {
			if(pPolys.input(j).semantic() == "VERTEX")		sV = j;
			if(pPolys.input(j).semantic() == "NORMAL")		sN = j;
			if(pPolys.input(j).semantic() == "TEXCOORD")	sT = j;
			if(pPolys.input(j).semantic() == "COLOR")		sC = j;
		}

		m_VFaces.Resize(NrFaces);

		uint iV=0, iF=0, iA=0;
		for(ulong k=0; k<NrFloats; k++) {

			iF = k / fpf; iV = k % fpv; iA = (k%fpf) / fpv;

			if(iV == sV)
				m_VFaces.SetElem(iF).Vertex[iA].Position = pPolys.p(k);		
			else if(iV == sN)
				m_VFaces.SetElem(iF).Vertex[iA].Normal = pPolys.p(k);
			else if(iV == sT)
				m_VFaces.SetElem(iF).Vertex[iA].Texcoord = pPolys.p(k);
		}
	}

	void Load3Float(
		AttribData<vec3>&							Vector,
		const Source&								Source)
	{
		const Array* pFloats			= &Source.array();
		const std::vector<Param>& Param	= Source.technique_common().accessor().param();

		const uint NrParams=3;
		uint iX=0, iY=0, iZ=0;
		for(uint iP=0; iP<NrParams; iP++) {
			if((Param[iP].name() == "X") || (Param[iP].name() == "R")) iX=iP;
			if((Param[iP].name() == "Y") || (Param[iP].name() == "G")) iY=iP;
			if((Param[iP].name() == "Z") || (Param[iP].name() == "B")) iZ=iP;
		}

		uint NrFloats = pFloats->count();
		uint NrAttribs = NrFloats / NrParams;

		Vector.Resize(NrAttribs);
		for(uint iF=0; iF<NrFloats; iF++) {
			uint iV = iF/NrParams; uint F = iF%NrParams;
		
			if(F == iX) Vector.SetElem(iV).x = pFloats->item(iF);
			if(F == iY) Vector.SetElem(iV).y = pFloats->item(iF);
			if(F == iZ) Vector.SetElem(iV).z = pFloats->item(iF);
		}
	}

	void Load2Float(
		AttribData<vec2>&							Vector,
		const Source&								Source)
	{
		const Array* pFloats			= &Source.array();
		const std::vector<Param>& Param	= Source.technique_common().accessor().param();

		const uint NrParams=2;
		uint iX=0, iY=0;
		for(uint iP=0; iP<NrParams; iP++) {
			if((Param[iP].name() == "S") || (Param[iP].name() == "U")) iX=iP;
			if((Param[iP].name() == "T") || (Param[iP].name() == "V")) iY=iP;
		}

		uint NrFloats = pFloats->count();
		uint NrAttribs = NrFloats / NrParams;

		Vector.Resize(NrAttribs);

		for(uint iF=0; iF<NrFloats; iF++) {
			uint iV = iF/NrParams; uint F = iF%NrParams;

			if(F == iX) Vector.SetElem(iV).s = pFloats->item(iF);
			if(F == iY) Vector.SetElem(iV).t = pFloats->item(iF);
		}
	}

	void SetBoneParent(
		std::vector<BoneData>&						BoneLib,
		const Node&									CurrentNode,
		const uint&									ParentID)
	{
		uint CurrID = 0;
		for(uint i=0; i<BoneLib.size(); i++) {
			if(BoneLib[i].Name.Equals(CurrentNode.name().c_str())) {
				CurrID = BoneLib[i].ID;
				break;
			}
		}

		//	Recursively set parent to all child bones.
		Node cNode;
		uint nrnodes = CurrentNode.NrChildren();
		if(CurrID) {
			BoneLib[CurrID-1].ParentID = ParentID;
			for(uint i=0; i<nrnodes; i++) {
				cNode = CurrentNode.subnode(i);
				SetBoneParent(BoneLib, cNode, CurrID);
			}
		}
	}

	//- Setup Indexed Mesh --------------------------------------------------------

	uint PutAttributeSet(
		std::vector<vec3>&							Vertices,
		std::vector<vec3>&							Normals,
		std::vector<vec2>&							Texcoords,
		const vec3&									Vertex,
		const vec3&									Normal,
		const vec2&									Coords)
	{
		uint i=Vertices.size();

		//	Start searching from the back
		for(uint t=i-1; i>0; i--, t=i-1) {
			if(Vertices[t] == Vertex)
				if(Normals[t] == Normal)
					if(Texcoords[t] == Coords)
						return t;
		};

		//	The set hasn't been found.
		Vertices.push_back(Vertex);
		Texcoords.push_back(Coords);
		Normals.push_back(Normal);

		return Vertices.size() - 1;
	}

	void VModelData::SetupIndexedSkinnedMesh()
	{
		if(!m_IsIndexed) {

			const uint NrMeshes = this->GetMeshData().Size();
	
			for(uint m=0; m<NrMeshes; m++) {

				const AttribData<vinfs> & ovinfs = GetVSkinData(m);

				if(ovinfs.Size() != 0) {
					const AttribData<vec3> & iVertexData =
						this->GetMeshData().GetItem(m).GetVertexArray();

					const AttribData<vec3> & oVertexData =
						this->GetVMeshData(m).GetVertexArray();
		
					//	The output vector
					std::vector<vinfs> vinfs(iVertexData.Size());

					const uint NrVertices = oVertexData.Size();
					const uint NrAttributes = iVertexData.Size();
					for(uint v=0; v<NrVertices; v++)
						for(uint i=0; i<NrAttributes; i++)
							if(oVertexData.GetElem(v) == iVertexData.GetElem(i))
								vinfs[i] = ovinfs.GetElem(v);

					this->SetVSkinData(m, &vinfs[0], vinfs.size());
				}

				ModelData::SetMaxWeights(this->GetMaxWeights());
			}
		}
	}

	void VModelData::ConvertSkinToShaderFormat()
	{
		for(uint m=0; m<GetMeshData().Size(); m++)
		{
			//	Sources
			const AttribData<vinfs>	& Source = this->GetVMeshData(m).GetVertexInfArray();

			const uint MaxWeights	= this->GetMaxWeights();
			const uint NrAttribs	= Source.Size();

			//	Target Containers.
			std::vector<uvec4>	Joints(NrAttribs);
			std::vector<vec4>	Weights1(NrAttribs);
			std::vector<vec4>	Weights2(NrAttribs);
			std::vector<vec4>	Weights3(NrAttribs);
			std::vector<vec4>	Weights4(NrAttribs);

			ubyte* lpJoints = NULL;
			for(uint i=0; i<NrAttribs; i++) {

				const vinfs& cVinfs = Source.GetElem(i);
				const uint size = cVinfs.Size();

				lpJoints = (ubyte*)(&Joints[i]);

				float* lpWeights1 = (float*)&Weights1[i];
				for(uint j=0; j<size; j++, lpJoints++) {
					const vinf& cVinf = cVinfs.GetElem(j);
					*lpJoints = cVinf.Joint-1;

					if(j<4) {
						lpWeights1[j] = cVinf.Weight;
					} else if(j<8) {
						float* lpWeights2 = (float*)&Weights2[i];
						lpWeights2[j-4] = cVinf.Weight;
					} else if(j<12) {
						float* lpWeights3 = (float*)&Weights3[i];
						lpWeights3[j-8] = cVinf.Weight;
					} else if(j<16) {
						float* lpWeights4 = (float*)&Weights4[i];
						lpWeights4[j-12] = cVinf.Weight;
					}				
				}	//	Attribs per vertex j
			}	//	Attribs i

			//	Place into meshdata
			if(Joints.size() != 0)	ModelData::SetJointIndices(m, &Joints[0], NrAttribs);
			if(Weights1.size() !=0)	ModelData::SetWeights1(m, &Weights1[0], NrAttribs);
			if(MaxWeights>4)		ModelData::SetWeights2(m, &Weights2[0], NrAttribs);
			if(MaxWeights>8)		ModelData::SetWeights3(m, &Weights3[0], NrAttribs);
			if(MaxWeights>12)		ModelData::SetWeights4(m, &Weights4[0], NrAttribs);

		}	// Meshes m	
	}

	void VModelData::SetupIndexedMesh()
	{
		if(!m_IsIndexed) {

			const uint NrMeshes = GetVMeshDataSize();

			std::vector<MeshData> LibMeshData(NrMeshes);

			for(uint m=0; m<NrMeshes; m++) {

				MeshData NewMesh;
		
				//	New vectors for Attributes; copy over later.
				std::vector<vec3> nVertices;
				std::vector<vec3> nNormals;
				std::vector<vec2> nTexcoords;
		
				//	Easy access references
				const AttribData<vec3> & Vertices	= GetVMeshData(m).GetVertexArray();
				const AttribData<vec3> & Normals	= GetVMeshData(m).GetNormalArray();
				const AttribData<vec2> & TexCoords	= GetVMeshData(m).GetUVArray();

				const uint NrPolyLists = GetVMeshData(m).GetNrVPolyLists();

				NewMesh.ResizePolyLists(NrPolyLists);

				for(uint md=0; md<NrPolyLists; md++)
				{
					printf("Indexing %s polygonlist %i",
						GetVMeshData(m).GetName(),
						md);

					//	Save reference to current mesh description (faces)
					const VPolygons & polylist = GetVMeshData(m).GetVPolygonList(md);
			
					//	Nr Triangles * 3
					const uint NrVertices = polylist.GetVFaces().Size() * 3;

					//	Progress bar
					const uint count10p = (NrVertices > 10) ? NrVertices / 10 : 1;

					//	Safe pointer to the original faces, cast to
					const VVertex* VVertices = (VVertex*)polylist.GetVFaces().GetBufferPointer();

					//	Reserve memory for GLFaces
					ushort* pIndexList = new ushort[NrVertices];
					memset(pIndexList, 0, NrVertices * sizeof(ushort));

					//	Go through the vertices and construct the new indexed mesh.
					for(uint iV=0; iV<NrVertices; iV++)
					{
						const VVertex & VVErtex	= VVertices[iV];
						const vec3 & Vertex		= Vertices.GetElem(VVErtex.Position);
						const vec3 & Normal		= Normals.GetElem(VVErtex.Normal);
						const vec2 & TexCoord	= TexCoords.GetElem(VVErtex.Texcoord);

						//	Add attribute to the indexed model and see if it exists already
						//	Let it handle the index itself
						pIndexList[iV] = PutAttributeSet(
							nVertices,	nNormals,	nTexcoords,
							Vertex,		Normal,		TexCoord);

						if(iV%count10p == count10p-1) printf(".");
					}

					NewMesh.SetName(GetVMeshData(m).GetName());
					NewMesh.SetPolygonList(md, (GLFace16*)pIndexList, polylist.GetVFaces().Size());
					NewMesh.SetMaterial(md, polylist.GetMaterial());
					NewMesh.SetParentJoint(GetVMeshData(m).GetParentJoint());

					//	Clear the buffer, not needed anymore
					this->SetVMeshData().SetItem(m).SetVPolygonList(md).SetVFaces().Clear();

					//	Delete the temp Indexlist
					delete [] pIndexList;

					printf("done\n");

				}	//	Faceslist per mesh

				//	Copy over the final atttibute data to their buffers
				if(nVertices.size() != 0)
				{
					NewMesh.LoadVertexArray(&nVertices[0], nVertices.size());
					NewMesh.LoadNormalArray(&nNormals[0], nNormals.size());
					NewMesh.LoadTexCoordsArray(&nTexcoords[0], nTexcoords.size());
				}

				LibMeshData[m] = NewMesh;
			}

			ModelData::SetMeshData(&LibMeshData[0], LibMeshData.size());
		}
	}

	//- Bind To Skeleton ----------------------------------------------------------

	//	The skinning calculation for each vertex v in a bind shape is
	//	for i to n
	//	     v += {[(v * BSM) * IBMi * JMi] * JW}
	//	 
	//	• n: The number of joints that influence vertex v
	//	• BSM: Bind-shape matrix
	//	• IBMi: Inverse bind-pose matrix of joint i
	//	• JMi: Transformation matrix of joint i
	//	• JW: Weight of the influence of joint i on vertex v

	//	Then inverse bind-pose matrix is: 
	//		boneInverseBindMatrix = 
	//			Inverse( bone-bind-pose-matrix * parent-bind-pose-matrix * parent's-parent-bind-pose-matrix * ... * root-bone-bind-pose-matrix);
	//
	//	The inverse bind-pose matrix can be calculated before any animation is done.
	//	The animation matrix is calculated each frame, normally a slerp of the bone's
	//	keyframe matrices for the current render time:
	//		boneAnimationMatrix = boneSlerpedMat * parent-slerpedMat * parent's-parent-slerpedMat * ... * root-SlerpedMat;
	//
	//	The matrix that you send to the shader each frame for each bone is:
	//		boneMatInShader = boneInverseBindMatrix * boneAnimationMatrix;

	//-----------------------------------------------------------------------------
	//- Store the vertices modified by BindShapeMatrix ----------------------------
	//- Do this as the last step. The original data is used for searching ---------
	//-----------------------------------------------------------------------------

	void VModelData::SetBindShape()
	{
		//	Reference the Bind Shape Matrix (default identity)
		mat4 bsm = this->GetBSM();

		if(bsm == mat4(0.0f)) bsm = mat4(1.0f);

		//	Set corrected Bind Shape Matrix
		bsm = bsm * m_UpAxis;

		for(uint m=0; m<this->GetVMeshDataSize(); m++) {

			uint NrAttributes = this->GetMeshData(m).GetVertexArray().Size();
			if(NrAttributes) {
				const AttribData<vec3> & VertexArray	= this->GetMeshData(m).GetVertexArray();
				const AttribData<vec3> & NormalArray	= this->GetMeshData(m).GetNormalArray();

				//	Setup temp storage for geometry
				std::vector<vec3> Vertices(NrAttributes);
				std::vector<vec3> Normals(NrAttributes);

				//	Go through all vertices
				for(uint a=0; a<NrAttributes; a++) {
					Vertices[a]	= vec3(vec4(VertexArray.GetElem(a), 1.0f) * bsm);
					Normals[a]	= glm::normalize(vec3(vec4(NormalArray.GetElem(a), 0.0f) * bsm));
				}

				ModelData::SetVertexArray(m, &Vertices[0], NrAttributes);
				ModelData::SetNormalArray(m, &Normals[0], NrAttributes);
			}

			if(this->GetMeshData(m).GetTangentArray().Size()) {
				std::vector<vec3> nTangents(NrAttributes), nBitangents(NrAttributes);
				const AttribData<vec3> & TangentArray = this->GetMeshData(m).GetTangentArray();
				const AttribData<vec3> & BitanArray	 = this->GetMeshData(m).GetBitangentArray();

				for(uint a=0; a<NrAttributes; a++) {
					nTangents[a] = glm::normalize(vec3(vec4(TangentArray.GetElem(a), 0.0f) * bsm));
					nBitangents[a] = glm::normalize(vec3(vec4(BitanArray.GetElem(a), 0.0f) * bsm));
				}

				ModelData::SetTangentArray(m, &nTangents[0], NrAttributes);
				ModelData::SetBitangentArray(m, &nBitangents[0], NrAttributes);
			}
		}	
	}

	//	Transforms all solid meshes to their respective locations to their parent nodes.
	void VModelData::SetMeshBindLocation()
	{

	}

	//- ColladaModelData main setup funtions --------------------------------------

	uint FindMaterial(
		const Library<VMaterialData>&				LibMaterials,
		const char*									MaterialName)
	{
		const uint NrMaterials = LibMaterials.Size();
		const String materialname = MaterialName;

		String NameToComp;
		for(uint i=0; i<NrMaterials; i++) {
			NameToComp = LibMaterials.GetItem(i).Name;
			if(materialname.Equals(NameToComp.Val()))
					return i;
		}

		return 0xFFFF;
	}

	void ResolveSurfaces(
		const std::vector<VSurfaceData>&			Surfaces,
		std::vector<VMaterialData>&					Materials)
	{
		//	Resolve surfaces
		for(uint m=0; m<Materials.size(); m++) {
			for(uint s=0; s<Surfaces.size(); s++) {
				if(Surfaces[s].GetName().Equals(Materials[m].AmbientName.Val()))
					Materials[m].setAmbient(s);
				if(Surfaces[s].GetName().Equals(Materials[m].DiffuseName.Val()))
					Materials[m].setDiffuse(s);
				if(Surfaces[s].GetName().Equals(Materials[m].BumpmapName.Val()))
					Materials[m].setBumpmap(s);
				if(Surfaces[s].GetName().Equals(Materials[m].SpecularName.Val()))
					Materials[m].setSpecular(s);
			}
		}
	}

	uint ReadSurfacesFromDisk(
		const char*									Path,
		std::vector<VSurfaceData>&					Surfaces)
	{
		const uint NrSurfaces = Surfaces.size();

		std::string FullPath, Name;
		for(uint i=0; i<NrSurfaces; i++)
			if(!Surfaces[i].Load(Path))
				return false;

		return true;
	}

	void VModelData::LoadUpAxis(
		const ColladaDoc&							Doc)
	{
		const std::string UpAxis = Doc.asset().UpAxis();

		if(UpAxis == "Z_UP") {

			m_UpAxis = mat4(1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							0.0f,-1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f);

		} else if(UpAxis == "Y_UP") {

			m_UpAxis = mat4(1.0f);

		}
	}

	void VModelData::LoadMaterials(
		const ColladaDoc&							Doc)
	{
		const ushort NrEffects		= Doc.GetNrEffects();
		const ushort NrMaterials	= Doc.GetNrMaterials();

		std::string ModelPath		= std::string(GetModelPath());
		std::vector<VSurfaceData>	Surfaces;
		std::vector<VMaterialData>	Materials;

		for(uint i=0; i<NrMaterials; i++)
		{
			const Material		& material	= Doc.material(i);
			const Effect		& eff		= Doc.effect(material.instance().url());
			const MaterialDesc	& desc		= eff.profile().technique().phong();

			VMaterialData md;
			md.Name		 = material.id().c_str();
			md.setColors(
				*(vec4*)& desc.ambient().color(),
				*(vec4*)& desc.diffuse().color(),
				*(vec4*)& desc.specular().color(),
				desc.shininess());

			String AmbientName	= desc.ambient().texture().texture().c_str();
			String DiffuseName	= desc.diffuse().texture().texture().c_str();
			String SpecularName	= desc.specular().texture().texture().c_str();
			String BumpmapName	= desc.bump().texture().c_str();

			//	resolve the textures and add the unique surfaces
			if(AmbientName.Size() > 1) {
				AmbientName = Doc.GetSurfaceFileName(AmbientName.Val()).c_str();
				if(CheckFileExists((ModelPath + AmbientName.Val()).c_str())) {
					AddUniqueSurface(Surfaces, AmbientName.Val(), "2D");
					md.AmbientName = AmbientName;
				}
			}

			if(DiffuseName.Size() > 1) {
				DiffuseName = Doc.GetSurfaceFileName(DiffuseName.Val()).c_str();
				if(CheckFileExists((ModelPath + DiffuseName.Val()).c_str())) {
					AddUniqueSurface(Surfaces, DiffuseName.Val(), "2D");
					md.DiffuseName = DiffuseName;
				}
			}

			//- Normalmap -------------------------------------------------------------------

			if(BumpmapName.Size() > 1) {
				BumpmapName = Doc.GetSurfaceFileName(BumpmapName.Val()).c_str();
				if(CheckFileExists((ModelPath + BumpmapName.Val()).c_str())) {
					AddUniqueSurface(Surfaces, BumpmapName.Val(), "2D");
					md.BumpmapName = BumpmapName;
				}
			} else if(md.DiffuseName.EndsWith("_0.dds") || md.DiffuseName.EndsWith("_0.bmp")) {
				BumpmapName = SerializeFileName(md.DiffuseName.Val()).c_str();
				if(CheckFileExists((ModelPath + BumpmapName.Val()).c_str())) {
					AddUniqueSurface(Surfaces, BumpmapName.Val(), "2D");
					md.BumpmapName = BumpmapName;
				}
			}

			//- Specularmap -----------------------------------------------------------------

			if(SpecularName.Size() > 1) {
				SpecularName = Doc.GetSurfaceFileName(SpecularName.Val()).c_str();
				if(CheckFileExists((ModelPath + SpecularName.Val()).c_str())) {
					AddUniqueSurface(Surfaces, SpecularName.Val(), "2D");
					md.SpecularName = SpecularName;
				}
			} else if(md.BumpmapName.EndsWith("_1.dds") || md.BumpmapName.EndsWith("_1.bmp")) {
				SpecularName = SerializeFileName(md.BumpmapName.Val()).c_str();
				if(CheckFileExists((ModelPath + SpecularName.Val()).c_str())) {
					AddUniqueSurface(Surfaces, SpecularName.Val(), "2D");
					md.SpecularName = SpecularName;
				}
			}

			//-------------------------------------------------------------------------------

			//	Setting materialtype
//			if(md.AmbientName.Size() > 1)	md.MaterialType	|= HasAmbientMap;
//			if(md.DiffuseName.Size() > 1)	md.MaterialType	|= HasDiffuseMap;
//			if(md.SpecularName.Size() > 1)	md.MaterialType |= HasSpecularMap;
//			if(md.BumpmapName.Size() > 1)	md.MaterialType	|= HasBumpMap;

			Materials.push_back(md);
		}

		ResolveSurfaces(Surfaces, Materials);

		std::vector<MaterialData> Mats(Materials.size());
		for(uint i=0; i<Materials.size(); i++)
			Mats[i] = (MaterialData)Materials[i];

		if(Materials.size()) {
			VModelData::AddMaterialData(&Materials[0], Materials.size());	//	Keep the V Data for reference
			ModelData::AddMaterialData(&Mats[0], Materials.size());
		}

		if(Surfaces.size())	{
			ModelData::ResizeSurfaceData(Surfaces.size());
			if(ReadSurfacesFromDisk(GetModelPath(), Surfaces))
				for(uint s=0; s<Surfaces.size(); s++)
					ModelData::SetSurfaceData(s, (SurfaceData)Surfaces[s]);
		}
	}

	const Source* GetSource(
		const Mesh&									Mesh,
		const std::string&							InputSemantic)
	{
		std::string SourceName;

		if (Mesh.GetNrPolyLists()) {
			SourceName = Mesh.polylist(0).GetSource(InputSemantic);
			if (InputSemantic == "VERTEX")
				SourceName = Mesh.vertices().GetSource("POSITION");
		}
		else if (Mesh.GetNrTriangleLists()) {

			SourceName = Mesh.trianglelist(0).GetSource("VERTEX");
			//	SourceName = Mesh.polylist(0).GetSource(InputSemantic);

			if (Mesh.vertices().id() == SourceName)
				if (InputSemantic == "VERTEX")
					SourceName = Mesh.vertices().GetSource("POSITION");
				else
					SourceName = Mesh.GetSource(InputSemantic);
		}
		else
			LogError("Source not found");

		return Mesh.ResolveSource(StripPrefix(SourceName, '#'));
	}

	void CorrectGLTexCoords(
		AttribData<vec2>&							TexCoords)
	{
		for(uint i=0; i<TexCoords.Size(); i++)
			TexCoords.SetElem(i).y = TexCoords.GetElem(i).y * -1.0f;
	}

	void ReadIndexedMeshData(
		Library<MeshData>&							Meshes,
		const Library<VMaterialData>&				VMaterials,
		const Geometry&								Geometry,
		const uint&									MeshIndex)
	{
		AttribData<vec3> Vertices, Normals, Colors;
		AttribData<vec2> TexCoords;

		const uint NrTriangleLists	=	Geometry.mesh().GetNrTriangleLists();
		const Source* sVertices		=	GetSource(Geometry.mesh(), "VERTEX");
		const Source* sNormals		=	GetSource(Geometry.mesh(), "NORMAL");
		const Source* sTexCoords	=	GetSource(Geometry.mesh(), "TEXCOORD");
		const Source* sColors		=	GetSource(Geometry.mesh(), "COLOR");

		if(sVertices)	Load3Float(Vertices,	*sVertices);
		if(sNormals)	Load3Float(Normals,		*sNormals);
		if(sTexCoords)	Load2Float(TexCoords,	*sTexCoords);
		if(sColors)		Load3Float(Colors,		*sColors);

	#ifndef VModelData_DisableInvertUVMaps
		CorrectGLTexCoords(TexCoords);
	#endif //	VModelData_DisableInvertUVMaps

		AttribData<Polygons> polygons;	polygons.Resize(NrTriangleLists);

		for(uint j=0; j<NrTriangleLists; j++) {

			std::string mat = Geometry.mesh().trianglelist(j).material();

			const Polylist & in_triangles = Geometry.mesh().polylist(j);

			Polygons triangles;
			triangles.SetMaterial(FindMaterial(VMaterials, mat.c_str()));
			triangles.SetFaces((GLFace16*)in_triangles.p(), in_triangles.GetSize()/3);

			polygons.Set(j, triangles);
		}

		MeshData NewMesh;
		NewMesh.SetName(Geometry.id().c_str());
		NewMesh.SetPolygonList(polygons);
		NewMesh.SetVertexArray(Vertices);
		NewMesh.SetNormalArray(Normals);
		NewMesh.SetUVArray(TexCoords);
		NewMesh.SetColorArray(Colors);
			
		Meshes.Set(MeshIndex, NewMesh);
	}

	void ReadNonIndexedMeshData(
		Library<VMeshData>&							VMeshes,
		const Library<VMaterialData>&				VMaterials,
		const Geometry&								Geometry,
		const uint&									MeshIndex)
	{
		AttribData<vec3> Vertices, Normals, Colors;
		AttribData<vec2> TexCoords;
	
		//	Reading the actual data
		const uint NrPolyLists		=	Geometry.mesh().GetNrPolyLists();
		const uint NrTriangleLists	=	Geometry.mesh().GetNrTriangleLists();
		const Source* sVertices		=	GetSource(Geometry.mesh(), "VERTEX");
		const Source* sNormals		=	GetSource(Geometry.mesh(), "NORMAL");
		const Source* sTexCoords	=	GetSource(Geometry.mesh(), "TEXCOORD");
	//	const Source* sColors		=	GetSource(Geometry.mesh(), "COLOR");

		if(sVertices)	Load3Float(Vertices, *sVertices);
		if(sNormals)	Load3Float(Normals,	*sNormals);
		if(sTexCoords)	Load2Float(TexCoords, *sTexCoords);
	//	if(sColors)		Load3Float(Colors, *sColors);

	//#ifndef VModelData_DisableInvertUVMaps
		CorrectGLTexCoords(TexCoords);
	//#endif //	VModelData_DisableInvertUVMaps

		AttribData<VPolygons> vpolygons;

		if (NrPolyLists) {
			vpolygons.Resize(NrPolyLists);

			for (uint j = 0; j<NrPolyLists; j++) {
				std::string mat = Geometry.mesh().polylist(j).material();

				VPolygons polys;
				polys.SetMaterial(FindMaterial(VMaterials, mat.c_str()));
				polys.LoadVFaces(Geometry.mesh().polylist(j));
				vpolygons.Set(j, polys);
			}
		}
		else if (NrTriangleLists) {
			vpolygons.Resize(NrTriangleLists);

			for (uint j = 0; j<NrTriangleLists; j++) {
				std::string mat = Geometry.mesh().trianglelist(j).material();

				VPolygons polys;
				polys.SetMaterial(FindMaterial(VMaterials, mat.c_str()));
				polys.LoadVFaces(Geometry.mesh().trianglelist(j));
				vpolygons.Set(j, polys);
			}
		}

		VMeshData NewVMesh;
		NewVMesh.SetName(Geometry.id().c_str());
		NewVMesh.SetVPolygonList(vpolygons);
		NewVMesh.SetVertexArray(Vertices);
		NewVMesh.SetColorArray(Colors);
		NewVMesh.SetNormalArray(Normals);
		NewVMesh.SetUVArray(TexCoords);

		VMeshes.Set(MeshIndex, NewVMesh);
	}

	void VModelData::LoadGeometries(
		const ColladaDoc&							Doc)
	{
		const uint NrMeshes	= Doc.GetNrGeometries();

	//	m_IsIndexed = Doc.geometry(0).mesh().GetNrPolyLists() ? 0 : 1;
		m_IsIndexed = false;

		//	Reserve memory
		if(!m_IsIndexed) {
			m_VMeshData.Resize(NrMeshes);
			for(uint mesh=0; mesh<NrMeshes; mesh++)
				ReadNonIndexedMeshData(m_VMeshData, m_VMaterialData, Doc.geometry(mesh), mesh);
		} else {
			Library<MeshData> Model;
			Model.Resize(NrMeshes);
			for(uint mesh=0; mesh<NrMeshes; mesh++)
				ReadIndexedMeshData(Model, m_VMaterialData, Doc.geometry(mesh), mesh);
			ModelData::SetMeshData(Model);

			m_VMeshData.Resize(NrMeshes);	//	Needs references in VMeshdata
			for(uint i=0; i<Model.Size(); i++)
				m_VMeshData.SetItem(i).SetName(Model.GetItem(i).GetName());
		}
	}

	void LoadFullBones(
		std::vector<BoneData>&						BoneLib,
		const Node&									Joint,
		const uint&									ParentID)
	{
		BoneData NewBone;

		NewBone.Name				= Joint.name().c_str();
		NewBone.ID					= BoneLib.size() + 1;
		NewBone.ParentID			= ParentID;
		NewBone.NrKeyFrames			= 0;

		if(Joint.type() == "JOINT")
			NewBone.JointMatrix = *(mat4*)&Joint.matrix();
		else if(Joint.type() == "NODE") {
			NewBone.JointMatrix = mat4(1.0f);
			NewBone.InstanceGeometryURL = Joint.instance_geometry().url().c_str();
		}

		BoneLib.push_back(NewBone);

		for(uint i=0; i<Joint.NrChildren(); i++)
			LoadFullBones(BoneLib, Joint.subnode(i), NewBone.ID);
	}

	void VModelData::LoadBones(
		const ColladaDoc&							Doc)
	{
		printf("Loading bones\n");

		std::vector<BoneData> BoneLib, FullBoneLib;

		if(Doc.GetNrControllers())	//	Not zero
		{
			//	Only read the first one.
			const Skin & skin = Doc.controller(0).skin();

			//	Set bindshapematrix
			SetBindShapeMatrix(*(mat4*)skin.bind_shape_matrix());

			for(uint i=0; i<skin.NrSources(); i++) {
				const Source & src = skin.source(i);
				const std::string & Name = 
					src.technique_common().accessor().param(0).name();

				if(Name == "JOINT") {

					const uint NrBones = skin.source(i).array().count();
					if(NrBones > BoneLib.size()) BoneLib.resize(NrBones);
					for(uint j=0; j<NrBones; j++) {
						BoneLib[j].Name = src.array().name(j).c_str();
						BoneLib[j].ID = j+1;
					}

				} else if(Name == "TRANSFORM") {

					const uint NrFloats = skin.source(i).array().count();
					const uint NrBones = NrFloats / 16;
					if(NrBones > BoneLib.size()) BoneLib.resize(NrBones);

					mat4* pMat = (mat4*)src.array().lpitem(0);
					for(uint j=0, k=0; j<NrBones;j++) {
						BoneLib[k++].InverseBindMatrix = pMat[j];
					}
				}
			}

			//	Find root bone
			const Node* pRoot = Doc.visual_scene(0).FindRootNode(BoneLib[0].Name.Val());

			//	Load all the nodes and joints from root.
			LoadFullBones(FullBoneLib, *pRoot, 0);

			//	Search for instanced geometry
			for(uint i=0; i<FullBoneLib.size(); i++)
				if(!FullBoneLib[i].InstanceGeometryURL.Equals(""))
					SetGeometryInstance(
						FullBoneLib[i].InstanceGeometryURL.Val(),
						FullBoneLib[i].ParentID);

			//	Identify skinning joints and set IBM
			for(uint i=0; i<BoneLib.size(); i++) {
				for(uint j=0; j<FullBoneLib.size(); j++) {
					if(BoneLib[i].Name.Equals(FullBoneLib[j].Name.Val())) {
						FullBoneLib[j].Flags = BoneLib[i].Flags | SkinningJoint;
						FullBoneLib[j].InverseBindMatrix = BoneLib[i].InverseBindMatrix;
					}
				}
			}
		}

	#ifdef DumpBoneLibEnabled
		DumpBoneLib(FullBoneLib);
	#endif

		//	Copy the BoneLib to the object
		if(FullBoneLib.size()) ModelData::SetBoneData(&FullBoneLib[0], FullBoneLib.size());
	}

	void VModelData::LoadSkin(
		const ColladaDoc&							Doc)
	{
		printf("Loading skin\n");

		const uint NrControllers = Doc.GetNrControllers();
	
		if(NrControllers == 0)
			this->SetMaxWeights(0);				//	No skin
		else {

			for(uint c=0; c<NrControllers; c++)	{

				//	Fetch skin info from controller
				const Skin					&	skin		 = Doc.controller(c).skin();
				const Vertex_weights		&	vw			 = skin.vertex_weights();
				const Source				&	src			 = skin.source("WEIGHT");
				const std::vector<float>	&	Weights		 = src.array().vec_floats();
				const std::vector<ushort>	&	Vcount		 = vw.vcount();
				const std::vector<ushort>	&	V			 = vw.v();
				const uint						NrInfluences = V.size() / 2;
				const uint						NrVertices	 = Vcount.size();

				//	Read influences
				AttribData<vinfs> SkinData;		SkinData.Resize(NrVertices);
				uint MaxWeights = this->GetMaxWeights();
				for(uint v=0, i=0; v<NrVertices; v++) {

					vinfs Vinfs; Vinfs.Resize(Vcount[v]);
					for(uint vc=0; vc<Vcount[v]; vc++)	{
						if(Vcount[v] > MaxWeights) MaxWeights = Vcount[v];
						const ubyte& joint	= (ubyte)V[i++];
						const float& weight	= Weights[V[i++]];
						Vinfs.Set(vc, vinf(joint, weight));
					}

					SkinData.Set(v, Vinfs);
				};

				//	Find source
				std::string source = skin.source(); source = source.erase(0, 1);
				for(uint m=0; m<m_VMeshData.Size(); m++) {
					if(std::string(m_VMeshData.GetItem(m).GetName()) == source) {
						this->SetVSkinData(m, SkinData);	break;
					}
				}

				this->SetMaxWeights(MaxWeights);
			}
		}
	}

	void VModelData::LoadAnimations(
		const ColladaDoc&							Doc)
	{
		const uint NrJoints = Doc.GetNrAnimations();

		if(NrJoints) {

			std::vector<Animations> Anims(NrJoints);

			//	Animation consists of a series of matrices per joint.
			for(uint a=0; a<NrJoints; a++) {

				//	Find semantic, pairs of three
				String sem_input, sem_output, sem_interpolation;
				for(uint i=0; i<3; i++) {
					const Input & input = Doc.animation(a).sampler().input(i);
					if(input.semantic() == "INPUT")
						sem_input = input.source().c_str();
					if(input.semantic() == "OUTPUT")
						sem_output = input.source().c_str();
					if(input.semantic() == "INTERPOLATION")
						sem_interpolation = input.source().c_str();
				}	//	End for sampler

				//	Find source, pairs of three
				for(uint i=0; i<3; i++) {

					const Source & src = Doc.animation(a).source(i);
					const std::vector<float> & tmp = src.array().vec_floats();

					//	Read Input
					if(sem_input.EndsWith(src.id().c_str())) {
						Anims[a].SetInput((float*)&tmp[0], tmp.size());
						Anims[a].SetNrKeyFrames(tmp.size());
					}

					//	Read Output
					if(sem_output.EndsWith(src.id().c_str()))
						Anims[a].SetOutput((float*)&tmp[0], tmp.size());

					//	Read Interpolation, not yet needed
					if(sem_interpolation.EndsWith(src.id().c_str())) { }

				}	//	End for source
			}

			ModelData::SetAnimations(&Anims[0], NrJoints);
		}
	}

	void VModelData::DumpVSkinData()
	{
		const AttribData<vec3> & Vertices = this->GetMeshData(0).GetVertexArray();
		const AttribData<vinfs> & VertInfs = this->GetVSkinData(0);

		const uint NrVertices = Vertices.Size();

		LogMessage("");
		LogMessage("Dumping %i vertices:", NrVertices);

		char* nString = new char[512];
		for(uint i=0; i<NrVertices; i++) {

			const vinfs & vi = VertInfs.GetElem(i);

			memset(nString, 0, 512);
			sprintf(nString, "<b>%.5i:</b>  <b>vx:</b>%4.4f  <b>vy:</b>%4.4f  <b>vz:</b>%4.4f", i, 
				Vertices.GetElem(i).x, Vertices.GetElem(i).y, Vertices.GetElem(i).z);

			for(uint j=0; j<vi.Size(); j++)
				sprintf(nString, "%s  <b>vw%i:</b>%4.4f  <b>vb%i:</b>%i", nString, 
					j, vi.GetElem(j).Weight,
					j, vi.GetElem(j).Joint);

			LogMessage(nString);
		}
	}

	//---------------------------------------------------------------------------------------
	//- Bind all vertices to the BindShape --------------------------------------------------
	//---------------------------------------------------------------------------------------

	void VModelData::BindToSkeleton()
	{
		//	Setup references
		const Library<BoneData> & Bones = this->GetBoneData();

		if(Bones.Size() > 0) {

			//	Iterate the matrices and setup what you would send to the vertex shader.
			std::vector<mat4> JointMatrices(Bones.Size());
			std::vector<mat4> SkinningMatrices(Bones.Size());
			JointMatrices[0] = Bones.GetItem((uint)0).JointMatrix;	// put the root matrix in [0]
			SkinningMatrices[0] = Bones.GetItem((uint)0).InverseBindMatrix * JointMatrices[0];
			for(uint jm=1, sm=1; jm<Bones.Size(); jm++) {

				const BoneData & cBone = Bones.GetItem(jm);

				const mat4 & cJoint = cBone.JointMatrix;
				const mat4 & cInvBM = cBone.InverseBindMatrix;
				const mat4 & pJoint = JointMatrices[cBone.ParentID-1];

				JointMatrices[jm] = cJoint * pJoint;

				if(cInvBM != mat4(0.0f)) SkinningMatrices[sm++] = cInvBM * JointMatrices[jm];
			}
	
			LogMessage("");
			LogMessage("<b>Skinning Matrices VModelData:</b>");
			DumpMatrices((float*)&SkinningMatrices[0], SkinningMatrices.size());

			//	This is what the vertexshader should do.
			const uint NrMeshes = this->GetMeshData().Size();
			for(uint m=0; m<NrMeshes; m++) {

				const AttribData<vec3>	& Verts	= ModelData::GetMeshData(m).GetVertexArray();
				const AttribData<vec3>	& Norms	= ModelData::GetMeshData(m).GetNormalArray();
				const AttribData<uvec4> & Jnts  = ModelData::GetMeshData(m).GetJointIndices();
				const AttribData<vec4>  & Wght1 = ModelData::GetMeshData(m).GetWeights1();
		
				//	Displace vertices and normals
				ubyte joint1, joint2, joint3, joint4;
				float weight1, weight2, weight3, weight4;
				std::vector<vec3> nVerts(Verts.Size());
				std::vector<vec3> nNorms(Norms.Size());
				for(uint v=0; v<Verts.Size(); v++) {

					const vec3  & oVertex = Verts.GetElem(v);
					const vec3  & oNormal = Norms.GetElem(v);
					const uvec4 & iIndex  = Jnts.GetElem(v);
					const vec4  & iWght1  = Wght1.GetElem(v);

					uint index = iIndex.x;
					joint1 = ((index & 0x000000FF) >> 0x00); weight1 = iWght1.x;
					joint2 = ((index & 0x0000FF00) >> 0x08); weight2 = iWght1.y;
					joint3 = ((index & 0x00FF0000) >> 0x10); weight3 = iWght1.z;
					joint4 = ((index & 0xFF000000) >> 0x18); weight4 = iWght1.w;

					if(weight1) {
						nVerts[v]  = vec3(vec4(oVertex, 1.0f) * SkinningMatrices[joint1] * weight1);
						nNorms[v]  = vec3(vec4(oNormal, 0.0f) * SkinningMatrices[joint1] * weight1);
						if(weight2) {
							nVerts[v] += vec3(vec4(oVertex, 1.0f) * SkinningMatrices[joint2] * weight2);
							nNorms[v] += vec3(vec4(oNormal,0.0f) * SkinningMatrices[joint2] * weight2);
							if(weight3) {
								nVerts[v] += vec3(vec4(oVertex, 1.0f) * SkinningMatrices[joint3] * weight3);
								nNorms[v] += vec3(vec4(oNormal, 0.0f) * SkinningMatrices[joint3] * weight3);
								if(weight4) {
									nVerts[v] += vec3(vec4(oVertex, 1.0f) * SkinningMatrices[joint4] * weight4);
									nNorms[v] += vec3(vec4(oNormal, 0.0f) * SkinningMatrices[joint4] * weight4);
								}	//	weight4
							}	//	weight3
						}	//	weight2
					}	//	weight1

				}	//	Vertices v

				//	Normalize normals
				for(uint n=0; n<nNorms.size(); n++)
					nNorms[n] = glm::normalize(nNorms[n]);

				ModelData::SetVertexArray(m, &nVerts[0], nVerts.size());
				ModelData::SetNormalArray(m, &nNorms[0], nNorms.size());

			}	//	Meshes m
		}	//	If (bones.size() > 0)
	}

	//---------------------------------------------------------------------------------------
	//- Main loading and setup function -----------------------------------------------------
	//---------------------------------------------------------------------------------------

	void VModelData::LoadColladaDoc(
		const ColladaDoc&							Doc)
	{
		LoadUpAxis(Doc);
		LoadMaterials(Doc);
		LoadGeometries(Doc);
		LoadBones(Doc);
		LoadSkin(Doc);
		LoadAnimations(Doc);

		SetupIndexedMesh();
		RecalculateNormals();
		SetupIndexedSkinnedMesh();
		ConvertSkinToShaderFormat();
		SetBindShape();
		GenerateTangents();
		SetBounds();

		//	SetMeshBindLocation();

	//	DumpIndexedAttribData();
		WriteToDisk();
	}
}
