#include<Windows.h>
#include<vector>
#include<random>

constexpr int TIMER = 500, TICKRATE = 10, STARRATE = 1000, DEBUG = 1500;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	WNDCLASS wndclass = { };

	wndclass.lpszClassName = L"My Windows App";
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndclass.lpszMenuName = nullptr;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wndclass);
	
	HWND hwnd = CreateWindow(wndclass.lpszClassName, L"My Windows App", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

	SetTimer(hwnd, TIMER, TICKRATE, nullptr);
	SetTimer(hwnd, STARRATE, TICKRATE * 2, nullptr);
	//SetTimer(hwnd, DEBUG, TICKRATE * 10, nullptr);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

class Star
{
	RECT rectangle;
	int ticks, quadrant = 0, zValue = 1;
	int centerX, centerY;
public:
	Star(int x, int y, int centerX, int centerY)
		: centerX{ centerX }, centerY{ centerY }
	{
		ticks = 0;
		if (x < centerX && y < centerY)
		{
			quadrant = 2;
			rectangle.left = x;
			rectangle.top = y;
			rectangle.right = x + 1;
			rectangle.bottom = y + 1;
		}
		if (x > centerX && y < centerY)
		{
			quadrant = 1;
			rectangle.right = x;
			rectangle.top = y;
			rectangle.left = x - 1;
			rectangle.bottom = y + 1;
		}
		if (x < centerX && y > centerY)
		{
			quadrant = 3;
			rectangle.left = x;
			rectangle.right = x + 1;
			rectangle.bottom = y;
			rectangle.top = y - 1;
		}
		if (x > centerX && y > centerY)
		{
			quadrant = 4;
			rectangle.bottom = y;
			rectangle.right = x;
			rectangle.top = y - 1;
			rectangle.left = x - 1;
		}
	}

	void Update()
	{
		ticks++;
		if (quadrant == 1)
		{
			rectangle.right = rectangle.right + 1;
			rectangle.top = rectangle.top - 1;
			rectangle.left = rectangle.left + 1;
			rectangle.bottom = rectangle.bottom - 1;
			if (ticks % 30 == 0)
			{
				rectangle.left -= 1;
				rectangle.bottom += 1;
			}
		}
		if (quadrant == 2)
		{
			rectangle.right = rectangle.right - 1;
			rectangle.top = rectangle.top - 1;
			rectangle.left = rectangle.left - 1;
			rectangle.bottom = rectangle.bottom - 1;
			if (ticks % 30 == 0)
			{
				rectangle.right += 1;
				rectangle.bottom += 1;
			}
		}
		if (quadrant == 3)
		{
			rectangle.right = rectangle.right - 1;
			rectangle.top = rectangle.top + 1;
			rectangle.left = rectangle.left - 1;
			rectangle.bottom = rectangle.bottom + 1;
			if (ticks % 30 == 0)
			{
				rectangle.right += 1;
				rectangle.top -= 1;
			}
		}
		if (quadrant == 4)
		{
			rectangle.right = rectangle.right + 1;
			rectangle.top = rectangle.top + 1;
			rectangle.left = rectangle.left + 1;
			rectangle.bottom = rectangle.bottom + 1;
			if (ticks % 30 == 0)
			{
				rectangle.left -= 1;
				rectangle.top -= 1;
			}
		}
	}

	void DrawStar(HDC hdc)
	{
		FillRect(hdc, &rectangle, (HBRUSH)GetStockObject(WHITE_BRUSH));
	}

	~Star() { }
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static int xClient, yClient, xCenter, yCenter;
	static std::uniform_int_distribution<> xDistrib(0, 1600), yDistrib(0, 2000);
	static std::vector<Star> stars;
	static int i = 0;
	switch (message)
	{
	case WM_CREATE:
		stars.push_back( Star(xDistrib(gen), yDistrib(gen), xCenter, yCenter));
		return 0;
	case WM_SIZE:
		xClient = LOWORD(lparam);
		yClient = HIWORD(lparam);
		xCenter = xClient / 2;
		yCenter = yClient / 2;
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		for (Star& s : stars)
		{
			s.DrawStar(hdc);
		}
		EndPaint(hwnd ,&ps);
		return 0;
	case WM_TIMER:
		if (wparam == TIMER)
		{
			for (Star& s : stars)
			{
				s.Update();
			}
			InvalidateRect(hwnd, nullptr, true);
		}
		if (wparam == STARRATE)
		{
			stars.push_back(Star(xDistrib(gen), yDistrib(gen), xCenter, yCenter));

		}
		if (wparam == DEBUG)
		{
			i++;
		}
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wparam, lparam);
}