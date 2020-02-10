/* 33cz1

Compile: g++ -O2 33cz1.cpp -o 33cz1
RUN: ./33cz1

*/


// basic file operations
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <time.h>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atof */
#include <math.h>
#include <iomanip>
#include <limits>       // std::numeric_limits
#include <cfloat>

using std::vector;

using namespace std;


/////////////////////////////////////////////////


//GeoTIFF:
struct coordinatedata{
	string filename;
	int X;
	int Y;
	double LONGI; //X coordinate
	double LATI; //Y coordinate
	double elev;
	//double Neerslag
	string vakindex;
	int KNMI_X; //Vakindex - X
	int KNMI_Y; //Vakindex - Y
	int vakvakje_X; //vakindex in een vak - X
	int vakvakje_Y; //vakindex in een vak - Y
} schaap[1250][1000];

////////////////////////
//KNMI:
struct KNMIdata{
	double KNMI_LONGI; //X coordinate
	double KNMI_LATI; //Y coordinate
	int KNMI_X; //Vakindex - X
	int KNMI_Y; //Vakindex - Y
	double LTRstaprangeLONG;
	double LTRstaprangeLAT;
	double TTBstaprangeLONG;
	double TTBstaprangeLAT;
//	int vakvakje_X; //vakindex in een vak - X
//	int vakvakje_Y; //vakindex in een vak - Y
} geit[766][701]; //Extra row + column voor de laatste grenzen van de laatste vakjes.


//KNMI: Vakjes in Vak.
struct KNMIvakjes{
	double v_LONGI;
	double v_LATI;
	int v_X;
	int v_Y;
} koe[201][201];

//////////////////////////////////////


//maximale grootte matrix - GeoTIFF:
const int MMAX = 1000; //column = X
const int NMAX = 1250; //row = Y


//maximale grootte matrix - KNMI:
const int JMAX = 700; //column = X
const int KMAX = 765; //row = Y


//maximale grootte matrix - KNMI:
const int GMAX = 200; //column = X
const int HMAX = 200; //row = Y


//////////////////////////////////////////////////


class dataset{
    public:
		dataset(string file);	//als parameter een string mee voor bestandsnaam
				
		
    private:		

};


//INITIALISATIE
dataset::dataset(string file){	
	//Variables:
	int i;
	int j;
	int p;
	int q;
	
	int r;
	int s;
	int u;
	int v;
	
//GeoTIFF:	
	//Counter coordinates:
	double ULLong = 5.7525083333333;
	double ULLat = 52.0949194444445;
	double URLong = 5.8254805555556;
	double URLat = 52.0946722222222;
	double LLLong = 5.7520500000000;
	double LLLat = 52.0387388888889;
	double LRLong = 5.8249305555556; 
	double LRLat = 52.0384916666667;
	
	/*
	double pixelstep_lefttorightLONG = (URLong - ULLong) / MMAX-1;
	double pixelstep_lefttorightLAT = (URLat - ULLat) / MMAX-1;
	double pixelstep_toptobottomLONG = (LLLong - ULLong) / NMAX-1;
	double pixelstep_toptobottomLAT = (LLLat - ULLat) / NMAX-1;*/

	//Stappen voor de randen:
	//left to right from Upperleft corner:
	double firstpixelstep_lefttorightLONG = (URLong - ULLong) / (MMAX-1);
	double firstpixelstep_lefttorightLAT = (URLat - ULLat) / (MMAX-1);
	//Up - down from Upperleft corner:
	double firstpixelstep_toptobottomLONG = (LLLong - ULLong) / (NMAX-1);
	double firstpixelstep_toptobottomLAT = (LLLat - ULLat) / (NMAX-1);

	//Left to right from Lowerleft corner:
	double lastpixelstep_lefttorightLONG = (LRLong - LLLong) / (MMAX-1);
	double lastpixelstep_lefttorightLAT = (LRLat - LLLat) / (MMAX-1);
	//Up - down from Upperright corner:
	double lastpixelstep_toptobottomLONG = (LRLong - URLong) / (NMAX-1);
	double lastpixelstep_toptobottomLAT = (LRLat - URLat) / (NMAX-1);



///////////////////////////
//KNMI:
	//Counter coordinates:
	double VULLong = 0.0000000;
	double VULLat = 49.3620640;
	double VURLong = 0.0000000;
	double VURLat = 55.9736020;
	double VLLLong = 9.0093000;
	double VLLLat = 48.8953000;
	double VLRLong = 10.8564530;
	double VLRLat = 55.3889730;



//Stappen voor de randen:
	//left to right from Upperleft corner:
	double Vfirstpixelstep_lefttorightLONG = (VURLong - VULLong) / JMAX;
	double Vfirstpixelstep_lefttorightLAT = (VURLat - VULLat) / JMAX;
	//Up - down from Upperleft corner:
	double Vfirstpixelstep_toptobottomLONG = (VLLLong - VULLong) / KMAX;
	double Vfirstpixelstep_toptobottomLAT = (VLLLat - VULLat) / KMAX;

	//Left to right from Lowerleft corner:
	double Vlastpixelstep_lefttorightLONG = (VLRLong - VLLLong) / JMAX;
	double Vlastpixelstep_lefttorightLAT = (VLRLat - VLLLat) / JMAX;
	//Up - down from Upperright corner:
	double Vlastpixelstep_toptobottomLONG = (VLRLong - VURLong) / KMAX;
	double Vlastpixelstep_toptobottomLAT = (VLRLat - VURLat) / KMAX;



////////////////////////////////////////////////////
//EPSILON:

//Is said to replace the code below but I wanted to be extremely sure:
//std::cout << "Epsilon: " << std::numeric_limits<double>::epsilon() << '\n';
  
  
  // taking a floating type variable 
    double prev_epsilon;
    double EPS = 0.5;
  
    // run until condition satisfy 
    while((1+EPS) != 1){ 
        // copying value of epsilon into previous epsilon 
        prev_epsilon = EPS; 
  
        // dividing epsilon by 2 
        EPS /=2; 
    } 
  
    // print output of the program 
    cout << "Machine Epsilon: " << prev_epsilon << endl;



///////////////////////////////////////////////////////////////////
//GeoTIFF - Fill in Longitude and Latitude Matrices:
//Might need adjustment (same code as for KNMI matrices).

	for(p=0; p<NMAX; p++){
		for(q=0; q<MMAX; q++){
			schaap[p][q].LONGI = -1;
			schaap[p][q].LATI = -1;
			schaap[p][q].KNMI_X = -1;
			schaap[p][q].KNMI_Y = -1;
			schaap[p][q].vakvakje_X = -1;
			schaap[p][q].vakvakje_Y = -1;
			schaap[p][q].X = -1;
			schaap[p][q].Y = -1;
			schaap[p][q].elev = -1;
		}
	}



	
	//Randen eerst vullen:
	//Linkerbovenhoek:
	schaap[0][0].LONGI = ULLong;
	schaap[0][0].LATI = ULLat;
	schaap[0][0].X = 0;
	schaap[0][0].Y = 0;
	
	//Rechterbovenhoek:
	schaap[0][999].LONGI = URLong;
	schaap[0][999].LATI = URLat;
	schaap[0][999].X = 999;
	schaap[0][999].Y = 0;
	
	//Linkeronderhoek:
	schaap[1249][0].LONGI = LLLong;
	schaap[1249][0].LATI = LLLat;
	schaap[1249][0].X = 0;
	schaap[1249][0].Y = 1249;
	
	//Rechteronderhoek:
	schaap[1249][999].LONGI = LRLong;
	schaap[1249][999].LATI = LRLat;
	schaap[1249][999].X = 999;
	schaap[1249][999].Y = 1249;
	
	
	//Temporary Variables:
	int LTRLONGtemp = 0;
	int TTBLONGtemp = 0;
	int LTRLATtemp = 0;	//Not used. Yet.
	int TTBLATtemp = 0;	//Not used. Yet.
	
	
	
	//Vul randen GeoTIFF Matrix:
	for(q=1; q<(MMAX-1); q++){
		if((schaap[0][q].LONGI == -1) || (schaap[0][q].LATI == -1)){
			schaap[0][q].LONGI = schaap[0][q-1].LONGI + firstpixelstep_lefttorightLONG;
			schaap[0][q].LATI = schaap[0][q-1].LATI + firstpixelstep_lefttorightLAT;
			schaap[0][q].X = q;
			schaap[0][q].Y = 0;
		}
		if((schaap[1249][q].LONGI == -1) || (schaap[1249][q].LATI == -1)){
			schaap[1249][q].LONGI = schaap[1249][q-1].LONGI + lastpixelstep_lefttorightLONG;
			schaap[1249][q].LATI = schaap[1249][q-1].LATI + lastpixelstep_lefttorightLAT;
			schaap[1249][q].X = q;
			schaap[1249][q].Y = 1249;
		}
	}
		
		
	for(p=1; p<(NMAX-1); p++){
		if((schaap[p][0].LONGI == -1) || (schaap[p][0].LATI == -1)){
			schaap[p][0].LONGI = schaap[p-1][0].LONGI + firstpixelstep_toptobottomLONG;
			schaap[p][0].LATI = schaap[p-1][0].LATI + firstpixelstep_toptobottomLAT;
			schaap[p][0].X = 0;
			schaap[p][0].Y = p;
		}
		if((schaap[p][999].LONGI == -1) || (schaap[p][999].LATI == -1)){
			schaap[p][999].LONGI = schaap[p-1][999].LONGI + lastpixelstep_toptobottomLONG;
			schaap[p][999].LATI = schaap[p-1][999].LATI + lastpixelstep_toptobottomLAT;
			schaap[p][999].X = 999;
			schaap[p][999].Y = p;
		}
	}
	

	
	//Vul gebied binnen de randen - GeoTIFF Matrix (Verticaal invullen):
	for(q=1; q<(MMAX-1); q++){
		TTBLONGtemp = (schaap[1249][q].LONGI - schaap[0][q].LONGI) / (NMAX-1);
		TTBLATtemp = (schaap[1249][q].LATI - schaap[0][q].LATI) / (NMAX-1);
		for(p=1; p<(NMAX-1); p++){
			if((schaap[p][q].LONGI == -1) || (schaap[p][q].LATI == -1)){
				schaap[p][q].LONGI = schaap[p-1][q].LONGI + TTBLONGtemp;
				schaap[p][q].LATI = schaap[p-1][q].LATI + TTBLATtemp;
			}
		}
	}



  
/*	//Longitude 
	for(p=0; p<NMAX; p++){
		for(q=0; q<MMAX; q++){
			if((LONG[p][q] == -1) || (LAT[p][q] == -1)){
			//Corners are filled in:
				if((p == 0) && (q == 0)){
					LONG[p][q] = ULLong;
					LAT[p][q] = ULLat;
				}
				else if((p == 0) && (q == MMAX-1)){
					LONG[p][q] = URLong;
					LAT[p][q] = URLat;
				}
				else if((p == NMAX-1) && (q == 0)){
					LONG[p][q] = LLLong;
					LAT[p][q] = LLLat;
				}
				else if((p == NMAX-1) && (q == MMAX-1)){
					LONG[p][q] = LRLong;
					LAT[p][q] = LRLat;
				}
				
			//Everything inside the corners is filled in: Moet wellicht hetzelfde als KNMI data.	
				else{
					if(p == 0){
						LONG[p][q] = LONG[p][q-1] + pixelstep_lefttorightLONG;
						LAT[p][q] = LAT[p][q-1] + pixelstep_lefttorightLAT;
					}
					else if(q == 0){
						LONG[p][q] = LONG[p-1][q] + pixelstep_toptobottomLONG;
						LAT[p][q] = LAT[p-1][q] + pixelstep_toptobottomLAT;
					}
					else{
						LONG[p][q] = LONG[p-1][q-1] + pixelstep_lefttorightLONG + pixelstep_toptobottomLONG;
						LAT[p][q] = LAT[p-1][q-1] + pixelstep_lefttorightLAT + pixelstep_toptobottomLAT;
					}
				}
			}
		}		
	}*/

///////////////////////////////////////////////////////////////////////////
//KNMI - Fill in Longitude and Latitude Matrices:


	//Initialise Longitude & Latitude Matrices: -1'en:
	for(r=0; r<=KMAX; r++){
		for(s=0; s<=JMAX; s++){
			//VLONG[r][s] = -1;
			//VLAT[r][s] = -1;
			
			geit[r][s].KNMI_LONGI = -1;
			geit[r][s].KNMI_LATI = -1;
			geit[r][s].KNMI_X = -1;
			geit[r][s].KNMI_Y = -1;
			geit[r][s].LTRstaprangeLAT = -1;
			geit[r][s].LTRstaprangeLONG = -1;
			geit[r][s].TTBstaprangeLAT = -1;
			geit[r][s].TTBstaprangeLONG = -1;
			//KNMILONG[r][s] = -1;
			//KNMILAT[r][s] = -1;
		}
	}


/*
	//Randen eerst vullen:
	//Linkerbovenhoek:
	VLONG[0][0] = VULLong;
	VLAT[0][0] = VULLat;
	
	//Rechterbovenhoek:
	VLONG[0][699] = VURLong;
	VLAT[0][699] = VURLat;
	
	//Linkeronderhoek:
	VLONG[764][0] = VLLLong;
	VLAT[764][0] = VLLLat;
	
	//Rechteronderhoek:
	VLONG[764][699] = VLRLong;
	VLAT[764][699] = VLRLat;
		
	
	
	
	
	//Vul randen KNMI Temp LONG/LAT Matrices:
	for(s=1; s<JMAX-1; s++){
		if((VLONG[0][s] == -1) || (VLAT[0][s] == -1)){
			VLONG[0][s] = VLONG[0][s-1] + Vfirstpixelstep_lefttorightLONG;
			VLAT[0][s] = VLAT[0][s-1] + Vfirstpixelstep_lefttorightLAT;
		}
		if((VLONG[764][s] == -1) || (VLAT[764][s] == -1)){
			VLONG[764][s] = VLONG[764][s-1] + Vlastpixelstep_lefttorightLONG;
			VLAT[764][s] = VLAT[764][s-1] + Vlastpixelstep_lefttorightLAT;
		}
	}
	
	for(r=1; r<KMAX-1; r++){
		if((VLONG[r][0] == -1) || (VLAT[r][0] == -1)){
			VLONG[r][0] = VLONG[r-1][0] + Vfirstpixelstep_toptobottomLONG;
			VLAT[r][0] = VLAT[r-1][0] + Vfirstpixelstep_toptobottomLAT;
		}
		if((VLONG[r][699] == -1) || (VLAT[r][699] == -1)){
			VLONG[r][699] = VLONG[r-1][699] + Vlastpixelstep_toptobottomLONG;
			VLAT[r][699] = VLAT[r-1][699] + Vlastpixelstep_toptobottomLAT;
		}
	}*/
	
	
////////////////////////////////////////////////////////////////////////////////
//Stappen voor de randen:
	//left to right from Upperleft corner:
	//double Vfirstpixelstep_lefttorightLONG = (VURLong - VULLong) / JMAX;
	//double Vfirstpixelstep_lefttorightLAT = (VURLat - VULLat) / JMAX;
	//Up - down from Upperleft corner:
	//double Vfirstpixelstep_toptobottomLONG = (VLLLong - VULLong) / KMAX;
	//double Vfirstpixelstep_toptobottomLAT = (VLLLat - VULLat) / KMAX;

	//Left to right from Lowerright corner:
	//double Vlastpixelstep_lefttorightLONG = (VLRLong - VLLLong) / JMAX;
	//double Vlastpixelstep_lefttorightLAT = (VLRLat - VLLLat) / JMAX;
	//Up - down from Lowerright corner:
	//double Vlastpixelstep_toptobottomLONG = (VLRLong - VURLong) / KMAX;
	//double Vlastpixelstep_toptobottomLAT = (VLRLat - VURLat) / KMAX;
	
	
	
	//Temporary Variables:
	//int firstlefttorightLONGtemp = (VURLong - VULLong) / JMAX;
	//int firstlefttorightLATtemp = (VURLat - VULLat) / JMAX;
	//int lastlefttorightLONGtemp = (VLRLong - VLLLong) / JMAX;
	//int lastlefttorightLATtemp = (VLRLat - VLLLat) / JMAX;
	//int firsttoptobotLONGtemp = (VLLLong - VULLong) / KMAX;
	//int firsttoptobotLATtemp = (VLLLat - VULLat) / KMAX;
	//int lasttoptobotLONGtemp = (VLRLong - VURLong) / KMAX;
	//int lasttoptobotLATtemp = (VLRLat - VURLat) / KMAX;
	
	double toptobotLONGtemp = 0;
	double toptobotLATtemp = 0;
	double lefttorightLONGtemp = 0;
	double lefttorightLATtemp = 0;
	/////////////////////////////////////////
	
	
	//Linkerbovenhoek vullen - KNMI struct:
	geit[0][0].KNMI_LONGI = VULLong;
	geit[0][0].KNMI_LATI = VULLat;
	geit[0][0].LTRstaprangeLONG = Vfirstpixelstep_lefttorightLONG;
	geit[0][0].LTRstaprangeLAT = Vfirstpixelstep_lefttorightLAT;
	geit[0][0].TTBstaprangeLONG = Vfirstpixelstep_toptobottomLONG;
	geit[0][0].TTBstaprangeLAT = Vfirstpixelstep_toptobottomLAT;
	geit[0][0].KNMI_X = 0;
	geit[0][0].KNMI_Y = 0;
	
	
//////////////////////////////////////	
	//Vul linkeronderhoek:
	geit[765][0].KNMI_LONGI = VLLLong;
	geit[765][0].KNMI_LATI = VLLLat;
	geit[765][0].LTRstaprangeLONG = Vlastpixelstep_lefttorightLONG;
	geit[765][0].LTRstaprangeLAT = Vlastpixelstep_lefttorightLAT;
	geit[765][0].TTBstaprangeLONG = Vfirstpixelstep_toptobottomLONG;
	geit[765][0].TTBstaprangeLAT = Vfirstpixelstep_toptobottomLAT;
	geit[765][0].KNMI_X = 0;
	geit[765][0].KNMI_Y = 765;
	
	//Vul rechterbovenhoek:
	geit[0][700].KNMI_LONGI = VURLong;
	geit[0][700].KNMI_LATI = VURLat;
	geit[0][700].LTRstaprangeLONG = Vfirstpixelstep_lefttorightLONG;
	geit[0][700].LTRstaprangeLAT = Vfirstpixelstep_lefttorightLAT;
	geit[0][700].TTBstaprangeLONG = Vlastpixelstep_toptobottomLONG;
	geit[0][700].TTBstaprangeLAT = Vlastpixelstep_toptobottomLAT;
	geit[0][700].KNMI_X = 700;
	geit[0][700].KNMI_Y = 0;
	
	
	//Vul rechteronderhoek:
	geit[765][700].KNMI_LONGI = VLRLong;
	geit[765][700].KNMI_LATI = VLRLat;
	geit[765][700].LTRstaprangeLONG = Vlastpixelstep_lefttorightLONG;
	geit[765][700].LTRstaprangeLAT = Vlastpixelstep_lefttorightLAT;
	geit[765][700].TTBstaprangeLONG = Vlastpixelstep_toptobottomLONG;
	geit[765][700].TTBstaprangeLAT = Vlastpixelstep_toptobottomLAT;
	geit[765][700].KNMI_X = 700;
	geit[765][700].KNMI_Y = 765;
	
	
	
	for(r=1; r<KMAX; r++){
		//Vul rand tussen linkerbovenhoek en linkeronderhoek.:
		if((geit[r][0].KNMI_LONGI == -1) || (geit[r][0].KNMI_LATI == -1)){  
			geit[r][0].KNMI_LONGI = geit[r-1][0].KNMI_LONGI + Vfirstpixelstep_toptobottomLONG;
			geit[r][0].KNMI_LATI = geit[r-1][0].KNMI_LATI + Vfirstpixelstep_toptobottomLAT;
			geit[r][0].TTBstaprangeLONG = Vfirstpixelstep_toptobottomLONG;
			geit[r][0].TTBstaprangeLAT = Vfirstpixelstep_toptobottomLAT;
			geit[r][0].KNMI_X = 0;
			geit[r][0].KNMI_Y = r;
		}
		
		//Vul rand tussen rechterbovenhoek en rechteronderhoek.:
		if((geit[r][700].KNMI_LONGI == -1) || (geit[r][700].KNMI_LATI == -1)){  
			geit[r][700].KNMI_LONGI = geit[r-1][700].KNMI_LONGI + Vlastpixelstep_toptobottomLONG;
			geit[r][700].KNMI_LATI = geit[r-1][700].KNMI_LATI + Vlastpixelstep_toptobottomLAT;
			geit[r][700].TTBstaprangeLONG = Vlastpixelstep_toptobottomLONG;
			geit[r][700].TTBstaprangeLAT = Vlastpixelstep_toptobottomLAT;
			geit[r][700].KNMI_X = 700;
			geit[r][700].KNMI_Y = r;
		}
	}
	
	
	
	for(s=1; s<JMAX; s++){
		//Vul rand tussen linkerbovenhoek en rechterbovenhoek.:
		if((geit[0][s].KNMI_LONGI == -1) || (geit[0][s].KNMI_LATI == -1)){
			geit[0][s].KNMI_LONGI = geit[0][s-1].KNMI_LONGI + Vfirstpixelstep_lefttorightLONG;
			geit[0][s].KNMI_LATI = geit[0][s-1].KNMI_LATI + Vfirstpixelstep_lefttorightLAT;
			geit[0][s].LTRstaprangeLONG = Vfirstpixelstep_lefttorightLONG;
			geit[0][s].LTRstaprangeLAT = Vfirstpixelstep_lefttorightLAT;
			geit[0][s].KNMI_X = s;
			geit[0][s].KNMI_Y = 0;
		}
		
		//Vul rand tussen linkeronderhoek en rechteronderhoek.:
		if((geit[765][s].KNMI_LONGI == -1) || (geit[765][s].KNMI_LATI == -1)){
			geit[765][s].KNMI_LONGI = geit[765][s-1].KNMI_LONGI + Vlastpixelstep_lefttorightLONG;
			geit[765][s].KNMI_LATI = geit[765][s-1].KNMI_LATI + Vlastpixelstep_lefttorightLAT;
			geit[765][s].LTRstaprangeLONG = Vlastpixelstep_lefttorightLONG;
			geit[765][s].LTRstaprangeLAT = Vlastpixelstep_lefttorightLAT;
			geit[765][s].KNMI_X = s;
			geit[765][s].KNMI_Y = 765;
		}
	}
	

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Vul gebied binnen de randen - KNMI Matrix:
	for(s=1; s<JMAX; s++){
		toptobotLONGtemp = (geit[765][s].KNMI_LONGI - geit[0][s].KNMI_LONGI) / KMAX;
		toptobotLATtemp = (geit[765][s].KNMI_LATI - geit[0][s].KNMI_LATI) / KMAX;
		//Vul ontbrekende verticale stapgegevens in de bovenste rand:
		geit[0][s].TTBstaprangeLONG = toptobotLONGtemp;
		geit[0][s].TTBstaprangeLAT = toptobotLATtemp;
		
		for(r=1; r<KMAX; r++){
			if((geit[r][s].KNMI_LONGI == -1) || (geit[r][s].KNMI_LATI == -1)){
				geit[r][s].KNMI_LONGI = geit[r-1][s].KNMI_LONGI + toptobotLONGtemp;
				geit[r][s].KNMI_LATI = geit[r-1][s].KNMI_LATI + toptobotLATtemp;
				geit[r][s].TTBstaprangeLONG = toptobotLONGtemp;
				geit[r][s].TTBstaprangeLAT = toptobotLATtemp;
				geit[r][s].KNMI_X = s;
				geit[r][s].KNMI_Y = r;
			}
		}
	}
	

/*	//Vul gebied binnen de randen - KNMI Matrix (Horizontaal overige indices invullen (laatste kolom)):
	for(r=1; r<KMAX-1; r++){
		lefttorightLONGtemp = (geit[r][700].KNMI_LONGI - geit[r][0].KNMI_LONGI) / JMAX;
		lefttorightLATtemp = (geit[r][700].KNMI_LATI - geit[r][0].KNMI_LATI) / JMAX;
		//Vul ontbrekende verticale stapgegevens in de linker rand:
		geit[r][0].LTRstaprangeLONG = lefttorightLONGtemp;
		geit[r][0].LTRstaprangeLAT = lefttorightLATtemp;
		
		for(s=1; s<JMAX; s++){
			if((geit[r][s].KNMI_LONGI == -1) || (geit[r][s].KNMI_LATI == -1)){
				geit[r][s].KNMI_LONGI = geit[r][s-1].KNMI_LONGI + lefttorightLONGtemp;
				geit[r][s].KNMI_LATI = geit[r][s-1].KNMI_LATI + lefttorightLATtemp;
				geit[r][s].LTRstaprangeLONG = lefttorightLONGtemp;
				geit[r][s].LTRstaprangeLAT = lefttorightLATtemp;
				geit[r][s].KNMI_X = s;
				geit[r][s].KNMI_Y = r;
			}
		}
	}*/

		
			
			



///////////////////////////////////////////////////////////////////////////
//Reading extracted GeoTIFF data retrieved with Python:
	
	//Leest een GeoTIFF file in:
	ifstream invoer;
	invoer.open(file.c_str(), ios::in);
	
	//Tijdelijke variabelen voor het inlezen:
	string banaan;
	int row = 0; //row (1250)
	int col = 0; //col (1000)
	
	
	//Foutmelding als de file niet geopend/gelezen kan worden:
	if(invoer.fail()){
		cout << "Unable to open the file." << endl;
		exit(1);
	}
	
	//Bestand uitlezen:	
	else{
		//Lees elke regel in het bestand uit:
		while(getline(invoer, banaan)){
	        stringstream linestream(banaan);
	        string temp;
	        //Lees regel en splits op de plek van de delimiter:
	        while(getline(linestream, temp, ',')){       
		        schaap[row][col].elev = atof(temp.c_str());
		        //schaap[row][col].X = col; //Begint op 0, eindigt op 1000.
		        //schaap[row][col].Y = row; //Begint op 0, eindigt op 1250.	        
		        schaap[row][col].filename = "33cz1"; //ADJUST!!!   
				col++;
			}
			row++;
	    	col = 0;
		}//while
	}//else
	
/////////////////////////////////////////////////////////////
	
	
	
	bool found = false;
	
	//Voor elk punt in een GeoTIFF file (kan ook in de inlees-whiles):
	for(i=0; i<NMAX; i++){
		for(j=0; j<MMAX; j++){

			//Elke pixel (vak) afgaan in KNMI map:
			for(r=0; r<KMAX; r++){
				for(s=0; s<JMAX; s++){
					//found = false;
					//Oplopend van links naar rechts en van boven naar beneden.
					
					//[i][j] - [i][j+1] = UL t/m UR
					//[i][j] - [i+1][j] = UL t/m LL
					//[i][j+1] - [i+1][j+1] = UR t/m LR
					//[i+1][j] - [i+1][j+1] = LL t/m LR
					
					if((geit[r][700].KNMI_LONGI - geit[r][0].KNMI_LONGI) >= 0){
						if((geit[765][s].KNMI_LONGI - geit[0][s].KNMI_LONGI) >= 0){
							if((geit[r+1][700].KNMI_LONGI - geit[r+1][0].KNMI_LONGI) >= 0){
								if((geit[765][s+1].KNMI_LONGI - geit[0][s+1].KNMI_LONGI) >= 0){
									//TTTT
									
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= (geit[r][s+1].KNMI_LONGI - prev_epsilon) && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LONGI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LONGI) || 
											(schaap[i][j].LATI >= (geit[r][s+1].KNMI_LATI - prev_epsilon) && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LONGI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= (geit[r+1][s].KNMI_LONGI - prev_epsilon) && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LONGI) || 
											(schaap[i][j].LATI >= (geit[r+1][s].KNMI_LATI - prev_epsilon) && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LONGI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LONGI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= (geit[r+1][s].KNMI_LONGI - prev_epsilon) && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= (geit[r][s+1].KNMI_LONGI - prev_epsilon) && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= (geit[r+1][s].KNMI_LATI - prev_epsilon) && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= (geit[r][s+1].KNMI_LATI - prev_epsilon) && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
								}
								else{
									//TTTF
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									
								}
							}
							else{
								//TTFT
								if((geit[765][s+1].KNMI_LONGI - geit[0][s+1].KNMI_LONGI) >= 0){
									//TTFT
									
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
								}
								else{
									//TTFF
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									
								}
								
							}
						}
						
						else{
							//TFTT
							if((geit[r+1][700].KNMI_LONGI - geit[r+1][0].KNMI_LONGI) >= 0){
								if((geit[765][s+1].KNMI_LONGI - geit[0][s+1].KNMI_LONGI) >= 0){
									
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
								}
								else{
									//TTTF
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									
								}
							}
							else{
								//TTFT
								if((geit[765][s+1].KNMI_LONGI - geit[0][s+1].KNMI_LONGI) >= 0){
									//TTFT
									
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
								}
								else{
									//TTTF
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									
								}
								
							}
							
						}
					}
					else{
						//FTTT
						if((geit[765][s].KNMI_LONGI - geit[0][s].KNMI_LONGI) >= 0){
							if((geit[r+1][700].KNMI_LONGI - geit[r+1][0].KNMI_LONGI) >= 0){
								if((geit[765][s+1].KNMI_LONGI - geit[0][s+1].KNMI_LONGI) >= 0){
									
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
								}
								else{
									//TTTF
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									
								}
							}
							else{
								//TTFT
								if((geit[765][s+1].KNMI_LONGI - geit[0][s+1].KNMI_LONGI) >= 0){
									//TTFT
									
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
								}
								else{
									//TTFF
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									
								}
								
							}
						}
						
						else{
							//TFTT
							if((geit[r+1][700].KNMI_LONGI - geit[r+1][0].KNMI_LONGI) >= 0){
								if((geit[765][s+1].KNMI_LONGI - geit[0][s+1].KNMI_LONGI) >= 0){
									
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
								}
								else{
									//TTTF
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r+1][s].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									
								}
							}
							else{
								//TTFT
								if((geit[765][s+1].KNMI_LONGI - geit[0][s+1].KNMI_LONGI) >= 0){
									//TTFT
									
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI <= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI > geit[r][s+1].KNMI_LATI && schaap[i][j].LATI < geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
								}
								else{
									//TTTF
									if(r == (KMAX-1) && s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(r == (KMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else if(s == (JMAX-1)){
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI >= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI >= geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI && schaap[i][j].LATI >= geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									else{
										if((schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI <= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI > geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI <= geit[r][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r+1][s].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI) || 
											(schaap[i][j].LATI < geit[r][s+1].KNMI_LATI && schaap[i][j].LATI > geit[r+1][s+1].KNMI_LATI)){
												schaap[i][j].KNMI_X = s; //kolom
												schaap[i][j].KNMI_Y = r; //rij
												found = true; // So we can break out of the outer loop as well.
												break;
											}
											else{
												continue;
											}
				
										}
										else{
											continue;
										}
									}
									
								}
								
							}
							
						}
						
					}
				}
				
				
				
				
				if(found == true){
					found = false;
					break;
				}//if
			}//for
			
		}//for
	}//for	
	
	
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//Voor vakjes in KNMI vakje:

	bool v_found = false;

	//Voor elk punt in een GeoTIFF file (kan ook in de inlees-whiles):
	for(i=0; i<NMAX; i++){
		for(j=0; j<MMAX; j++){
			
			//Initialise Longitude & Latitude Matrices: -1'en:
			for(u=0; u<=HMAX; u++){
				for(v=0; v<=GMAX; v++){			
					koe[u][v].v_LONGI = -1;
					koe[u][v].v_LATI = -1;
					koe[u][v].v_X = -1;
					koe[u][v].v_Y = -1;
				}
			}


			//Temporary Variables - Moet misschien aparte variabelen maken voor de indices hierboven:
			double ULLong_temp = geit[schaap[i][j].KNMI_Y][schaap[i][j].KNMI_X].KNMI_LONGI;
			double ULLat_temp = geit[schaap[i][j].KNMI_Y][schaap[i][j].KNMI_X].KNMI_LATI;
			double URLong_temp = geit[schaap[i][j].KNMI_Y][schaap[i][j].KNMI_X+1].KNMI_LONGI;
			double URLat_temp = geit[schaap[i][j].KNMI_Y][schaap[i][j].KNMI_X+1].KNMI_LATI;
			double LLLong_temp = geit[schaap[i][j].KNMI_Y+1][schaap[i][j].KNMI_X].KNMI_LONGI;
			double LLLat_temp = geit[schaap[i][j].KNMI_Y+1][schaap[i][j].KNMI_X].KNMI_LATI;
			double LRLong_temp = geit[schaap[i][j].KNMI_Y+1][schaap[i][j].KNMI_X+1].KNMI_LONGI;
			double LRLat_temp = geit[schaap[i][j].KNMI_Y+1][schaap[i][j].KNMI_X+1].KNMI_LATI;
			
			
			double FLTRLONG = (URLong_temp - ULLong_temp) / GMAX;
			double FLTRLAT = (URLat_temp - ULLat_temp) / GMAX;
			double LLTRLONG = (LRLong_temp - LLLong_temp) / GMAX;
			double LLTRLAT = (LRLat_temp - LLLat_temp) / GMAX;
			double FTTBLONG = (LLLong_temp - ULLong_temp) / HMAX;
			double FTTBLAT = (LLLat_temp - ULLat_temp) / HMAX;
			double LTTBLONG = (LRLong_temp - URLong_temp) / HMAX;
			double LTTBLAT = (LRLat_temp - URLat_temp) / HMAX;
			
			double TTBLONGtemp = 0;
			double TTBLATtemp = 0;
			double LTRLONGtemp = 0;
			double LTRLATtemp = 0;

	/////////////////////////////////////////
	
	
			//Linkerbovenhoek vullen - KNMI struct:
			koe[0][0].v_LONGI = ULLong_temp;
			koe[0][0].v_LATI = ULLat_temp;
			koe[0][0].v_X = 0;
			koe[0][0].v_Y = 0;
	
	
//////////////////////////////////////	
			//Vul linkeronderhoek:
			koe[200][0].v_LONGI = LLLong_temp;
			koe[200][0].v_LATI = LLLat_temp;
			koe[200][0].v_X = 0;
			koe[200][0].v_Y = 200;
			
			//Vul rechterbovenhoek:
			koe[0][200].v_LONGI = URLong_temp;
			koe[0][200].v_LATI = URLat_temp;
			koe[0][200].v_X = 200;
			koe[0][200].v_Y = 0;
			
			//Vul rechteronderhoek:
			koe[200][200].v_LONGI = LRLong_temp;
			koe[200][200].v_LATI = LRLat_temp;
			koe[200][200].v_X = 200;
			koe[200][200].v_Y = 200;
	
	
	
			//Vul randen:
			for(u=1; u<HMAX; u++){
				//Vul rand tussen linkerbovenhoek en linkeronderhoek.
				if((koe[u][0].v_LONGI == -1) || (koe[u][0].v_LATI == -1)){  
					koe[u][0].v_LONGI = koe[u-1][0].v_LONGI + FTTBLONG;
					koe[u][0].v_LATI = koe[u-1][0].v_LATI + FTTBLAT;
					koe[u][0].v_X = 0;
					koe[u][0].v_Y = u;
				}
				//Vul rand tussen linkerbovenhoek en rechterbovenhoek.
				if((koe[0][u].v_LONGI == -1) || (koe[0][u].v_LATI == -1)){
					koe[0][u].v_LONGI = koe[0][u-1].v_LONGI + FLTRLONG;
					koe[0][u].v_LATI = koe[0][u-1].v_LATI + FLTRLAT;
					koe[0][u].v_X = u;
					koe[0][u].v_Y = 0;
				}
				//Vul rand tussen rechterbovenhoek en rechteronderhoek.
				if((koe[u][200].v_LONGI == -1) || (koe[u][200].v_LATI == -1)){  
					koe[u][200].v_LONGI = koe[u-1][200].v_LONGI + LTTBLONG;
					koe[u][200].v_LATI = koe[u-1][200].v_LATI + LTTBLAT;
					koe[u][200].v_X = 200;
					koe[u][200].v_Y = u;
				}
				//Vul rand tussen linkeronderhoek en rechteronderhoek.
				if((koe[200][u].v_LONGI == -1) || (koe[200][u].v_LATI == -1)){
					koe[200][u].v_LONGI = koe[200][u-1].v_LONGI + LLTRLONG;
					koe[200][u].v_LATI = koe[200][u-1].v_LATI + LLTRLAT;
					koe[200][u].v_X = u;
					koe[200][u].v_Y = 200;
				}
			}
			
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
			//Vul gebied binnen de randen - KNMI Matrix:
			for(v=1; v<GMAX; v++){
				TTBLONGtemp = (koe[200][v].v_LONGI - koe[0][v].v_LONGI) / HMAX;
				TTBLATtemp = (koe[200][v].v_LATI - koe[0][v].v_LATI) / HMAX;
				
				for(u=1; u<HMAX; u++){
					if((koe[u][v].v_LONGI == -1) || (koe[u][v].v_LATI == -1)){
						koe[u][v].v_LONGI = koe[u-1][v].v_LONGI + TTBLONGtemp;
						koe[u][v].v_LATI = koe[u-1][v].v_LATI + TTBLATtemp;
						koe[u][v].v_X = v;
						koe[u][v].v_Y = u;
					}
				}
			}
		

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//Elke pixel (vak) afgaan in KNMI map:
			for(r=0; r<HMAX; r++){
				for(s=0; s<GMAX; s++){
					//kun je openklappen
					if((koe[r][200].v_LONGI - koe[r][0].v_LONGI) >= 0){
						if((koe[200][s].v_LONGI - koe[0][s].v_LONGI) >= 0){
							if((koe[r+1][200].v_LONGI - koe[r+1][0].v_LONGI) >= 0){
								if((koe[200][s+1].v_LONGI - koe[0][s+1].v_LONGI) >= 0){
									//TTTT
									
									
									if((schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r+1][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r][s+1].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI > koe[r+1][s].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI > koe[r][s+1].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
									
								}
								else{
									//TTTF
									
									if((schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r+1][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r][s+1].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI > koe[r+1][s].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI < koe[r][s+1].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
								
								
								}
							}
							else{
								//TTFT
								if((koe[200][s+1].v_LONGI - koe[0][s+1].v_LONGI) >= 0){
									//TTFT
									
									if((schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r+1][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r][s+1].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI < koe[r+1][s].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI > koe[r][s+1].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}

								}
								else{
									//TTFF
									
									if((schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r+1][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r][s+1].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI < koe[r+1][s].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI < koe[r][s+1].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
									
								}
								
							}
						}
						
						else{
							//TFTT
							if((koe[r+1][200].v_LONGI - koe[r+1][0].v_LONGI) >= 0){
								if((koe[200][s+1].v_LONGI - koe[0][s+1].v_LONGI) >= 0){
									
									
									if((schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r+1][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r][s+1].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI > koe[r+1][s].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI > koe[r][s+1].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
								}
								else{
									//TTTF
									
									if((schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r+1][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r][s+1].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI > koe[r+1][s].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI < koe[r][s+1].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
									
								}
							}
							else{
								//TTFT
								if((koe[200][s+1].v_LONGI - koe[0][s+1].v_LONGI) >= 0){
									//TTFT
									
									if((schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r+1][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r][s+1].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI < koe[r+1][s].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI > koe[r][s+1].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
								}
								else{
									//TTTF
									
									if((schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r+1][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r][s+1].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI < koe[r+1][s].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI < koe[r][s+1].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
									
								}
								
							}
							
						}
					}
					else{
						//FTTT
						if((koe[200][s].v_LONGI - koe[0][s].v_LONGI) >= 0){
							if((koe[r+1][200].v_LONGI - koe[r+1][0].v_LONGI) >= 0){
								if((koe[200][s+1].v_LONGI - koe[0][s+1].v_LONGI) >= 0){
									
									
									if((schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r+1][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r][s+1].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI > koe[r+1][s].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI > koe[r][s+1].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
								}
								else{
									//TTTF
									
									if((schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r+1][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r][s+1].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI > koe[r+1][s].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI < koe[r][s+1].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
									
								}
							}
							else{
								//TTFT
								if((koe[200][s+1].v_LONGI - koe[0][s+1].v_LONGI) >= 0){
									//TTFT
									
									
									if((schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r+1][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r][s+1].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI < koe[r+1][s].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI > koe[r][s+1].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
								}
								else{
									//TTFF
									
									if((schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI >= koe[r][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r+1][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r][s+1].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI >= koe[r][s].v_LATI && schaap[i][j].LATI < koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI < koe[r+1][s].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI < koe[r][s+1].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
									
								}
								
							}
						}
						
						else{
							//TFTT
							if((koe[r+1][200].v_LONGI - koe[r+1][0].v_LONGI) >= 0){
								if((koe[200][s+1].v_LONGI - koe[0][s+1].v_LONGI) >= 0){
									
									
									if((schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r+1][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r][s+1].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI > koe[r+1][s].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI > koe[r][s+1].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
								}
								else{
									//TTTF
									
									if((schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r+1][s].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r][s+1].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI > koe[r+1][s].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI < koe[r][s+1].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
									
								}
							}
							else{
								//TTFT
								if((koe[200][s+1].v_LONGI - koe[0][s+1].v_LONGI) >= 0){
									//TTFT
									
									
									if((schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r+1][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI > koe[r][s+1].v_LONGI && schaap[i][j].LONGI < koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI < koe[r+1][s].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI > koe[r][s+1].v_LATI && schaap[i][j].LATI < koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
								}
								else{
									//TTTF
									
									if((schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r][s+1].v_LONGI) || 
									(schaap[i][j].LONGI <= koe[r][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r+1][s].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI) || 
									(schaap[i][j].LONGI < koe[r][s+1].v_LONGI && schaap[i][j].LONGI > koe[r+1][s+1].v_LONGI)){
										
										if((schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r][s+1].v_LATI) || 
										(schaap[i][j].LATI <= koe[r][s].v_LATI && schaap[i][j].LATI > koe[r+1][s].v_LATI) || 
										(schaap[i][j].LATI < koe[r+1][s].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI) || 
										(schaap[i][j].LATI < koe[r][s+1].v_LATI && schaap[i][j].LATI > koe[r+1][s+1].v_LATI)){
											schaap[i][j].vakvakje_X = s; //kolom
											schaap[i][j].vakvakje_Y = r; //rij
											v_found = true; // So we can break out of the outer loop as well.
											break;
										}
										else{
											continue;
										}
			
									}
									else{
										continue;
									}
									
									
								}
								
							}
							
						}
						
					}
				}
				
				
				
				
				if(v_found == true){
					v_found = false;
					break;
				}//if
			}//for
			
		}//for
	}//for	
	
	
	
	
/////////////////////////////////////////////////////////////		
	//Schrijf naar file - CSV:	
	ofstream myfile; 
	myfile.open("33cz1.csv"); //Filename te schrijven file.

	myfile << "Filename,X,Y,Longitude,Latitude,Elevation,KNMIX,KNMIY,KNMILONG,KNMILAT,VakjeX,VakjeY" << endl;
	//Schrijft per regel:
	for(i=0; i<NMAX; i++){
		for(j=0; j<MMAX; j++){
			int KNMI_X_temp = schaap[i][j].KNMI_X;
			int KNMI_Y_temp = schaap[i][j].KNMI_Y;
			myfile << schaap[i][j].filename << ",";
			myfile << schaap[i][j].X << "," << schaap[i][j].Y << ",";
			myfile << fixed << setprecision(13) << schaap[i][j].LONGI << ",";
			myfile << fixed << setprecision(13) << schaap[i][j].LATI << ",";
			myfile << fixed << setprecision(13) << schaap[i][j].elev << ",";
			myfile << schaap[i][j].KNMI_X << "," << schaap[i][j].KNMI_Y << ",";
			myfile << geit[KNMI_Y_temp][KNMI_X_temp].KNMI_LONGI << "," << geit[KNMI_Y_temp][KNMI_X_temp].KNMI_LATI << ",";
			myfile << schaap[i][j].vakvakje_X << "," << schaap[i][j].vakvakje_Y << endl;
		}
	}

	myfile.close();
	
}

/////////////////////////////////////////////////////////////


//MAIN - runt initialisatie:
int main () {
	dataset banaan("A1.csv");

	return 0;
}

