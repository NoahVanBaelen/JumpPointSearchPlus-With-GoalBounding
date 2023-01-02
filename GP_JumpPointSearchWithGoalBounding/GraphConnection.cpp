#include "GraphConnection.h"

GraphConnection::GraphConnection (int from, int to)
	:m_From{from}
	,m_To{to}
{
}

int GraphConnection::GetFrom() const
{
	return m_From;
}

int GraphConnection::GetTo() const
{
	return m_To;
}

void GraphConnection::SetFrom(int newFrom)
{
	m_From = newFrom;
}

void GraphConnection::SetTo(int newTo)
{
	m_To = newTo;
}

bool GraphConnection::IsValid() const
{
	return (m_From != -1 && m_To != -1);
}