#include "Shape.h"

Shape::Shape() {
	mModel = glm::mat4(1.0f);
	mModelInv = glm::mat4(1.0f);
}

Shape::~Shape() {
	if (mBound != nullptr) {
		delete mBound;
		mBound = nullptr;
	}
}

bool Shape::HasEmission() {
	if (mMaterial == nullptr) {
		return false;
	}
	return mMaterial->HasEmission();
}

BoundingBox* Shape::GetBoundingBox() {
	return mBound;
}

void Shape::SetModelMatrix(const glm::mat4& model) {
	mModel = model;
	mModelInv = glm::inverse(model);
}
