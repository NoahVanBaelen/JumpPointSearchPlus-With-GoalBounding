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

	//West
	if (m_Graph->isWithinBounds(x - 1, y) && !m_Graph->isWallHere(x - 1, y))
	{
		boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x - 1, y),4 });
	}

	//East
	if (m_Graph->isWithinBounds(x + 1, y) && !m_Graph->isWallHere(x + 1, y))
	{
		boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x + 1, y),2 });
	}

	//Noord
	if (m_Graph->isWithinBounds(x, y - 1) && !m_Graph->isWallHere(x, y - 1))
	{
		boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x, y - 1),1 });
	}

	//South
	if (m_Graph->isWithinBounds(x, y + 1) && !m_Graph->isWallHere(x, y + 1))
	{
		boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x, y + 1),3 });
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

			//West
			if (m_Graph->isWithinBounds(x - 1, y) && !m_Graph->isWallHere(x - 1, y))
			{
				boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x - 1, y),startConnection });
			}

			//East
			if (m_Graph->isWithinBounds(x + 1, y) && !m_Graph->isWallHere(x + 1, y))
			{
				boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x + 1, y),startConnection });
			}

			//Noord
			if (m_Graph->isWithinBounds(x, y - 1) && !m_Graph->isWallHere(x, y - 1))
			{
				boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x, y - 1),startConnection });
			}

			//South
			if (m_Graph->isWithinBounds(x, y + 1) && !m_Graph->isWallHere(x, y + 1))
			{
				boundingQueue.push(NodeBoundingBox{ m_Graph->GetNode(x, y + 1),startConnection });
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
	closedList.push_back(startRecord);
	bool goToNextConnection = false;

	Vector2 startpos{startRecord.pNode->GetColumn(), startRecord.pNode->GetRow()};

	while (!openList.empty())
	{
		currentRecord = *std::min_element(openList.begin(), openList.end());
		if (currentRecord.pNode == pGoalNode)
		{
			break;
		}

		Vector2 startpos{ currentRecord.pNode->GetColumn(), currentRecord.pNode->GetRow() };
		Vector2 currentpos{ startpos };
		bool doneWithMoving = false;

		HorizontalScan(startpos, openList, closedList, currentRecord.estimatedTotalCost, pGoalNode);
		VerticalScan(startpos, openList, closedList, currentRecord.estimatedTotalCost, pGoalNode);

		//diagonaal UpRight

		currentRecord.costSoFar = 0;
		while (!doneWithMoving)
		{
			++currentpos.x;
			++currentpos.y;
			currentRecord.costSoFar += 1.5f;

			if (m_Graph->isWallHere(currentpos.x, currentpos.y + 1) &&
				m_Graph->isWallHere(currentpos.x + 1, currentpos.y) || 
				!IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(currentpos.x, currentpos.y))
				|| !(m_Graph->isWithinBounds(currentpos.x, currentpos.y)) || (m_Graph->isWallHere(currentpos.x, currentpos.y)))
			{
				doneWithMoving = true;
			}
			else
			{
				if (m_Graph->isWallHere(currentpos.x + 1, currentpos.y) ||
					m_Graph->isWallHere(currentpos.x - 1, currentpos.y) ||
					m_Graph->GetNode(currentpos.x, currentpos.y) == pGoalNode)
				{
					NodeRecord newRecord{};
					newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
					newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
					newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

					closedList.push_back(newRecord);
					openList.push_back(newRecord);
					doneWithMoving = true;
				}
				else
				{
					if (HorizontalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
						newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

						closedList.push_back(newRecord);
						openList.push_back(newRecord);
						doneWithMoving = true;
					}
					else
					{
						if (VerticalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
						{
							NodeRecord newRecord{};
							newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
							newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
							newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

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
		currentRecord.costSoFar = 0;

		//diagonaal UpLeft

		while (!doneWithMoving)
		{
			--currentpos.x;
			++currentpos.y;
			currentRecord.costSoFar += 1.5f;

			if (m_Graph->isWallHere(currentpos.x, currentpos.y + 1) &&
				m_Graph->isWallHere(currentpos.x - 1, currentpos.y) ||
				!IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(currentpos.x, currentpos.y))
				|| !(m_Graph->isWithinBounds(currentpos.x, currentpos.y)) || (m_Graph->isWallHere(currentpos.x, currentpos.y)))
			{
				doneWithMoving = true;
			}
			else
			{
				if (m_Graph->isWallHere(currentpos.x + 1, currentpos.y) ||
					m_Graph->isWallHere(currentpos.x - 1, currentpos.y) ||
					m_Graph->GetNode(currentpos.x, currentpos.y) == pGoalNode)
				{
					NodeRecord newRecord{};
					newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
					newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
					newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

					closedList.push_back(newRecord);
					openList.push_back(newRecord);
					doneWithMoving = true;
				}
				else
				{
					if (HorizontalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
						newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

						closedList.push_back(newRecord);
						openList.push_back(newRecord);
						doneWithMoving = true;
					}
					else
					{
						if (VerticalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
						{
							NodeRecord newRecord{};
							newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
							newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
							newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

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
		currentRecord.costSoFar = 0;

		//diagonaal DownRight

		while (!doneWithMoving)
		{
			++currentpos.x;
			--currentpos.y;
			currentRecord.costSoFar += 1.5f;

			if (m_Graph->isWallHere(currentpos.x, currentpos.y - 1) &&
				m_Graph->isWallHere(currentpos.x + 1, currentpos.y) ||
				!IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(currentpos.x, currentpos.y))
				|| !(m_Graph->isWithinBounds(currentpos.x, currentpos.y)) || (m_Graph->isWallHere(currentpos.x, currentpos.y)))
			{
				doneWithMoving = true;
			}
			else
			{
				if (m_Graph->isWallHere(currentpos.x + 1, currentpos.y) ||
					m_Graph->isWallHere(currentpos.x - 1, currentpos.y) ||
					m_Graph->GetNode(currentpos.x, currentpos.y) == pGoalNode)
				{
					NodeRecord newRecord{};
					newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
					newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
					newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

					closedList.push_back(newRecord);
					openList.push_back(newRecord);
					doneWithMoving = true;
				}
				else
				{
					if (HorizontalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
						newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

						closedList.push_back(newRecord);
						openList.push_back(newRecord);
						doneWithMoving = true;
					}
					else
					{
						if (VerticalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
						{
							NodeRecord newRecord{};
							newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
							newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
							newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

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
		currentRecord.costSoFar = 0;

		//diagonaal DownLeft

		while (!doneWithMoving)
		{
			--currentpos.x;
			--currentpos.y;
			currentRecord.costSoFar += 1.5f;

			if (m_Graph->isWallHere(currentpos.x, currentpos.y - 1) &&
				m_Graph->isWallHere(currentpos.x - 1, currentpos.y) ||
				!IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(currentpos.x, currentpos.y))
				|| !(m_Graph->isWithinBounds(currentpos.x, currentpos.y)) || (m_Graph->isWallHere(currentpos.x, currentpos.y)))
			{
				doneWithMoving = true;
			}
			else
			{
				if (m_Graph->isWallHere(currentpos.x + 1, currentpos.y) ||
					m_Graph->isWallHere(currentpos.x - 1, currentpos.y) ||
					m_Graph->GetNode(currentpos.x, currentpos.y) == pGoalNode)
				{
					NodeRecord newRecord{};
					newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
					newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
					newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

					closedList.push_back(newRecord);
					openList.push_back(newRecord);
					doneWithMoving = true;
				}
				else
				{
					if (HorizontalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
						newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

						closedList.push_back(newRecord);
						openList.push_back(newRecord);
						doneWithMoving = true;
					}
					else
					{
						if (VerticalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
						{
							NodeRecord newRecord{};
							newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
							newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
							newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

							closedList.push_back(newRecord);
							openList.push_back(newRecord);
							doneWithMoving = true;
						}
					}
				}
			}
		}
		currentRecord.costSoFar = 0;

		openList.erase(std::remove(openList.begin(), openList.end(), currentRecord));
	}

	//Create Path
	while (currentRecord.pNode != pStartNode)
	{
		goToNextConnection = false;
		path.push_back(currentRecord.pNode);
		for (const NodeRecord& foundRecord : closedList)
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

bool JPS::HorizontalScan(Vector2 startpos, std::vector<NodeRecord>& openList, std::vector<NodeRecord>& closedList, float cost, GraphNode* pGoalNode)
{
	bool doneWithMoving = false;
	Vector2 currentpos{ startpos };
	NodeRecord newRecord{};
	bool hasFoundConnection{ false };

	//positive horizontal
	while (!doneWithMoving)
	{
		++currentpos.x;
		++newRecord.costSoFar;
		if (m_Graph->isWithinBounds(currentpos.x, currentpos.y) && IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(currentpos.x, currentpos.y)))
		{
			GraphNode* nextNode{ m_Graph->GetNode(currentpos.x,currentpos.y) };
			if (m_Graph->isWallHere(currentpos.x, currentpos.y + 1) ||
				m_Graph->isWallHere(currentpos.x, currentpos.y - 1) ||
				m_Graph->GetNode(currentpos.x,currentpos.y) == pGoalNode)
			{
				newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
				newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
				newRecord.estimatedTotalCost = cost + newRecord.costSoFar;
				newRecord.costSoFar = 0;

				closedList.push_back(newRecord);
				openList.push_back(newRecord);
				doneWithMoving = true;
				hasFoundConnection = true;
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
		if (m_Graph->isWithinBounds(currentpos.x, currentpos.y) && IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(currentpos.x, currentpos.y)))
		{
			GraphNode* nextNode{ m_Graph->GetNode(currentpos.x,currentpos.y) };
			if (m_Graph->isWallHere(currentpos.x, currentpos.y + 1) ||
				m_Graph->isWallHere(currentpos.x, currentpos.y - 1) ||
				m_Graph->GetNode(currentpos.x, currentpos.y) == pGoalNode)
			{
				newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
				newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
				newRecord.estimatedTotalCost = cost + newRecord.costSoFar;
				newRecord.costSoFar = 0;

				closedList.push_back(newRecord);
				openList.push_back(newRecord);
				doneWithMoving = true;
				hasFoundConnection = true;
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
	return hasFoundConnection;
}

bool JPS::VerticalScan(Vector2 startpos, std::vector<NodeRecord>& openList, std::vector<NodeRecord>& closedList, float cost, GraphNode* pGoalNode)
{
	bool doneWithMoving = false;
	Vector2 currentpos{ startpos };
	NodeRecord newRecord{};
	bool hasFoundConnection{ false };

	//positive vertical
	while (!doneWithMoving)
	{
		++currentpos.y;
		++newRecord.costSoFar;
		if (m_Graph->isWithinBounds(currentpos.x, currentpos.y) && IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(currentpos.x, currentpos.y)))
		{
			GraphNode* nextNode{ m_Graph->GetNode(currentpos.x,currentpos.y) };
			if (m_Graph->isWallHere(currentpos.x - 1, currentpos.y) ||
				m_Graph->isWallHere(currentpos.x + 1, currentpos.y) ||
				m_Graph->GetNode(currentpos.x, currentpos.y) == pGoalNode)
			{
				newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
				newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
				newRecord.estimatedTotalCost = cost + newRecord.costSoFar;
				newRecord.costSoFar = 0;

				closedList.push_back(newRecord);
				openList.push_back(newRecord);
				doneWithMoving = true;
				hasFoundConnection = true;
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
		if (m_Graph->isWithinBounds(currentpos.x, currentpos.y) && IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(currentpos.x, currentpos.y)))
		{
			GraphNode* nextNode{ m_Graph->GetNode(currentpos.x,currentpos.y) };
			if (m_Graph->isWallHere(currentpos.x - 1, currentpos.y) ||
				m_Graph->isWallHere(currentpos.x + 1, currentpos.y) ||
				m_Graph->GetNode(currentpos.x, currentpos.y) == pGoalNode)
			{
				newRecord.pNode = m_Graph->GetNode(currentpos.x, currentpos.y);
				newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(startpos.x, startpos.y),m_Graph->GetIndex(currentpos.x, currentpos.y) });
				newRecord.estimatedTotalCost = cost + newRecord.costSoFar;
				newRecord.costSoFar = 0;

				closedList.push_back(newRecord);
				openList.push_back(newRecord);
				doneWithMoving = true;
				hasFoundConnection = true;
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

	return hasFoundConnection;
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
			m_NorthNodes.push_back(node.pNode);
		}

		//East
		if (node.startConnection == 2)
		{
			m_EastXPos.push_back(node.pNode->GetPosition().x);
			m_EastYPos.push_back(node.pNode->GetPosition().y);
			m_EastNodes.push_back(node.pNode);
		}

		//South
		if (node.startConnection == 3)
		{
			m_SouthXPos.push_back(node.pNode->GetPosition().x);
			m_SouthYPos.push_back(node.pNode->GetPosition().y);
			m_SouthNodes.push_back(node.pNode);
		}

		//West
		if (node.startConnection == 4)
		{
			m_WestXPos.push_back(node.pNode->GetPosition().x);
			m_WestYPos.push_back(node.pNode->GetPosition().y);
			m_WestNodes.push_back(node.pNode);
		}
	}

	//North
	std::vector<float>::iterator result = std::min_element(m_NorthXPos.begin(), m_NorthXPos.end());
	m_NoordBoundingBox.startPosition.x = result[0];
	result = std::min_element(m_NorthYPos.begin(), m_NorthYPos.end());
	m_NoordBoundingBox.startPosition.y = result[0];

	result = std::max_element(m_NorthXPos.begin(), m_NorthXPos.end());
	m_NoordBoundingBox.width = result[0] - m_NoordBoundingBox.startPosition.x;
	result = std::max_element(m_NorthYPos.begin(), m_NorthYPos.end());
	m_NoordBoundingBox.height = result[0] - m_NoordBoundingBox.startPosition.y;

	//East
	result = std::min_element(m_EastXPos.begin(), m_EastXPos.end());
	m_EastBoundingBox.startPosition.x = result[0];
	result = std::min_element(m_EastYPos.begin(), m_EastYPos.end());
	m_EastBoundingBox.startPosition.y = result[0];

	result = std::max_element(m_EastXPos.begin(), m_EastXPos.end());
	m_EastBoundingBox.width = result[0] - m_EastBoundingBox.startPosition.x;
	result = std::max_element(m_EastYPos.begin(), m_EastYPos.end());
	m_EastBoundingBox.height = result[0] - m_EastBoundingBox.startPosition.y;

	//South
	result = std::min_element(m_SouthXPos.begin(), m_SouthXPos.end());
	m_SouthBoundingBox.startPosition.x = result[0];
	result = std::min_element(m_SouthYPos.begin(), m_SouthYPos.end());
	m_SouthBoundingBox.startPosition.y = result[0];

	result = std::max_element(m_SouthXPos.begin(), m_SouthXPos.end());
	m_SouthBoundingBox.width = result[0] - m_SouthBoundingBox.startPosition.x;
	result = std::max_element(m_SouthYPos.begin(), m_SouthYPos.end());
	m_SouthBoundingBox.height = result[0] - m_SouthBoundingBox.startPosition.y;

	//West
	result = std::min_element(m_WestXPos.begin(), m_WestXPos.end());
	m_WestBoundingBox.startPosition.x = result[0];
	result = std::min_element(m_WestYPos.begin(), m_WestYPos.end());
	m_WestBoundingBox.startPosition.y = result[0];

	result = std::max_element(m_WestXPos.begin(), m_WestXPos.end());
	m_WestBoundingBox.width = result[0] - m_WestBoundingBox.startPosition.x;
	result = std::max_element(m_WestYPos.begin(), m_WestYPos.end());
	m_WestBoundingBox.height = result[0] - m_WestBoundingBox.startPosition.y;
}

bool JPS::IsInBoundingBoxes(GraphNode* pGoalNode, GraphNode* pCurrentNode)
{
	for (const GraphNode* eastNode : m_EastNodes)
	{
		if (eastNode->GetIndex() == pGoalNode->GetIndex())
		{
			for (const GraphNode* node : m_EastNodes)
			{
				if (node->GetIndex() == pCurrentNode->GetIndex())
				{
					return true;
				}
			}
			return false;
		}
	}

	for (const GraphNode* westNode : m_WestNodes)
	{
		if (westNode->GetIndex() == pGoalNode->GetIndex())
		{
			for (const GraphNode* node : m_WestNodes)
			{
				if (node->GetIndex() == pCurrentNode->GetIndex())
				{
					return true;
				}
			}
			return false;
		}
	}

	for ( const GraphNode* northNode : m_NorthNodes)
	{
		if (northNode->GetIndex() == pGoalNode->GetIndex())
		{
			for (const GraphNode* node : m_NorthNodes)
			{
				if (node->GetIndex() == pCurrentNode->GetIndex())
				{
					return true;
				}
			}
			return false;
		}
	}

	for (const GraphNode* southNode : m_SouthNodes)
	{
		if (southNode->GetIndex() == pGoalNode->GetIndex())
		{
			for (const GraphNode* node : m_SouthNodes)
			{
				if (node->GetIndex() == pCurrentNode->GetIndex())
				{
					return true;
				}
			}
			return false;
		}
	}

	return false;
}

JPS::BoundingBox JPS::GetNorthBoundingBox()
{
	 return m_NoordBoundingBox;
}

JPS::BoundingBox JPS::GetEastBoundingBox()
{
	return m_EastBoundingBox;
}

JPS::BoundingBox JPS::GetSouthBoundingBox()
{
	return m_SouthBoundingBox;
}

JPS::BoundingBox JPS::GetWestBoundingBox()
{
	return m_WestBoundingBox;
}
