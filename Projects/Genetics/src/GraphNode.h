#ifndef _GRAPHNODE_H_
#define _GRAPHNODE_H_

#include <string>
#include <vector>
#include <glm/glm.hpp>

class Edge;

class GraphNode
{
public:
	GraphNode();
	GraphNode(glm::vec2 pos, glm::ivec2 loc);
	~GraphNode();

	void SetPos(glm::vec2 pos);
	void SetLoc(glm::ivec2 loc);
	void NewEdge(GraphNode* start, GraphNode* end, float cost);
	void SetIsVisited(bool visited);
	void SetPreviousNode(GraphNode* previousNode);
	void SetFScore(float fScore);
	void SetGScore(float gScore);
	void SetHScore(float hScore);

	glm::vec2 GetPos();
	glm::ivec2 GetLoc();
	
	std::vector<Edge*> GetEdgeList();

	bool GetIsVisited();

	GraphNode* GetPreviousNode();

	float GetFScore();
	float GetGScore();
	float GetHScore();

	GraphNode* goalNode;

private:
	glm::vec2 m_nodePosition;
	glm::ivec2 m_nodeLocation;

	std::vector<Edge*> m_edges;

	bool m_visited;

	GraphNode* m_previousNode;

	float m_fScore;
	float m_gScore;
	float m_hScore;
};

#endif