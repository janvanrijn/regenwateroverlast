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

using std::vector;

using namespace std;


/////////////////////////////////////////////////



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


//////////////////////////////////////


//maximale grootte matrix - GeoTIFF:
const int MMAX = 1000; //column = X
const int NMAX = 1250; //row = Y


//maximale grootte matrix - KNMI:
const int JMAX = 700; //column = X
const int KMAX = 765; //row = Y



////////////////
double LONG[1250][1000]; //GeoTIFF
double LAT[1250][1000]; //GeoTIFF

//double VLONG[765][700]; //KNMI randen
//double VLAT[765][700]; //KNMI randen

//double KNMILONG[765][700]; //KNMI linkerbovenhoeken
//double KNMILAT[765][700]; //KNMI linkerbovenhoeken

//double elevation[1250][1000];


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

	//Left to right from Lowerright corner:
	double Vlastpixelstep_lefttorightLONG = (VLRLong - VLLLong) / JMAX;
	double Vlastpixelstep_lefttorightLAT = (VLRLat - VLLLat) / JMAX;
	//Up - down from Lowerright corner:
	double Vlastpixelstep_toptobottomLONG = (VLRLong - VURLong) / KMAX;
	double Vlastpixelstep_toptobottomLAT = (VLRLat - VURLat) / KMAX;



///////////////////////////////////////////////////////////////////
//GeoTIFF - Fill in Longitude and Latitude Matrices:
//Might need adjustment (same code as for KNMI matrices).

	for(p=0; p<NMAX; p++){
		for(q=0; q<MMAX; q++){
			LONG[p][q] = -1;
			LAT[p][q] = -1;
			schaap[p][q].LONGI = -1;
			schaap[p][q].LATI = -1;
		}
	}



	
	//Randen eerst vullen:
	//Linkerbovenhoek:
	schaap[0][0].LONGI = ULLong;
	schaap[0][0].LATI = ULLat;
	
	//Rechterbovenhoek:
	schaap[0][999].LONGI = URLong;
	schaap[0][999].LATI = URLat;
	
	//Linkeronderhoek:
	schaap[1249][0].LONGI = LLLong;
	schaap[1249][0].LATI = LLLat;
	
	//Rechteronderhoek:
	schaap[1249][999].LONGI = LRLong;
	schaap[1249][999].LATI = LRLat;
	
	
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
		}
		if((schaap[1249][q].LONGI == -1) || (schaap[1249][q].LATI == -1)){
			schaap[1249][q].LONGI = schaap[1249][q-1].LONGI + lastpixelstep_lefttorightLONG;
			schaap[1249][q].LATI = schaap[1249][q-1].LATI + lastpixelstep_lefttorightLAT;
		}
	}
		
		
	for(p=1; p<(NMAX-1); p++){
		if((schaap[p][0].LONGI == -1) || (schaap[p][0].LATI == -1)){
			schaap[p][0].LONGI = schaap[p-1][0].LONGI + firstpixelstep_toptobottomLONG;
			schaap[p][0].LATI = schaap[p-1][0].LATI + firstpixelstep_toptobottomLAT;
		}
		if((schaap[p][999].LONGI == -1) || (schaap[p][999].LATI == -1)){
			schaap[p][999].LONGI = schaap[p-1][999].LONGI + lastpixelstep_toptobottomLONG;
			schaap[p][999].LATI = schaap[p-1][999].LATI + lastpixelstep_toptobottomLAT;
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

	
	
	
	//Temporary Variables:
	int firstlefttorightLONGtemp = (VURLong - VULLong) / JMAX;
	int firstlefttorightLATtemp = (VURLat - VULLat) / JMAX;
	int lastlefttorightLONGtemp = (VLRLong - VLLLong) / JMAX;
	int lastlefttorightLATtemp = (VLRLat - VLLLat) / JMAX;
	int firsttoptobotLONGtemp = (VLLLong - VULLong) / KMAX;
	int firsttoptobotLATtemp = (VLLLat - VULLat) / KMAX;
	int lasttoptobotLONGtemp = (VLRLong - VURLong) / KMAX;
	int lasttoptobotLATtemp = (VLRLat - VURLat) / KMAX;
	
	int toptobotLONGtemp = 0;
	int toptobotLATtemp = 0;
	int lefttorightLONGtemp = 0;
	int lefttorightLATtemp = 0;
	/////////////////////////////////////////
	
	
	//Linkerbovenhoek vullen - KNMI struct:
	geit[0][0].KNMI_LONGI = VULLong;
	geit[0][0].KNMI_LATI = VULLat;
	geit[0][0].LTRstaprangeLONG = firstlefttorightLONGtemp;
	geit[0][0].LTRstaprangeLAT = firstlefttorightLATtemp;
	geit[0][0].TTBstaprangeLONG = firsttoptobotLONGtemp;
	geit[0][0].TTBstaprangeLAT = firsttoptobotLATtemp;
	geit[0][0].KNMI_X = 0;
	geit[0][0].KNMI_Y = 0;
	
	
//////////////////////////////////////	
	//Vul linkeronderhoek:
	geit[765][0].KNMI_LONGI = VLLLong;
	geit[765][0].KNMI_LATI = VLLLat;
	
	//Vul rechterbovenhoek:
	geit[0][700].KNMI_LONGI = VURLong;
	geit[0][700].KNMI_LATI = VURLat;
	
	//Vul rechteronderhoek:
	geit[765][700].KNMI_LONGI = VLRLong;
	geit[765][700].KNMI_LATI = VLRLat;
	
	
	
	//Vul rand tussen linkerbovenhoek en linkeronderhoek.
	for(r=1; r<KMAX; r++){
		if((geit[r][0].KNMI_LONGI == -1) || (geit[r][0].KNMI_LATI == -1)){  
			geit[r][0].KNMI_LONGI = geit[r-1][0].KNMI_LONGI + firsttoptobotLONGtemp;
			geit[r][0].KNMI_LATI = geit[r-1][0].KNMI_LATI + firsttoptobotLATtemp;
			geit[r][0].TTBstaprangeLONG = firsttoptobotLONGtemp;
			geit[r][0].TTBstaprangeLAT = firsttoptobotLATtemp;
			geit[r][0].KNMI_X = 0;
			geit[r][0].KNMI_Y = r;
		}
	}
	
	
	//Vul rand tussen linkerbovenhoek en rechterbovenhoek.
	for(s=1; s<JMAX; s++){
		if((geit[0][s].KNMI_LONGI == -1) || (geit[0][s].KNMI_LATI == -1)){
			geit[0][s].KNMI_LONGI = geit[0][s-1].KNMI_LONGI + firstlefttorightLONGtemp;
			geit[0][s].KNMI_LATI = geit[0][s-1].KNMI_LATI + firstlefttorightLATtemp;
			geit[0][s].LTRstaprangeLONG = firstlefttorightLONGtemp;
			geit[0][s].LTRstaprangeLAT = firstlefttorightLATtemp;
			geit[0][s].KNMI_X = s;
			geit[0][s].KNMI_Y = 0;
		}
	}
	

	//Vul rand tussen rechterbovenhoek en rechteronderhoek.
	for(r=1; r<KMAX; r++){
		if((geit[r][700].KNMI_LONGI == -1) || (geit[r][700].KNMI_LATI == -1)){  
			geit[r][700].KNMI_LONGI = geit[r-1][700].KNMI_LONGI + lasttoptobotLONGtemp;
			geit[r][700].KNMI_LATI = geit[r-1][700].KNMI_LATI + lasttoptobotLATtemp;
			geit[r][700].TTBstaprangeLONG = lasttoptobotLONGtemp;
			geit[r][700].TTBstaprangeLAT = lasttoptobotLATtemp;
			geit[r][700].KNMI_X = 700;
			geit[r][700].KNMI_Y = r;
		}
	}


	//Vul rand tussen linkeronderhoek en rechteronderhoek.
	for(s=1; s<JMAX; s++){
		if((geit[765][s].KNMI_LONGI == -1) || (geit[765][s].KNMI_LATI == -1)){
			geit[765][s].KNMI_LONGI = geit[765][s-1].KNMI_LONGI + lastlefttorightLONGtemp;
			geit[765][s].KNMI_LATI = geit[765][s-1].KNMI_LATI + lastlefttorightLATtemp;
			geit[765][s].LTRstaprangeLONG = lastlefttorightLONGtemp;
			geit[765][s].LTRstaprangeLAT = lastlefttorightLATtemp;
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
		        
		        schaap[row][col].X = col; //Begint op 0, eindigt op 1000.
		        schaap[row][col].Y = row; //Begint op 0, eindigt op 1250.
		        
		        //schaap[row][col].LONGI = LONG[row][col];
		        //schaap[row][col].LATI = LAT[row][col];
		        
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
					
					//kun je openklappen
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
										(schaap[i][j].LONGI >= geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
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
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI <= geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI <= geit[r][s+1].KNMI_LATI) || 
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
										if((schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI >= geit[r][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r+1][s].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI) || 
										(schaap[i][j].LONGI > geit[r][s+1].KNMI_LONGI && schaap[i][j].LONGI < geit[r+1][s+1].KNMI_LONGI)){
											
											if((schaap[i][j].LATI >= geit[r][s].KNMI_LATI && schaap[i][j].LATI < geit[r][s+1].KNMI_LATI) || 
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
	//Schrijf naar file - CSV:	
	ofstream myfile; 
	myfile.open("33cz1.csv"); //Filename te schrijven file.

	myfile << "Filename,X,Y,Longitude,Latitude,Elevation,KNMIX,KNMIY" << endl;
	//Schrijft per regel:
	for(i=0; i<NMAX; i++){
		for(j=0; j<MMAX; j++){
			myfile << schaap[i][j].filename << ",";
			myfile << schaap[i][j].X << "," << schaap[i][j].Y << ",";
			myfile << fixed << setprecision(13) << schaap[i][j].LONGI << ",";
			myfile << fixed << setprecision(13) << schaap[i][j].LATI << ",";
			myfile << fixed << setprecision(13) << schaap[i][j].elev << ",";
			myfile << schaap[i][j].KNMI_X << "," << schaap[i][j].KNMI_Y << endl;
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

