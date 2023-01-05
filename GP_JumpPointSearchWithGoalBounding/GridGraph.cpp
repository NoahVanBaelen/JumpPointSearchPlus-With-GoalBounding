#include "GridGraph.h"
#include "JPS.h"

GridGraph::GridGraph(int nrColumns, int nrRows, int cellSize, Engine* enginePtr)
	:m_NrOfColumns{nrColumns}
	,m_NrOfRows{nrRows}
	,m_CellSize{cellSize}
	,m_MyEnginePtr{enginePtr}
{
	for (int row = 0; row < m_NrOfRows; row++)
	{
		for (int column = 0; column < m_NrOfColumns; column++)
		{
			m_Nodes.push_back(new GraphNode{ column + m_NrOfColumns * row,
											Vector2{static_cast<float>(cellSize / 2.f + cellSize * column),static_cast<float>(cellSize / 2.f + cellSize * row)},
											row,column });
		}
	}

	m_Walls.push_back(Wall{ 3,4 });
	m_Walls.push_back(Wall{ 5,4 });

	m_Walls.push_back(Wall{ 3,6 });
	m_Walls.push_back(Wall{ 5,6 });

	m_StartNode = m_Nodes[54];
	m_GoalNode = m_Nodes[12];

	m_pJumpPointSearch = new JPS{ this };
	m_pJumpPointSearch->CreateBoundingBox(m_StartNode);
	m_Path = m_pJumpPointSearch->FindPath(m_StartNode, m_GoalNode);
}

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
	m_MyEnginePtr->SetColor(0, 0, 0);
	m_MyEnginePtr->DrawRect(0, 0, m_CellSize * m_NrOfColumns, m_CellSize * m_NrOfRows);

	m_MyEnginePtr->SetColor(255, 255, 0);
	m_MyEnginePtr->FillEllipse(m_StartNode->GetPosition().x, m_StartNode->GetPosition().y, 20, 20);

	m_MyEnginePtr->SetColor(0, 255, 255);
	m_MyEnginePtr->FillEllipse(m_GoalNode->GetPosition().x, m_GoalNode->GetPosition().y, 20, 20);

	m_MyEnginePtr->SetColor(0, 0, 0);
	for (int i = 1; i < m_NrOfColumns; i++)
	{
		m_MyEnginePtr->DrawLine(m_CellSize * i, 0, m_CellSize * i, m_CellSize * m_NrOfRows);
	}

	for (int i = 1; i < m_NrOfRows; i++)
	{
		m_MyEnginePtr->DrawLine(0, m_CellSize * i, m_CellSize * m_NrOfColumns, m_CellSize * i);
	}

	DrawWall();
}

void GridGraph::DrawWall()
{
	m_MyEnginePtr->SetColor(0, 0, 0);

	for (const Wall& wall : m_Walls)
	{
		m_MyEnginePtr->FillRect(wall.column * m_CellSize, wall.row * m_CellSize, m_CellSize, m_CellSize);
	}
}

void GridGraph::DrawBoundingBox()
{
	m_MyEnginePtr->SetColor(0, 0, 255);
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