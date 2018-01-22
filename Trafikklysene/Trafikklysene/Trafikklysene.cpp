// trafikklysene.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Trafikklysene.h"
#define _USE_MATH_DEFINES
#include <Math.h>
#include "Car.h"



#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

												//System.Windows.Forms::Control control = new Control();

												//lyskryss farger
const COLORREF red = RGB(255, 0, 0);
const COLORREF yellow = RGB(255, 255, 0);
const COLORREF green = RGB(0, 255, 0);
const COLORREF unlit = RGB(100, 100, 100);

std::vector<tagPOINT> clickedPoints;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TRAFIKKLYSENE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRAFIKKLYSENE));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAFIKKLYSENE));
	//wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);

	//wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_TRAFIKKLYSENE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	//sette bakgrunnsfarge
	wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(40, 200, 40));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		100, 100, 900, 700, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	return TRUE;
}

void drawLight(HDC& hdc, int offsetx, int offsety, int lightheigth, COLORREF col) {
	HBRUSH brush = CreateSolidBrush(col);
	HGDIOBJ hOrg = SelectObject(hdc, brush);

	Ellipse(hdc, offsetx, offsety, (int)(0.98*lightheigth + offsetx), (int)(0.98*lightheigth + offsety));
	SelectObject(hdc, hOrg);
	DeleteObject(brush);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;


	static int lightstate = 0;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hWnd, &ps);

		HBRUSH brush;
		HPEN pen;

		HFONT hOrgFont, hMinFont; // hendler til to font-objekter

								  //grey
		brush = CreateSolidBrush(RGB(40, 40, 40));
		pen = CreatePen(PS_SOLID, 2, RGB(20, 20, 20));

		HGDIOBJ hOrg = SelectObject(hdc, brush);
		HGDIOBJ hOrgPen = SelectObject(hdc, pen);

		RECT screen;
		//GetClientRect(hWnd, &screen);
		GetClientRect(GetActiveWindow(), &screen);

		//Size

		int light1offsetx = (int)screen.bottom *0.05;
		int light1offsety = (int)screen.bottom * 0.05;
		int light2offsetx = (int)screen.bottom *0.85;
		int light2offsety = (int)screen.bottom * 0.65;

		int lightheigth = (int)screen.bottom * 0.1;
		int cornersize = lightheigth * 0.1;

		//Bitmap

		HBITMAP hBitmap;
		hBitmap = (HBITMAP)::LoadImage(NULL, _T("bg_img.bmp"), IMAGE_BITMAP, (int)(screen.bottom * 1.78), (int)screen.bottom, LR_LOADFROMFILE);

		HDC hLocalDC;
		hLocalDC = ::CreateCompatibleDC(hdc);

		BITMAP qBitmap;
		int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP), reinterpret_cast<LPVOID>(&qBitmap));

		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hLocalDC, hBitmap);
		//			BOOL qRetBlit = ::BitBlt(hdc, 820, 0, qBitmap.bmWidth, qBitmap.bmHeight, hLocalDC, 0, 0, SRCCOPY);
		BOOL qRetBlit = ::BitBlt(hdc, 0, 0, (int)screen.right, (int)screen.bottom, hLocalDC, 0, 0, SRCCOPY);

		SelectObject(hLocalDC, hOldBmp);
		DeleteDC(hLocalDC);
		DeleteObject(hBitmap);
		RoundRect(hdc, light1offsetx - cornersize, light1offsety - cornersize, light1offsetx + lightheigth + cornersize,
			light1offsety + (3 * lightheigth) + cornersize, cornersize, cornersize);
		RoundRect(hdc, light2offsetx - cornersize, light2offsety - cornersize, light2offsetx + lightheigth + cornersize,
			light2offsety + (3 * lightheigth) + cornersize, cornersize, cornersize);
		SelectObject(hdc, hOrg);
		SelectObject(hdc, hOrgPen);
		DeleteObject(pen);
		DeleteObject(brush);


		// Testcommit

		//		MoveToEx(hdc, (light1offsetx+light2offsetx) * 0.6, 0, 0);
		//		LineTo(hdc, (light1offsetx + light2offsetx) * 0.6, screen.bottom);
		//		MoveToEx(hdc, (light1offsetx + light2offsetx) * 0.88, 0, 0);
		//		LineTo(hdc, (light1offsetx + light2offsetx) * 0.88, screen.bottom);

		//Tekst

		//brush = CreateSolidBrush(RGB(255,0,0));
		//hOrg = SelectObject(hdc, brush);




		// lager en ny font - en serie med parametre, men de
		// fleste kan settes til "default" - det er bare første
		// og siste parameter som er "ikke-default"
		hMinFont = CreateFont(lightheigth, 0, 100, 0, FW_EXTRABOLD, TRUE, FALSE, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Comic Sans MS"));

		SetTextColor(hdc, RGB(255, 128, 0));
		SetBkMode(hdc, TRANSPARENT);
		//			SetBkColor(hdc, RGB(255, 255, 0,0));

		// setter inn den nye fonten - den gamle tas vare på
		hOrgFont = (HFONT)SelectObject(hdc, hMinFont);
		// skriver litt tekst
		TextOut(hdc, light1offsetx*1.1, light2offsety*0.75, _T("Her er min nye font."), 20);
		// bytter til den gamle fonten og skriver litt til
		SelectObject(hdc, hOrgFont);
		TextOut(hdc, 50, 400, _T("... og er er standardfonten."), 28);
		// rydder opp
		DeleteObject(hMinFont);
		//SelectObject(hdc, hOrg);
		//DeleteObject(brush);



		//Lysene

		switch (lightstate) {
		case 0:
			drawLight(hdc, light1offsetx, light1offsety + 0, lightheigth, red);		//left red
			drawLight(hdc, light1offsetx, light1offsety + lightheigth, lightheigth, unlit);	//left yellow
			drawLight(hdc, light1offsetx, light1offsety + 2 * lightheigth, lightheigth, unlit);	//left green

			drawLight(hdc, light2offsetx, light2offsety + 0, lightheigth, unlit);	//right red
			drawLight(hdc, light2offsetx, light2offsety + lightheigth, lightheigth, unlit);//right yellow
			drawLight(hdc, light2offsetx, light2offsety + 2 * lightheigth, lightheigth, green);//right green
			break;
		case 1:
			drawLight(hdc, light1offsetx, light1offsety + 0, lightheigth, red);		//left red
			drawLight(hdc, light1offsetx, light1offsety + lightheigth, lightheigth, yellow);	//left yellow
			drawLight(hdc, light1offsetx, light1offsety + 2 * lightheigth, lightheigth, unlit);	//left green

			drawLight(hdc, light2offsetx, light2offsety + 0, lightheigth, unlit);	//right red
			drawLight(hdc, light2offsetx, light2offsety + lightheigth, lightheigth, yellow);//right yellow
			drawLight(hdc, light2offsetx, light2offsety + 2 * lightheigth, lightheigth, unlit);//right green
			break;
		case 2:
			drawLight(hdc, light1offsetx, light1offsety + 0, lightheigth, unlit);		//left red
			drawLight(hdc, light1offsetx, light1offsety + lightheigth, lightheigth, unlit);	//left yellow
			drawLight(hdc, light1offsetx, light1offsety + 2 * lightheigth, lightheigth, green);	//left green

			drawLight(hdc, light2offsetx, light2offsety + 0, lightheigth, red);	//right red
			drawLight(hdc, light2offsetx, light2offsety + lightheigth, lightheigth, unlit);//right yellow
			drawLight(hdc, light2offsetx, light2offsety + 2 * lightheigth, lightheigth, unlit);//right green
			break;
		case 3:
			drawLight(hdc, light1offsetx, light1offsety + 0, lightheigth, unlit);		//left red
			drawLight(hdc, light1offsetx, light1offsety + lightheigth, lightheigth, yellow);	//left yellow
			drawLight(hdc, light1offsetx, light1offsety + 2 * lightheigth, lightheigth, unlit);	//left green

			drawLight(hdc, light2offsetx, light2offsety + 0, lightheigth, red);	//right red
			drawLight(hdc, light2offsetx, light2offsety + lightheigth, lightheigth, yellow);//right yellow
			drawLight(hdc, light2offsetx, light2offsety + 2 * lightheigth, lightheigth, unlit);//right green
			break;
		default:
			drawLight(hdc, light1offsetx, light1offsety + 0, lightheigth, unlit);		//left red
			drawLight(hdc, light1offsetx, light1offsety + lightheigth, lightheigth, unlit);	//left yellow
			drawLight(hdc, light1offsetx, light1offsety + 2 * lightheigth, lightheigth, unlit);	//left green

			drawLight(hdc, light2offsetx, light2offsety + 0, lightheigth, unlit);	//right red
			drawLight(hdc, light2offsetx, light2offsety + lightheigth, lightheigth, unlit);//right yellow
			drawLight(hdc, light2offsetx, light2offsety + 2 * lightheigth, lightheigth, unlit);//right green

		} //end of lightstate

		  //Linjer
/*
		if (!clickedPoints.empty()) {

			//std::vector<tagPOINT> clickedPoints2;
			//std::copy(clickedPoints.begin(), clickedPoints.end(), back_inserter(clickedPoints2));

			POINT p;
			for (std::vector<tagPOINT>::iterator it = clickedPoints.begin(); it != clickedPoints.end(); ++it) {
				p = (*it);

				MoveToEx(hdc, p.x, p.y - cornersize, 0);
				LineTo(hdc, p.x, p.y + cornersize);
				MoveToEx(hdc, p.x - cornersize, p.y, 0);
				LineTo(hdc, p.x + cornersize, p.y);
			}
		}
*/
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		//case WM_SETCURSOR:
		//	break;
	case WM_TIMER:
	{
		lightstate++;
		lightstate %= 4;

		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_LBUTTONDOWN:

		//lightstate++;
		//lightstate %= 4;

		/*
		POINT point;
		if (GetCursorPos(&point)) {
			ScreenToClient(hWnd, &point);
			//point.x = point.x * screen2.bottom;
			//point.y = point.y * screen2.bottom;

			clickedPoints.push_back(point);
		}
		*/
		InvalidateRect(hWnd, NULL, TRUE);
		SetTimer( hWnd, 1, 1000, NULL);
		break;
	case WM_RBUTTONDOWN:
		KillTimer(hWnd, 1);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
