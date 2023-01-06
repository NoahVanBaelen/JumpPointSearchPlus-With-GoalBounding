#pragma once
#include <vector>
#include <list>
#include "GraphNode.h"
#include "GraphConnection.h"
#include "GP_JumpPointSearchWithGoalBounding.h"

class JPS;
class GridGraph
{
public:

	struct Wall
	{
		int column; 
		int row;
	};

	std::vector<GraphNode*> NodeVector;
	std::list<GraphConnection*> ConnectionList;
	std::vector <std::list<GraphConnection*>> ConnectionListVector;

	GridGraph(int nrColumns, int nrRows, int cellSize, Engine* enginePtr);
	~GridGraph();

	void DrawGrid();

	GraphNode* GetNode(int index) const;
	GraphNode* GetNode(int col, int row) const;

	const GraphConnection& GetConnection(const GraphNode& node) const;
	const GraphConnection& GetConnection(int idx) const;

	int GetColumns() const;
	int GetRows() const;

	bool isWithinBounds(int col, int row) const;
	int GetIndex(int col, int row) const;

	void AddConnectionsToAdjacentCells(int col, int row);
	void AddConnectionsToAdjacentCells(int idx);

	bool isWallHere(int col, int row);

private:
	int m_NrOfColumns;
	int m_NrOfRows;
	int m_CellSize;

	GraphNode* m_StartNode;
	GraphNode* m_GoalNode;

	JPS* m_pJumpPointSearch;

	Engine* m_MyEnginePtr;
	
	std::vector<GraphNode*> m_Nodes;
	std::vector<GraphConnection> m_Connections;
	std::vector<Wall> m_Walls;
	std::vector<GraphNode*> m_Path;

	void AddConnectionsInDirections(int idx, int col, int row);
	void initializeWalls();
	void DrawWall();
	void DrawBoundingBox();
	void DrawPath();
};

