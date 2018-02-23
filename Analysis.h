#include "TFile.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TPad.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TH1.h"
#include "TF1.h"
#include "TF2.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TImage.h"
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
#include <string>
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
// #include "Label_Arronax.h"	// 11-12
// #include "Label_CPO.h"	// 1-2
#include "Label_Cyrce.h"	// 4-6

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
const double uma_MeV_c2=931.49;	// atomic mass unit in MeV/c2
const double c=2.9971e8;	// speed of ligth in m/s
const double me_kg=9.1094e-31;	// electron mass
const double me_MeV_c2=0.510998; // MeV/c2
const double q=1.6022e-19;	// electronic charge in C
const double Na=6.0222e23;	// Avogadro number in mol-1
const double e0=8.8542e-12;	// Vaccum permitivity (F/m)
const double W_air=34.5e-6; //MeV 

const int N_box_per_irradation_max=5000;
const int N_STRIPS=32;
const int FIRST_ELEC=1;
const int LAST_ELEC=31;
const int MAX_PEAK=10;
const int MAX_SMPL=10000;
const int MAX_INTEGR=1000000;
const double strip_width=.3; //cm
const double gap_Dosion=3.; //mm

char Execution[80];
bool dosedistribution=true;
int bkgnd_param=0;
int Gausstofit=1;
int calibrage_used=0;
int area_find_param=0;
int lissage_param=0;
int Strip_label;
int Strip_number;
int borne_m_x=16;
int borne_M_x=17;
int borne_m_y=16;
int borne_M_y=17;
double bound_min=30.;
double bound_max=60.;
double bound_eoff=20.;
double energy=0.;
double calib_energy=0.;
int ind_en;
int ind_cal;
double multiple_energy[25];
double multiple_calib_th[25];
double multiple_calib[25];
double area_calib[25];
double calib_entry=0.;
double external_dose=0.;

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
double offXY[N_STRIPS][2];
double lissage_factor[N_STRIPS][2];

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

