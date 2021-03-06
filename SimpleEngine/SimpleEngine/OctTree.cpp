#include "OctTree.h"
#include <numeric>

OctTree::OctTree(Box box, std::vector<Triangle>& tr) : triangle(tr)
{
	std::vector<int> tr1(triangle.size());
	std::iota(std::begin(tr1), std::end(tr1), 0);
	root = new Node(box, tr1);
	divCube(root);
}

void OctTree::divCube(Node*& root)
{
	Vector3 center((root->box.getMax().x + root->box.getMin().x) / 2, (root->box.getMax().y + root->box.getMin().y) / 2, (root->box.getMax().z + root->box.getMin().z) / 2);
	Vector3 hx(center.x - root->box.getMin().x, 0, 0);
	Vector3 hy(0, center.y - root->box.getMin().y, 0);
	Vector3 hz(0, 0, center.z - root->box.getMin().z);
	
	std::vector<Box> boxes;
	boxes.push_back(Box(root->box.getMin(), center));
	boxes.push_back(Box(root->box.getMin() + hz, center + hz));
	boxes.push_back(Box(root->box.getMin() + hx, center + hx));
	boxes.push_back(Box(root->box.getMin() + hz + hx, center + hz + hx));
	boxes.push_back(Box(root->box.getMin() + hy, center + hy));
	boxes.push_back(Box(root->box.getMin() + hz + hy, center + hz + hy));
	boxes.push_back(Box(root->box.getMin() + hx + hy, center + hx + hy));
	boxes.push_back(Box(center, root->box.getMax()));

	for (size_t j = 0; j < boxes.size(); j++)
	{
		std::vector<int> tr1;
		for (auto i : root->nTringle)
		{
			if (triangle[i].inBox(boxes[j]))
				tr1.push_back(i);
		}
		root->ptr_node[j] = new Node(boxes[j], tr1);
		if ((root->ptr_node[j]->nTringle.size() > 10) && (hx.x > 0.001))
			divCube(root->ptr_node[j]);
	}
}
bool OctTree::findMinIntersection(Ray r, int& minTriangle, float& minDist, float& u, float& v)
{
	recursivelyFindMinIntersection(root, r, minTriangle, minDist, u, v);
	if (minDist < 10'000)
		return true;
	return false;
}

void OctTree::recursivelyFindMinIntersection(Node* root, Ray r, int& minTriangle, float& minDist, float& u, float& v)
{
	if (root == nullptr || !r.boxIntersaction(root->box))
		return;

	if (root->ptr_node[0] == nullptr)
	{
		for (auto i : root->nTringle)
		{
			float u1, v1;
			float dist = r.triangleIntersection(triangle[i], u1, v1);
			if (dist > 0.0001)
			{
				if (dist < minDist)
				{
					minTriangle = i;
					minDist = dist;
					u = u1;
					v = v1;
				}
			}
		}
	}
	else
	{
		for (auto& node : root->ptr_node)
		{
			recursivelyFindMinIntersection(node, r, minTriangle, minDist, u, v);
		}
	}
}

