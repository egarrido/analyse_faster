#include "TFile.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TPad.h"
#include "TGraph.h"
#include "TH1.h"
#include "TF1.h"
#include "TF2.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TVirtualFitter.h"
#include "TSpectrum.h"
#include "TMath.h"
#include "TROOT.h"
#include "TColor.h"

#include "TDatime.h"
#include "TLine.h"
#include "TText.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TArrow.h"
#include "TGaxis.h"
#include "TFrame.h"
#include "TVirtualFitter.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <sstream>
#include <fstream>
#include "Riostream.h"

#include "fasterac/fasterac.h"
#include "fasterac/electrometer.h"
#include "fasterac/scaler.h"

#include "Seuil_data.h"
#include "Calibrage_data.h"

std::ofstream logfile;
const bool logfileprint=true;

std::vector<string> Variable_init;
std::vector<double> Value_init;

std::vector<double> PreSFB;
std::vector<double> PostSFB;

std::vector<double> Vect_calib_factor;
std::vector<double> Vect_calib_charge;
std::vector<long int> Vect_calib_quanta;

string data_folder;
string data_faster_file;
string data_audio_file;

const double epsilon=1.E-9;

const int N_box_per_irradation_max=5000;
const int N_STRIPS=32;
const int FIRST_ELEC=1;
const int LAST_ELEC=31;
const int MAX_PEAK=10;
const int MAX_SMPL=1000;
const int MAX_INTEGR=1000000;

// Manip Mai 2014 : X->4 Y->6 Scaler->1009
const int LabelX=4;
const int LabelY=6;
const int LabelScaler=7;
const int LabelCount=1007;
const int LabelHVp=102;
const int LabelHVn=101;

int bkgnd_param=0;
int calibrage_used=0;
int area_find_param=0;
int Strip_label;
int Strip_number;
int borne_m_x=16;
int borne_M_x=17;
int borne_m_y=16;
int borne_M_y=17;

// const double SamplingTime=40e-6;  	// 40   us
// const double SamplingTime=DBL_MAX;
// const double SamplingTime=1.;
int IntegrationStep;
int nbSummedSample;
double SamplingTime=1.;
const double default_offset=0.419;		// 0.418   pC
const double defaultRange_pC=24;    	// 24   pC
const double sizeStrip=0.3; 					// 0.3 cm
const	double ele2pC_coef=4.5777e-05; 
double corrXY[N_STRIPS][2]={{1.00000,1.00000},
														{1.00000,1.00000},
														{1.00000,1.00000},
														{1.00000,1.00000},
														{1.00000,1.00000},
														{1.00000,1.00000},
														{1.00287,1.00000},
														{0.99886,0.97900},
														{0.99496,0.99777},
														{1.00102,1.00786},
														{0.99815,1.00352},
														{1.00640,1.01185},
														{0.99210,1.00963},
														{1.01579,1.00328},
														{0.99180,0.99555},
														{1.00119,1.00284},
														{0.99623,0.99784},
														{0.99712,0.99653},
														{1.00402,0.98870},
														{1.00000,0.99598},
														{1.00000,0.99437},
														{1.00000,0.99987},
														{1.00000,1.01356},
														{1.00000,1.00000},
														{1.00000,1.00000},
														{1.00000,1.00000},
														{1.00000,1.00000},
														{1.00000,1.00000},
														{1.00000,1.00000},
														{1.00000,1.00000},
														{1.00000,1.00000},
														{1.00000,1.00000}};

double debut_eoff;
double fin_eoff;
double offXY[N_STRIPS][2]= {{18.9862,25.1487},
														{25.1365,25.1032},
														{25.0929,25.064},
														{25.0629,25.0316},
														{25.2609,25.2731},
														{25.216,25.2227},
														{25.1741,25.1792},
														{25.1416,25.1462},
														{25.1801,25.1225},
														{25.1353,25.0838},
														{25.1098,25.047},
														{25.1115,25.0435},
														{25.1381,25.0929},
														{25.1035,25.0671},
														{25.0769,25.0289},
														{25.0804,25.0212},
														{25.1584,25.1591},
														{25.1132,25.1053},
														{25.0731,25.0703},
														{25.0581,25.0508},
														{25.2678,25.172},
														{25.2149,25.123},
														{25.1767,25.0819},
														{25.1472,25.0535},
														{25.1028,25.0956},
														{25.0715,25.0517},
														{25.0439,25.0222},
														{25.0399,25.0312},
														{25.0618,25.1049},
														{25.0354,25.0725},
														{25.0055,25.0375},
														{25.0062,25.0369}};
// double offXY[N_STRIPS][2]= {{18.8852,25.1482},
// 														{25.1405,25.1037},
// 														{25.1021,25.0689},
// 														{25.0746,25.0353},
// 														{25.2621,25.2743},
// 														{25.2215,25.2254},
// 														{25.1843,25.1836},
// 														{25.1533,25.1491},
// 														{25.1807,25.1236},
// 														{25.14,25.0863},
// 														{25.1203,25.0528},
// 														{25.1231,25.0494},
// 														{25.1443,25.0994},
// 														{25.1137,25.0742},
// 														{25.0914,25.0391},
// 														{25.0818,25.0289},
// 														{25.1576,25.1599},
// 														{25.1199,25.11},
// 														{25.0835,25.0745},
// 														{25.0699,25.0552},
// 														{25.2709,25.1734},
// 														{25.222,25.1262},
// 														{25.1877,25.086},
// 														{25.1594,25.0584},
// 														{25.1063,25.0957},
// 														{25.0778,25.0532},
// 														{25.055,25.0254},
// 														{25.0535,25.0356},
// 														{25.0683,25.1092},
// 														{25.046,25.0789},
// 														{25.0197,25.0457},
// 														{25.022,25.0464}};

void set_plot_style()
{
	const Int_t NRGBs = 5;
	const Int_t NCont = 100;

	Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
	Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
	Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
	Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	gStyle->SetNumberContours(NCont);
	gStyle->SetPadGridX(kTRUE);
	gStyle->SetPadGridY(kTRUE);
	gStyle->SetHistMinimumZero();
}

