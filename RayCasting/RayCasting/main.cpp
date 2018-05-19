#include <windows.h>
#include <cstdio>
#include <iostream>
#pragma warning (disable : 4996) 

#define WINDOW_TITLE   "RayCastingDemo"
#define TEXTURE__NAME "head256.raw"
#define window_w  640
#define window_h  480
#define img_w 256
#define img_h 256
#define img_d 225

using namespace std;

bool m_keys[256];
byte img[img_d][img_w][img_h];
byte buffer[img_w * img_h * 4];

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void UpdateBuffer(HWND);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wndclass = { 0 };
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = (HICON)::LoadImage(NULL, "icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = WINDOW_TITLE;
	if (!RegisterClassEx(&wndclass))
		return -1;

	HWND hwnd;
	hwnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, window_w, window_h, NULL, NULL, hInstance, NULL);
	MoveWindow(hwnd, 250, 80, window_w, window_h, true);

	FILE* f = fopen(TEXTURE__NAME, "rb");
	if (f == NULL) return 0;
	int size = img_w * img_h * img_d;
	fread(img, sizeof(byte), size, f);
	fclose(f);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	
	// add console to print log
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT rc = { 0, 0, 60, 30 };
	SetConsoleWindowInfo(hOut, 1, &rc);
	/*
	for (int d = 0; d < img_d; d++)
		for (int w = 0; w < img_w; w++)
			for (int h = 0; h < img_h; h++)
				cout << "(" << (int)img[d][w][h].r << "," << (int)img[d][w][h].g
				<< "," << (int)img[d][w][h].b << "," << (int)img[d][w][h].a << ")" << endl;

	*/
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			UpdateBuffer(hwnd);
		}
	}
	UnregisterClass("RayCastingDemo", wndclass.hInstance);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:

		ValidateRect(hwnd, NULL);
		break;

	case WM_KEYDOWN:
		m_keys[(unsigned int)wParam] = true;

		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		break;

	case WM_KEYUP:
		m_keys[(unsigned int)wParam] = false;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

void UpdateBuffer(HWND hwnd)
{
	//create bitmap
	static BITMAPINFO binfo;
	ZeroMemory(&binfo, sizeof(BITMAPINFO));
	binfo.bmiHeader.biBitCount = 32;
	binfo.bmiHeader.biCompression = BI_RGB;
	binfo.bmiHeader.biHeight = -img_h;
	binfo.bmiHeader.biPlanes = 1;
	binfo.bmiHeader.biSizeImage = 0;
	binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	binfo.bmiHeader.biWidth = img_w;

	HDC hdc = GetDC(hwnd);

	static int d = 0;
	d = (d + 1) % img_d;
	Sleep(10);

	for (int y = 0; y < img_h; y++)
	{
		for (int x = 0; x < img_w; x++)
		{
			// for z axis
			buffer[y * img_w * 4 + x * 4 + 0] = img[x][y][d]; //r
			buffer[y * img_w * 4 + x * 4 + 1] = img[x][y][d]; //g
			buffer[y * img_w * 4 + x * 4 + 2] = img[x][y][d]; //b
			buffer[y * img_w * 4 + x * 4 + 3] = img[x][y][d]; //a

			/* for y axis
			buffer[y * img_w * 4 + x * 4 + 0] = img[x][d][y]; //r
			buffer[y * img_w * 4 + x * 4 + 1] = img[x][d][y]; //g
			buffer[y * img_w * 4 + x * 4 + 2] = img[x][d][y]; //b
			buffer[y * img_w * 4 + x * 4 + 3] = img[x][d][y]; //a
			*/

			/* for x axis
			buffer[y * img_w * 4 + x * 4 + 0] = img[d][x][y]; //r
			buffer[y * img_w * 4 + x * 4 + 1] = img[d][x][y]; //g
			buffer[y * img_w * 4 + x * 4 + 2] = img[d][x][y]; //b
			buffer[y * img_w * 4 + x * 4 + 3] = img[d][x][y]; //a
			*/
		}
	}
				
	StretchDIBits(hdc, 0, 0, window_w,
		window_h, 0, 0, img_w,
		img_h, buffer, (BITMAPINFO*)&binfo,
		DIB_RGB_COLORS, SRCCOPY);

	ReleaseDC(hwnd, hdc);
}