#include<Windows.h>
#include<vector>
#include<random>

constexpr int TIMER = 500, TICKRATE = 10, STARRATE = 1000, DEBUG = 1500;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	WNDCLASS wndclass = { };

	wndclass.lpszClassName = L"Starfield";
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
	
	HWND hwnd = CreateWindow(wndclass.lpszClassName, L"Starfield", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

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

enum class yPreviousState
{
	INCREASING,
	DECREASING
};

class Star
{
	RECT rectangle;
	int ticks, zValue = 1;
	int x;
	double y;
	double angle, slope;
	int xClient, yClient, startX, startY;
	double percentageX, percentageY;
	yPreviousState previousState;
	bool drawFlag = true;
public:
	Star(int x, double y, int xClient, int yClient, int startX, int startY) : x{ x }, y{ y }, xClient{ xClient }, yClient{ yClient }, startX{ startX }, startY{startY}
	{
		ticks = 0;
		
		if (startX != 0)
		{
			percentageX = x / startX;
			percentageY = y / startY;
		}
	}

	void Update()
	{
		ticks++;
		if (startX != xClient || startY != yClient)
		{
			startX++;
			startY++;
		}
		x = (startX / 2) * percentageX;
		y = (startY / 2) * percentageY;
		if (x == 0)
		{
			drawFlag = false;
		}
		rectangle.left = x;
		rectangle.right = x + zValue;
		rectangle.top = y;
		rectangle.bottom = y + zValue;
		if (ticks % 100 == 0)
		{
			zValue++;
		}
		if (startX == xClient || startY == yClient)
		{
			drawFlag = false;
		}
	}

	void DrawStar(HDC hdc)
	{
		if (drawFlag)
		{
			FillRect(hdc, &rectangle, (HBRUSH)GetStockObject(WHITE_BRUSH));
		}
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
	static std::uniform_int_distribution<> xDistrib(-400, 400), yDistrib(-200, 200);
	static std::vector<Star> stars;
	static int i = 0;
	switch (message)
	{
	case WM_CREATE:
		stars.push_back(Star(xDistrib(gen), yDistrib(gen), xClient, yClient, xClient / 32, yClient / 32));
		return 0;
	case WM_SIZE:
		xClient = LOWORD(lparam);
		yClient = HIWORD(lparam);
		xCenter = xClient / 2;
		yCenter = yClient / 2;
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SetViewportOrgEx(hdc, xCenter, yCenter, nullptr);
		/*SelectObject(hdc, (HBRUSH) GetStockObject(BLACK_BRUSH));
		SelectObject(hdc, (HPEN)GetStockObject(WHITE_PEN));
		Rectangle(hdc, -100, -100, 100, 100);
		SelectObject(hdc, (HBRUSH) GetStockObject(WHITE_BRUSH));
		SelectObject(hdc, (HBRUSH)GetStockObject(BLACK_PEN));*/
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
			stars.push_back(Star(xDistrib(gen), yDistrib(gen), xClient, yClient, xClient / 32, yClient / 32));

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