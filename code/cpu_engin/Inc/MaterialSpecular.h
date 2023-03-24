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
	virtual bool SampleAndEval(SampleData& data, TraceInfo info) override;
	virtual bool SampleWithImportance(SampleData& data) override;
	virtual void Eval(SampleData& data) override;
	bool IsExtremelySpecular(glm::vec2 texCoord) override;

};

#endif // !MATERIAL_SPECULAR

