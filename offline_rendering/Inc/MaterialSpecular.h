#ifndef MATERIAL_SPECULAR
#define MATERIAL_SPECULAR

#include "Material.h"

class MaterialSpecular : public Material 
{
public:
	glm::vec3 mSpecularRate;
public:
	MaterialSpecular();
	explicit MaterialSpecular(glm::vec3 specularRate);
	virtual bool SampleAndEval(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf, glm::vec3& fr) override;
	virtual bool SampleWithImportance(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf) override;
	virtual glm::vec3 Eval(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& wo) override;

};

#endif // !MATERIAL_SPECULAR

