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

