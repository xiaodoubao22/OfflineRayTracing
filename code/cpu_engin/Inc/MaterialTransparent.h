#ifndef MATERIAL_TRANSPARENT
#define MATERIAL_TRANSPARENT

#include "Material.h"

class MaterialTransparent : public Material
{
public:
	float mIor;
	glm::vec3 mColor = glm::vec3(0.0f);
public:
	MaterialTransparent();
	explicit MaterialTransparent(float ior, glm::vec3 color = glm::vec3(1.0f));
    virtual ~MaterialTransparent() override;

	virtual bool SampleAndEval(SampleData& data, TraceInfo info) override;
	virtual bool SampleWithImportance(SampleData& data, const TraceInfo& info) override;
	virtual void Eval(SampleData& data) override;
	bool IsExtremelySpecular(glm::vec2 texCoord) override;
};


#endif // !MATERIAL_TRANSPARENT

