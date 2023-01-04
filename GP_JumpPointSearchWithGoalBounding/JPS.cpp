#include "JPS.h"
#include "GraphNode.h"
#include "GraphConnection.h"
#include <algorithm>
#include <queue>

JPS::JPS(GridGraph* gridGraph)
	:m_Graph{gridGraph}
{}

void JPS::CreateBoundingBox(GraphNode* pStartNode)
{
	std::queue<NodeBoundingBox> boundingQueue;
	int x{ pStartNode->GetColumn() };
	int y{ pStartNode->GetRow() };

	//Setup the 4  node around the startNode
	//Noord
	if (m_Graph->isWithinBounds(x, y + 1) && !m_Graph->isWallHere(x, y + 1))
	{
		boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x, y + 1),1 });
	}

	//East
	if (m_Graph->isWithinBounds(x + 1, y) && !m_Graph->isWallHere(x + 1, y))
	{
		boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x + 1, y),2 });
	}

	//South
	if (m_Graph->isWithinBounds(x, y - 1) && !m_Graph->isWallHere(x, y - 1))
	{
		boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x, y - 1),3 });
	}

	//West
	if (m_Graph->isWithinBounds(x - 1, y) && !m_Graph->isWallHere(x - 1, y))
	{
		boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x - 1, y),4 });
	}

	while (!boundingQueue.empty())
	{
		NodeBoundingBox currentNode{ boundingQueue.front()};
		boundingQueue.pop();

		x = currentNode.pNode->GetColumn();
		y = currentNode.pNode->GetRow();
		int startConnection{ currentNode.startConnection };

		if (!IsAlreadyInBoundingBox(x,y))
		{
			m_vectorBoundingBoxes.push_back(NodeBoundingBox{ m_Graph->GetNode(x, y),startConnection });

			//Noord
			if (m_Graph->isWithinBounds(x, y + 1) && !m_Graph->isWallHere(x, y + 1))
			{
				boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x, y + 1),startConnection });
			}

			//East
			if (m_Graph->isWithinBounds(x + 1, y) && !m_Graph->isWallHere(x + 1, y))
			{
				boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x + 1, y),startConnection });
			}

			//South
			if (m_Graph->isWithinBounds(x, y - 1) && !m_Graph->isWallHere(x, y - 1))
			{
				boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x, y - 1),startConnection });
			}

			//West
			if (m_Graph->isWithinBounds(x - 1, y) && !m_Graph->isWallHere(x - 1, y))
			{
				boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x - 1, y),startConnection });
			}
		}
	}

	SetBoundingBoxes();
}

std::vector<GraphNode*> JPS::FindPath(GraphNode* pStartNode, GraphNode* pGoalNode)
{
	std::vector<GraphNode*> path;
	std::vector<NodeRecord> openList;
	std::vector<NodeRecord> closedList;

	NodeRecord currentRecord{};
	NodeRecord startRecord{};

	startRecord.pNode = pStartNode;
	startRecord.pConnection = nullptr;
	startRecord.costSoFar = 0;
	startRecord.estimatedTotalCost = 0;

	openList.push_back(startRecord);
	bool goToNextConnection = false;

	Vector2 startpos{startRecord.pNode->GetColumn(), startRecord.pNode->GetRow()};

	while (!openList.empty())
	{
		currentRecord = *std::min_element(openList.begin(), openList.end());
		if (currentRecord.pNode == pGoalNode)
		{
			break;
		}

		currentRecord = openList.back();
		Vector2 startpos{ currentRecord.pNode->GetColumn(), currentRecord.pNode->GetRow() };
		Vector2 currentpos{ startpos };
		bool doneWithMoving = false;

		HorizontalScan(startpos, openList, closedList, startRecord.estimatedTotalCost);
		VerticalScan(startpos, openList, closedList, startRecord.estimatedTotalCost);

		//diagonaal UpRight

		while (!doneWithMoving)
		{
			++currentpos.x;
			++currentpos.y;
			startRecord.costSoFar += 1.5f;

			if (m_Graph->isWallHere(currentpos.x, currentpos.y + 1) &&
				m_Graph->isWallHere(currentpos.x + 1, currentpos.y))
			{
				doneWithMoving = true;
			}
			else
			{
				if (m_Graph->isWallHere(currentpos.x + 1, currentpos.y) ||
					m_Graph->isWallHere(currentpos.x - 1, currentpos.y))
				{
					NodeRecord newRecord{};
					newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
					newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });

					closedList.push_back(newRecord);
					openList.push_back(newRecord);
					doneWithMoving = true;
				}
				else
				{
					if (HorizontalScan(currentpos, openList, closedList, startRecord.estimatedTotalCost + startRecord.costSoFar))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });

						closedList.push_back(newRecord);
						openList.push_back(newRecord);
						doneWithMoving = true;
					}
					else
					{
						if (VerticalScan(currentpos, openList, closedList, startRecord.estimatedTotalCost + startRecord.costSoFar))
						{
							NodeRecord newRecord{};
							newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
							newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });

							closedList.push_back(newRecord);
							openList.push_back(newRecord);
							doneWithMoving = true;
						}
					}
				}
			}
		}

		currentpos = startpos;
		doneWithMoving = false;
		startRecord.costSoFar = 0;

		//diagonaal UpLeft

		while (!doneWithMoving)
		{
			--currentpos.x;
			++currentpos.y;
			startRecord.costSoFar += 1.5f;

			if (m_Graph->isWallHere(currentpos.x, currentpos.y + 1) &&
				m_Graph->isWallHere(currentpos.x - 1, currentpos.y))
			{
				doneWithMoving = true;
			}
			else
			{
				if (m_Graph->isWallHere(currentpos.x + 1, currentpos.y) ||
					m_Graph->isWallHere(currentpos.x - 1, currentpos.y))
				{
					NodeRecord newRecord{};
					newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
					newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });

					closedList.push_back(newRecord);
					openList.push_back(newRecord);
					doneWithMoving = true;
				}
				else
				{
					if (HorizontalScan(currentpos, openList, closedList, startRecord.estimatedTotalCost + startRecord.costSoFar))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });

						closedList.push_back(newRecord);
						openList.push_back(newRecord);
						doneWithMoving = true;
					}
					else
					{
						if (VerticalScan(currentpos, openList, closedList, startRecord.estimatedTotalCost + startRecord.costSoFar))
						{
							NodeRecord newRecord{};
							newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
							newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });

							closedList.push_back(newRecord);
							openList.push_back(newRecord);
							doneWithMoving = true;
						}
					}
				}
			}
		}

		currentpos = startpos;
		doneWithMoving = false;
		startRecord.costSoFar = 0;

		//diagonaal DownRight

		while (!doneWithMoving)
		{
			++currentpos.x;
			--currentpos.y;
			startRecord.costSoFar += 1.5f;

			if (m_Graph->isWallHere(currentpos.x, currentpos.y - 1) &&
				m_Graph->isWallHere(currentpos.x + 1, currentpos.y))
			{
				doneWithMoving = true;
			}
			else
			{
				if (m_Graph->isWallHere(currentpos.x + 1, currentpos.y) ||
					m_Graph->isWallHere(currentpos.x - 1, currentpos.y))
				{
					NodeRecord newRecord{};
					newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
					newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });

					closedList.push_back(newRecord);
					openList.push_back(newRecord);
					doneWithMoving = true;
				}
				else
				{
					if (HorizontalScan(currentpos, openList, closedList, startRecord.estimatedTotalCost + startRecord.costSoFar))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });

						closedList.push_back(newRecord);
						openList.push_back(newRecord);
						doneWithMoving = true;
					}
					else
					{
						if (VerticalScan(currentpos, openList, closedList, startRecord.estimatedTotalCost + startRecord.costSoFar))
						{
							NodeRecord newRecord{};
							newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
							newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });

							closedList.push_back(newRecord);
							openList.push_back(newRecord);
							doneWithMoving = true;
						}
					}
				}
			}
		}

		currentpos = startpos;
		doneWithMoving = false;
		startRecord.costSoFar = 0;

		//diagonaal DownLeft

		while (!doneWithMoving)
		{
			--currentpos.x;
			--currentpos.y;
			startRecord.costSoFar += 1.5f;

			if (m_Graph->isWallHere(currentpos.x, currentpos.y - 1) &&
				m_Graph->isWallHere(currentpos.x - 1, currentpos.y))
			{
				doneWithMoving = true;
			}
			else
			{
				if (m_Graph->isWallHere(currentpos.x + 1, currentpos.y) ||
					m_Graph->isWallHere(currentpos.x - 1, currentpos.y))
				{
					NodeRecord newRecord{};
					newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
					newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });

					closedList.push_back(newRecord);
					openList.push_back(newRecord);
					doneWithMoving = true;
				}
				else
				{
					if (HorizontalScan(currentpos, openList, closedList, startRecord.estimatedTotalCost + startRecord.costSoFar))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });

						closedList.push_back(newRecord);
						openList.push_back(newRecord);
						doneWithMoving = true;
					}
					else
					{
						if (VerticalScan(currentpos, openList, closedList, startRecord.estimatedTotalCost + startRecord.costSoFar))
						{
							NodeRecord newRecord{};
							newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
							newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });

							closedList.push_back(newRecord);
							openList.push_back(newRecord);
							doneWithMoving = true;
						}
					}
				}
			}
		}

		currentpos = startpos;
		doneWithMoving = false;
		startRecord.costSoFar = 0;
	}

	//Create Path
	while (currentRecord.pNode != pStartNode)
	{
		goToNextConnection = false;
		path.push_back(currentRecord.pNode);
		for (NodeRecord foundRecord : closedList)
		{
			if (foundRecord.pNode->GetIndex() == currentRecord.pConnection->GetFrom())
			{
				currentRecord = foundRecord;
				goToNextConnection = true;
			}
			if (goToNextConnection)
			{
				break;

			}
		}
	}

	path.push_back(currentRecord.pNode);
	std::reverse(path.begin(), path.end());

	return path;
}

bool JPS::HorizontalScan(Vector2 startpos, std::vector<NodeRecord>& openList, std::vector<NodeRecord>& closedList, float cost)
{
	bool doneWithMoving = false;
	Vector2 currentpos{ startpos };
	NodeRecord newRecord{};

	//positive horizontal
	while (!doneWithMoving)
	{
		++currentpos.x;
		++newRecord.costSoFar;
		if (m_Graph->isWithinBounds(currentpos.x, currentpos.y))
		{
			GraphNode* nextNode{ m_Graph->GetNode(currentpos.x,currentpos.y) };
			if (m_Graph->isWallHere(currentpos.x, currentpos.y + 1) ||
				m_Graph->isWallHere(currentpos.x, currentpos.y - 1))
			{
				newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
				newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
				newRecord.estimatedTotalCost = cost + newRecord.costSoFar;

				closedList.push_back(newRecord);
				openList.push_back(newRecord);
				doneWithMoving = true;
				return true;
			}
			if (m_Graph->isWallHere(currentpos.x + 1, currentpos.y))
			{
				doneWithMoving = true;
			}
		}
		else
		{
			doneWithMoving = true;
		}
	}

	currentpos = startpos;
	doneWithMoving = false;
	newRecord.costSoFar = 0;

	//negative horizontal
	while (!doneWithMoving)
	{
		--currentpos.x;
		++newRecord.costSoFar;
		if (m_Graph->isWithinBounds(currentpos.x, currentpos.y))
		{
			GraphNode* nextNode{ m_Graph->GetNode(currentpos.x,currentpos.y) };
			if (m_Graph->isWallHere(currentpos.x, currentpos.y + 1) ||
				m_Graph->isWallHere(currentpos.x, currentpos.y - 1))
			{
				newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
				newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
				newRecord.estimatedTotalCost = cost + newRecord.costSoFar;

				closedList.push_back(newRecord);
				openList.push_back(newRecord);
				doneWithMoving = true;
				return true;
			}
			if (m_Graph->isWallHere(currentpos.x - 1, currentpos.y))
			{
				doneWithMoving = true;
			}
		}
		else
		{
			doneWithMoving = true;
		}
	}
	return false;
}

bool JPS::VerticalScan(Vector2 startpos, std::vector<NodeRecord>& openList, std::vector<NodeRecord>& closedList, float cost)
{
	bool doneWithMoving = false;
	Vector2 currentpos{ startpos };
	NodeRecord newRecord{};

	//positive vertical
	while (!doneWithMoving)
	{
		++currentpos.y;
		++newRecord.costSoFar;
		if (m_Graph->isWithinBounds(currentpos.x, currentpos.y))
		{
			GraphNode* nextNode{ m_Graph->GetNode(currentpos.x,currentpos.y) };
			if (m_Graph->isWallHere(currentpos.x - 1, currentpos.y) ||
				m_Graph->isWallHere(currentpos.x + 1, currentpos.y))
			{
				newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
				newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
				newRecord.estimatedTotalCost = cost + newRecord.costSoFar;

				closedList.push_back(newRecord);
				doneWithMoving = true;
				return true;
			}
			if (m_Graph->isWallHere(currentpos.x, currentpos.y + 1))
			{
				doneWithMoving = true;
			}
		}
		else
		{
			doneWithMoving = true;
		}
	}

	currentpos = startpos;
	doneWithMoving = false;
	newRecord.costSoFar = 0;

	//negative vertical
	while (!doneWithMoving)
	{
		--currentpos.y;
		++newRecord.costSoFar;
		if (m_Graph->isWithinBounds(currentpos.x, currentpos.y))
		{
			GraphNode* nextNode{ m_Graph->GetNode(currentpos.x,currentpos.y) };
			if (m_Graph->isWallHere(currentpos.x - 1, currentpos.y) ||
				m_Graph->isWallHere(currentpos.x + 1, currentpos.y))
			{
				newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
				newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
				newRecord.estimatedTotalCost = cost + newRecord.costSoFar;

				closedList.push_back(newRecord);
				doneWithMoving = true;
				return true;
			}
			if (m_Graph->isWallHere(currentpos.x, currentpos.y - 1))
			{
				doneWithMoving = true;
			}
		}
		else
		{
			doneWithMoving = true;
		}
	}

	return false;
}

bool JPS::IsAlreadyInBoundingBox(int x, int y)
{
	for (const NodeBoundingBox& node : m_vectorBoundingBoxes)
	{
		if (x == node.pNode->GetColumn() && y == node.pNode->GetRow())
		{
			return true;
		}
	}

	return false;
}

void JPS::SetBoundingBoxes()
{
	std::vector<float> m_NorthXPos;
	std::vector<float> m_NorthYPos;

	std::vector<float> m_EastXPos;
	std::vector<float> m_EastYPos;

	std::vector<float> m_SouthXPos;
	std::vector<float> m_SouthYPos;

	std::vector<float> m_WestXPos;
	std::vector<float> m_WestYPos;

	for (const NodeBoundingBox& node : m_vectorBoundingBoxes)
	{
		//Noord
		if (node.startConnection == 1)
		{
			m_NorthXPos.push_back(node.pNode->GetPosition().x);
			m_NorthYPos.push_back(node.pNode->GetPosition().y);
		}

		//East
		if (node.startConnection == 2)
		{
			m_EastXPos.push_back(node.pNode->GetPosition().x);
			m_EastYPos.push_back(node.pNode->GetPosition().y);
		}

		//South
		if (node.startConnection == 3)
		{
			m_SouthXPos.push_back(node.pNode->GetPosition().x);
			m_SouthYPos.push_back(node.pNode->GetPosition().y);
		}

		//West
		if (node.startConnection == 4)
		{
			m_WestXPos.push_back(node.pNode->GetPosition().x);
			m_WestYPos.push_back(node.pNode->GetPosition().y);
		}
	}

	//North
	std::vector<float>::iterator result = std::min_element(m_NorthXPos.begin(), m_NorthXPos.end());
	m_NoordBoundingBox.startPosition.x = std::distance(m_NorthXPos.begin(), result);
	result = std::min_element(m_NorthYPos.begin(), m_NorthYPos.end());
	m_NoordBoundingBox.startPosition.y = std::distance(m_NorthYPos.begin(), result);

	result = std::max_element(m_NorthXPos.begin(), m_NorthXPos.end());
	m_NoordBoundingBox.width = std::distance(m_NorthXPos.begin(), result);
	result = std::max_element(m_NorthYPos.begin(), m_NorthYPos.end());
	m_NoordBoundingBox.height = std::distance(m_NorthYPos.begin(), result);

	//East
	result = std::min_element(m_EastXPos.begin(), m_EastXPos.end());
	m_EastBoundingBox.startPosition.x = std::distance(m_EastXPos.begin(), result);
	result = std::min_element(m_EastYPos.begin(), m_EastYPos.end());
	m_EastBoundingBox.startPosition.y = std::distance(m_EastYPos.begin(), result);

	result = std::max_element(m_EastXPos.begin(), m_EastXPos.end());
	m_EastBoundingBox.width = std::distance(m_EastXPos.begin(), result);
	result = std::max_element(m_EastYPos.begin(), m_EastYPos.end());
	m_EastBoundingBox.height = std::distance(m_EastYPos.begin(), result);

	//South
	result = std::min_element(m_SouthXPos.begin(), m_SouthXPos.end());
	m_SouthBoundingBox.startPosition.x = std::distance(m_SouthXPos.begin(), result);
	result = std::min_element(m_SouthYPos.begin(), m_SouthYPos.end());
	m_SouthBoundingBox.startPosition.y = std::distance(m_SouthYPos.begin(), result);

	result = std::max_element(m_SouthXPos.begin(), m_SouthXPos.end());
	m_SouthBoundingBox.width = std::distance(m_SouthXPos.begin(), result);
	result = std::max_element(m_SouthYPos.begin(), m_SouthYPos.end());
	m_SouthBoundingBox.height = std::distance(m_SouthYPos.begin(), result);

	//West
	result = std::min_element(m_WestXPos.begin(), m_WestXPos.end());
	m_SouthBoundingBox.startPosition.x = std::distance(m_WestXPos.begin(), result);
	result = std::min_element(m_WestYPos.begin(), m_WestYPos.end());
	m_SouthBoundingBox.startPosition.y = std::distance(m_WestYPos.begin(), result);

	result = std::max_element(m_WestXPos.begin(), m_WestXPos.end());
	m_SouthBoundingBox.width = std::distance(m_WestXPos.begin(), result);
	result = std::max_element(m_WestYPos.begin(), m_WestYPos.end());
	m_SouthBoundingBox.height = std::distance(m_WestYPos.begin(), result);
}
