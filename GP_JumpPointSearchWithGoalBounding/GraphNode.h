#pragma once

struct Vector2
{
	float x;
	float y;
};

class GraphNode
{
public:
	GraphNode(int index, Vector2 position);
	~GraphNode() = default;

	int GetIndex() const;
	void SetIndex(int newIdx);

	Vector2 GetPosition() const;
	void SetPosition(Vector2 newPosition);

private:
	int m_Index;
	Vector2 m_Position;
};

