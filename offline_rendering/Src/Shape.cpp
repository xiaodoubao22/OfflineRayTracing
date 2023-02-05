#include "Shape.h"

Shape::Shape() {

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