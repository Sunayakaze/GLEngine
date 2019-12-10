
#include <stdio.h>

#include "ColladaDoc.h"

#pragma warning (disable : 4996)	//	Disable fopen warning.

//- Get info ------------------------------------------------------------------

const std::string ColladaDoc::GetSurfaceFileName(
	const std::string&				Name)					const
{
	std::string Search;
	//	Find the tag in sampler2d
	for(uint f=0; f<GetNrEffects(); f++) {
		const Effect & eff = effect(f);
		for(uint i=0; i<eff.profile().NrSamplers(); i++) {
			if(eff.profile().sampler2D(i).id() == Name) {
				Search = eff.profile().sampler2D(i).source();
				break;
			}
		}
	}

	//	Find the tag in Surfaces
	for(uint s=0; s<GetNrEffects(); s++) {
		const Effect & eff = effect(s);
		for(uint i=0; i<eff.profile().NrSurfaces(); i++) {
			if(eff.profile().surface(i).sid() == Search) {
				Search = eff.profile().surface(i).init_from();
				break;
			}
		}
	}

	for(uint i=0; i<GetNrImages(); i++) {
		std::string name = image(i).id();
		if(image(i).id() == Search) {
			Search =  image(i).init_from();
			break;
		}
	}

	return Search;
}

const std::string ColladaDoc::GetEffectFileName(
	const std::string&				Name)					const
{
	for(uint i=0; i<GetNrImages(); i++) {
		if(image(i).name() == Name)
			return image(i).init_from();
	}

	return std::string("");
}

//- Single Value Helper Function ----------------------------------------------

char* GetAttributeStringValue(
	char*							 AttrName,
	lpxmlnode						 pNode)
{
	if(pNode != NULL) {
		lpxmlattr pCurrAttr = pNode->first_attribute();

		do {
			if(!strcmp(pCurrAttr->name(), AttrName))
				return pCurrAttr->value();
			pCurrAttr = pCurrAttr->next_attribute();
		}	while(pCurrAttr != NULL);
	}

	return "";
}

uint GetAttributeIntegerValue(
	char*							 AttrName,
	lpxmlnode						 pNode)
{
	return atoi(GetAttributeStringValue(AttrName, pNode));
}

double GetAttributeDoubleValue(
	char*							 AttrName,
	lpxmlnode						 pNode)
{
	return atof(GetAttributeStringValue(AttrName, pNode));
}

lpxmlnode GetSubnode(
	char*							 Name,
	lpxmlnode						 pNode)
{
	if(pNode != NULL) {
		lpxmlnode pCurrNode = pNode;
		do	{
			if(!strcmp(pCurrNode->name(), Name))
				return pCurrNode;
			pCurrNode = pCurrNode->first_node();
		}	while (pCurrNode != NULL);
	}

	return NULL;
}

char* GetSubnodeStringValue(
	char*							 Name,
	lpxmlnode						 pNode)
{
	if(pNode != NULL) {
		lpxmlnode pCurrNode = pNode;;
		do	{
			if(!strcmp(pCurrNode->name(), Name))
				return pCurrNode->value();
			pCurrNode = pCurrNode->first_node();
		}	while (pCurrNode != NULL);
	}

	return NULL;
}

uint GetSubnodeIntegerValue(
	char*							 Name,
	lpxmlnode						 pNode)
{
	return atoi(GetSubnodeStringValue(Name, pNode));
}

double GetSubnodeDoubleValue(
	char*							 Name,
	lpxmlnode						 pNode)
{
	return atof(GetSubnodeStringValue(Name, pNode));
}

//- Array Value Helper Function -----------------------------------------------

//	Use this when the array you are about to read has a
//	fixed number of elements, like a Matrix4x4
//	where the storage has already been resized accordingly

void GetFixedFloatArray(
	float							*pFloats,
	char							*Data)
{
	char* tBuffer = strtok(Data, " ");

	pFloats[0] = (float)atof(tBuffer);

	uint j=1;
	tBuffer = strtok(NULL, " ");
	do {
		pFloats[j++] = (float)atof(tBuffer);
		tBuffer = strtok(NULL, " ");
	} while(tBuffer != NULL);
}

//-	GetNameArray --------------------------------------------------------------
//	Copies a space separated string to new mem, chops it up and saves the pointers

void GetNameArray(
	std::vector<std::string>		&Array,
	char							*Data)
{
	char* tBuffer = Data;
	Array.push_back(std::string(strtok(tBuffer, " ")));

	tBuffer = strtok(NULL, " ");
	while(tBuffer != NULL) {
		Array.push_back(std::string(tBuffer));
		tBuffer = strtok(NULL, " ");
	};
}

void GetFloatArray(
	std::vector<float>				&Array,
	char*							 Data)
{
	char* tBuffer = Data;

	tBuffer = strtok(Data, " ");

	Array.push_back((float)atof(tBuffer));

	tBuffer = strtok(NULL, " ");
	do {
		Array.push_back((float)atof(tBuffer));
		tBuffer = strtok(NULL, " ");
	} while(tBuffer != NULL);
}

void GetIndexArray(
	std::vector<ushort>				&Array,
	char*							 Data)
{
	char* tBuffer = Data;

	tBuffer = strtok(Data, " ");

	Array.push_back(atoi(tBuffer));

	tBuffer = strtok(NULL, " ");
	do {
		Array.push_back(atoi(tBuffer));
		tBuffer = strtok(NULL, " ");
	} while(tBuffer != NULL);
}

//- String operations ---------------------------------------------------------

std::string StripPrefix(std::string in, char prefix)
{
	if (in[0] == prefix)
		return in.erase(0, 1);

	return in;

}

//- Assets Loading ------------------------------------------------------------

void Asset::Load(lpxmlnode pNode)
{
	lpxmlnode pCurrNode = pNode;

	while(pCurrNode != NULL) {
		std::string Name = pCurrNode->name();

		if(Name == "up_axis")
			m_up_axis	= pCurrNode->value();

		pCurrNode = pCurrNode->next_sibling();
	}
}

//- Visual Scenes Loading -----------------------------------------------------

void Instance_Material::Load(lpxmlnode pNode)
{
	m_symbol	= GetAttributeStringValue("symbol", pNode);
	m_target	= GetAttributeStringValue("target", pNode);
}

void Bind_Material::Load(lpxmlnode pNode)
{
	m_instance_material.Load(pNode->first_node()->first_node());
}

Bind_Material::Bind_Material(lpxmlnode pNode)
{
	this->Load(pNode);
}

void Instance_Geometry::Load(lpxmlnode pNode)
{
	m_url = GetAttributeStringValue("url", pNode);

	//	m_bind_material.Load(pNode->first_node());
}

Instance_Geometry::Instance_Geometry(lpxmlnode pNode)
{
	this->Load(pNode->first_node());
}

Node::Node(lpxmlnode pNode)
{
	Node::Node();

	m_id		= GetAttributeStringValue("id", pNode);
	m_name		= GetAttributeStringValue("name", pNode);
	m_type		= GetAttributeStringValue("type", pNode);

	lpxmlnode pCurrNode = pNode->first_node();
	while(pCurrNode != NULL) {
		std::string Name = pCurrNode->name();

		if(Name == "node")
			m_nodes.push_back(Node(pCurrNode));
		else if(Name == "translate")
			GetFixedFloatArray((float*)&m_translate, pCurrNode->value());
		else if(Name == "scale")
			GetFixedFloatArray((float*)&m_scale, pCurrNode->value());
		else if(Name == "rotate") {
			std::string Sid	= GetAttributeStringValue("sid", pCurrNode);
			if(Sid == "rotationX")
				GetFixedFloatArray((float*)&m_rotateX,	pCurrNode->value());
			else if(Sid == "rotationY")
				GetFixedFloatArray((float*)&m_rotateY,	pCurrNode->value());
			else if(Sid == "rotationZ")
				GetFixedFloatArray((float*)&m_rotateZ,	pCurrNode->value());
		} else if(Name == "instance_geometry") {
			m_instance_geometry.Load(pCurrNode);
		} else if(Name == "matrix")
			GetFixedFloatArray((float*)&m_jointmatrix, pCurrNode->value());

		pCurrNode = pCurrNode->next_sibling();
	}
}

//- Materials loading ---------------------------------------------------------

void Instance_Effect::Load(
	lpxmlnode						 pNode)
{
	m_url		= GetAttributeStringValue("url", pNode);
}

void Material::Load(
	lpxmlnode						 pNode)
{
	m_id		= GetAttributeStringValue("id", pNode);
	m_name		= GetAttributeStringValue("name", pNode);

	m_instance_effect.Load(pNode->first_node());
}

//- Effects loading -----------------------------------------------------------

void Texture::Load(
	lpxmlnode						 pNode)
{
	m_texture	= GetAttributeStringValue("texture", pNode);
	m_texcoord	= GetAttributeStringValue("texcoord", pNode);
}

void CColor4::Load(
	lpxmlnode						pNode)
{
	std::string Buffer = pNode->value();
	sscanf(&Buffer[0], "%f %f %f %f", &m_r, &m_g, &m_b, &m_a);
}

void EffectData::Load(
	lpxmlnode						 pNode)
{
	uint index=0;
	memset(&m_color, 0, sizeof(m_color));

	std::string Name = pNode->name();
	if(Name == "color")		m_color.Load(pNode);
	if(Name == "texture")	m_texture.Load(pNode);
}

void MaterialDesc::Load(
	lpxmlnode						 pNode)
{
	lpxmlnode pCurrNode = pNode->first_node();
	lpxmlnode pFirstNode = NULL;

	do {
		std::string Name = pCurrNode->name();
		pFirstNode = pCurrNode->first_node();
		if(Name == "emission")	m_emission.Load(pFirstNode);
		if(Name == "ambient")	m_ambient.Load(pFirstNode);
		if(Name == "diffuse")	m_diffuse.Load(pFirstNode);
		if(Name == "specular")	m_specular.Load(pFirstNode);
		if(Name == "shininess")	m_shininess = 
			(float)GetSubnodeDoubleValue("float", pFirstNode);
		pCurrNode = pCurrNode->next_sibling();
	}	while(pCurrNode != NULL);

	//	Get bump surface
	pCurrNode = pNode->next_sibling();
	if(pCurrNode)
		m_bump.Load(GetSubnode("texture", pCurrNode));
}

void Technique::Load(
	lpxmlnode						 pNode)
{
	m_phong.Load(pNode);
}

void Surface::Load(
	lpxmlnode						 pNode)
{
	m_sid		= GetAttributeStringValue("sid", pNode);
	m_type		= GetAttributeStringValue("type", pNode->first_node());
	m_init_from	= GetSubnodeStringValue("init_from", pNode->first_node());
}

void profile_COMMON::Load(
	lpxmlnode						 pNode)
{
	lpxmlnode pCurrNode = pNode->first_node();
	lpxmlnode pFirstNode;

	uint iparam=0;
	std::string Name;
	do {
		pFirstNode = pCurrNode->first_node();
		Name = pFirstNode->name();
		if(Name == "surface") {
			Surface surf;
			surf.Load(pCurrNode);
			m_surface.push_back(surf);
		} else if(Name == "sampler2D") {
			Sampler2D sampler;
			sampler.Load(pFirstNode);
			m_sampler2d.push_back(sampler);
		} else if(Name == "phong") {
			m_technique.Load(pFirstNode);
		}

		pCurrNode = pCurrNode->next_sibling();
	}	while(pCurrNode != NULL);
}

void Effect::Load(
	lpxmlnode						 pNode)
{
	m_id		= GetAttributeStringValue("id",		pNode);
	m_profile.Load(pNode->first_node());
}

void Image::Load(
	lpxmlnode						 pNode)
{
	m_id		= GetAttributeStringValue("id",		pNode);
	m_name		= GetAttributeStringValue("name",	pNode);
	m_init_from	= GetSubnodeStringValue("init_from", pNode);
}

//- Geometry loading ----------------------------------------------------------

void Param::Load(
	lpxmlnode						 pNode)
{
	m_name		= GetAttributeStringValue("name", pNode);
	m_type		= GetAttributeStringValue("type", pNode);
}

Accessor::Accessor()
{
	this->m_source = "";
	this->m_count = 0;
	this->m_stride = 0;
	this->m_param.clear();
}

void Accessor::Load(
	lpxmlnode						pNode)
{
	m_source	= GetAttributeStringValue("source", pNode);
	m_count		= GetAttributeIntegerValue("count", pNode);
	m_stride	= GetAttributeIntegerValue("stride", pNode);

	uint i=0;
	lpxmlnode pCurrNode = pNode->first_node();
	do {
		std::string Name = pCurrNode->name();
		if(Name == "param") {
			Param prm;
			prm.Load(pCurrNode);
			m_param.push_back(prm);
		} 
		pCurrNode = pCurrNode->next_sibling();
	}	while(pCurrNode != NULL);
}

Technique_Common::Technique_Common()
{
	this->m_accessor = Accessor();
}

void Technique_Common::Load(
	lpxmlnode						 pNode)
{
	m_accessor.Load(pNode->first_node());
}

void Array::Load(
	lpxmlnode						 pNode)
{
	m_id		= GetAttributeStringValue("id", pNode);
	m_count		= GetAttributeIntegerValue("count", pNode);

	printf("Loading %s\n", m_id.c_str());

	std::string type = pNode->name();
	if(type == "float_array") {
		GetFloatArray(m_floats,	GetSubnodeStringValue("float_array", pNode));
	} else if(type == "Name_array") {
		GetNameArray(m_names, GetSubnodeStringValue("Name_array", pNode));
	}
}

void Source::Load(
	lpxmlnode						 pNode)
{
	lpxmlnode pCurrNode = pNode->first_node();
	
	m_id		= GetAttributeStringValue("id", pNode);

	while(pCurrNode != NULL) {
		std::string Name = pCurrNode->name();
		if(Name == "technique_common")
			m_technique_common.Load(pCurrNode);
		else
			m_array.Load(pCurrNode);
		pCurrNode = pCurrNode->next_sibling();
	};
}

void Input::Load(
	lpxmlnode						 pNode)
{
	m_semantic	= GetAttributeStringValue("semantic",	pNode);
	m_source	= GetAttributeStringValue("source",		pNode);
	m_offset	= GetAttributeIntegerValue("offset",	pNode);
}

//	void Polylist::Load(
//		lpxmlnode						 pNode)
//	{
//		lpxmlnode pCurrNode = pNode->first_node();
//	
//		m_material	= GetAttributeStringValue("material",	pNode);
//		m_count		= GetAttributeIntegerValue("count",		pNode);
//	
//		while(pCurrNode != NULL) {
//			std::string Name = pCurrNode->name();
//			if(Name == "input") {
//				Input inp;
//				inp.Load(pCurrNode);
//				m_input.push_back(inp);
//			} else if(Name == "p")
//				GetIndexArray(m_p, pCurrNode->value());
//			pCurrNode = pCurrNode->next_sibling();
//		};
//	}

void Triangles::Load(
	lpxmlnode						pNode)
{
	lpxmlnode pCurrNode = pNode->first_node();

	m_material	= GetAttributeStringValue("material",	pNode);
	m_count		= GetAttributeIntegerValue("count",		pNode);

	while(pCurrNode != NULL) {
		std::string Name = pCurrNode->name();
		if(Name == "input") {
			Input inp;
			inp.Load(pCurrNode);
			m_input.push_back(inp);
		} else if(Name == "p")
			GetIndexArray(m_p, pCurrNode->value());
		pCurrNode = pCurrNode->next_sibling();
	};
}

void Vertices::Load(
	lpxmlnode						pNode)
{
	lpxmlnode pCurrNode = pNode->first_node();

	m_id		= GetAttributeStringValue("id", pNode);
	m_semantic	= GetAttributeStringValue("semantic", pCurrNode);

	while(pCurrNode != NULL) {
		std::string Name = pCurrNode->name();
		if(Name == "input") {
			Input inp;
			inp.Load(pCurrNode);
			m_input.push_back(inp);
		}
		pCurrNode = pCurrNode->next_sibling();
	};
}

void Mesh::Load(
	lpxmlnode						pNode)
{
	lpxmlnode pCurrNode = pNode->first_node();

	while(pCurrNode != NULL) {
		std::string Name = pCurrNode->name();
		if(Name == "source") {
			Source src;
			src.Load(pCurrNode);
			m_source.push_back(src);
		} else if(Name == "polylist") {
			Polylist pl;
			pl.Load(pCurrNode);
			m_polylist.push_back(pl);
		} else if(Name == "triangles") {
			Triangles triangles;
			triangles.Load(pCurrNode);
			m_triangles.push_back(triangles);
		} else if(Name == "vertices") {
			Vertices vertices;
			m_vertices.Load(pCurrNode);
		}
		pCurrNode = pCurrNode->next_sibling();
	};
}

void Geometry::Load(
	lpxmlnode						 pNode)
{
	lpxmlnode pCurrNode = pNode;

	m_id		= GetAttributeStringValue("id", pCurrNode);
	m_name		= GetAttributeStringValue("name", pCurrNode);
	m_mesh.Load(pCurrNode->first_node());
}

//- Collada Document ----------------------------------------------------------

ColladaDoc::ColladaDoc()
{

}

ColladaDoc::ColladaDoc(lpxmlnode	 pNode)
{
	this->Load(pNode);
}

void ColladaDoc::LoadAssets(lpxmlnode	pNode)
{
	this->assets.Load(pNode);
}

void ColladaDoc::LoadImages(
	lpxmlnode						 pNode)
{
	lpxmlnode pCurrNode = pNode;
	
	while(pCurrNode != NULL) {
		Image new_image;
		new_image.Load(pCurrNode);
		library_images.push_back(new_image);
		pCurrNode = pCurrNode->next_sibling();
	};
}

void ColladaDoc::LoadMaterials(
	lpxmlnode						pNode)
{
	lpxmlnode pCurrNode = pNode;

	while(pCurrNode != NULL) {
		Material new_material;
		new_material.Load(pCurrNode);
		library_materials.push_back(new_material);
		pCurrNode = pCurrNode->next_sibling();
	}
}

void ColladaDoc::LoadEffects(
	lpxmlnode						pNode)
{
	lpxmlnode pCurrNode = pNode;
	
	while(pCurrNode != NULL) {
		Effect new_effect;
		new_effect.Load(pCurrNode);
		library_effects.push_back(new_effect);
		pCurrNode = pCurrNode->next_sibling();
	};
}

void ColladaDoc::LoadGeometries(
	lpxmlnode						 pNode)
{
	lpxmlnode pCurrNode = pNode;
	
	while(pCurrNode != NULL) {
		Geometry new_geo;
		new_geo.Load(pCurrNode);
		library_geometries.push_back(new_geo);
		pCurrNode = pCurrNode->next_sibling();
	};
}

void ColladaDoc::Read(
	const char*						 FilePath)
{
	std::string filepath = FilePath;

	rapidxml::file<> xmlFile(filepath.c_str());
	rapidxml::xml_document<> ColladaFile;
	ColladaFile.parse<0>(xmlFile.data());

	// Strip filename
	char last = filepath.back();
	while(last != '\\') {
		filepath.pop_back();
		last = filepath.back();
	}

	m_filepath = FilePath;
	this->Load(ColladaFile.first_node());
}

void JointDescription::Load(
	lpxmlnode						 pNode)
{
	lpxmlnode pCurrNode = pNode->first_node();

	while(pCurrNode != NULL) {

		if(std::string(pCurrNode->name()) == "input") {
			Input inp;
			inp.Load(pCurrNode);
			m_input.push_back(inp);
		}

		pCurrNode = pCurrNode->next_sibling();
	}
}

void Vertex_weights::Load(
	lpxmlnode						 pNode)
{
	m_count = GetAttributeIntegerValue("count", pNode);

	lpxmlnode pCurrNode = pNode->first_node();	
	while(pCurrNode != NULL) {

		if(std::string(pCurrNode->name()) == "input") {

			Input inp;
			inp.Load(pCurrNode);
			m_input.push_back(inp);

		} else if(std::string(pCurrNode->name()) == "vcount") {

			//	char* pData = GetSubnodeStringValue("vcount", pNode);
			GetIndexArray(m_vcount, pCurrNode->value());

		} else if(std::string(pCurrNode->name()) == "v") {

			//char* pData = GetSubnodeStringValue("v", pNode);
			GetIndexArray(m_v, pCurrNode->value());
		}

		pCurrNode = pCurrNode->next_sibling();
	}
}

const Source Skin::source(std::string ParamName)	const
{
	for(uint i=0; i< this->NrSources(); i++) {
		const Source& src = this->source(i);
		std::string Name = 
			src.technique_common().accessor().param(0).name();
		if(Name == ParamName) return src;
	}

	return Source();
}

void Skin::Load(
	lpxmlnode						 pNode)
{
	m_source = GetAttributeStringValue("source", pNode);

	char* Data = GetSubnodeStringValue("bind_shape_matrix", pNode);

	lpxmlnode pCurrNode = pNode->first_node();
	while(pCurrNode != NULL) {
		std::string Name = pCurrNode->name();

		if(Name == "bind_shape_matrix") {

			GetFixedFloatArray(m_bind_shape_matrix,
				GetSubnodeStringValue(
					"bind_shape_matrix",
					pNode));

		} else if(Name == "source") {

			Source src;
			src.Load(pCurrNode);
			m_sources.push_back(src);

		} else if(Name == "joints") {

			m_joint.Load(pCurrNode);

		} else if(Name == "vertex_weights") {

			m_vertex_weights.Load(pCurrNode);
		}

		pCurrNode = pCurrNode->next_sibling();
	}
}

void Sampler::Load(
	lpxmlnode						 pNode)
{
	lpxmlnode pCurrNode = pNode;

	m_id = GetAttributeStringValue("id", pNode);
	
	pCurrNode = pCurrNode->first_node();
	while(pCurrNode != NULL) {
		std::string Name = pCurrNode->name();
		if(Name == "input") {
			Input inp;
			inp.Load(pCurrNode);
			m_input.push_back(inp);
		}
		pCurrNode = pCurrNode->next_sibling();
	};
}

void Channel::Load(
	lpxmlnode						 pNode)
{
	m_source	= GetAttributeStringValue("source", pNode);
	m_target	= GetAttributeStringValue("target", pNode);
}

void Controller::Load(
	lpxmlnode						 pNode)
{
	m_id		= GetAttributeStringValue("id",		pNode);
	m_name		= GetAttributeStringValue("name",	pNode);
	
	lpxmlnode pCurrNode = pNode->first_node();
	while(pCurrNode != NULL) {
		std::string Name = pCurrNode->name();
		if(Name == "skin")
			m_skin.Load(pCurrNode);
		pCurrNode = pCurrNode->next_sibling();
	};
}

void Animation::Load(
	lpxmlnode						 pNode)
{
	lpxmlnode pCurrNode = pNode;

	m_id = GetAttributeStringValue("id", pNode);

	while(pCurrNode != NULL) {

		std::string Name = pCurrNode->name();
		if(Name == "source") {
			Source src;
			src.Load(pCurrNode);
			m_source.push_back(src);
		} else if(Name == "sampler") {
			m_sampler.Load(pCurrNode);
		} else if (Name == "channel") {
			m_channel.Load(pCurrNode);
		}

		pCurrNode = pCurrNode->next_sibling();
	};
}

void ColladaDoc::LoadAnimations(
	lpxmlnode						 pNode)
{
	lpxmlnode pCurrNode = pNode;

	while(pCurrNode != NULL) {
		Animation anim;
		anim.Load(pCurrNode->first_node());
		library_animations.push_back(anim);
		pCurrNode = pCurrNode->next_sibling();
	};
}

void ColladaDoc::LoadControllers(
	lpxmlnode						 pNode)
{
	lpxmlnode pCurrNode = pNode;

	while(pCurrNode != NULL) {

		std::string Name = pCurrNode->name();
		if(Name == "controller") {
			Controller ctrl;
			ctrl.Load(pCurrNode);
			library_controller.push_back(ctrl);
		}

		pCurrNode = pCurrNode->next_sibling();
	};
}

Visual_Scene::Visual_Scene(
	lpxmlnode						 pNode)
{
	m_id	= GetAttributeStringValue("id", pNode);
	m_name	= GetAttributeStringValue("name", pNode);

	lpxmlnode pCurrNode = pNode->first_node();
	while(pCurrNode != NULL) {
		std::string Name = pCurrNode->name();
		if(Name == "node") m_nodes.push_back(Node(pCurrNode));
		pCurrNode = pCurrNode->next_sibling();
	}
}

void ColladaDoc::LoadVisualScenes(
	lpxmlnode						 pNode)
{
	library_visual_scenes.push_back(Visual_Scene(pNode));
}

void ColladaDoc::Load(
	lpxmlnode						 pNode)
{
	lpxmlnode pCurrNode = pNode;
	std::string Name = pCurrNode->name();	
	if(Name == "COLLADA") {

		//	COLLADA header found
		pCurrNode = pCurrNode->first_node();
		while(pCurrNode != NULL) {

			Name = pCurrNode->name();

			if(Name == "library_images")
				LoadImages(pCurrNode->first_node());

			else if(Name ==  "library_effects")
				LoadEffects(pCurrNode->first_node());

			else if(Name == "library_materials")
				LoadMaterials(pCurrNode->first_node());

			else if(Name == "library_geometries")
				LoadGeometries(pCurrNode->first_node());

			else if(Name == "library_animations")
				LoadAnimations(pCurrNode->first_node());

			else if(Name =="library_controllers")
				LoadControllers(pCurrNode->first_node());

			else if(Name == "library_visual_scenes")
				LoadVisualScenes(pCurrNode->first_node());

			else if(Name == "asset")
				LoadAssets(pCurrNode->first_node());

			pCurrNode = pCurrNode->next_sibling();
		};
	}
}
