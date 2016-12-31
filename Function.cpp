#include "Function.h"
	void PrintSymbol(char Charachter,HDC hdc,int & xspot,int yspot){
		switch(Charachter){
		case ' ':
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T(" "),1,NULL);
			break;
		case '+':
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("+"),1,NULL);
			break;
		case '!':
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("!"),1,NULL);
			break;
		case '^':
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("^"),1,NULL);
			break;
		case 'X':
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("X"),1,NULL);
			break;
		case 'E':
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("E"),1,NULL);
			break;
		case '.':
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("."),1,NULL);
			break;
		case '-':
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("-"),1,NULL);
			break;
		}
		xspot += 8;
	}
	void PrintDigit(int Num,HDC hdc,int & xspot,int yspot){
		switch(Num){
		case 0:
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("0"),1,NULL);
			break;
		case 1:
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("1"),1,NULL);
			break;
		case 2:
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("2"),1,NULL);
			break;
		case 3:
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("3"),1,NULL);
			break;
		case 4:
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("4"),1,NULL);
			break;
		case 5:
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("5"),1,NULL);
			break;
		case 6:
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("6"),1,NULL);
			break;
		case 7:
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("7"),1,NULL);
			break;
		case 8:
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("8"),1,NULL);
			break;
		case 9:
			ExtTextOut(hdc,xspot,yspot,ETO_NUMERICSLATIN,NULL ,_T("9"),1,NULL);
			break;
		}
		xspot += 8;
	}
	void PrintInt(int Num,HDC hdc,int & xspot,int yspot){
		if(Num == 0){
			PrintDigit(0,hdc,xspot,yspot);
			return;
		}
		if(Num < 0){
			PrintSymbol('-',hdc,xspot,yspot);
			Num = -Num;
		}
		long Marker = 10000000;
		int k = 0;
		// gets marker down to just below money
		while (Marker > Num) 
			Marker = Marker / 10;
		//breaks it down to its number
		while (Marker >= 1){
			PrintDigit(Num / Marker,hdc,xspot,yspot);
			Num %= Marker;
			Marker /= 10;
			k++;
		}
	}
	void PrintFloat(double Num,HDC hdc,int & xspot,int yspot){
		if(Num == 0 || Abs(Num) > 1000000000000000.0)//secures the function against oddities
			return ;
		int OrigiXSpot = xspot;
		if(Num < 0){
			PrintSymbol('-',hdc,xspot,yspot);
			Num = -Num;
		}
		double SciNum = Abs(Num);
		int EE = 0;
		while(SciNum >= 10){
			SciNum /= 10;
			EE++;
		}
		while(SciNum < 1){
			SciNum *= 10;
			EE--;
		}
		//scinum is now between 1 and 10
		PrintDigit(Int(SciNum),hdc,xspot,yspot);//converts it to integer to print the first digit
		PrintSymbol('.',hdc,xspot,yspot);//prints the decimal point
		PrintInt(Int(SciNum * 10000) % 1000,hdc,xspot,yspot);//prints the next 4 digits
		xspot += 2;
		PrintSymbol('E',hdc,xspot,yspot);//prints the E symbol
		PrintInt(EE,hdc,xspot,yspot);//prints the power
	}
	void SortVector(vector <double> & Nums){
		int Size = Nums.size();
		double TempNum;
		for(int g = 0; g < Size; g++){
			for(int f = 0; f < Size; f++){
				if(Nums[f] > Nums[g]){//if it is greater, swap values
					TempNum = Nums[g];
					Nums[g] = Nums[f];
					Nums[f] = TempNum;
				}
			}
		}
	}