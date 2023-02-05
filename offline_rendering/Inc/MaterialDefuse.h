#ifndef MATERIAL_DEFUSE
#define MATERIAL_DEFUSE

#include "Material.h"

class MaterialDefuse : public Material
{
public:
	glm::vec3 mAlbedo = glm::vec3(0.0f, 0.0f, 0.0f);

public:
	MaterialDefuse();
	explicit MaterialDefuse(glm::vec3 albedo);

	virtual bool SampleAndEval(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf, glm::vec3& fr) override;
	virtual bool SampleWithImportance(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf) override;
	virtual glm::vec3 Eval(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& wo) override;
};


#endif // !MATERIAL_DEFUSE
