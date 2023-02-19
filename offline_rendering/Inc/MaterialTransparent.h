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
	virtual bool SampleAndEval(SampleData& data, TraceInfo info) override;
	virtual bool SampleWithImportance(SampleData& data) override;
	virtual void Eval(SampleData& data) override;
	bool IsExtremelySpecular(glm::vec2 texCoord) override;
};


#endif // !MATERIAL_TRANSPARENT

