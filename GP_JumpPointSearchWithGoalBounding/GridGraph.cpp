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

	m_StartNode = m_Nodes[54];
	m_GoalNode = m_Nodes[70];
	initializeWalls();

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
	DrawBoundingBox();
	DrawPath();

	m_MyEnginePtr->SetColor(0, 255, 0);
	m_MyEnginePtr->FillEllipse(m_StartNode->GetPosition().x, m_StartNode->GetPosition().y, 20, 20);

	m_MyEnginePtr->SetColor(255, 0, 0);
	m_MyEnginePtr->FillEllipse(m_GoalNode->GetPosition().x, m_GoalNode->GetPosition().y, 20, 20);
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
	m_MyEnginePtr->SetColor(100, 100, 255);

	std::vector<GraphNode*> boundingBox{ m_pJumpPointSearch->GetUsedBoundingBox(m_GoalNode) };

	for (const GraphNode* boundingNode : boundingBox)
	{
		m_MyEnginePtr->FillRect(boundingNode->GetColumn() * m_CellSize, boundingNode->GetRow() * m_CellSize, m_CellSize, m_CellSize);
	}

	m_MyEnginePtr->FillRect(m_StartNode->GetColumn() * m_CellSize, m_StartNode->GetRow() * m_CellSize, m_CellSize, m_CellSize);
}

void GridGraph::DrawPath()
{
	m_MyEnginePtr->SetColor(255, 255, 100);
	for (const GraphNode* node : m_Path)
	{
		m_MyEnginePtr->FillRect(node->GetColumn() * m_CellSize, node->GetRow() * m_CellSize, m_CellSize, m_CellSize);
	}
	m_MyEnginePtr->SetColor(0, 0, 0);
	for (size_t i = 0; i < m_Path.size() - 1; i++)
	{
		m_MyEnginePtr->DrawLine(m_Path[i]->GetColumn() * m_CellSize + m_CellSize / 2.f,
								m_Path[i]->GetRow() * m_CellSize + m_CellSize / 2.f,
								m_Path[i+1]->GetColumn() * m_CellSize + m_CellSize / 2.f,
								m_Path[i+1]->GetRow() * m_CellSize + m_CellSize / 2.f);
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
	if (isWithinBounds(col,row))
	{
		return row * m_NrOfColumns + col;
	}
	return NULL;
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

void GridGraph::initializeWalls()
{
	m_Walls.push_back(Wall{ 0,2 });
	m_Walls.push_back(Wall{ 1,2 });
	m_Walls.push_back(Wall{ 2,2 });


	m_Walls.push_back(Wall{ 1,5 });
	m_Walls.push_back(Wall{ 1,6 });
	m_Walls.push_back(Wall{ 1,7 });

	m_Walls.push_back(Wall{ 3,4 });
	m_Walls.push_back(Wall{ 3,6 });

	m_Walls.push_back(Wall{ 5,0 });
	m_Walls.push_back(Wall{ 5,1 });
	m_Walls.push_back(Wall{ 5,3 });
	m_Walls.push_back(Wall{ 5,4 });
	m_Walls.push_back(Wall{ 5,5 });
	m_Walls.push_back(Wall{ 5,6 });

	m_Walls.push_back(Wall{ 8,0 });
	m_Walls.push_back(Wall{ 8,1 });

	m_Walls.push_back(Wall{ 7,8 });
	m_Walls.push_back(Wall{ 9,8 });
}