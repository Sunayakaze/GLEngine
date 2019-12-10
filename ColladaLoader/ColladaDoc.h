
#ifndef _COLLADADOC_H_
#define _COLLADADOC_H_

#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

typedef unsigned char				 ubyte;
typedef unsigned short				 ushort;
typedef unsigned int				 uint;
typedef unsigned long				 ulong;

typedef unsigned int				 t_type;

//- My Typedefs ---------------------------------------------------------------

typedef rapidxml::xml_node<>		*lpxmlnode;
typedef rapidxml::xml_attribute<>	*lpxmlattr;

//- Tool functions ------------------------------------------------------------

std::string StripPrefix(std::string, char);

//- Vectors and Matrices ------------------------------------------------------

struct Vector3						 
{
	float							 x;
	float							 y;
	float							 z;

	void							 Load(
		lpxmlnode					 	pNode);
};

struct Vector4						 
{
	float							 a;
	float							 b;
	float							 c;
	float							 d;

	void							 Load(
		lpxmlnode					 	pNode);
};

struct Matrix3x3
{
	Vector3							 v[3];

	void							 Load(
		lpxmlnode					 	pNode);
};

struct Matrix4x4
{
	Vector4							 v[4];

	void							 Load(
		lpxmlnode					 	pNode);
};

//- Generic Dataholders --------------------------------------------------------

class Asset
{

private:

	std::string						m_up_axis;

public:

	void							Load(
		lpxmlnode						pNode);

	const std::string&				UpAxis()					const
	{
		return m_up_axis;
	}
};

class Input
{
private:

	std::string						m_semantic;
	std::string						m_source;
	uint							m_offset;

public:

	void							Load(
		lpxmlnode						pNode);

	const std::string&				semantic()				const
	{
		return m_semantic;
	}

	const std::string&				source()				const
	{
		return m_source;
	}

	const uint&						offset()				const
	{
		return m_offset;
	}
};

class Param
{
private:

	std::string						m_name;
	std::string						m_type;

public:

	void							Load(
		lpxmlnode						pNode);

	const std::string&				name()					const
	{
		return m_name;
	}

	const std::string&				type()					const
	{
		return m_type;
	}
};

class Accessor
{
private:

	std::string						m_source;
	uint							m_count;
	uint							m_stride;
	std::vector<Param>				m_param;

public:

	Accessor();

	void							Load(
		lpxmlnode						pNode);

	const std::string&				source()				const
	{
		return m_source;
	}

	const uint						count()					const
	{
		return m_count;
	}

	const uint						stride()				const
	{
		return m_stride;
	}

	const Param&					param(
		const uint&						i)					const
	{
		return m_param[i];
	}

	const std::vector<Param>&		param()					const
	{
		return m_param;
	}

};

class Technique_Common
{
private:

	Accessor						m_accessor;

public:

	Technique_Common();

	void							Load(
		lpxmlnode						pNode);

	const Accessor&					accessor()				const
	{
		return m_accessor;
	}
};

class Array
{
private:

	std::string						m_id;
	uint							m_count;
	std::vector<float>				m_floats;
	std::vector<std::string>		m_names;

public:

	void							Load(
		lpxmlnode						pNode);

	std::string&					id()
	{
		return m_id;
	}

	const uint&						count()					const
	{
		return m_count;
	}

	const std::vector<float>&		vec_floats()			const
	{
		return m_floats;
	}

	const float&					item(
		uint							 Index)				const
	{
		return m_floats[Index];
	}

	const float*					lpitem(
		uint							 Index)				const
	{
		return &m_floats[Index];
	}

	const std::string&				name(uint i)			const
	{
		return m_names[i];
	}
};

class Source
{
private:

	std::string						m_id;
	Array							m_array;
	Technique_Common				m_technique_common;

public:

	Source()
	{
		memset(this, 0, sizeof(*this));
	}

	Source(const Source& rhs)
	{
		m_id = rhs.id();
		m_array = rhs.array();
		m_technique_common = rhs.technique_common();
	}

	Source(lpxmlnode pNode) { this->Load(pNode); }

	void							Load(
		lpxmlnode						pNode);

	const std::string&				id()					const
	{
		return m_id;
	}

	const Array&					array()					const
	{
		return m_array;
	}

	const Technique_Common&			technique_common()		const
	{
		return m_technique_common;
	}
};

//- Library Animations --------------------------------------------------------

class Vertex_weights
{

private:

	uint							m_count;
	std::vector<Input>				m_input;
	std::vector<ushort>				m_vcount;
	std::vector<ushort>				m_v; 
									 
public:								 
									 
	void							Load(
		lpxmlnode					 	pNode);

	const Input&					input(
		uint							i)					const
	{
		return m_input[i];
	}

	const std::vector<ushort>&		vcount()				const
	{
		return m_vcount;
	}

	const std::vector<ushort>&		v()						const
	{
		return m_v;
	}

};									 

class JointDescription
{

private:
	std::vector<Input>				 m_input;

public:

	void							 Load(
		lpxmlnode					 	pNode);

};

class Skin							 
{

private:
	std::string						m_source;
	float							m_bind_shape_matrix[16];
	std::vector<Source>				m_sources;
	JointDescription				m_joint;
	Vertex_weights					m_vertex_weights;

public:
	void							Load(
		lpxmlnode						pNode);

	const uint						NrSources()				const
	{
		return m_sources.size();
	}

	const Vertex_weights&			vertex_weights()		const
	{
		return m_vertex_weights;
	}

	const float*					bind_shape_matrix()		const
	{
		return &m_bind_shape_matrix[0];
	}

	const std::string&				source()				const
	{
		return m_source;
	}

	const Source&					source(uint i)			const
	{
		return m_sources[i];
	}

	const Source					source( 
		std::string						ParamName)			const;
};
									 
class Controller
{
private:
	std::string						m_id;
	std::string						m_name;
	Skin							m_skin;

public:								 
									 
	void							Load(
		lpxmlnode					 	pNode);

	const Skin&						skin()					const
	{
		return m_skin;
	}
									 
};

//- Library Visual Scenes -----------------------------------------------------

class Instance_Controller
{

private:

	std::string						m_url;
//	Skeleton						m_skeletor;
//	Bind_Material					m_bind_material;

public:

	const std::string&				url()					const
	{
		return m_url;
	}

	void							 Load(
		lpxmlnode					 	pNode);

};

class Instance_Material
{
private:
	std::string						m_symbol;
	std::string						m_target;

public:

	Instance_Material(lpxmlnode pNode)
	{
		this->Load(pNode);
	}

	Instance_Material()
	{
		m_symbol = "";
		m_target = "";
	}

	void Load(lpxmlnode pNode);
};

class Bind_Material
{

private:

	Instance_Material				m_instance_material;

public:

	Bind_Material(lpxmlnode);

	Bind_Material()
	{
		m_instance_material = Instance_Material();
	}

	void							Load(lpxmlnode pNode);

};

class Instance_Geometry
{

public:

	Instance_Geometry(lpxmlnode pNode);

	Instance_Geometry()
	{
		m_url = "";
		m_bind_material = Bind_Material();
	}

	void							Load(lpxmlnode);

	const Bind_Material&			bind_material()			const
	{
		return m_bind_material;
	}

	const std::string&				url()					const
	{
		return m_url;
	}

private:

	std::string						m_url;
	Bind_Material					m_bind_material;
};

class Node
{

private:

	std::string						m_id;
	std::string						m_name;
	std::string						m_type;

	//	Joints
	Matrix4x4						m_jointmatrix;

	//	Nodes
	Vector3							m_translate;
	Vector4							m_rotateX;
	Vector4							m_rotateY;
	Vector4							m_rotateZ;
	Vector3							m_scale;

	std::vector<Node>				m_nodes;
	Instance_Geometry				m_instance_geometry;
//	Instance_Light					m_instance_light;
//	Instance_Controller				m_instance_controller;

public:

	Node(lpxmlnode pNode);

	Node()
	{
		m_id = ""; m_name = ""; m_type = "";
		memset(&m_translate, 0, sizeof(m_translate));
		memset(&m_rotateX, 0, sizeof(m_rotateX));
		memset(&m_rotateY, 0, sizeof(m_rotateY));
		memset(&m_rotateZ, 0, sizeof(m_rotateZ));
		memset(&m_scale, 0, sizeof(m_scale));
	}

	const Instance_Geometry&		instance_geometry()		const
	{
		return m_instance_geometry;
	}

	const ushort					NrChildren()			const
	{
		return m_nodes.size();
	}

	const Node&						subnode(uint i)			const
	{
		return m_nodes[i];
	}

	const Node*						lpsubnode(uint i)		const
	{
		return &m_nodes[i];
	}

	const std::string&				name()					const
	{
		return m_name;
	}

	const std::string&				id()					const
	{
		return m_id;
	}

	const std::string&				type()					const
	{
		return m_type;
	}

	const Matrix4x4&				matrix()				const
	{
		return m_jointmatrix;
	}
};

class Visual_Scene						 
{

private:

	std::string						m_id;
	std::string						m_name;
	std::vector<Node>				m_nodes;

public:

	Visual_Scene(lpxmlnode pNode);

	const Node*						FindRootNode(
		const std::string&				RootBoneName)				const
	{
		const Node* pNode = NULL;

		for(uint n=0; n<m_nodes.size(); n++) 
			if(RootBoneName == m_nodes[n].name())
				return &m_nodes[n];
			else
				for(uint j=0; j<m_nodes[n].NrChildren(); j++)
					if(RootBoneName == m_nodes[n].subnode(j).name())
						pNode = m_nodes[n].lpsubnode(j);

		return pNode;
	}

	const std::vector<Node>&		lpnodes()				const
	{
		return m_nodes;
	}

	const std::string&				name()					const
	{
		return m_name;
	}

	const Node&						lpnode(uint i)			const
	{
		return m_nodes[i];
	}

	const uint						NrNodes()				const
	{
		return m_nodes.size();
	}
};
									 
//- Library Animations --------------------------------------------------------
									 
class Sampler						 
{

private:

	std::string						 m_id;
	std::vector<Input>				 m_input;

public:

	void							 Load(
		lpxmlnode					 	pNode);

	const Input&					 input(uint i)			const
	{
		return m_input[i];
	}
};

class Channel						 
{									 
									 
private:							 
									 
	std::string						 m_source;
	std::string						 m_target;
									 
public:								 
									 
	void							 Load(
		lpxmlnode						pNode);

};

class Animation
{

private:

	std::string						m_id;
	std::vector<Source>				m_source;
	Sampler							m_sampler;
	Channel							m_channel;

public:

	void							Load(
		lpxmlnode						 pNode);

	const std::string&				id()
	{
		return m_id;
	}

	const Sampler&					sampler()				const
	{
		return m_sampler;
	}

	const Channel&					channel()				const
	{
		return m_channel;
	}

	const uint						NrSources()				const
	{
		return m_source.size();
	}

	const Source&					source(uint i)			const
	{
		return m_source[i];
	}
};

//- Library Geometries --------------------------------------------------------

/*
class Polylist
{

private:

	std::string						m_material;
	uint							m_count;
	std::vector<Input>				m_input;
	std::vector<ushort>				m_p;

public:

	void							Load(
		lpxmlnode						pNode);

	const std::string				GetSource(
		const std::string&				InputSemantic)		const
	{
		std::string SourceName;
		for(uint i=0; i<m_input.size(); i++) {
			if(m_input[i].semantic() == InputSemantic) {
				SourceName = m_input[i].source();
				SourceName.erase(0, 1);
				break;
			}
		}

		return SourceName;
	}

	const std::string &				material()				const
	{
		return m_material;
	}

	const uint &					count()					const
	{
		return m_count;
	}

	const Input&					GetInput(
		const std::string				InputSemantic)		const
	{
		for (uint i = 0; i < m_input.size(); i++)
			if (m_input[i].semantic() == InputSemantic)
				return m_input[i];

		return Input();
	}

	const std::vector<Input>*		lpinputs()				const
	{
		return &m_input;
	}

	const ushort*					p()						const
	{
		return &m_p[0];
	}

	const ushort&					p(
		const uint						Index)				const
	{
		return m_p[Index];
	}

	const uint						GetSize()				const
	{
		return m_p.size();
	}

	const ushort					GetNrInputs()			const
	{
		return m_input.size();
	}

	const Input&					input(uint Index)		const
	{
		return m_input[Index];
	}
};
*/

//	Unindexed collada mesh
class Triangles
{
private:

	std::string						m_material;
	uint							m_count;
	std::vector<Input>				m_input;
	std::vector<ushort>				m_p;

public:
	void							Load(
		lpxmlnode						pNode);

	const std::string				GetSource(
		const std::string&				InputSemantic)		const
	{
		std::string SourceName;
		for(uint i=0; i<m_input.size(); i++) {
			if(m_input[i].semantic() == InputSemantic) {
				SourceName = m_input[i].source();
				SourceName.erase(0, 1);
				break;
			}
		}

		return SourceName;
	}
	
	const std::string&				material()				const
	{
		return m_material;
	}

	const uint&						count()					const
	{
		return m_count;
	}

	const std::vector<Input>*		lpinputs()				const
	{
		return &m_input;
	}

	const Input&					GetInput(
		const std::string				InputSemantic)		const
	{
		for (uint i = 0; i < m_input.size(); i++)
			if (m_input[i].semantic() == InputSemantic)
				return m_input[i];

		return Input();
	}

	const Input&					input(uint Index)		const
	{
		return m_input[Index];
	}

	const ushort*					p()						const
	{
		return &m_p[0];
	}

	const ushort&					p(uint Index)			const
	{
		return m_p[Index];
	}

	const ushort					GetNrInputs()			const
	{
		return m_input.size();
	}

	const ulong						GetSize()				const
	{
		return m_p.size();
	}
};

class Polylist : public Triangles
{

};

class Vertices
{
private:

	std::string						m_id;
	std::string						m_semantic;
	std::vector<Input>				m_input;

public:

	const std::string				GetSource(
		const std::string&				InputSemantic)		const
	{
		std::string SourceName;
		for(uint i=0; i<m_input.size(); i++) {
			if(m_input[i].semantic() == InputSemantic) {
				SourceName = m_input[i].source();
				break;
			}
		}

		return SourceName;
	}

	void							Load(
		lpxmlnode						pNode);

	const std::string&				id()					const
	{
		return m_id;
	}

	const std::string&				semantic()				const
	{
		return m_semantic;
	}

	const std::vector<Input>*		lpinputs()				const
	{
		return &m_input;
	}

	const Input&					input(uint Index)		const
	{
		return m_input[Index];
	}
};

class Mesh
{
private:

	Vertices						m_vertices;
	std::vector<Source>				m_source;
	std::vector<Polylist>			m_polylist;
	std::vector<Triangles>			m_triangles;

public:

	void							Load(
		lpxmlnode						pNode);

	const uint						GetNrSources()			const
	{
		return m_source.size();
	}

	const std::vector<Input>*		GetInputs()				const
	{
		//	assuming all polylists use the same geometries.
		if(m_polylist.size())
			return m_polylist[0].lpinputs();
		else if(m_triangles.size())
			return m_triangles[0].lpinputs();
		else
			return NULL;
	}

	const Source*					ResolveSource(
		const std::string&				SourceURL)			const
	{
		std::string SourceName;
		for(uint i=0; i<m_source.size(); i++)
			if(m_source[i].id() == SourceURL)
				return &m_source[i];

		return NULL;
	}
	
	const Source&					source(uint Index)		const
	{
		return m_source[Index];
	}

	const std::vector<Source>&		sources()				const
	{
		return m_source;
	}

	const uint						GetNrTriangleLists()	const
	{
		return m_triangles.size();
	}
	
	const uint						GetNrPolyLists()		const
	{
		return m_polylist.size();
	}

	const Polylist&					polylist(uint i)		const
	{
		return m_polylist[i];
	}

	const Triangles					trianglelist(uint i)	const
	{
		return m_triangles[i];
	}

	const std::vector<Triangles>&	triangles()				const
	{
		return m_triangles;
	}

	const std::string				GetSource(
		const std::string&				InputSemantic)		const
	{
		return m_triangles[0].GetInput(InputSemantic).source();
	}

	const Vertices&					vertices()				const
	{
		return m_vertices;
	}
};

class Geometry
{
private:

	std::string						m_id;
	std::string						m_name;
	Mesh							m_mesh;

public:

	void							Load(
		lpxmlnode						pNode);

	const std::string&				id()					const
	{
		return m_id;
	}

	const std::string&				name()					const
	{
		return m_name;
	}

	const Mesh&						mesh()					const
	{
		return m_mesh;
	}
};

//- Library_Materials ---------------------------------------------------------

class Instance_Effect
{
private:

	std::string						 m_url;

public:

	const std::string&				url()					const
	{
		return m_url;
	}

	void							Load(
		lpxmlnode						pNode);

};

struct Material
{
private:

	std::string						 m_id;
	std::string						 m_name;
	Instance_Effect					 m_instance_effect;

public:

	const std::string&				id()					const
	{
		return m_id;
	}

	const std::string&				name()					const
	{	
		return m_name;
	}

	const Instance_Effect&			instance()				const
	{
		return m_instance_effect;
	}

	void							 Load(
		lpxmlnode						pNode);
};

//- Library_Effects -----------------------------------------------------------

class Sampler2D
{
	std::string						m_id;
	std::string						m_source;

public:

	const std::string &				id()					const
	{
		return m_id;
	}

	const std::string &				source()				const
	{
		return m_source;
	}

	void							Load(
		lpxmlnode						pNode)
	{
		lpxmlnode pCurrNode = pNode->first_node();

		m_id = pNode->parent()->first_attribute("sid")->value();

		std::string Name = pCurrNode->name();
		if(Name == "source")
			m_source = pCurrNode->value();
		
	}
};

struct Surface
{
private:

	std::string						m_sid;
	std::string						m_type;
	std::string						m_init_from;

public:

	const std::string&				type()					const
	{
		return m_type;
	}

	const std::string&				sid()					const
	{
		return m_sid;
	}

	const std::string&				init_from()				const
	{
		return m_init_from;
	}

	void							Load(
		lpxmlnode						pNode);

};

struct Texture
{
private:

	std::string						m_texture;
	std::string						m_texcoord;

public:

	void							Load(
		lpxmlnode						pNode);

	const std::string&				texture()				const
	{
		return this->m_texture;
	}

	const std::string&				texcoord()				const
	{
		return this->m_texcoord;
	}
};

struct CColor4
{

private:

	float							m_r;
	float							m_g;
	float							m_b;
	float							m_a;

public:

	const float&					r()						const
	{
		return this->m_r;
	}

	const float&					g()						const
	{
		return this->m_g;
	}

	const float&					b()						const
	{
		return this->m_b;
	}

	const float&					a()						const
	{
		return this->m_a;
	}

	void							 Load(
		lpxmlnode						pNode);
};

struct EffectData
{

private:

	CColor4							m_color;
	Texture							m_texture;

public:

	const CColor4&					color()					const
	{
		return m_color;
	}

	const Texture&					texture()				const
	{
		return m_texture;
	}

	const float						GetIndex()				const;

	void							Load(
		lpxmlnode						pNode);
	
};

class MaterialDesc
{

private:

	EffectData						 m_emission;
	EffectData						 m_ambient;
	EffectData						 m_diffuse;
	EffectData						 m_specular;
	Texture							 m_bump;
	float							 m_shininess;

public:

	const EffectData&				emission()				const
	{
		return m_emission;
	}

	const EffectData&				ambient()				const
	{
		return m_ambient;
	}

	const EffectData&				diffuse()				const
	{
		return m_diffuse;
	}

	const EffectData&	 			specular()				const
	{
		return m_specular;
	}

	const Texture&					bump()					const
	{
		return m_bump;
	}

	const float&					shininess()				const
	{
		return m_shininess;
	}

	void							 Load(
		lpxmlnode						pNode);

};

class Technique
{
private:

	MaterialDesc					m_phong;
	MaterialDesc					m_lambert;

public:

	const MaterialDesc&				phong()					const
	{
		return m_phong;
	}

	void							Load(
		lpxmlnode						pNode);

};

class newparam
{

private:

	std::string						m_sid;
	Surface							m_surface;

public:

	void							Load(
		lpxmlnode						pNode);

};

class profile_COMMON
{
private:

	std::vector<Sampler2D>			m_sampler2d;
	std::vector<Surface>			m_surface;
	Technique						m_technique;

public:

	const uint						NrSurfaces()			const
	{
		return m_surface.size();
	}

	const Surface&					surface(uint Index)		const
	{
		return m_surface[Index];
	}

	const Sampler2D&				sampler2D(uint Index)	const
	{
		return m_sampler2d[Index];
	}

	const uint						NrSamplers()			const
	{
		return m_sampler2d.size();
	}

	const Technique&				technique()				const
	{
		return m_technique;
	}

	void							Load(
		lpxmlnode						pNode);

};

struct Effect
{
private:

	std::string						 m_id;
	profile_COMMON					 m_profile;

public:

	const std::string&				id()					const
	{
		return m_id;
	}

	const profile_COMMON&			profile()				const
	{
		return m_profile;
	}

	void							 Load(
		lpxmlnode						pNode);

};

//- Library_Images ------------------------------------------------------------

struct Image
{
private:

	std::string						 m_id;
	std::string						 m_name;
	std::string						 m_init_from;
	
public:

	const std::string&				 id()					const
	{
		return m_id;
	}

	const std::string&				 name()					const
	{
		return m_name;
	}

	const std::string&				 init_from()			const
	{
		return m_init_from;
	}

	void							 Load(
		lpxmlnode						pNode);
};

//- Main Class ----------------------------------------------------------------

class ColladaDoc
{
private:

	Asset							assets;							
	std::vector<Image>				library_images;
	std::vector<Effect>				library_effects;
	std::vector<Material>			library_materials;
	std::vector<Geometry>			library_geometries;
	std::vector<Animation>			library_animations;
	std::vector<Controller>			library_controller;
	std::vector<Visual_Scene>		library_visual_scenes;

	std::string						m_filepath;
	std::string						m_name;

	//- Loading ---------------------------------------------------------------

public:

	const Asset&					asset()							const
	{
		return assets;
	}
	
	const Image&					image(uint Index)				const
	{
		return library_images[Index];
	}

	const Material					material(uint Index)			const
	{
		return library_materials[Index];
	}

	const Effect&					effect(uint Index)				const
	{
		return library_effects[Index];
	}

	const Effect					effect(const std::string Name)	const
	{
		std::string Search = Name;
		Search = Search.erase(0, 1);
		for(uint i=0; i<GetNrEffects(); i++)
			if(library_effects[i].id() == Search)
				return library_effects[i];

		return Effect();
	}

	const Geometry&					geometry(uint Index)			const
	{
		return library_geometries[Index];
	}

	const Animation&				animation(uint Index)			const
	{
		return library_animations[Index];
	}

	const Controller&				controller(uint Index)			const
	{
		return library_controller[Index];
	}

	const Visual_Scene&				visual_scene(uint Index)		const
	{
		return library_visual_scenes[Index];
	}

	const std::string&				filepath()						const
	{
		return m_filepath;
	}

	const ushort					GetNrMaterials()				const
	{
		return (ushort)library_materials.size();
	}

	const ushort					GetNrEffects()					const
	{
		return (ushort)library_effects.size();
	}
	
	const ushort					GetNrImages()					const
	{
		return (ushort)library_images.size();
	}

	const ushort					GetNrAnimations()				const
	{
		return (ushort)library_animations.size();
	}

	const ushort					GetNrGeometries()				const
	{
		return library_geometries.size();
	}

	const ushort					GetNrControllers()				const
	{
		return library_controller.size();
	}

	const ushort					GetNrVisualSceneItems()			const
	{
		return library_visual_scenes.size();
	}

	ColladaDoc();
	ColladaDoc(
		lpxmlnode					pNode);

	void Read(
		const char*					FilePath);

	void Load(
		lpxmlnode					pNode);

	void LoadImages(
		lpxmlnode					pNode);

	void LoadEffects(
		lpxmlnode					pNode);

	void LoadMaterials(
		lpxmlnode					pNode);

	void LoadGeometries(
		lpxmlnode					pNode);

	void LoadAnimations(
		lpxmlnode					pNode);

	void LoadControllers(
		lpxmlnode					pNode);

	void LoadVisualScenes(
		lpxmlnode					pNode);

	void LoadAssets(
		lpxmlnode					pNode);

	//- Other stuff ---------------------------------------------------------------

	const std::string				GetSurfaceFileName(
		const std::string&				Name)				const;

	const std::string				GetEffectFileName(
		const std::string&				Name)				const;
};

#endif //	_COLLADADOC_H_
