#include "Graph.h"
#include "Edge.h"
#include "GraphNode.h"
#include <list>
#include <stack>
#include <queue>
#include <iostream>

using glm::vec2;
using std::vector;
using std::list;

Graph::Graph()
{
	m_grid = nullptr;
	m_currentNode = nullptr;
	m_gridCols = 0;
	m_gridRows = 0;
}

void Graph::AddNode(GraphNode* node)
{
	m_nodes.push_back(node);
}

void Graph::RemoveNode(GraphNode* node)
{
	for (vector<GraphNode*>::iterator j = m_nodes.begin(); j != m_nodes.end(); ++j)
	{
		for (vector<Edge*>::iterator i = (*j)->GetEdgeList().begin(); i != (*j)->GetEdgeList().end(); ++i)
		{
			if ((*i)->GetStart() == node)
			{
				i = (*j)->GetEdgeList().erase(i);
			}
			if ((*i)->GetStart() == node)
			{
				i = (*j)->GetEdgeList().erase(i);
			}
		}
	}
	for (vector<GraphNode*>::iterator j = m_nodes.begin(); j != m_nodes.end(); ++j)
	{
		if ((*j) == node)
		{
			j = m_nodes.erase(j);
		}
		break;
	}
}

void Graph::ResetVisited()
{
	for (vector<GraphNode*>::iterator i = m_nodes.begin(); i != m_nodes.end(); ++i)
	{
		(*i)->SetIsVisited(false);
	}
}

void Graph::ResetVisitedAlt()
{
	for (vector<GraphNode*>::iterator i = m_nodes.begin(); i != m_nodes.end(); ++i)
	{
		(*i)->SetIsVisited(false);
		(*i)->SetPreviousNode(nullptr);
		(*i)->SetFScore(float(INT_MAX));
		(*i)->SetGScore(float(INT_MAX));
	}
}

bool Graph::SearchDFS(GraphNode* start, GraphNode* end)
{
	ResetVisited();

	std::stack<GraphNode*> nodeStack;
	nodeStack.push(start);

	//keep looping until the stack is empty. 
	//This will only happen once we've checked every node. 
	while (!nodeStack.empty())
	{
		//the rest of the algorithm goes in here
		GraphNode* current = nodeStack.top();
		nodeStack.pop();

		if (!current->GetIsVisited())
		{
			current->SetIsVisited(true);

			if (current == end)
			{
				return true;
			}

			for (int i = 0; i < int(current->GetEdgeList().size()); ++i)
			{
				nodeStack.push(current->GetEdgeList()[i]->GetEnd());
			}
		}
		return false;
	}

	return false;
}

bool Graph::SearchBFS(GraphNode* start, GraphNode* end)
{
	ResetVisited();

	std::queue<GraphNode*> nodeQueue;
	nodeQueue.push(start);

	//keep looping until the stack is empty. 
	//This will only happen once we've checked every node. 
	while (!nodeQueue.empty())
	{
		GraphNode* current = nodeQueue.front();

		if (!current->GetIsVisited())
		{
			current->SetIsVisited(true);

			if (current == end)
			{
				return true;
			}

			for (int i = 0; i < int(current->GetEdgeList().size()); ++i)
			{
				nodeQueue.push(current->GetEdgeList()[i]->GetEnd());
			}
		}

		nodeQueue.pop();

		return false;
	}

	return false;
}

int Graph::NodeCount(GraphNode* firstNode, GraphNode* secondNode)
{
	int count = 0;
	firstNode->GetEdgeList();
	for (vector<Edge*>::iterator i = firstNode->GetEdgeList().begin(); i != firstNode->GetEdgeList().end(); ++i)
	{
		if ((*i)->GetStart() == firstNode)
		{
			if ((*i)->GetEnd() == secondNode)
			{
				count++;
			}
		}
	}
	return count;
}

void Graph::CreateGrid(unsigned int cols, unsigned int rows, float screenWidth, float screenHeight)
{
	GraphVertex* vertices = new GraphVertex[cols * rows];
	GraphNode** gridNodes = new GraphNode*[cols * rows];

	float nodeX = screenWidth / (float)cols;
	float nodeY = screenHeight / (float)rows;

	for (unsigned int c = 0; c < cols; ++c)
	{
		for (unsigned int r = 0; r < rows; ++r)
		{
			vertices[c * rows + r].position = vec2((float)c, (float)r);
		}
	}

	glm::vec2 currentPos;

	for (unsigned int c = 0; c < cols; c++)
	{
		for (unsigned int r = 0; r < rows; r++)
		{
			gridNodes[c * rows + r] = new GraphNode();
			currentPos.x = (c * nodeX) + (nodeX / 2);
			currentPos.y = (r * nodeY) + (nodeY / 2);
			gridNodes[c * rows + r]->SetPos(currentPos);
			gridNodes[c * rows + r]->SetLoc(vec2(c, r));
		}
	}
	for (unsigned int c = 0; c < cols; c++)
	{
		for (unsigned int r = 0; r < rows; r++)
		{
			if (c != 0)
			{
				gridNodes[c * rows + r]->NewEdge(gridNodes[c * rows + r], gridNodes[(c - 1) * rows + r], 1);
			}
			if (c != cols - 1)
			{
				gridNodes[c * rows + r]->NewEdge(gridNodes[c * rows + r], gridNodes[(c + 1) * rows + r], 1);
			}
			if (r != 0)
			{
				gridNodes[c * rows + r]->NewEdge(gridNodes[c * rows + r], gridNodes[c * rows + r - 1], 1);
			}
			if (r != rows - 1)
			{
				gridNodes[c * rows + r]->NewEdge(gridNodes[c * rows + r], gridNodes[c * rows + r + 1], 1);
			}

			AddNode(gridNodes[c * rows + r]);
		}
	}
	m_grid = gridNodes;
	m_gridCols = cols;
	m_gridRows = rows;
}

GraphNode* Graph::GetClosestNode(vec2 position)
{
	GraphNode* closestNode;

	float nodeWidth = (1024.f / m_gridCols);
	float nodeHeight = (720.f / m_gridRows);

	int c = int(position.x / nodeWidth);
	int r = int(position.y / nodeHeight);

	closestNode = m_grid[c * m_gridRows + r];

	return closestNode;
}

float GetHeuristic(GraphNode* currentNode, GraphNode* goalNode)
{
	vec2 one = goalNode->GetPos();
	vec2 two = currentNode->GetPos();
	return glm::length(one - two);
}

bool NodeCompare(GraphNode* left, GraphNode* right)
{
	float leftF = left->GetGScore();
	float rightF = right->GetGScore();
	return(leftF < rightF);
}

void Graph::Dijkstra(GraphNode* end)
{
	ResetVisitedAlt();

	//for (int c = 0; c < 23; c++)
	//{
	//	for (int r = 15; r < 31; r++)
	//	{
	//		m_grid[c * m_gridRows + r]->SetFScore(0);
	//		m_grid[c * m_gridRows + r]->SetGScore(0);
	//	}
	//}
	//for (int c = 42; c < 64; c++)
	//{
	//	for (int r = 15; r < 31; r++)
	//	{
	//		m_grid[c * m_gridRows + r]->SetFScore(0);
	//		m_grid[c * m_gridRows + r]->SetGScore(0);
	//	}
	//}

	m_currentNode->SetGScore(0);

	list<GraphNode*> nodeList;
	nodeList.push_back(m_currentNode);

	while (!nodeList.empty())
	{
		m_currentNode->goalNode = end;
		nodeList.sort(NodeCompare);
		m_currentNode = nodeList.front();
		nodeList.pop_front();
		m_currentNode->SetIsVisited(true);
		vector<Edge*> vectorOfEdgePointers = m_currentNode->GetEdgeList();

		for (vector<Edge*>::iterator i = vectorOfEdgePointers.begin(); i != vectorOfEdgePointers.end(); ++i)
		{
			if (!(*i)->GetEnd()->GetIsVisited())
			{
				if ((m_currentNode->GetGScore() + (*i)->GetCost()) < (*i)->GetEnd()->GetGScore())
				{
					(*i)->GetEnd()->SetPreviousNode(m_currentNode);
					(*i)->GetEnd()->SetGScore(m_currentNode->GetGScore() + (*i)->GetCost());
					bool isInList = false;
					for (list<GraphNode*>::iterator j = nodeList.begin(); j != nodeList.end(); ++j)
					{
						if ((*j) == (*i)->GetEnd())
						{
							isInList = true;
							break;
						}
					}
					if (!isInList)
					{
						nodeList.push_back((*i)->GetEnd());
					}
				}
			}
		}
	}
}

void Graph::Ax(GraphNode* end)
{
	ResetVisitedAlt();

	//for (int c = 0; c < 23; c++)
	//{
	//	for (int r = 15; r < 31; r++)
	//	{
	//		m_grid[c * 64 + r]->SetFScore(0);
	//		m_grid[c * 64 + r]->SetGScore(0);
	//	}
	//}
	//for (int c = 42; c < 64; c++)
	//{
	//	for (int r = 15; r < 31; r++)
	//	{
	//		m_grid[c * 64 + r]->SetFScore(0);
	//		m_grid[c * 64 + r]->SetGScore(0);
	//	}
	//}

	m_currentNode->SetGScore(0);

	list<GraphNode*> nodeList;
	nodeList.push_back(m_currentNode);

	while (!nodeList.empty())
	{
		m_currentNode->goalNode = end;
		nodeList.sort(NodeCompare);
		m_currentNode = nodeList.front();
		nodeList.pop_front();
		m_currentNode->SetIsVisited(true);
		vector<Edge*> vectorOfEdgePointers = m_currentNode->GetEdgeList();

		for (vector<Edge*>::iterator i = vectorOfEdgePointers.begin(); i != vectorOfEdgePointers.end(); ++i)
		{
			if (!(*i)->GetEnd()->GetIsVisited())
			{
				if ((m_currentNode->GetGScore() + (*i)->GetCost() + GetHeuristic(m_currentNode, end)) < (*i)->GetEnd()->GetFScore())
				{
					(*i)->GetEnd()->SetPreviousNode(m_currentNode);
					(*i)->GetEnd()->SetGScore(m_currentNode->GetGScore() + (*i)->GetCost());
					(*i)->GetEnd()->SetFScore(m_currentNode->GetGScore() + (*i)->GetCost() + GetHeuristic(m_currentNode, end));
					bool isInList = false;
					for (list<GraphNode*>::iterator j = nodeList.begin(); j != nodeList.end(); ++j)
					{
						if ((*j) == (*i)->GetEnd())
						{
							isInList = true;
							break;
						}
					}
					if (!isInList)
					{
						nodeList.push_back((*i)->GetEnd());
					}
				}
			}
		}
	}
}

vector<GraphNode*> Graph::BuildPath(GraphNode* goal)
{
	vector<GraphNode*> path;
	if (goal == nullptr)
	{
		return path;
	}
	GraphNode* theCurrentNode = goal;
	path.push_back(theCurrentNode);
	while (theCurrentNode->GetPreviousNode() != nullptr)
	{
		theCurrentNode = theCurrentNode->GetPreviousNode();
		path.push_back(theCurrentNode);
	}
	std::reverse(path.begin(), path.end());
	return path;
}

void Graph::SetCurrentNode(GraphNode* newCurrentNode)
{
	m_currentNode = newCurrentNode;
}

GraphNode* Graph::GetCurrentNode()
{
	return m_currentNode;
}

GraphNode* Graph::GetNode(int col, int row)
{
	return m_grid[col * m_gridCols + row];
}

GraphNode** Graph::GetGrid()
{
	return m_grid;
}

GraphNode** Graph::GetRow(int begin, int end, int col)
{
	int length = end - begin + 1;
	GraphNode** row = new GraphNode*[length];
	int j = begin;
	for (int i = 0; i < length; ++i)
	{
		row[i] = GetNode(j, col);
		j++;
	}
	return row;
}

GraphNode** Graph::GetCol(int begin, int end, int row)
{
	int length = end - begin;
	GraphNode** col = new GraphNode*[length];
	for (int i = 0; i < length; ++i)
		col[i] = GetNode(row, i);
	return col;
}

vec2 Graph::GetGridDimentions()
{
	return vec2(float(m_gridCols), float(m_gridRows));
}

void Graph::SetNodeVector(vector<GraphNode*> path)
{
	m_nodes = path;
}

vector<GraphNode*> Graph::GetNodeVector()
{
	return m_nodes;
}

Graph::~Graph()
{
	//delete m_currentNode;
	delete[] m_grid;
}