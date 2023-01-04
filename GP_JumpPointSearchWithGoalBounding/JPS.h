#pragma once
#include <vector>
#include "GridGraph.h"

class GraphNode;
class GraphConnection;

class JPS
{
public:

	struct NodeRecord
	{
		GraphNode* pNode = nullptr;
		GraphConnection* pConnection = nullptr;
		float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
		float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

		bool operator<(const NodeRecord& other) const
		{
			return estimatedTotalCost < other.estimatedTotalCost;
		};
	};

	struct NodeBoundingBox
	{
		GraphNode* pNode = nullptr;
		int startConnection = 0;

	};

	struct BoundingBox
	{
		Vector2 startPosition;
		float width;
		float height;
	};

	JPS(GridGraph* gridGraph);
	~JPS() = default;

	void CreateBoundingBox(GraphNode* pStartNode);
	std::vector<GraphNode*> FindPath(GraphNode* pStartNode, GraphNode* pGoalNode);

private:
	GridGraph* m_Graph;

	std::vector<NodeBoundingBox> m_vectorBoundingBoxes;
	BoundingBox m_NoordBoundingBox;
	BoundingBox m_EastBoundingBox;
	BoundingBox m_WestBoundingBox;
	BoundingBox m_SouthBoundingBox;

	bool HorizontalScan(Vector2 startpos, std::vector<NodeRecord>& openList, std::vector<NodeRecord>& closedList, float cost);
	bool VerticalScan(Vector2 startpos, std::vector<NodeRecord>& openList, std::vector<NodeRecord>& closedList, float cost);
	bool IsAlreadyInBoundingBox(int x, int y);
	void SetBoundingBoxes();
};

