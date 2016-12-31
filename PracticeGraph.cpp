// PracticeGraph.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PracticeGraph.h"
#include "FunctionOrganiser.h"
#include "UserFunction.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void DealWithFunction(HWND hWnd);
void DealWithOrganisedFunction(HWND hWnd);
void ConsolMessageCenter(void * id);
UserFunction F1;
FunctionOrganiser F2;
bool User;

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
	LoadString(hInstance, IDC_PRACTICEGRAPH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PRACTICEGRAPH));
	
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRACTICEGRAPH));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PRACTICEGRAPH);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		User = true;
		DealWithFunction(hWnd);
		//DealWithOrganisedFunction(hWnd);
		AllocConsole();
	    freopen("CON", "w", stdout);
		freopen("CON", "r", stdin);
		_beginthread(ConsolMessageCenter,5,NULL);
		break;
	case WM_LBUTTONUP:
		{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if(User)
			F1.SetPixelTaylorSpot(x);
		InvalidateRect(hWnd,NULL,true);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
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
		case IDM_CHANGE:
			User = !User;
			DealWithFunction(hWnd);
			InvalidateRect(hWnd,NULL,true);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if(User)
			F1.Draw(hdc);
		else
			F2.Draw(hdc);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
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
void DealWithFunction9(HWND hWnd){
	vector <double> OrigiNums;
	OrigiNums.resize(4000);
	double RandNum;
	for(int g = 0; g < 4000; g++){
		RandNum = (rand() - 32767 / 2) / (32767 * 4.0);
		OrigiNums[g] = sqrt(1-(g-2000)*(1.5/2000.0));//cos((g - 250) / 100.0) + RandNum;
	}
	if(User)
		F1.SetFunction(OrigiNums,(20 / 3.1415)/1000,250,0.27/2,hWnd);
	else
		F2.SetFunctionOrganiser(OrigiNums,(20 / 3.1415)/1000,250,0.27/2,hWnd);
}
void DealWithFunction5(HWND hWnd){
		vector <double> XIntegral;
		XIntegral.resize(4000);
		int BitWidth = 4000;
		int XCenter = 2000,YCenter = 2000;
		//constant that makes the curve 2 wide in total
		const double Divsior = (BitWidth / 4) * (BitWidth / 4);
	
		//finds the total area under the curve for later
		double Sum = 0;
		const int HalfBitWidth = BitWidth / 2;
		const int HalfBitHight = BitWidth / 2;
		//makes sum and puts it into x integral
		int XVal,YVal;
		for(int x = -HalfBitWidth; x < HalfBitWidth; x++){
			XVal = XCenter - (x + HalfBitWidth);
			for(int y = -HalfBitHight; y < HalfBitHight; y++){
				YVal = YCenter - (y + HalfBitHight);
				Sum += 1 / (1 + Square((XVal * XVal) / Divsior + (YVal * YVal) / Divsior));
			}
			XIntegral[x + HalfBitWidth] = Sum;
		}
		//divides to make the end integral be 1
		for(int x = 0; x < BitWidth; x++)
			XIntegral[x] /= Sum;

		vector <double> Inverse;
		Inverse.resize(2000);
		for(int x = 0; x < 2000; x++){
			int Top = 4000 - 1;
			int Bottom = 0;
			double Val = x / 2000.0;
			while(Top != Bottom && Top != Bottom + 1){
				if(Val > XIntegral[(Top - Bottom) / 2 + Bottom])
					Bottom = (Top - Bottom) / 2 + Bottom;
				else
					Top = (Top - Bottom) / 2 + Bottom;
			}
			Inverse[x] = Top / 4000.0;
		}
		if(User)
			F1.SetFunction(Inverse,1/2000.0,0,0.0005,hWnd);
		else
			F2.SetFunctionOrganiser(Inverse,1/2000.0,0,0.0005,hWnd);
}
void DealWithFunction(HWND hWnd){
	vector <double> OrigiNums;
	OrigiNums.resize(1000);
	double RandNum;
	for(int g = 0; g < 1000; g++){
		RandNum = (rand() - 32767 / 2) / (32767 * 4.0);
		OrigiNums[g] = cos((g - 250) / 50.0) + RandNum;
	}
	if (User)
		F1.SetFunction(OrigiNums, (20 / 3.1415) / 1000, 250, 0.27 / 2, hWnd);
	else
		F2.SetFunctionOrganiser(OrigiNums, (20 / 3.1415) / 1000, 250, 0.27 / 2, hWnd);
}
double GetDouble(){
	double Num;
	while(true){
		cin >> Num;
		if(cin.fail()){
			cin.clear();
			continue;
		}
		else
			return Num;
	}
}
int GetInt(){
	int Num;
	while(true){
		cin >> Num;
		if(cin.fail()){
			cin.clear();
			continue;
		}
		else
			return Num;
	}
}
void ConsolMessageCenter(void * id){
	string Input;
	
	while(true){//returns true if it fails
		cin >> Input;
		if(cin.fail()){
			cin.clear();
			continue;
		}
		if(User){
			if(Input == "Manual")
				F1.Auto = false;
			else if(Input == "Auto")
				F1.Auto = true;
			else if(Input == "Set_Length")
				F1.RegressionLength = GetInt();
			else if(Input == "Set_Derivative")
				F1.NumOfDerivs = GetInt() + 1;
			else if(Input == "Set_XScale")
				F1.XScale = GetDouble();
			else if(Input == "Get_Length")
				cout << F1.CalculateLenghtOfFit();
			else if(Input == "Set_Precision")
				F1.Precision = GetDouble();
			else if(Input == "Calculate")
				cout << F1.Calculate(GetDouble());
			else if(Input == "RunGraph")
				F1.StartGraphRunning();
			else if(Input == "Set_Taylor_Spot")
				F1.SetPlaceTaylorSpot(GetDouble());
			else if(Input == "Convert_To_Derivative")
				F1.ConvertToDerivative();
			else if(Input == "Convert_To_Integral")
				F1.ConvertToIntegral();
		}
		else{
			if(Input == "Set_XScale")
				F2.XScale = GetDouble();
			else if(Input == "Calculate")
				F2.Calculate(GetDouble());
			else if(Input == "Convert_To_Derivative")
				F2.ConvertToDerivative();
			else if(Input == "Convert_To_Integral")
				F2.ConvertToIntegral();
		}
	}
}