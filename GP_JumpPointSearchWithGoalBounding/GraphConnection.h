#pragma once
class GraphConnection
{
public:
	GraphConnection(int from, int to);
	~GraphConnection() = default;

	int GetFrom() const;
	void SetFrom(int newFrom);

	int GetTo() const;
	void SetTo(int newTo);

	bool IsValid() const;
private:
	int m_From;
	int m_To;
};

