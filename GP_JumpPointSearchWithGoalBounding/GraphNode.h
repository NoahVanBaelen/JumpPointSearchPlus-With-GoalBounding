#pragma once

struct Vector2
{
	float x;
	float y;
};

class GraphNode
{
public:
	GraphNode();
	GraphNode(int index, Vector2 position, int row, int column);
	~GraphNode() = default;

	bool operator==(const GraphNode& other) const
	{
		return m_Index == other.m_Index &&
			m_Position.x == other.m_Position.x &&
			m_Position.y == other.m_Position.y &&
			m_Row == other.m_Row &&
			m_Column == other.m_Column;
	}

	int GetIndex() const;
	void SetIndex(int newIdx);

	Vector2 GetPosition() const;
	void SetPosition(Vector2 newPosition);

	int GetRow() const;

	int GetColumn() const;

private:
	int m_Index;
	Vector2 m_Position;
	int m_Row;
	int m_Column;
};

