#include "Mesh.h"

Mesh::Mesh() {
}

Mesh::Mesh(aiMesh* assImpMesh, Material* material) {
	mMaterial = material;
	mTriangles.clear();
	mBound = new BoundingBox;
	for (unsigned int cntFace = 0; cntFace < assImpMesh->mNumFaces; cntFace++) {
		aiFace face = assImpMesh->mFaces[cntFace];

		unsigned int ind0 = face.mIndices[0];
		unsigned int ind1 = face.mIndices[1];
		unsigned int ind2 = face.mIndices[2];
			
		glm::vec3 p0 = Utils::AiVector3DToGlm(assImpMesh->mVertices[ind0]);
		glm::vec3 p1 = Utils::AiVector3DToGlm(assImpMesh->mVertices[ind1]);
		glm::vec3 p2 = Utils::AiVector3DToGlm(assImpMesh->mVertices[ind2]);

		Triangle* triangle = new Triangle(p0, p1, p2, mMaterial);
		mTriangles.push_back(triangle);
		mBound->Union(*triangle->GetBoundingBox());
	}

	mBvhTree.Build(mTriangles);
	//std::cout << "min = (" << mBound->mMin.x << mBound->mMin.y << mBound->mMin.z << ")" << std::endl;
	//std::cout << "max = (" << mBound->mMax.x << mBound->mMax.y << mBound->mMax.z << ")" << std::endl;
}

Mesh::~Mesh() {
	for (auto triangle : mTriangles) {
		delete triangle;
	}
}

HitResult Mesh::Intersect(Ray ray) {
#define USE_BVH
#ifdef USE_BVH
	HitResult res = mBvhTree.GetIntersect(ray);
#else
	HitResult res;
	for (auto triangle : mTriangles) {
		HitResult res = triangle->Intersect(ray);
		if (res.isHit == true) {
			return res;
		}
	}
#endif // USE_BVH

	return res;
}

void Mesh::UniformSample(float& pdf, HitResult& res) {

}

float Mesh::GetArea() {
	return 0.0f;
}

glm::vec3 Mesh::GetCenter() {
	return (mBound->mMin + mBound->mMax) / 2.0f;
}