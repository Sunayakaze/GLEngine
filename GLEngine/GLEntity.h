
#pragma once

#include "GLShader.h"

namespace GLEngine
{
	//	Controls how transforms are calculated
	typedef void(*DelegateResolveTransform)(const double&, mat4&);

	class GLEntity
	{

	public:

		GLEntity()
		{
			LastTime			= 0.0l;
			ModelTransform		= mat4(1.0);
			WorldTransform		= mat4(1.0);
			pParentEntity		= nullptr;
			Transformation		= nullptr;
		}

		GLEntity(const GLEntity& Param)
			: LastTime(Param.LastTime)
			, ModelTransform(Param.ModelTransform)
			, WorldTransform(Param.WorldTransform)
			, Transformation(Param.Transformation)
			, pParentEntity(Param.pParentEntity) { }

		virtual void						Update(const double&) = 0;

		virtual const mat4&					GetTransform() const {
			return WorldTransform;
		}
		
	protected:

		double								LastTime;
		mat4								ModelTransform;			//	Modeltransform
		mat4								WorldTransform;			//	Resolved modeltransform
		DelegateResolveTransform			Transformation;
		GLEntity*							pParentEntity;

	};

	class GLObject : public GLEntity
	{

	public:

		GLObject() : GLEntity()
		{
			pObject			= nullptr;
			pShader			= nullptr;
		}

		GLObject(
			GLModel* const						pObject,
			const mat4&							Transform,
			GLEntity*							pParentEntity,
			GL3DShader* const					pShader)
			: pObject(pObject)
			, pShader(pShader)
		{
			this->pParentEntity = pParentEntity;
			this->ModelTransform = Transform;
			this->WorldTransform = mat4(1.0f);
			this->Transformation = nullptr;
		}

		GLObject(
			GLModel* const						pObject,
			DelegateResolveTransform			Transformation,
			GLEntity*							pParentEntity,
			GL3DShader* const					pShader)
			: pObject(pObject)
			, pShader(pShader)
		{
			this->pParentEntity = pParentEntity;
			this->ModelTransform = mat4(1.0f);
			this->WorldTransform = mat4(1.0f);
			this->Transformation = Transformation;
		}

		GLObject(
			GLModel* const						pObject,
			const mat4&							Transform,
			DelegateResolveTransform			Transformation,
			GLEntity*							pParentEntity,
			GL3DShader* const					pShader)
			: GLEntity()
			, pObject(pObject)
			, pShader(pShader)
		{
			this->pParentEntity = pParentEntity;
			this->ModelTransform = Transform;
			this->WorldTransform = mat4(1.0f);
			this->Transformation = Transformation;
		}

		void								SetObject(GLModel* const);

		const GLModel*						GetModelPointer() const;

		GL3DShader* const					GetShaderPointer() const {
			return pShader;
		}

		void								Update(const double& Time)
		{
			if (LastTime != Time)
			{
				mat4 Transform, ParentTransform;

//				if (pParentEntity) {
//					pParentEntity->Update(Time);
//					const mat4& P = pParentEntity->GetTransform();
//					ParentTransform = P * ModelTransform;
//				}
//				else {
//					ParentTransform = ModelTransform;
//				}
//				
//				if (Transformation != nullptr) {
//					mat4 Transform;
//					Transformation(Time, Transform);
//				}

//				WorldTransform = Transform * ParentTransform;

				if (pParentEntity) {
					pParentEntity->Update(Time);
					const mat4& P = pParentEntity->GetTransform();
					WorldTransform = P * ModelTransform;
				} else {
					WorldTransform = ModelTransform;
				}

				if (Transformation != nullptr) {
					mat4 Transform;
					Transformation(Time, WorldTransform);
				}

				if (pShader) {
					pShader->Update(Time);
				}

				LastTime = Time;
			}
		}

		virtual void						Render();
		
	protected:

		GLModel*							pObject;
		GL3DShader*							pShader;
	};

	class GLSubject : public GLEntity
	{

	public:

		GLSubject() : GLEntity()
		{
			View = mat4(1.0f);
			Projection = mat4(1.0f);
			Transformation = nullptr;
			MVMatrix = mat4(1.0f);
			MVPMatrix = mat4(1.0f);
			NMatrix = mat4(1.0f);
		}

		GLSubject(
			const mat4&								Projection,
			const mat4&								View,
			DelegateResolveTransform				Transformation,
			GLEntity*								pParentEntity)
		{
			this->pParentEntity = pParentEntity;
			this->Projection = Projection;
			this->View = View;
			this->Transformation = Transformation;

			MVMatrix = mat4(1.0f);
			MVPMatrix = mat4(1.0f);
			NMatrix = mat4(1.0f);
		}

		void								TransformObject(const mat4& Entity)
		{
			MVMatrix = View * Entity;
			MVPMatrix = Projection * MVMatrix;
			NMatrix = transpose(inverse(MVMatrix));
		}

		void								SendMatricesToShader();

		const mat4&							GetTransform()
		{
			return View;
		}

		const mat4&							GetMVPMatrix() const
		{
			return MVPMatrix;
		}

		const mat4&							GetMVMatrix() const
		{
			return MVMatrix;
		}

		const mat4&							GetNMatrix() const
		{
			return NMatrix;
		}

		void								Update(const double& Time)
		{
			if (LastTime != Time)
			{
				if (pParentEntity) {
					pParentEntity->Update(Time);
					const mat4& P = pParentEntity->GetTransform();
					WorldTransform = P * ModelTransform;
				}
				else
					WorldTransform = ModelTransform;

				if (Transformation != nullptr) {
					Transformation(Time, WorldTransform);
				}

				View = WorldTransform;

				LastTime = Time;
			}
		}

	protected:

		mat4								MVMatrix;
		mat4								MVPMatrix;
		mat4								NMatrix;

		mat4								View;
		mat4								Projection;
	};

	class GLLight : public GLObject
	{

	public:

		GLLight() : GLObject()
		{
			Type = 0;
			Ambient = color3(0.0f);
			Diffuse = color3(0.0f);
			Specular = color3(0.0f);
		}

		const color3&						getAmbient() const;
		const color3&						getDiffuse() const;
		const color3&						getSpecular() const;
		const flags&						getLightType() const;

		virtual void						SendLightToShader() const = 0;

	protected:

		color3								Ambient;
		color3								Diffuse;
		color3								Specular;

		flags								Type;

	};

	class GLOmniLight : public GLLight
	{

	public:

		GLOmniLight() : GLLight()
		{
			this->Position = vec3(0.0f);
			this->ConstantAttenuation = 0.0;
			this->LinearAttenuation = 0.0f;
			this->QuadraticAttenuation = 0.0f;
		}

		GLOmniLight(
			const color3&						Ambient,
			const color3&						Diffuse,
			const color3&						Specular,
			const float&						ConstantAttenuation,
			const float&						LinearAttenuation,
			const float&						QuadraticAttenuation,
			const mat4&							ModelTransform,
			GLEntity* const						pParentEntity,
			DelegateResolveTransform			ResolveTransform)
		{
			this->Ambient				= Ambient;
			this->Diffuse				= Diffuse;
			this->Specular				= Specular;
			this->ConstantAttenuation	= ConstantAttenuation;
			this->LinearAttenuation		= LinearAttenuation;
			this->QuadraticAttenuation	= QuadraticAttenuation;

			this->pParentEntity			= pParentEntity;
			this->ModelTransform		= mat4(1.0f);
			this->WorldTransform		= mat4(1.0f);
			this->Transformation		= ResolveTransform;
		}

		const mat4&							GetTransform() const
		{
			return WorldTransform;
		}

		const vec3&							getPosition() const;
		const float&						getConstantAttenuation() const;
		const float&						getLinearAttenuation() const;
		const float&						getQuadraticAttenuation() const;

		virtual void						SendLightToShader() const;

		vec3								Position;
		float								ConstantAttenuation;
		float								LinearAttenuation;
		float								QuadraticAttenuation;
	};

	class GLSpotLight : public GLLight
	{
	public:

		GLSpotLight()
			: Direction(vec3(0.0f))
			, Cutoff(0.0f)
			, Exponent(0.0f)
		{

		}

	protected:

		vec3								Direction;
		float								Cutoff;
		float								Exponent;

	};

	//- GLRenderer -----------------------------------------------------------------------------

	class GLRenderController
	{

	public:

		static void							setShader(GL3DShader*);
		static void							setSubject(GLSubject*);

		static void							SendLightToShader(const GLOmniLight&);
		static void							SendMatricesToShader(const GLSubject&);
		static void							RenderObject(const GLObject&);

	private:

		static GL3DShader*					pCurrentShader;
		static GLSubject*					pCurrentSubject;

	};
}