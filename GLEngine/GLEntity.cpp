
#include "GLEntity.h"

namespace GLEngine
{
	GL3DShader*				GLRenderController::pCurrentShader = nullptr;
	GLSubject*				GLRenderController::pCurrentSubject = nullptr;

	//- GLLight -------------------------------------------------------------------

	const color3 & GLLight::getAmbient() const
	{
		return this->Ambient;
	}

	const color3 & GLLight::getDiffuse() const
	{
		return this->Diffuse;
	}

	const color3 & GLLight::getSpecular() const
	{
		return this->Specular;
	}

	const flags & GLLight::getLightType() const
	{
		return this->Type;
	}

	const vec3 & GLOmniLight::getPosition() const
	{
		return this->Position;
	}

	const float & GLOmniLight::getConstantAttenuation() const
	{
		return this->ConstantAttenuation;
	}

	const float & GLOmniLight::getLinearAttenuation() const
	{
		return this->LinearAttenuation;
	}

	const float & GLOmniLight::getQuadraticAttenuation() const
	{
		return this->QuadraticAttenuation;
	}

	void GLOmniLight::SendLightToShader() const
	{
		GLRenderController::SendLightToShader(*this);
	}
	
	//- GLObjectTransform ---------------------------------------------------------

	void GLSubject::SendMatricesToShader()
	{
		GLRenderController::SendMatricesToShader(*this);
	}
	
	void GLObject::SetObject(GLModel* const pObject)
	{
		this->pObject = pObject;
	}

	const GLModel* GLObject::GetModelPointer() const
	{
		return pObject;
	}

	void GLObject::Render()
	{
		GLRenderController::RenderObject(*this);
	}

	void GLRenderController::setShader(GL3DShader* pShader)
	{
		if (pCurrentShader != pShader) {
			pCurrentShader = pShader;
			pCurrentShader->enableShader();
		}
	}

	void GLRenderController::setSubject(GLSubject* pSubject)
	{
		if (pCurrentSubject != pSubject) {
			pCurrentSubject = pSubject;
		}
	}

	void GLRenderController::SendMatricesToShader(const GLSubject& Subject)
	{
		pCurrentShader->SendMVPMatrix(Subject.GetMVPMatrix());
		pCurrentShader->SendMVMatrix(Subject.GetMVMatrix());
		pCurrentShader->SendNMatrix(Subject.GetNMatrix());
	}

	void GLRenderController::SendLightToShader(
		const GLOmniLight&					Light)
	{
		const mat4& M = Light.GetTransform();
		const mat4& MV = pCurrentSubject->GetTransform() * M;

		vec3 pos = (vec3)(MV * vec4(0.0f, 0.0f, 0.0f, 1.0f));

		glGetError();

		pCurrentShader->SendPosition(pos);
		pCurrentShader->SendDiffuse(Light.getDiffuse());
		pCurrentShader->SendAmbient(Light.getAmbient());
		pCurrentShader->SendSpecular(Light.getSpecular());
		pCurrentShader->SendCAttenuation(Light.getConstantAttenuation());
		pCurrentShader->SendLAttenuation(Light.getLinearAttenuation());
		pCurrentShader->SendQAttenuation(Light.getQuadraticAttenuation());
	}

	void GLRenderController::RenderObject(const GLObject& Object)
	{
		const GLModel& Model					= *Object.GetModelPointer();
		const AttribData<GLBuffers>& MBuffers	= Model.GetModelBuffers();		
		const GLBones* pBones					= Model.GetBoneStructure();
		
		pCurrentSubject->TransformObject(Object.GetTransform());
		pCurrentSubject->SendMatricesToShader();
		pCurrentShader->SendBonestructure(*Model.GetBoneStructure());
		
		for (uint i = 0; i < MBuffers.Size(); i++) {
		
			pCurrentShader->SendAttributes(MBuffers[i]);

			const AttribData<GLPolygons>& T = MBuffers[i].getPolygons();
			for (uint j = 0; j < T.Size(); j++) {
				pCurrentShader->RenderPolygons(T[j]);
			}

			pCurrentShader->DisableAttribArrays();
		}
	}
}
