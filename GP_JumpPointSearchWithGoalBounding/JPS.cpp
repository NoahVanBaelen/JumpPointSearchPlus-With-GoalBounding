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
	startRecord.pConnection = new GraphConnection{pStartNode->GetIndex(),pStartNode->GetIndex()};
	startRecord.costSoFar = 0;
	startRecord.estimatedTotalCost = 0;

	openList.push_back(startRecord);
	closedList.push_back(startRecord);
	bool goToNextConnection = false;

	Vector2 startpos{ static_cast<float>(startRecord.pNode->GetColumn()), static_cast<float>(startRecord.pNode->GetRow())};

	while (!openList.empty())
	{
		// get node with lowest cost
		currentRecord = *std::min_element(openList.begin(), openList.end());
		if (currentRecord.pNode == pGoalNode)
		{
			break;
		}

		Vector2 startpos{ static_cast<float>(currentRecord.pNode->GetColumn()), static_cast<float>(currentRecord.pNode->GetRow()) };
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

			//Check if next diagonaal is cornerd
			if (m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) + 1) &&
				m_Graph->isWallHere(static_cast<int>(currentpos.x) + 1, static_cast<int>(currentpos.y)) ||
				!IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)))
				|| !(m_Graph->isWithinBounds(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y))) 
				|| (m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y))))
			{
				doneWithMoving = true;
			}
			else
			{
				if (m_Graph->isWallHere(static_cast<int>(currentpos.x) + 1, static_cast<int>(currentpos.y)) ||
					m_Graph->isWallHere(static_cast<int>(currentpos.x) - 1, static_cast<int>(currentpos.y)) ||
					m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) + 1) ||
					m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) - 1) ||
					m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) == pGoalNode)
				{
					NodeRecord newRecord{};
					newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
					newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																				,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
					newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

					CheckList(newRecord, openList);
					CheckList(newRecord, closedList);
					doneWithMoving = true;
				}
				else
				{
					if (HorizontalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																					,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
						newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

						CheckList(newRecord, closedList);
						doneWithMoving = true;
					}
					if (VerticalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																					,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
						newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

						CheckList(newRecord, closedList);
						doneWithMoving = true;
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

			//Check if next diagonaal is corned
			if (m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) + 1) &&
				m_Graph->isWallHere(static_cast<int>(currentpos.x) - 1, static_cast<int>(currentpos.y)) ||
				!IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)))
				|| !(m_Graph->isWithinBounds(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y))) 
				|| (m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y))))
			{
				doneWithMoving = true;
			}
			else
			{
				if (m_Graph->isWallHere(static_cast<int>(currentpos.x) + 1, static_cast<int>(currentpos.y)) ||
					m_Graph->isWallHere(static_cast<int>(currentpos.x) - 1, static_cast<int>(currentpos.y)) ||
					m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) + 1) ||
					m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) - 1) ||
					m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) == pGoalNode)
				{
					NodeRecord newRecord{};
					newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
					newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																				,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
					newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

					CheckList(newRecord, openList);
					CheckList(newRecord, closedList);
					doneWithMoving = true;
				}
				else
				{
					if (HorizontalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																					,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
						newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

						CheckList(newRecord, closedList);
						doneWithMoving = true;
					}
					if (VerticalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																					,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
						newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

						CheckList(newRecord, closedList);
						doneWithMoving = true;
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
			//Check if next diagonaal is corner 
			if (m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) - 1) &&
				m_Graph->isWallHere(static_cast<int>(currentpos.x) + 1, static_cast<int>(currentpos.y)) ||
				!IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)))
				|| !(m_Graph->isWithinBounds(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y))) 
				|| (m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y))))
			{
				doneWithMoving = true;
			}
			else
			{
				if (m_Graph->isWallHere(static_cast<int>(currentpos.x) + 1, static_cast<int>(currentpos.y)) ||
					m_Graph->isWallHere(static_cast<int>(currentpos.x) - 1, static_cast<int>(currentpos.y)) ||
					m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) + 1) ||
					m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) - 1) ||
					m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) == pGoalNode)
				{
					NodeRecord newRecord{};
					newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
					newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																				,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
					newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

					CheckList(newRecord, openList);
					CheckList(newRecord, closedList);
					doneWithMoving = true;
				}
				else
				{
					if (HorizontalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																					,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
						newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

						CheckList(newRecord, closedList);
						doneWithMoving = true;
					}
					if (VerticalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																					,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
						newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

						CheckList(newRecord, closedList);;
						doneWithMoving = true;
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
			//Check if next diagonaal is corner
			if (m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) - 1) &&
				m_Graph->isWallHere(static_cast<int>(currentpos.x) - 1, static_cast<int>(currentpos.y)) ||
				!IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)))
				|| !(m_Graph->isWithinBounds(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y))) 
				|| (m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y))))
			{
				doneWithMoving = true;
			}
			else
			{
				if (m_Graph->isWallHere(static_cast<int>(currentpos.x) + 1, static_cast<int>(currentpos.y)) ||
					m_Graph->isWallHere(static_cast<int>(currentpos.x) - 1, static_cast<int>(currentpos.y)) ||
					m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) + 1) ||
					m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) - 1) ||
					m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) == pGoalNode)
				{
					NodeRecord newRecord{};
					newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
					newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																				,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
					newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

					CheckList(newRecord, openList);
					CheckList(newRecord, closedList);
					doneWithMoving = true;
				}
				else
				{
					if (HorizontalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																					,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
						newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

						CheckList(newRecord, closedList);
						doneWithMoving = true;
					}
					if (VerticalScan(currentpos, openList, closedList, currentRecord.estimatedTotalCost + currentRecord.costSoFar, pGoalNode))
					{
						NodeRecord newRecord{};
						newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
						newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																					,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
						newRecord.estimatedTotalCost = currentRecord.estimatedTotalCost + currentRecord.costSoFar;

						CheckList(newRecord, closedList);
						doneWithMoving = true;
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
		std::vector<NodeRecord> allFoundRecord{};
		for (const NodeRecord& foundRecord : closedList)
		{
			if (foundRecord.pNode->GetIndex() == currentRecord.pConnection->GetFrom())
			{
				allFoundRecord.push_back(foundRecord);
			}
		}
		currentRecord = *std::min_element(allFoundRecord.begin(), allFoundRecord.end());
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
		if (m_Graph->isWithinBounds(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) 
			&& IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y))))
		{
			GraphNode* nextNode{ m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) };
			if (m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) + 1) ||
				m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) - 1) ||
				m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) == pGoalNode)
			{
				newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
				newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																			,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
				newRecord.estimatedTotalCost = cost + newRecord.costSoFar;
				newRecord.costSoFar = 0;

				CheckList(newRecord, openList);
				CheckList(newRecord, closedList);
				doneWithMoving = true;
				hasFoundConnection = true;
			}
			if (m_Graph->isWallHere(static_cast<int>(currentpos.x) + 1, static_cast<int>(currentpos.y)))
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
		if (m_Graph->isWithinBounds(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) 
			&& IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y))))
		{
			GraphNode* nextNode{ m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) };
			if (m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) + 1) ||
				m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) - 1) ||
				m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) == pGoalNode)
			{
				newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
				newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																			,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
				newRecord.estimatedTotalCost = cost + newRecord.costSoFar;
				newRecord.costSoFar = 0;

				CheckList(newRecord, openList);
				CheckList(newRecord, closedList);
				doneWithMoving = true;
				hasFoundConnection = true;
			}
			if (m_Graph->isWallHere(static_cast<int>(currentpos.x) - 1, static_cast<int>(currentpos.y)))
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
		if (m_Graph->isWithinBounds(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) 
			&& IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y))))
		{
			GraphNode* nextNode{ m_Graph->GetNode(static_cast<int>(currentpos.x),static_cast<int>(currentpos.y)) };
			if (m_Graph->isWallHere(static_cast<int>(currentpos.x) - 1, static_cast<int>(currentpos.y)) ||
				m_Graph->isWallHere(static_cast<int>(currentpos.x) + 1, static_cast<int>(currentpos.y)) ||
				m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) == pGoalNode)
			{
				newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
				newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y)),m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
				newRecord.estimatedTotalCost = cost + newRecord.costSoFar;
				newRecord.costSoFar = 0;

				CheckList(newRecord, openList);
				CheckList(newRecord, closedList);
				doneWithMoving = true;
				hasFoundConnection = true;
			}
			if (m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) + 1))
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
		if (m_Graph->isWithinBounds(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) 
			&& IsInBoundingBoxes(pGoalNode, m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y))))
		{
			GraphNode* nextNode{ m_Graph->GetNode(static_cast<int>(currentpos.x),static_cast<int>(currentpos.y)) };
			if (m_Graph->isWallHere(static_cast<int>(currentpos.x) - 1, static_cast<int>(currentpos.y)) ||
				m_Graph->isWallHere(static_cast<int>(currentpos.x) + 1, static_cast<int>(currentpos.y)) ||
				m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) == pGoalNode)
			{
				newRecord.pNode = m_Graph->GetNode(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y));
				newRecord.pConnection = new GraphConnection(GraphConnection{ m_Graph->GetIndex(static_cast<int>(startpos.x), static_cast<int>(startpos.y))
																			,m_Graph->GetIndex(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y)) });
				newRecord.estimatedTotalCost = cost + newRecord.costSoFar;
				newRecord.costSoFar = 0;

				CheckList(newRecord, openList);
				CheckList(newRecord, closedList);
				doneWithMoving = true;
				hasFoundConnection = true;
			}
			if (m_Graph->isWallHere(static_cast<int>(currentpos.x), static_cast<int>(currentpos.y) - 1))
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
	for (const NodeBoundingBox& node : m_vectorBoundingBoxes)
	{
		//Noord
		if (node.startConnection == 1)
		{
			m_NorthNodes.push_back(node.pNode);
		}

		//East
		if (node.startConnection == 2)
		{
			m_EastNodes.push_back(node.pNode);
		}

		//South
		if (node.startConnection == 3)
		{
			m_SouthNodes.push_back(node.pNode);
		}

		//West
		if (node.startConnection == 4)
		{
			m_WestNodes.push_back(node.pNode);
		}
	}
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

std::vector<GraphNode*> JPS::GetUsedBoundingBox(GraphNode* pGoalNode)
{
	for (const GraphNode* eastNode : m_EastNodes)
	{
		if (eastNode->GetIndex() == pGoalNode->GetIndex())
		{
			return m_EastNodes;
		}
	}

	for (const GraphNode* westNode : m_WestNodes)
	{
		if (westNode->GetIndex() == pGoalNode->GetIndex())
		{
			return m_WestNodes;
		}
	}

	for (const GraphNode* northNode : m_NorthNodes)
	{
		if (northNode->GetIndex() == pGoalNode->GetIndex())
		{
			return m_NorthNodes;
		}
	}

	for (const GraphNode* southNode : m_SouthNodes)
	{
		if (southNode->GetIndex() == pGoalNode->GetIndex())
		{
			return m_SouthNodes;
		}
	}

	return std::vector<GraphNode*>{};
}

void JPS::CheckList(NodeRecord newRecord, std::vector<NodeRecord>& list)
{
	bool alreadyInList{false};
	for (const NodeRecord& foundRecord : list)
	{
		if (newRecord.pConnection->GetTo() == foundRecord.pConnection->GetTo())
		{
			if (newRecord.estimatedTotalCost < foundRecord.estimatedTotalCost)
			{
				list.erase(std::remove(list.begin(), list.end(), foundRecord));
				break;
			}
			else
			{
				alreadyInList = true;
				break;
			}
		}
	}

	if (!alreadyInList)
	{
		list.push_back(newRecord);
	}
}