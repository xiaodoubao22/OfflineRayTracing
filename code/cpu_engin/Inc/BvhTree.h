#ifndef BVH_TREE_H
#define BVH_TREE_H

#include "Shape.h"
#include "BoundingBox.h"

// *****************************
// 非叶子节点的shape==nullptr
// 当前节点的bound==Union(left->bound, right->bound)
// *****************************
struct BvhNode
{
	BvhNode* left = nullptr;
	BvhNode* right = nullptr;
	Shape* shape = nullptr;
	BoundingBox* bound = nullptr;
};

class BvhTree {
public:

	BvhNode* mRoot = nullptr;

public:
	BvhTree();
	BvhTree(std::vector<Shape*>& shapes);
	~BvhTree();

	void Build(std::vector<Shape*>& shapes);
	HitResult GetIntersect(Ray& ray);
	
private:
	HitResult TraversalIntersect(Ray& ray, BvhNode* node);
	BvhNode* TraversalBuild(std::vector<Shape*>& shapes);
};


#endif // !BVH_TREE_H

