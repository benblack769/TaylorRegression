#pragma once
#include "stdafx.h"
void PrintSymbol(char Charachter,HDC hdc,int & xspot,int yspot);
void PrintDigit(int Num,HDC hdc,int & xspot,int yspot);
void PrintInt(int Num,HDC hdc,int & xspot,int yspot);
void PrintFloat(double Num,HDC hdc,int & xspot,int yspot);
void SortVector(vector <double> & Nums);//slow!!
inline double Square(double x){
	return x*x;
}
inline double Abs(double x){
	if(x < 0)
		return -x;
	else
		return x;
}
inline __int64 Fact(int x){
	static __int64 y[20] = {1,1,2,6,24,120,720,5040,40320,362880,3628800,39916800,479001600,
		6227020800,87178291200,1307674368000,20922789888000,355687428096000,6402373705728000,121645100408832000};
	
	return y[x];
}
inline int Int(int x){
	return x;
}
class Function
{
public:
	Function(void){}
	~Function(void){};
	int FTop, FBottom;//top and bottom of usable function
	bool Auto;
	int TaylorSpot;
	int NumOfDerivs;
	double Precision;//basically the maximum residual allowed
	double XScale;
	int RegressionLength;//current length of lookaround, two sided
	
	double Calculate(double x){
		x -= TaylorSpot * XScale;
		double y = Coefs[0];
		y += Coefs[1] * x;
		int Size = Coefs.size();
		while(Size > 2){
			Size--;//this needs to be in front do it doesn't overflow the vector
			y += Coefs[Size] * pow(x,Size) / Fact(Size);
		}
		return y;
	}
	bool CalculateLenghtOfFit(int & OutStartSpot,int & OutEndSpot){
		OutEndSpot = -1;
		OutStartSpot = -1;
		int Spot = TaylorSpot;
		if(Abs(OrigiNums[TaylorSpot] - Calculate(TaylorSpot * XScale)) > Precision)
			return false;
		int JumpLength = DefLookArround / 4;
		if(JumpLength == 0)
			JumpLength = 1;
		int EndCoord = Size - 1;
		
		while(Spot + JumpLength <= EndCoord)
			if(Abs(OrigiNums[Spot + JumpLength] - Calculate((Spot + JumpLength) * XScale)) < Precision)
				Spot += JumpLength;
			else
				break;
		while(Spot + 1 <= EndCoord)
			if(Abs(OrigiNums[Spot + 1] - Calculate((Spot + 1) * XScale)) < Precision)
				Spot += 1;
			else
				break;
		OutEndSpot = Spot;
		
		Spot = TaylorSpot;
		while(Spot - JumpLength >= 0)
			if(Abs(OrigiNums[Spot - JumpLength] - Calculate((Spot - JumpLength) * XScale)) < Precision)
				Spot -= JumpLength;
			else
				break;
		while(Spot - 1 >= 0)
			if(Abs(OrigiNums[Spot - 1] - Calculate((Spot - 1) * XScale)) < Precision)
				Spot -= 1;
			else
				break;
		OutStartSpot = Spot;
		return true;
	}
	
	int CalculateLenghtOfFit(){
		int StartSpot,EndSpot;
		CalculateLenghtOfFit(StartSpot,EndSpot);
		return EndSpot - StartSpot;
	}
	void SetTaylorCoordSpot(int Coord){//zero does not matter
		if(Coord < 0 || Coord >= Size)
			return;
		TaylorSpot = Coord;
		if(Auto)
			CalculateOptimumLengthAndDeriv();
		else
			CalculateCoefs();
		CalculateLenghtOfFit(FBottom,FTop);
	}
protected:
	vector <double> Coefs;
	vector <double> OrigiNums;
	vector <double> Nums;
	vector <double> NextDeriv;
	int CenterNum;//refrence number
	int Size;//size of Num Vector
	int DefLookArround;//appox of RegressionLengh
	double CXMean,CYMean,CxySum, Cx2Sum;//optimises derivative calculation
	int CEndSpot, CStartSpot,CNumOfSpots;//optimises derivative calculation

	void CalculateCoefs(){
		if(NumOfDerivs > 19)//factorial can't handle anything over 19
			return;
		Coefs.clear();
		Nums = OrigiNums;
		//double Mean = 0;
		//int LLookArround = RegressionLength,RLookArround = RegressionLength;
		//if(TaylorSpot + RLookArround > Size - 1)
		//	RLookArround = Size - 1 - TaylorSpot;
		//if(TaylorSpot - LLookArround < 0)
		//	LLookArround = TaylorSpot;
		////adds up nums and then divides it by the times to find the mean value
		//for(int x = -LLookArround + TaylorSpot; x <= RLookArround + TaylorSpot; x++){
		//	Mean += Nums[x];}
		//Mean /= 1 + LLookArround + RLookArround;
		CalcuateMeans();
		Coefs.push_back(Nums[TaylorSpot]);
		Nums = OrigiNums;
		CalculateFirstDerivSpot();
		for(int t = 1; t < Size; t++){
			CalculateNextDerivSpot(t);
		}
		Nums = NextDeriv;
		for(int g = 1; g < NumOfDerivs; g++){
			CalculateFirstDerivSpot();
			for(int t = 1; t < Size; t++){
				CalculateNextDerivSpot(t);
			}
			Coefs.push_back(Nums[TaylorSpot]);
			Nums = NextDeriv;
		}
	}
	void CalcuateMeans(){
		double Mean = 0;
		for(int g = 0; g <= RegressionLength; g++){
			Mean += OrigiNums[g];
		}
		Nums[0] = Mean / (1 + RegressionLength);
		//adds up nums and then divides it by the times to find the mean value
		int PEndSpot = RegressionLength,PStartSpot = 0;
		int PNumOfSpots = 1 + RegressionLength;
		for(int t = 0; t < Size; t++){
			int EndSpot = t + RegressionLength, StartSpot = t - RegressionLength;
			if(t + RegressionLength > Size - 1)
				EndSpot = Size - 1;
			if(t - RegressionLength < 0)
				StartSpot = 0;
			int NumberOfSpots = 1 + EndSpot - StartSpot;
			if(PEndSpot < EndSpot && PStartSpot < StartSpot){
				Mean = Mean + (Nums[EndSpot] - Nums[PStartSpot]) / NumberOfSpots;
			}
			else if(PEndSpot < EndSpot){
				Mean = ((Mean * PNumOfSpots) + Nums[EndSpot]) / NumberOfSpots;
			}
			else{//PStartSpot < StartSpot	
				Mean = ((Mean * PNumOfSpots) - Nums[PStartSpot]) / NumberOfSpots;
			}
			Nums[t] = Mean;
			PEndSpot = EndSpot;
			PStartSpot = StartSpot;
			PNumOfSpots = NumberOfSpots;
		}
	}
	void CalculateOptimumLengthAndDeriv(){
		NumOfDerivs = 2;
		int BestLength = 0;
		int BestDeriv = 2;//default values
		int BestRegLength = DefLookArround;
		while(NumOfDerivs <= 13){
			if(DefLookArround / 4 > 0)
				RegressionLength = DefLookArround / 4;
			else
				RegressionLength = DefLookArround;
			int Marker = 0;
			int CBestLength = 0;
			while(true){
				CalculateCoefs();
				int Length = CalculateLenghtOfFit();
				if(Length > CBestLength){
					CBestLength = Length;
					Marker = 0;
					if(CBestLength > BestLength){
						BestLength = CBestLength;
						BestDeriv = NumOfDerivs;
						BestRegLength = RegressionLength;
					}
				}
				else
					Marker++;
				if(Marker >= 5)
					break;
				RegressionLength++;
			}
			NumOfDerivs++;
		}
		RegressionLength = BestRegLength;
		NumOfDerivs = BestDeriv;
		CalculateCoefs();
		
	}
	void CalculateFirstDerivSpot(){
		int EndSpot = RegressionLength, StartSpot = 0;
		if(RegressionLength > Size - 1)
			EndSpot = Size - 1;
		int NumOfSpots = 1 + EndSpot;

		CEndSpot = EndSpot;
		CStartSpot = 0;
		CNumOfSpots = NumOfSpots;
		double MeanX = 0,MeanY = 0;
		//adds up nums and then divides it by the times to find the mean value
		for(int x = 0; x <= EndSpot; x++){
			MeanX += x;
			MeanY += Nums[x];
		}
		MeanX /= NumOfSpots;
		MeanY /= NumOfSpots;
		//finish this if you want a full liniar regression
		
		double xysum = 0,x2sum = 0,RxySum = 0;
		for(int x = 0; x <= EndSpot; x++){
			x2sum += Square(MeanX - x);
			xysum += (MeanY - Nums[x]) * (MeanX - x);
			RxySum += Nums[x] * x;
		}
		CXMean = MeanX;
		CYMean = MeanY;
		CxySum = RxySum;
		Cx2Sum = x2sum;

		NextDeriv[0] = xysum / (x2sum * XScale);
	}
	void CalculateNextDerivSpot(int Spot){
		int EndSpot = Spot + RegressionLength, StartSpot = Spot - RegressionLength;
		if(Spot + RegressionLength > Size - 1)
			EndSpot = Size - 1;
		if(Spot - RegressionLength < 0)
			StartSpot = 0;
		int NumberOfSpots = 1 + EndSpot - StartSpot;
		if(CEndSpot < EndSpot && CStartSpot < StartSpot){//if it is simply incrementing, NumberOfSpots should be the same
			/*Cx2Sum = Cx2Sum - Square(CXMean - CStartSpot) + Square(CXMean - EndSpot);
			CxySum = CxySum - (CXMean - CStartSpot) * (CYMean - Nums[Spot - CStartSpot])
				+ (CXMean + EndSpot) * (CYMean - Nums[Spot + EndSpot]);
				*/
			//xy is incrementing, needs to have the other stuff added to it
			CxySum += -CStartSpot * Nums[CStartSpot] + EndSpot * Nums[EndSpot];
			CXMean += 1;//spot should be approximately equal to this
			CYMean = CYMean + (Nums[EndSpot] - Nums[CStartSpot]) / NumberOfSpots;
		}
		else if(CEndSpot < EndSpot){/*
			Cx2Sum = Cx2Sum + Square(CXMean + EndSpot);
			CxySum = CxySum + (CXMean + EndSpot) * Nums[Spot + EndSpot];
*/			//xy is incrementing, needs to have the other stuff added to it
			CxySum += EndSpot * Nums[EndSpot];
			CXMean = ((CXMean * CNumOfSpots) + EndSpot) / NumberOfSpots;
			CYMean = ((CYMean * CNumOfSpots) + Nums[EndSpot]) / NumberOfSpots;
		}
		else if(CStartSpot < StartSpot){/*
			Cx2Sum = Cx2Sum - Square(CXMean - StartSpot);
			CxySum = CxySum - (CXMean - StartSpot) * (CYMean - Nums[Spot - CStartSpot]);
*/		//xy is incrementing, needs to have the other stuff added to it
			CxySum += -CStartSpot * Nums[CStartSpot];
			CXMean = (CXMean * CNumOfSpots - CStartSpot) / NumberOfSpots;
			CYMean = ((CYMean * CNumOfSpots) - Nums[CStartSpot]) / NumberOfSpots;
		}
		//strait calculation function for x
		Cx2Sum = (EndSpot * (EndSpot + 1) * (2 * EndSpot + 1) - StartSpot * (StartSpot - 1) * (2 * StartSpot - 1)) / 6.0 - CXMean * CXMean * NumberOfSpots;

		CStartSpot = StartSpot;
		CEndSpot = EndSpot;
		CNumOfSpots = NumberOfSpots;
		NextDeriv[Spot] = (CxySum - CXMean * CYMean * NumberOfSpots) / (Cx2Sum * XScale);
	}
	void SortVectorInUnisonWithVector(vector <unsigned int> & Nums,vector <unsigned int> & UniNums){
		if(Nums.size() != UniNums.size())
			return;
		int Size = Nums.size();
		unsigned int TempNum;
		unsigned int UniTempNum;
		for(int g = 0; g < Size; g++){
			for(int f = 0; f < Size; f++){
				if(Nums[f] > Nums[g]){//if it is lesser, swap values
					TempNum = Nums[g];
					UniTempNum = UniNums[g];

					Nums[g] = Nums[f];
					UniNums[g] = UniNums[f];

					Nums[f] = TempNum;
					UniNums[f] = UniTempNum;
				}
			}
		}
	}
};
