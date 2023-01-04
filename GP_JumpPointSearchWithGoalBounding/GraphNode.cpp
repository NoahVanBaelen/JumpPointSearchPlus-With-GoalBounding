#include "GraphNode.h"

GraphNode::GraphNode()
	: m_Index{ -1 }
	, m_Position{ 0,0 }
	, m_Row{ -1 }
	, m_Column{ -1 }
{}

GraphNode::GraphNode(int index, Vector2 position, int row, int column)
	: m_Index{index}
	,m_Position{position}
	,m_Row{row}
	,m_Column{column}
{}

int GraphNode::GetIndex() const
{
	return m_Index;
}

Vector2 GraphNode::GetPosition() const
{
	return m_Position;
}

void GraphNode::SetIndex(int newIdx)
{
	m_Index = newIdx;
}

void GraphNode::SetPosition(Vector2 newPosition)
{
	m_Position = newPosition;
}

int GraphNode::GetRow() const
{
	return m_Row;
}

int GraphNode::GetColumn() const
{
	return m_Column;
}