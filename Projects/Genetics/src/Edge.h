#ifndef _EDGE_H_
#define _EDGE_H_

class GraphNode;

class Edge
{
public:
	Edge() : m_start(nullptr), m_end(nullptr), m_cost(0) {}
	~Edge() {}

	void SetStart(GraphNode* start) { m_start = start; }
	void SetEnd(GraphNode* end) { m_end = end; }
	void SetCost(float cost) { m_cost = cost; }

	GraphNode* GetStart() { return m_start; }
	GraphNode* GetEnd() { return m_end; }
	
	float GetCost() { return m_cost; }
	
private:
	GraphNode* m_start;
	GraphNode* m_end;

	float m_cost;
};

#endif