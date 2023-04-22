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
    virtual ~MaterialSpecular() override;

	virtual bool SampleAndEval(SampleData& data, TraceInfo info) override;
	virtual bool SampleWithImportance(SampleData& data, const TraceInfo& info) override;
	virtual void Eval(SampleData& data) override;
	bool IsExtremelySpecular(glm::vec2 texCoord) override;

};

#endif // !MATERIAL_SPECULAR

