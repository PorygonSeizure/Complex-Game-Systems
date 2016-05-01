#ifndef _NODE_H_
#define _NODE_H_

#include <glm/glm.hpp>
#include <vector>

class BoundingSphere;

class Node
{
public:
	Node();
	virtual ~Node();

	void AddChild(Node* child);

	virtual void Update();

	void UpdateWorldTransform();
	void UpdateWorldBounds();

	void SetParent(Node* parent);

	double Loop();

	Node* data;

protected:
	Node* m_parent;

	BoundingSphere* m_localBounds;
	BoundingSphere* m_worldBounds;

	glm::mat4 m_localTransform;
	glm::mat4 m_worldTransform;

	std::vector<Node*> m_children;
};

#endif