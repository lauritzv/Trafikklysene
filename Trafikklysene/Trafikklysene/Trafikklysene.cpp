// trafikklysene.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Trafikklysene.h"
#define _USE_MATH_DEFINES
#include <Math.h>
#include "Car.h"
#include "time.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name


//Laste inn bakgrunn fra fil
HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL, _T("bg_img.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
BITMAP qBitmap;
int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP), reinterpret_cast<LPVOID>(&qBitmap));

//Laste inn bil fra fil
HBITMAP hCarBitmap = (HBITMAP)::LoadImage(NULL, _T("mm_yellowcar.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
BITMAP qCarBitmap;
int iCarReturn = GetObject(reinterpret_cast<HGDIOBJ>(hCarBitmap), sizeof(BITMAP), reinterpret_cast<LPVOID>(&qCarBitmap));


//lyskryss farger
const COLORREF red = RGB(255, 0, 0);
const COLORREF yellow = RGB(255, 255, 0);
const COLORREF green = RGB(0, 255, 0);
const COLORREF unlit = RGB(100, 100, 100);

//hastigheter
const int timerFrequency = 10;
const int lightspeedscaledown = 100;

//Bilhastigheter og størrelser målt i bilposisjoner

const double drivespeed = 1.0;
const double carpointheigth = 14.5;

//Stopp posisjoner målt i bilposisjoner

const double stopposh = 38.5 + carpointheigth;
const double stopposv = 66.0 - carpointheigth;

//spawn sannsynligheter
int vprobability = 40;
int hprobability = 40;

//gamle vinduhøyde ved resize
// int oldBottom;

//std::vector<tagPOINT> clickedPoints;
std::vector<Car*> hCars;
std::vector<Car*> vCars;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	TestDlgProc(HWND, UINT, WPARAM, LPARAM);


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
	//wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(40, 200, 40));
	wcex.hbrBackground = NULL;		//for å unngå flimmer

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

void spawnHorizontal(double carheigth)
{
	Car* hc;
	if (!hCars.empty()) {
		int prevpos = hCars.back()->getPos();
		if ((prevpos) < 0) {
			hc = new Car(prevpos - carheigth); //opprett bil bak siste bil
		}
		else {
			hc = new Car(-carheigth); //opprett bil like utenfor skjermen
		}
	}
	else {
		hc = new Car(-carheigth); //opprett bil like utenfor skjermen
	}
	hCars.push_back(hc);
}
void spawnVertical(double carheigth) {
	Car * vc;
	if (!vCars.empty()) {
		int prevpos = vCars.back()->getPos();
		if ((prevpos + carheigth) > 100) {
			vc = new Car(prevpos + carheigth);
		}
		else {
			vc = new Car(100);
		}
	}
	else {
		vc = new Car(100);
	}



	vCars.push_back(vc);
}

void randomSpawn(double carheigth) {

	int hroll = (int)(rand() % 100);
	int vroll = (int)(rand() % 100);

	if (hroll <= hprobability && hroll != 0)
		spawnHorizontal(carheigth);

	if (vroll <= vprobability)
		spawnVertical(carheigth);
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

	static long timer = 0;
	static int lightstate = 0;
	RECT screen;
	GetClientRect(hWnd, &screen);
	int carwidth = (int)screen.bottom * 0.1;
	int carheigth = carwidth * 1.52;



	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_DIALOGTEST_VISDIALOG:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, TestDlgProc);
			break;
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

	case WM_CREATE:
		//snurr igang timeren ved oppstart
		srand(time(NULL));
		SetTimer(hWnd, 1, timerFrequency, NULL);
		// oldBottom = screen.bottom;
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;

		// For doublebuffering
		HDC Memhdc;
		HBITMAP Membitmap;
		hdc = BeginPaint(hWnd, &ps);
		Memhdc = CreateCompatibleDC(hdc);
		Membitmap = CreateCompatibleBitmap(hdc, screen.right, screen.bottom);
		SelectObject(Memhdc, Membitmap);


		//start på ordinær tegnekode:


		HBRUSH brush;
		HPEN pen;

		HFONT hOrgFont, hMinFont; // hendler til to font-objekter

								  //grey
		brush = CreateSolidBrush(RGB(40, 40, 40));
		pen = CreatePen(PS_SOLID, 2, RGB(20, 20, 20));

		HGDIOBJ hOrg = SelectObject(Memhdc, brush);
		HGDIOBJ hOrgPen = SelectObject(Memhdc, pen);

		//RECT screen;
		//GetClientRect(hWnd, &screen);
		//GetClientRect(GetActiveWindow(), &screen);


		//Sizes

		int light1offsetx = (int)screen.bottom *0.48;
		int light1offsety = (int)screen.bottom * 0.2;
		int light2offsetx = (int)screen.bottom *0.85;
		int light2offsety = (int)screen.bottom * 0.69;

		int lightheigth = (int)screen.bottom * 0.1;
		int carwidth = (int)screen.bottom * 0.1;
		int carheigth = carwidth * 1.52;
		int cornersize = lightheigth * 0.1;

		int vertLanePos = ((int)screen.bottom *0.9) * 0.74;
		int horLanePos = (int)screen.bottom * 0.5;


		//Bakgrunns bitmap

		HDC hLocalDC;
		hLocalDC = ::CreateCompatibleDC(Memhdc);

		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hLocalDC, hBitmap);

		SetStretchBltMode(Memhdc, COLORONCOLOR);
		StretchBlt(Memhdc, 0, 0, (int)(screen.bottom * 1.78), (int)screen.bottom, hLocalDC, 0, 0, 500, 281, SRCCOPY);





		//BOOL qRetBlit = ::BitBlt(hdc, 820, 0, qBitmap.bmWidth, qBitmap.bmHeight, hLocalDC, 0, 0, SRCCOPY);
		//BOOL qRetBlit = ::BitBlt(hdc, 0, 0, (int)screen.right, (int)screen.bottom, hLocalDC, 0, 0, SRCCOPY);

		SelectObject(hLocalDC, hOldBmp);
		DeleteDC(hLocalDC);
		//DeleteObject(hBitmap);

		//Vertikale veikantlinjer
		/*
		MoveToEx(Memhdc, (int)screen.bottom *0.54, 0, 0);
		LineTo(Memhdc, (int)screen.bottom *0.54, screen.bottom);
		MoveToEx(Memhdc, (int)screen.bottom *0.792, 0, 0);
		LineTo(Memhdc, (int)screen.bottom *0.792, screen.bottom);
		*/

		//Bil bitmap

		HDC hCarLocalDC;
		hCarLocalDC = ::CreateCompatibleDC(Memhdc);

		hOldBmp = (HBITMAP)::SelectObject(hCarLocalDC, hCarBitmap);

		SetStretchBltMode(Memhdc, COLORONCOLOR);

		//Tegner vertikale biler
		Car vcar = NULL;
		for (std::vector<Car*>::iterator it = vCars.begin(); it != vCars.end(); ++it) {
			vcar = (**it);
			int vdrawpos = (vcar.getPos() / 100) * screen.bottom;
			StretchBlt(Memhdc, vertLanePos - (0.5*carwidth), vdrawpos, carwidth, carheigth, hCarLocalDC, 0, 0, 17, 26, SRCCOPY);
		}

		//Tegner horisontale biler
		Car hcar = NULL;
		for (std::vector<Car*>::iterator it = hCars.begin(); it != hCars.end(); ++it) {
			hcar = (**it);
			int hdrawpos = (hcar.getPos() / 178) * screen.bottom * 1.78;
			POINT point[3] = { { hdrawpos + carheigth, horLanePos - (0.5*carwidth) },{ hdrawpos + carheigth,horLanePos - (0.5*carwidth) + carwidth },{ hdrawpos, horLanePos - (0.5*carwidth) } };
			PlgBlt(Memhdc, point, hCarLocalDC, 0, 0, 17, 26, NULL, NULL, NULL);
		}
		SelectObject(hCarLocalDC, hOldBmp);
		DeleteDC(hCarLocalDC);


		//Trafikklys bg

		RoundRect(Memhdc, light1offsetx - 3 * lightheigth - cornersize, light1offsety - cornersize, light1offsetx + cornersize,
			light1offsety + lightheigth + cornersize, cornersize, cornersize);
		RoundRect(Memhdc, light2offsetx - cornersize, light2offsety - cornersize, light2offsetx + lightheigth + cornersize,
			light2offsety + (3 * lightheigth) + cornersize, cornersize, cornersize);

		SelectObject(Memhdc, hOrg);
		SelectObject(Memhdc, hOrgPen);
		DeleteObject(pen);
		DeleteObject(brush);






		//Tekst

		/*
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

		*/

		//Lysene
		switch (lightstate) {
		case 0:
			drawLight(Memhdc, light1offsetx - lightheigth, light1offsety, lightheigth, red);		//left red
			drawLight(Memhdc, light1offsetx - 2 * lightheigth, light1offsety, lightheigth, unlit);	//left yellow
			drawLight(Memhdc, light1offsetx - 3 * lightheigth, light1offsety, lightheigth, unlit);	//left green

			drawLight(Memhdc, light2offsetx, light2offsety + 0, lightheigth, unlit);	//right red
			drawLight(Memhdc, light2offsetx, light2offsety + lightheigth, lightheigth, unlit);//right yellow
			drawLight(Memhdc, light2offsetx, light2offsety + 2 * lightheigth, lightheigth, green);//right green
			break;
		case 1:
			drawLight(Memhdc, light1offsetx - lightheigth, light1offsety, lightheigth, red);		//left red
			drawLight(Memhdc, light1offsetx - 2 * lightheigth, light1offsety, lightheigth, yellow);	//left yellow
			drawLight(Memhdc, light1offsetx - 3 * lightheigth, light1offsety, lightheigth, unlit);	//left green

			drawLight(Memhdc, light2offsetx, light2offsety + 0, lightheigth, unlit);	//right red
			drawLight(Memhdc, light2offsetx, light2offsety + lightheigth, lightheigth, yellow);//right yellow
			drawLight(Memhdc, light2offsetx, light2offsety + 2 * lightheigth, lightheigth, unlit);//right green
			break;
		case 2:
			drawLight(Memhdc, light1offsetx - lightheigth, light1offsety, lightheigth, unlit);		//left red
			drawLight(Memhdc, light1offsetx - 2 * lightheigth, light1offsety, lightheigth, unlit);	//left yellow
			drawLight(Memhdc, light1offsetx - 3 * lightheigth, light1offsety, lightheigth, green);	//left green

			drawLight(Memhdc, light2offsetx, light2offsety + 0, lightheigth, red);	//right red
			drawLight(Memhdc, light2offsetx, light2offsety + lightheigth, lightheigth, unlit);//right yellow
			drawLight(Memhdc, light2offsetx, light2offsety + 2 * lightheigth, lightheigth, unlit);//right green
			break;
		case 3:
			drawLight(Memhdc, light1offsetx - lightheigth, light1offsety, lightheigth, unlit);		//left red
			drawLight(Memhdc, light1offsetx - 2 * lightheigth, light1offsety, lightheigth, yellow);	//left yellow
			drawLight(Memhdc, light1offsetx - 3 * lightheigth, light1offsety, lightheigth, unlit);	//left green

			drawLight(Memhdc, light2offsetx, light2offsety + 0, lightheigth, red);	//right red
			drawLight(Memhdc, light2offsetx, light2offsety + lightheigth, lightheigth, yellow);//right yellow
			drawLight(Memhdc, light2offsetx, light2offsety + 2 * lightheigth, lightheigth, unlit);//right green
			break;
		default:
			drawLight(Memhdc, light1offsetx - lightheigth, light1offsety, lightheigth, unlit);		//left red
			drawLight(Memhdc, light1offsetx - 2 * lightheigth, light1offsety, lightheigth, unlit);	//left yellow
			drawLight(Memhdc, light1offsetx - 3 * lightheigth, light1offsety, lightheigth, unlit);	//left green

			drawLight(Memhdc, light2offsetx, light2offsety + 0, lightheigth, unlit);	//right red
			drawLight(Memhdc, light2offsetx, light2offsety + lightheigth, lightheigth, unlit);//right yellow
			drawLight(Memhdc, light2offsetx, light2offsety + 2 * lightheigth, lightheigth, unlit);//right green

		} //end of lightstate


		  // Utplassering av punkter:
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


		  //slutt på ordinær tegnekode

		  //doublebuffering og opprydding
		BitBlt(hdc, 0, 0, screen.right, screen.bottom, Memhdc, 0, 0, SRCCOPY);
		DeleteObject(Membitmap);
		DeleteDC(Memhdc);


		DeleteDC(hdc);
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
		timer++;
		timer %= 4000;
		lightstate = (timer / lightspeedscaledown) % 4; //endrer lysstatus basert på globale variabler

		//random spawning
		if (timer % 100 == 0)
			randomSpawn(carpointheigth);


		//Kjørelogikk for biler i vertikal lane:
		double vprevcarpos = NULL;
		for (std::vector<Car*>::iterator it = vCars.begin(); it != vCars.end(); ++it) {

			if (((**it).getPos() - carpointheigth) < stopposv) {
				(**it).setThrough();
			}

			if ((**it).getThrough() || lightstate == 0) {
				(**it).drive(-drivespeed);
			}
			else {
				if (vprevcarpos != NULL) {
					if (((**it).getPos() - drivespeed - carpointheigth) > vprevcarpos) {
						if (((**it).getPos() - drivespeed - carpointheigth) > stopposv) {
							(**it).drive(-drivespeed);
						}
					}
				}
				else {
					if (((**it).getPos() - drivespeed - carpointheigth) > stopposv) {
						(**it).drive(-drivespeed);
					}
				}
			}
			vprevcarpos = (**it).getPos();
		}


		//Kjørelogikk for biler i horisontal lane:

		double hprevcarpos = NULL;
		for (std::vector<Car*>::iterator it = hCars.begin(); it != hCars.end(); ++it) {

			if (((**it).getPos() + carpointheigth) > stopposh) { //har passert lyskrysset, merker bilen sådan
				(**it).setThrough();
			}

			if ((**it).getThrough() || lightstate == 2) { //har passert lyskrysset, eller har grønnt lys
				(**it).drive(drivespeed);
			}
			else { //har ikke grønnt lys og har ikke passert lyskrysset
				if (hprevcarpos != NULL) { //er ikke første spawnede bil
					if ((**it).getPos() + drivespeed + carpointheigth < hprevcarpos) { //vil ikke treffe bilen foran ved kjøring
						if (((**it).getPos() + drivespeed + carpointheigth) < stopposh) { //har ikke nådd lyskrysset
							(**it).drive(drivespeed);
						}
					}

				}
				else { //er første spawnede bil
					if (((**it).getPos() + drivespeed + carpointheigth) < stopposh) { //har ikke nådd lyskrysset
						(**it).drive(drivespeed);
					}
				}
			}
			hprevcarpos = (**it).getPos(); //tar vare på bilposisjon for sammenligning for neste bil
		}


		//despawning av fremste bil etter utenfor bildet
		if (!vCars.empty() && vCars.front()->getPos() < -carpointheigth) {
			vCars.erase(vCars.begin());
		}

		if (!hCars.empty() && (hCars.front()->getPos() > 178)) {
			hCars.erase(hCars.begin());
		}
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
		//InvalidateRect(hWnd, NULL, TRUE);
		//SetTimer( hWnd, 1, 10, NULL); //har flyttet til wndproc for automatisk oppstart
		spawnHorizontal(carpointheigth);

		break;
	case WM_RBUTTONDOWN:
		spawnVertical(carpointheigth);
		//KillTimer(hWnd, 1);
		break;
		/*
		case WM_SIZE:
		{
			for (std::vector<Car*>::iterator it = hCars.begin(); it != hCars.end(); ++it) {
				(**it).setPos(((**it).getPos()*oldBottom) / screen.bottom);
			}

			for (std::vector<Car*>::iterator it = vCars.begin(); it != vCars.end(); ++it) {
				//(**it)
			}
			oldBottom = screen.bottom;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		*/

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RIGHT:
			if (hprobability < 100)
				hprobability += 10;
			break;
		case VK_LEFT:
			if (hprobability > 10) {
				hprobability -= 10;
			}
			else if (hprobability < 10) {
				hprobability = 0;
			}
			break;
		case VK_DOWN:
			if (vprobability > 10) {
				vprobability -= 10;
			}
			else if (vprobability < 10) {
				vprobability = 0;
			}
			break;
		case VK_UP:
			if (vprobability < 100)
				vprobability += 10;
			break;
		}
		break;
	case WM_QUIT:
		KillTimer(hWnd, 1);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


//Funksjoner for omgjøring fra WCHAR til INT
int miniPow(int a, int b)    // power with small number - just demo
{
	int r = 1;
	for (int i = 0; i < b; i++)
	{
		r *= a;
	}
	return r;
}
int WideToInt(wchar_t a[])
{
	int i = 0;
	int len = wcslen(a);
	int coefficient = 0;
	int RawNumber = 0;
	int Number = 0;

	for (int k = 0; k < len; k++)
	{
		coefficient = miniPow(10, len - 1 - k);
		RawNumber = (int)a[k];
		Number = RawNumber - 48;
		i += coefficient * Number;
	}
	return i;
}

INT_PTR CALLBACK TestDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		HWND pwH;
		WCHAR pw[100];
		WCHAR ps[100];

	case WM_INITDIALOG:
		//GetDlgItemText(hDlg, IDC_EDIT2, pw, 4);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT2, pw, 4);
			GetDlgItemText(hDlg, IDC_EDIT3, ps, 4);

			if (pw[0] != '\0') {
				if (WideToInt(pw) > 100) {
					hprobability = 100;
				}
				else {
					hprobability = WideToInt(pw);
				}
			}
			if (ps[0] != '\0') {
				if (WideToInt(ps) > 100) {
					vprobability = 100;
				}
				else
				{
					vprobability = WideToInt(ps);
				}

			}

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		}

	}
	return (INT_PTR)FALSE;
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
