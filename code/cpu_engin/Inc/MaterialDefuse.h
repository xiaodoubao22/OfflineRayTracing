#ifndef MATERIAL_DEFUSE
#define MATERIAL_DEFUSE

#include "Material.h"
#include "TexureSampler.h"

class MaterialDefuse : public Material
{
public:
	glm::vec3 mAlbedo = glm::vec3(0.0f, 0.0f, 0.0f);
	TexureSampler3F* mAlbedoTexure = nullptr;

public:
	MaterialDefuse();
	explicit MaterialDefuse(glm::vec3 albedo);
	explicit MaterialDefuse(TexureSampler3F* albedoTexure);

	virtual bool SampleAndEval(SampleData& data, TraceInfo info) override;
	virtual bool SampleWithImportance(SampleData& data) override;
	virtual void Eval(SampleData& data) override;
	bool IsExtremelySpecular(glm::vec2 texCoord) override;

public:
	void SetAlbedo(const glm::vec3& albedo);
	void SetAlbedo(TexureSampler3F* albedoTexure);
};


#endif // !MATERIAL_DEFUSE
