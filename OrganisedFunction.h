#pragma once
#include "function.h"
class OrganisedFunction : public Function
{
public:
	OrganisedFunction(void);
	~OrganisedFunction(void);
	void Set(vector <double> & InNums,double InPrecision,int InCenterNum,double InXScale){
		Size = InNums.size();
		DefLookArround = Size / 50;
		Nums = InNums;
		OrigiNums = InNums;
		NextDeriv.resize(Size);
		Precision = InPrecision;
		CenterNum = InCenterNum;
		RegressionLength = DefLookArround;
		NumOfDerivs = 6;
		XScale = InXScale;
		Auto = false;
	}
	void ConvertToDerivative(){
		if(Coefs.size() <= 2)
			return;
		for(int c = 1; c < Coefs.size(); c++){
			Coefs[c - 1] = Coefs[c];
		}
		Coefs.pop_back();
	}
	void ConvertToIntegral(){
		Coefs.push_back(1123.581321);//adds more room
		for(int c = Coefs.size() - 2; c >= 0; c--){
			Coefs[c + 1] = Coefs[c];
		}
		Coefs[0] = 0;
	}
	void SetPlaceForIntegral(double AddToBottom){//assumes that ConvertToIntegral has already been called
		Coefs[0] = AddToBottom - Calculate(FBottom*XScale);
	}
	/*double Calculate(int XRef){
		return Function::Calculate(XRef*XScale);
	}*/
	//not secure, put errors in if bottom or top is equal to -1;
	bool FindBestSpot(int GetSpot,int GuessedTaylorSpot){
		if(GuessedTaylorSpot < 0 || GuessedTaylorSpot >= Size)
			return false;
		bool Above = (GetSpot < GuessedTaylorSpot);
		vector <vector<double>>  Derivs;
		TaylorSpot = GuessedTaylorSpot;
		CalculateOptimumLengthAndDeriv();
		CalculateCoefsAndDerivs(Derivs);
		int Top,Bottom;
		if(!CalculateLenghtOfFit(Bottom,Top))
			return false;
		if(Bottom <= GetSpot && Top >= Size - 2 || Top >= GetSpot && Bottom <= 1 || Top == GetSpot || Bottom == GetSpot){
			FTop = Top;
			FBottom = Bottom;
			return true;
		}
		int Spot = GuessedTaylorSpot;
		if(Above){
			if(Bottom > GetSpot){//if it does not yet encompas GetSpot
				while(Bottom > GetSpot && Spot > GetSpot){
					Spot--;
					for(int d = 0; d < NumOfDerivs; d++){
						Coefs[d] = Derivs[d][Spot];
					}
					if(!CalculateLenghtOfFit(Bottom,Top)){
						Spot++;
						for(int d = 0; d < NumOfDerivs; d++){
							Coefs[d] = Derivs[d][Spot];
						}
						break;
					}
				}
			}
			else{//if it encompasses GetSpot too much
				while(Bottom < GetSpot && Spot < Size - 1){
					Spot++;
					for(int d = 0; d < NumOfDerivs; d++){
						Coefs[d] = Derivs[d][Spot];
					}
					if(!CalculateLenghtOfFit(Bottom,Top))
						break;
					if(Bottom <= GetSpot && Top >= Size - 2){
						Spot++;//ends up being the original becasue of the -- below
						break;
					}
				}
				//reverts to one that is definitely there
				Spot--;
				for(int d = 0; d < NumOfDerivs; d++){
					Coefs[d] = Derivs[d][Spot];
				}
			}
		}
		else{//if below
			if(Top < GetSpot){//if it does not yet encompas GetSpot
				while(Top < GetSpot && Spot < GetSpot){
					Spot++;
					for(int d = 0; d < NumOfDerivs; d++){
						Coefs[d] = Derivs[d][Spot];
					}
					if(!CalculateLenghtOfFit(Bottom,Top)){
						Spot--;
						for(int d = 0; d < NumOfDerivs; d++){
							Coefs[d] = Derivs[d][Spot];
						}
						break;
					}
				}
			}
			else{//if it encompasses GetSpot too much
				while(Top > GetSpot && Spot > 0){
					Spot--;
					for(int d = 0; d < NumOfDerivs; d++){
						Coefs[d] = Derivs[d][Spot];
					}
					if(!CalculateLenghtOfFit(Bottom,Top))
						break;
					if(Top >= GetSpot && Bottom <= 1){
						Spot--;//ends up being the original becasue of the ++ below
						break;
					}
				}
				//reverts to one that is definitely there
				Spot++;
				for(int d = 0; d < NumOfDerivs; d++){
					Coefs[d] = Derivs[d][Spot];
				}
			}
		}
		TaylorSpot = Spot;
		CalculateLenghtOfFit(FBottom,FTop);
		if(Above && FBottom > GetSpot || !Above && FTop < GetSpot || FBottom == -1)
			return false;
		else
			return true;
	}
	void PrintGraphBetweenPoints(HDC hdc,double XVal,double YVal){
		int Spot = FBottom,Top = FTop;
		HPEN hPenOLd;
		HPEN hLinePen;
		COLORREF qLineColor;
		qLineColor = RGB(0,255,0);
		// print the function itself

		hLinePen = CreatePen(PS_SOLID, 1, qLineColor);
		hPenOLd = (HPEN)SelectObject(hdc, hLinePen);
		int xspot,yspot;
		xspot = (Spot - CenterNum) * XVal + 690;
		yspot = -Calculate(Spot*XScale) * YVal + 345;
		Spot++;
		while(Spot < Top){
			MoveToEx(hdc,xspot, yspot, NULL);
			Spot++;
			xspot = (Spot - CenterNum) * XVal + 690;
			yspot = - Calculate(Spot*XScale) * YVal + 345;

			LineTo (hdc,xspot, yspot);
		}
		
		// print the blue length lines
		qLineColor = RGB(0,0,255);
		hLinePen = CreatePen(PS_SOLID, 2, qLineColor);
		hPenOLd = (HPEN)SelectObject(hdc, hLinePen);
		
		xspot = (FBottom - CenterNum) * XVal + 690;
		yspot = - Calculate(FBottom*XScale) * YVal + 345;
		MoveToEx(hdc,xspot, yspot - 5, NULL);
		LineTo (hdc,xspot, yspot + 5);
		
		xspot = (FTop - CenterNum) * XVal + 690;
		yspot = - Calculate(FTop*XScale) * YVal + 345;
		MoveToEx(hdc,xspot, yspot - 5, NULL);
		LineTo (hdc,xspot, yspot + 5);
	}
private:
	void CalculateCoefsAndDerivs(vector <vector<double>> & Derivs){
		if(NumOfDerivs > 19)//factorial can't handle anything over 19
			return;
		Coefs.clear();
		Derivs.resize(NumOfDerivs);
		for(int g = 0; g < NumOfDerivs; g++)
			Derivs[g].resize(Size);
		Nums = OrigiNums;
		CalcuateMeans();
		Derivs[0] = Nums;
		Coefs.push_back(Nums[TaylorSpot]);

		for(int g = 1; g < NumOfDerivs; g++){
			CalculateFirstDerivSpot();
			for(int t = 1; t < Size; t++){
				CalculateNextDerivSpot(t);
			}
			Coefs.push_back(NextDeriv[TaylorSpot]);
			Nums = NextDeriv;
			Derivs[g] = NextDeriv;
		}
	}
};

