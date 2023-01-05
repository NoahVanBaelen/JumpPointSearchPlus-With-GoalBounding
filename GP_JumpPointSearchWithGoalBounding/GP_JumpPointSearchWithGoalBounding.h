#pragma once
#include "framework.h"
#include "resource.h"

class GridGraph;

class Engine
{
public:
	Engine(HINSTANCE hInstance, int nCmdShow);
	~Engine();

	Engine(const Engine& yRef) = delete;
	Engine(Engine&& yRef) = delete;
	Engine& operator=(const Engine& yRef) = delete;
	Engine& operator=(Engine&& yRef) = delete;

	int Run(GridGraph* m_pGridGraph);
	LRESULT HandleEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void SetColor(int r, int g, int b);
	void DrawLine(int x1, int y1, int x2, int y2);
	void DrawRect(int x1, int y1, int width, int height);
	void FillRect(int x1, int y1, int width, int height);
	void FillEllipse(int x1, int y1, int radiusX, int radiusY);

	int m_Width;
	int m_Height;

private:

	HINSTANCE m_Instance;
	int m_Show;
	HDC m_hDC;
	HWND m_hWnd;

	COLORREF m_Color{ RGB(255,255,255)};
	GridGraph* m_pGridGraph{ nullptr };
};
