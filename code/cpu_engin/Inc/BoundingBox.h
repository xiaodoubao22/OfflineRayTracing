#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "Utils.h"
#include "HitResult.h"

class BoundingBox {
public:
	glm::vec3 mMin;
	glm::vec3 mMax;

public:
	BoundingBox();
	BoundingBox(glm::vec3 min, glm::vec3 max);
	~BoundingBox();

	float GetVolum();
	float GetSurfaceArea();
	int GetMaxAxis();
	bool Intersect(const Ray& ray);
	
	void BoundPoint(glm::vec3 p);
	void Union(const BoundingBox& b1);
	static BoundingBox Union(const BoundingBox& b1, const BoundingBox& b2);
};


#endif // !BOUNDING_BOX_H

