#include "Node.h"
#include <Timer.h>
#include "BoundingSphere.h"

Node::Node() : m_parent(nullptr), m_localTransform(1), data(nullptr)
{
	m_localBounds = new BoundingSphere();
	m_worldBounds = new BoundingSphere();
}

Node::~Node()
{
	//Node** data = m_children.data();

	delete m_localBounds;
	delete m_worldBounds;

	if (data != nullptr)
	{
		delete[] data;
		return;
	}

	for (auto child : m_children)
		delete child;
}

void Node::AddChild(Node* child)
{
	m_children.push_back(child);
	child->SetParent(this);
}

void Node::Update()
{
	UpdateWorldTransform();

	for (auto child : m_children)
		child->Update();

	UpdateWorldBounds();
}

void Node::UpdateWorldTransform()
{
	if (m_parent == nullptr)
		m_worldTransform = m_localTransform;
	else
		m_worldTransform = m_parent->m_worldTransform * m_localTransform;
}

void Node::UpdateWorldBounds()
{
	m_worldBounds->m_centre = m_worldTransform * m_localBounds->m_centre;
	m_worldBounds->m_radius = m_localBounds->m_radius;

	for (auto child : m_children)
		m_worldBounds->Fit(*child->m_worldBounds);
}

void Node::SetParent(Node* parent)
{
	m_parent = parent;
}

double Node::Loop()
{
	Timer timer;
	timer.Start();
	Update();
	timer.End();
	return timer.GetTimeDiff();
}