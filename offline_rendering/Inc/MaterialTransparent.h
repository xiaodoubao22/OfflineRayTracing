#ifndef MATERIAL_TRANSPARENT
#define MATERIAL_TRANSPARENT

#include "Material.h"

class MaterialTransparent : public Material
{
public:
	float mIor;
public:
	MaterialTransparent();
	explicit MaterialTransparent(float ior);
	virtual bool SampleAndEval(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf, glm::vec3& fr) override;
	virtual bool SampleWithImportance(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf) override;
	virtual glm::vec3 Eval(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& wo) override;
};


#endif // !MATERIAL_TRANSPARENT

