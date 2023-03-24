#include "Mesh.h"

Mesh::Mesh() {
}

Mesh::Mesh(aiMesh* assImpMesh, Material* material, float scale) {
	mMaterial = material;
	mTriangles.clear();
	mBound = new BoundingBox;

	for (unsigned int cntFace = 0; cntFace < assImpMesh->mNumFaces; cntFace++) {
		aiFace face = assImpMesh->mFaces[cntFace];

		unsigned int ind0 = face.mIndices[0];
		unsigned int ind1 = face.mIndices[1];
		unsigned int ind2 = face.mIndices[2];
			
		glm::vec3 p0 = scale * Utils::AiVector3DToGlm(assImpMesh->mVertices[ind0]);// - glm::vec3(0.0f, 40.0f, 0.0f);
		glm::vec3 p1 = scale * Utils::AiVector3DToGlm(assImpMesh->mVertices[ind1]);// - glm::vec3(0.0f, 40.0f, 0.0f);
		glm::vec3 p2 = scale * Utils::AiVector3DToGlm(assImpMesh->mVertices[ind2]);// - glm::vec3(0.0f, 40.0f, 0.0f);

		Triangle* triangle = nullptr;
		triangle = new Triangle(p0, p1, p2, mMaterial);

		if (assImpMesh->HasNormals()) {
			glm::vec3 n0 = Utils::AiVector3DToGlm(assImpMesh->mNormals[ind0]);
			glm::vec3 n1 = Utils::AiVector3DToGlm(assImpMesh->mNormals[ind1]);
			glm::vec3 n2 = Utils::AiVector3DToGlm(assImpMesh->mNormals[ind2]);
			triangle->SetNormal(n0, n1, n2);
		}

		if (assImpMesh->mTextureCoords[0]) {
			glm::vec2 t0(assImpMesh->mTextureCoords[0][ind0].x, assImpMesh->mTextureCoords[0][ind0].y);
			glm::vec2 t1(assImpMesh->mTextureCoords[0][ind1].x, assImpMesh->mTextureCoords[0][ind1].y);
			glm::vec2 t2(assImpMesh->mTextureCoords[0][ind2].x, assImpMesh->mTextureCoords[0][ind2].y);
			triangle->SetTexureCoord(t0, t1, t2);
		}
		
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
	// 光线转到局部坐标系下
	ray.origin = mModel * glm::vec4(ray.origin, 1.0f);
	ray.direction = glm::mat3(mModel) * ray.direction;

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

	if (res.isHit == 1) {
		res.hitPoint = mModelInv * glm::vec4(res.hitPoint, 1.0f);
		res.normal = glm::mat3(mModelInv) * res.normal;
	}

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