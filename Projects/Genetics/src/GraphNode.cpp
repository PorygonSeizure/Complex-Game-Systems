#include "GraphNode.h"
#include "Edge.h"

using glm::vec2;
using glm::ivec2;

GraphNode::GraphNode()
{
	m_nodePosition = vec2(0);
	m_nodeLocation = ivec2(0);
	m_previousNode = nullptr;
	m_fScore = 0;
	m_gScore = 0;
	m_hScore = 0;
}

GraphNode::GraphNode(vec2 pos, ivec2 loc)
{
	m_nodePosition = pos;
	m_nodeLocation = loc;
	m_previousNode = nullptr;
	m_fScore = 0;
	m_gScore = 0;
	m_hScore = 0;
}

void GraphNode::SetPos(vec2 pos)
{
	m_nodePosition = pos;
}

void GraphNode::SetLoc(ivec2 loc)
{
	m_nodeLocation = loc;
}

vec2 GraphNode::GetPos()
{
	return m_nodePosition;
}

ivec2 GraphNode::GetLoc()
{
	return m_nodeLocation;
}

void GraphNode::NewEdge(GraphNode* start, GraphNode* end, float cost)
{
	Edge* edge = new Edge;
	edge->SetStart(start);
	edge->SetEnd(end);
	edge->SetCost(cost);
	m_edges.push_back(edge);
}

std::vector<Edge*> GraphNode::GetEdgeList()
{
	return m_edges;
}

void GraphNode::SetIsVisited(bool visited)
{
	m_visited = visited;
}

bool GraphNode::GetIsVisited()
{
	return m_visited;
}

void GraphNode::SetPreviousNode(GraphNode* previousNode)
{
	m_previousNode = previousNode;
}

GraphNode* GraphNode::GetPreviousNode()
{
	return m_previousNode;
}

void GraphNode::SetFScore(float fScore)
{
	m_fScore = fScore;
}

float GraphNode::GetFScore()
{
	return m_fScore;
}

void GraphNode::SetGScore(float gScore)
{
	m_gScore = gScore;
}

float GraphNode::GetGScore()
{
	return m_gScore;
}

void GraphNode::SetHScore(float hScore)
{
	m_hScore = hScore;
}

float GraphNode::GetHScore()
{
	return m_hScore;
}

GraphNode::~GraphNode()
{

}