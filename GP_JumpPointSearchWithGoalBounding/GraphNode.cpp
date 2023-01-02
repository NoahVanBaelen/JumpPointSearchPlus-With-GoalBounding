#include "GraphNode.h"

GraphNode::GraphNode(int index, Vector2 position)
	: m_Index{index}
	,m_Position{position}
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