#pragma once
#include "function.h"
class UserFunction : public Function
{
public:
	UserFunction(void);
	~UserFunction(void);
	
	int AdjXSize,AdjYSize;//graphics variables
	int xEE,yEE;//graphics variables
	HWND hWnd;
	void SetFunction(vector <double> & InNums,double InXScale,int InCenter,double InPrecision,HWND hwnd){//always call before you do anything with this function
		if(InNums.size() < 5 || InXScale * InNums.size() > 2147483648 || InXScale < 0 || InCenter < 0 || InCenter >= InNums.size() || InPrecision < 0)
			return;
		hWnd = hwnd;
		Size = InNums.size();
		//loads the vector into class
		Nums = InNums;
		OrigiNums = InNums;
		CenterNum = InCenter;
		Precision = InPrecision;
		TaylorSpot = CenterNum;
		XScale = InXScale;
		RegressionLength = DefLookArround;
		NumOfDerivs = 6;
		Auto = true;
		double MaxNum = 0;
		for(int s = 0; s < Size; s++){
			if(Abs(Nums[s]) > MaxNum)
				MaxNum = Abs(Nums[s]);
		}	
		DefLookArround = Size / 50;//look at this carefully--also, it should be even for best effect
		//CalculateResiduals();
		//resizes it to be as big as nums so that the calculatespot can deal with it correctly
		NextDeriv.resize(Nums.size());
		CalculateCoefs();
		//graphics values
		int XSize,YSize;
		XSize = Size * XScale;
		YSize = MaxNum + 0.9999999;
		if(XSize > 1000000 || YSize > 1000000)
			return;
		xEE = 1;
		while(XSize / (xEE * 10) != 0)
			xEE *= 10;
		AdjXSize = XSize / xEE + 1;

		yEE = 1;
		while(YSize / (yEE * 10) != 0)
			yEE *= 10;
		AdjYSize = YSize / yEE + 1;
	}
	void StartGraphRunning(){
		if(Auto == true){
			cout << "Function needs to be on manual mode for Graphrunning to work";
			return;
		}
		PAINTSTRUCT ps;
		HDC hdc;
		
		for(int t = 0; t < 40; t++){
			InvalidateRect(hWnd,NULL,true);
			hdc = BeginPaint(hWnd, &ps);
			SetTaylorCoordSpot(t * (Size / 40));
			Draw(hdc);
			Sleep(100);
			EndPaint(hWnd, &ps);
		}
	}
	void SetPixelTaylorSpot(int XPixel){
		int Coord = (XPixel - 690) / (XScale * 1380 / (AdjXSize * xEE * 2.0)) + CenterNum;
		SetTaylorCoordSpot(Coord);
	}
	void SetPlaceTaylorSpot(double Place){
		int Coord = Place / XScale + CenterNum;
		SetTaylorCoordSpot(Coord);
		InvalidateRect(hWnd,NULL,true);
	}
	void ConvertToDerivative(){
		for(int c = 1; c < Coefs.size(); c++){
			Coefs[c - 1] = Coefs[c];
		}
		Coefs.pop_back();
		InvalidateRect(hWnd,NULL,true);
	}
	void ConvertToIntegral(){
		Coefs.push_back(1123.581321);//adds more room
		for(int c = Coefs.size() - 2; c >= 0; c--){
			Coefs[c + 1] = Coefs[c];
		}
		Coefs[0] = 0;
		InvalidateRect(hWnd,NULL,true);
	}
	void PrintGraph(HDC hdc){
		HPEN hPenOLd;
		HPEN hLinePen;
		COLORREF qLineColor;
		qLineColor = RGB(0,0,0);
		hLinePen = CreatePen(PS_SOLID, 4, qLineColor);
		hPenOLd = (HPEN)SelectObject(hdc, hLinePen);
		// draw y axis
			MoveToEx(hdc, 690, 0, NULL);
			LineTo (hdc,690,690);
		//draw x axis
			MoveToEx(hdc,0,345 , NULL);
			LineTo (hdc,1380,345);
		// draw the markings
			qLineColor = RGB(0,0,0);
			hLinePen = CreatePen(PS_SOLID, 2, qLineColor);
			hPenOLd = (HPEN)SelectObject(hdc, hLinePen);
			for(int f = -AdjXSize; f <= AdjXSize; f++){
				if(f == 0)
					continue;
				int xspot = f * 1380 / (AdjXSize * 2) + 690 - 5;
				MoveToEx(hdc,f * 1380 / (AdjXSize * 2) + 690,345 - 4, NULL);
				LineTo (hdc,f * 1380 / (AdjXSize * 2) + 690,345 + 4);
				PrintInt(f*xEE,hdc,xspot,345 + 10);		
			}
			for(int f = -AdjYSize; f <= AdjYSize; f++){
				if(f == 0)
					continue;
				int xspot = 690 + 10;
				MoveToEx(hdc,690 - 4,f * 690 / (AdjYSize * 2) + 345, NULL);
				LineTo (hdc,690 + 4,f * 690 / (AdjYSize * 2) + 345);
				PrintInt(-f*yEE,hdc,xspot,f * 690 / (AdjYSize * 2) - 5 + 345);
			}
	}
	void PrintOrigiNums(HDC hdc){
		HPEN hPenOLd;
		HPEN hLinePen;
		COLORREF qLineColor;
		qLineColor = RGB(0,0,0);
		// print the function itself
		hLinePen = CreatePen(PS_SOLID, 3, qLineColor);
		hPenOLd = (HPEN)SelectObject(hdc, hLinePen);
		double XVal = XScale * 1380 / (AdjXSize * xEE * 2.0);
		double YVal = 690 / (AdjYSize * 2.0 * yEE);
		int xspot,yspot;
		for(int g = 0; g < Size; g++){
			xspot = (g - CenterNum) * XVal + 690;
			yspot = - OrigiNums[g] * YVal + 345;
			MoveToEx(hdc,xspot, yspot, NULL);
			LineTo (hdc,xspot, yspot);
		}
	}
	void PrintFunction(HDC hdc){
		HPEN hPenOLd;
		HPEN hLinePen;
		COLORREF qLineColor;
		qLineColor = RGB(0,255,0);
		// print the function itself

		hLinePen = CreatePen(PS_SOLID, 1, qLineColor);
		hPenOLd = (HPEN)SelectObject(hdc, hLinePen);
		double XVal = XScale * 1380 / (AdjXSize * xEE * 2.0);
		double YVal = 690 / (AdjYSize * yEE * 2.0);
		int xspot,yspot;
		xspot = (-CenterNum) * XVal + 690;
		yspot = - Calculate(0) * YVal + 345;
		for(int g = 1; g < Size; g++){
			MoveToEx(hdc,xspot, yspot, NULL);

			xspot = (g - CenterNum) * XVal + 690;
			yspot = - Calculate(g*XScale) * YVal + 345;

			LineTo (hdc,xspot, yspot);
		}
		// print the blue length lines
		qLineColor = RGB(0,0,255);
		hLinePen = CreatePen(PS_SOLID, 2, qLineColor);
		hPenOLd = (HPEN)SelectObject(hdc, hLinePen);
		
		xspot = (FBottom - CenterNum) * XVal + 690;
		yspot = - Calculate(FBottom * XScale) * YVal + 345;
		MoveToEx(hdc,xspot, yspot - 5, NULL);
		LineTo (hdc,xspot, yspot + 5);
		
		xspot = (FTop - CenterNum) * XVal + 690;
		yspot = - Calculate(FTop* XScale) * YVal + 345;
		MoveToEx(hdc,xspot, yspot - 5, NULL);
		LineTo (hdc,xspot, yspot + 5);
	}
	void PrintCoefs(HDC hdc){
		HPEN hPenOLd;
		HPEN hLinePen;
		COLORREF qLineColor;
		qLineColor = RGB(0,0,0);
		hLinePen = CreatePen(PS_SOLID, 2, qLineColor);
		hPenOLd = (HPEN)SelectObject(hdc, hLinePen);
		int XSpot = 20, PrevXSpot;
		int YSpot = 500;
		int Length;
		PrintFloat(Coefs[0],hdc,XSpot,YSpot + 7);
		XSpot += 10;
		PrintSymbol('+',hdc,XSpot,YSpot + 7);//prints the + mark
		XSpot += 10;
		for(int Coef = 1; Coef < Coefs.size(); Coef++){
			if(Abs(Coefs[Coef]) > 100000000000000.0 || Coefs[Coef] == 0)
				continue;
			PrevXSpot = XSpot;
			PrintFloat(Coefs[Coef],hdc,XSpot,YSpot);
			Length = XSpot - PrevXSpot;
			MoveToEx(hdc, PrevXSpot, YSpot + 15, NULL);
			LineTo (hdc,XSpot,YSpot + 15);
			int FactSpot = PrevXSpot + Length / 2 - 10;
			PrintInt(Coef,hdc,FactSpot,YSpot + 20);
			PrintSymbol('!',hdc,FactSpot,YSpot + 20);//prints the ! mark
			XSpot += 10;
			PrintSymbol('+',hdc,XSpot,YSpot + 7);//prints the + mark
			XSpot += 10;
		}
	}
	void PrintInfo(HDC hdc){
		//prints out taylorspot
		int xspot = 10;
		int yspot = 550;
		ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("Center of Taylor Series: "),24,NULL);
		xspot += 23 * 8;
		PrintFloat((TaylorSpot - CenterNum)*XScale,hdc,xspot,yspot);
		//prints out length
		yspot = 570;
		xspot = 10;
		ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("Length within precision: "),24,NULL);
		xspot += 23 * 8;
		PrintInt(CalculateLenghtOfFit(),hdc,xspot,yspot);
		if(Auto){
			//prints out best regressionlength
			xspot = 10;
			yspot = 10;
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("Best Regression Length: "),25,NULL);
			xspot += 23 * 8;
			PrintInt(RegressionLength,hdc,xspot,yspot);
		}
	}
	void Draw(HDC hdc){
		PrintGraph(hdc);
		PrintOrigiNums(hdc);
		PrintFunction(hdc);
		PrintCoefs(hdc);
		PrintInfo(hdc);
	}
};

