#include "GridGraph.h"
#include "JPS.h"

GridGraph::GridGraph(int nrColumns, int nrRows, int cellSize, Engine* enginePtr)
	:m_NrOfColumns{nrColumns}
	,m_NrOfRows{nrRows}
	,m_CellSize{cellSize}
	,m_pMyEngine{enginePtr}
{
	//make nodes
	for (int row = 0; row < m_NrOfRows; row++)
	{
		for (int column = 0; column < m_NrOfColumns; column++)
		{
			m_Nodes.push_back(new GraphNode{ column + m_NrOfColumns * row,
											Vector2{static_cast<float>(cellSize / 2.f + cellSize * column),static_cast<float>(cellSize / 2.f + cellSize * row)},
											row,column});
		}
	}

	//default start and and goal nodes
	m_StartNode = m_Nodes[53];
	m_GoalNode = m_Nodes[58];
	initializeWalls();
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

	delete m_pMyEngine;
	m_pMyEngine = nullptr;
}

void GridGraph::DrawGrid()
{
	//Draw Grid
	m_pMyEngine->SetColor(0, 0, 0);
	m_pMyEngine->DrawRect(0, 0, m_CellSize * m_NrOfColumns, m_CellSize * m_NrOfRows);

	m_pMyEngine->SetColor(0, 0, 0);
	for (int i = 1; i < m_NrOfColumns; i++)
	{
		m_pMyEngine->DrawLine(m_CellSize * i, 0, m_CellSize * i, m_CellSize * m_NrOfRows);
	}

	for (int i = 1; i < m_NrOfRows; i++)
	{
		m_pMyEngine->DrawLine(0, m_CellSize * i, m_CellSize * m_NrOfColumns, m_CellSize * i);
	}

	DrawWall();
	if (m_CalculatePath)
	{
		DrawBoundingBox();
		DrawPath();
	}

	//DrawStartNode
	m_pMyEngine->SetColor(0, 255, 0);
	m_pMyEngine->FillEllipse(static_cast<int>(m_StartNode->GetPosition().x), static_cast<int>(m_StartNode->GetPosition().y), 20, 20);

	//DrawGoalNode
	m_pMyEngine->SetColor(255, 0, 0);
	m_pMyEngine->FillEllipse(static_cast<int>(m_GoalNode->GetPosition().x), static_cast<int>(m_GoalNode->GetPosition().y), 20, 20);
}

void GridGraph::DrawWall()
{
	m_pMyEngine->SetColor(0, 0, 0);

	for (const Wall& wall : m_Walls)
	{
		m_pMyEngine->FillRect(wall.column * m_CellSize, wall.row * m_CellSize, m_CellSize, m_CellSize);
	}
}

void GridGraph::DrawBoundingBox()
{
	m_pMyEngine->SetColor(100, 100, 255);

	std::vector<GraphNode*> boundingBox{ m_pJumpPointSearch->GetUsedBoundingBox(m_GoalNode) };

	for (const GraphNode* boundingNode : boundingBox)
	{
		m_pMyEngine->FillRect(boundingNode->GetColumn() * m_CellSize, boundingNode->GetRow() * m_CellSize, m_CellSize, m_CellSize);
	}

	m_pMyEngine->FillRect(m_StartNode->GetColumn() * m_CellSize, m_StartNode->GetRow() * m_CellSize, m_CellSize, m_CellSize);
}

void GridGraph::DrawPath()
{
	m_pMyEngine->SetColor(255, 255, 100);
	for (const GraphNode* node : m_Path)
	{
		m_pMyEngine->FillRect(node->GetColumn() * m_CellSize, node->GetRow() * m_CellSize, m_CellSize, m_CellSize);
	}
	m_pMyEngine->SetColor(0, 0, 0);
	for (size_t i = 0; i < m_Path.size() - 1; i++)
	{
		m_pMyEngine->DrawLine(m_Path[i]->GetColumn() * m_CellSize + m_CellSize / 2,
								m_Path[i]->GetRow() * m_CellSize + m_CellSize / 2,
								m_Path[i+1]->GetColumn() * m_CellSize + m_CellSize / 2,
								m_Path[i+1]->GetRow() * m_CellSize + m_CellSize / 2);
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

void GridGraph::SetStartAndGoalNode(int x, int y, bool isUp)
{
	if (isUp && !m_CalculatePath)
	{
		if (m_CanSetStart)
		{
			for (int row = 0; row < m_NrOfRows; row++)
			{
				for (int column = 0; column < m_NrOfColumns; column++)
				{
					if (x > m_CellSize * column && x < m_CellSize * (column + 1) &&
						y > m_CellSize * row && y < m_CellSize * (row + 1))
					{
						if (!isWallHere(column,row))
						{
							m_StartNode = m_Nodes[GetIndex(column,row)];
							break;
						}
					}
				}
			}
		}
		if (m_CanSetGoal)
		{
			for (int row = 0; row < m_NrOfRows; row++)
			{
				for (int column = 0; column < m_NrOfColumns; column++)
				{
					if (x > m_CellSize * column && x < m_CellSize * (column + 1) &&
						y > m_CellSize * row && y < m_CellSize * (row + 1))
					{
						if (!isWallHere(column, row))
						{
							m_GoalNode = m_Nodes[GetIndex(column, row)];
							break;
						}
					}
				}
			}
		}
	}
}

void GridGraph::toggleStart()
{
	m_CanSetStart = true;
	m_CanSetGoal = false;
}

void GridGraph::toggleGoal()
{
	m_CanSetStart = false;
	m_CanSetGoal = true;
}

void GridGraph::CalculatePath()
{
	//Create boundingboxes and create jump point search path
	m_CanSetStart = false;
	m_CanSetGoal = false;
	m_CalculatePath = true;

	m_pJumpPointSearch = new JPS{ this };
	m_pJumpPointSearch->CreateBoundingBox(m_StartNode);
	m_Path = m_pJumpPointSearch->FindPath(m_StartNode, m_GoalNode);
}