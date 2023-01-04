#include "GridGraph.h"

GridGraph::GridGraph(int nrColumns, int nrRows, int cellSize, Engine* enginePtr)
	:m_NrOfColumns{nrColumns}
	,m_NrOfRows{nrRows}
	,m_CellSize{cellSize}
	,m_MyEnginePtr{enginePtr}
{}

GridGraph::~GridGraph()
{
	for (GraphNode* node: NodeVector)
	{
		delete node;
		node = nullptr;
	}

	for (std::list<GraphConnection*> list : ConnectionListVector)
	{
		for (GraphConnection* connection : list)
		{
			delete connection;
			connection = nullptr;
		}
	}

	delete m_MyEnginePtr;
	m_MyEnginePtr = nullptr;
}

void GridGraph::DrawGrid()
{
	m_MyEnginePtr->SetColor(1, 1, 1);
	m_MyEnginePtr->DrawRect(0, 0, m_CellSize * m_NrOfColumns, m_CellSize * m_NrOfRows);

	for (int i = 1; i < m_NrOfColumns; i++)
	{
		m_MyEnginePtr->DrawLine(m_CellSize * i, 0, m_CellSize * i, m_CellSize * m_NrOfRows);
	}

	for (int i = 1; i < m_NrOfRows; i++)
	{
		m_MyEnginePtr->DrawLine(0, m_CellSize * i, m_CellSize * m_NrOfColumns, m_CellSize * i);
	}
}

GraphNode* GridGraph::GetNode(int index) const
{
	return m_Nodes[index];
}

GraphNode* GridGraph::GetNode(int col, int row) const
{
	return m_Nodes[GetIndex(col,row)];
}

const GraphConnection& GridGraph::GetConnection(const GraphNode& node) const
{
	return m_Connections[node.GetIndex()];
}

const GraphConnection& GridGraph::GetConnection(int idx) const
{
	return m_Connections[idx];
}

int GridGraph::GetColumns() const
{
	return m_NrOfColumns;
}

int GridGraph::GetRows() const
{
	return m_NrOfRows;
}

bool GridGraph::isWithinBounds(int col, int row) const
{
	return (col >= 0 && col < m_NrOfColumns&& row >= 0 && row < m_NrOfRows);
}

int GridGraph::GetIndex(int col, int row) const
{
	return row * m_NrOfColumns + col;
}

bool GridGraph::isWallHere(int col, int row)
{
	if (isWithinBounds(col,row))
	{
		for (const Wall& wall : m_Walls)
		{
			if (wall.column == col && wall.row == row)
			{
				return true;
			}
		}
	}
	return false;
}

void GridGraph::AddConnectionsToAdjacentCells(int col, int row)
{

}

void GridGraph::AddConnectionsToAdjacentCells(int idx)
{

}

void GridGraph::AddConnectionsInDirections(int idx, int col, int row)
{

}