#pragma once
#include "OrganisedFunction.h"
class FunctionOrganiser
{
public:
	FunctionOrganiser(void){};
	~FunctionOrganiser(void){};
	vector <OrganisedFunction> Functions;
	vector <double> OrigiNums;
	int Size;
	int FSize;
	int CenterNum;
	double XScale;
	double Precision;
	HWND hWnd;
	void SetFunctionOrganiser(vector <double> & InNums,double InXScale,int InCenter,double InPrecision,HWND hwnd){
		OrigiNums = InNums;
		FSize = 0;
		Size = InNums.size();
		CenterNum = InCenter;
		XScale = InXScale;
		Precision = InPrecision;
		hWnd = hwnd;
		
		SetGraphics();

		int StartSpot = 0;
		int EndSpot = InNums.size() - 1;
		int Spot = 0;
		int SmallLength = Size / 20;
		InitiFunction(InNums);
		Functions[0].Auto = true;
		Functions[0].SetTaylorCoordSpot(Size / 2);
		int Top,Bottom;
		Bottom = Functions[0].FBottom;
		Top = Functions[0].FTop;
		int CurFunc = 0;
		while(true){
			if(Top + SmallLength < EndSpot - 1){//gives one point of leeway
				InitiFunction(InNums);
				CurFunc++;
				if(!Functions[CurFunc].FindBestSpot(Top,Top + SmallLength)){
					Functions.pop_back();
					FSize--;
					CurFunc--;
					break;//error! hasn't found any length at guessed spot
				}
				else
					Top = Functions[CurFunc].FTop;
			}
			else if(Top < EndSpot - 2){
				InitiFunction(InNums);
				CurFunc++;
				if(!Functions[CurFunc].FindBestSpot(Top,(Top + EndSpot - 1) / 2)){
					Functions.pop_back();
					FSize--;
					CurFunc--;
					break;//error! hasn't found any length at guessed spot
				}
				else
					Top = Functions[CurFunc].FTop;
			}
			else
				break;
		}
		while(true){
			if(Bottom - SmallLength > 1){//gives one point of leeway
				InitiFunction(InNums);
				CurFunc++;
				if(!Functions[CurFunc].FindBestSpot(Bottom,Bottom - SmallLength)){
					Functions.pop_back();
					FSize--;
					CurFunc--;
					break;//error! hasn't found any length at guessed spot
				}
				else
					Bottom = Functions[CurFunc].FBottom;
			}
			else if(Bottom > 1){
				InitiFunction(InNums);
				CurFunc++;
				if(!Functions[CurFunc].FindBestSpot(Bottom,Bottom / 2)){
					Functions.pop_back();
					FSize--;
					CurFunc--;
					break;//error! hasn't found any length at guessed spot
				}
				else
					Bottom = Functions[CurFunc].FBottom;
			}
			else
				break;
		}
		SortFunctions();
	}
	void InitiFunction(vector <double> & InNums){
		//basically adds one more fuction on
		FSize++;
		Functions.resize(FSize);
		Functions[FSize - 1].Set(InNums,Precision,CenterNum,XScale);
	}
	void Draw(HDC hdc){
		PrintGraph(hdc);
		PrintOrigiNums(hdc);
		double XVal = XScale * 1380 / (AdjXSize * xEE * 2.0);
		double YVal = 690 / (AdjYSize * 2.0 * yEE);
		for(int s = 0; s < FSize; s++){
			Functions[s].PrintGraphBetweenPoints(hdc,XVal,YVal);
		}
	}
	void ConvertToDerivative(){
		for(int f = 0; f < FSize; f++){
			Functions[f].ConvertToDerivative();
		}
		InvalidateRect(hWnd,NULL,true);
	}
	void ConvertToIntegral(){
		for(int f = 0; f < FSize; f++){
			Functions[f].ConvertToIntegral();
		}
		double Bottom = Functions[0].Calculate(Functions[0].FBottom * XScale);
		Functions[0].SetPlaceForIntegral(0);
		for(int f = 0; f < FSize - 1; f++){
			Bottom = Functions[f].Calculate(Functions[f + 1].FBottom * XScale);
			Functions[f + 1].SetPlaceForIntegral(Bottom);
		}
		InvalidateRect(hWnd,NULL,true);
	}
	double Calculate(double x){
		int Coord = x / XScale + CenterNum;
		for(int f = 0; f < FSize; f++){
			if(Functions[f].FBottom < Coord && Functions[f].FTop > Coord)
				return Functions[f].Calculate(x);
		}
	}
protected:
	int AdjXSize,AdjYSize;//graphics variables
	int xEE,yEE;//graphics variables
	void SetGraphics(){
		double MaxNum = 0;
		for(int s = 0; s < Size; s++){
			if(Abs(OrigiNums[s]) > MaxNum)
				MaxNum = Abs(OrigiNums[s]);
		}	
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
	void SortFunctions(){//from lowest to highest
		int Size = Functions.size();
		OrganisedFunction TempFunction;
		for(int g = 0; g < Size; g++){
			for(int f = 0; f < Size; f++){
				if(Functions[g].FBottom < Functions[f].FBottom){
					TempFunction = Functions[g];
					Functions[g] = Functions[f];
					Functions[f] = TempFunction;
				}
			}
		}
	}
};

