#include "Analysis.h"

using namespace std;

bool CheckDouble(std::string const& s) 
{
  std::istringstream ss(s);
  double d;
  return (ss>>d)&&(ss>>std::ws).eof();
}

void EntryParameters(int config_simu)
{
	stringstream ss;
	ss<<config_simu;
	string indice=ss.str();
	stringstream ss2;
	int n;
	double m;

	int iint;
	int ichar;
	int ivar;
	char next;
	string data_file;
	string path_file;
	string filename="./Entry/Entry_param_"+indice+".txt";
	string pathname="./Entry/Entry_path.txt";
	// string filename="./Entry/Entry_param_1.txt";
	string tmp;

	Variable_init.clear();
	Value_init.clear();
	Variable_init.push_back("File"); // 0
	Value_init.push_back(0.);	//	0
	Variable_init.push_back("Path for the file"); // 1
	Value_init.push_back(0.);	//	1
	Variable_init.push_back("Background extraction (yes/no sub/function/default)"); // 2
	Value_init.push_back(0.);	//	2
	Variable_init.push_back("Calibrate data"); // 3
	Value_init.push_back(0.);	//	3
	Variable_init.push_back("Time sampling (s)"); // 4
	Value_init.push_back(0.);	//	4
	Variable_init.push_back("Exclusion strips X"); // 5
	Value_init.push_back(0.);	//	5
	Variable_init.push_back("Exclusion strips Y"); // 6
	Value_init.push_back(0.);	//	6
	Variable_init.push_back("Irradiation area finding way (charge/derivative/quanta/manual)"); // 7
	Value_init.push_back(0.);	//	7
	Variable_init.push_back("Particle energy"); // 8
	Value_init.push_back(0.);	//	8
	Variable_init.push_back("Threshold"); // 9
	Value_init.push_back(0.);	//	9
	Variable_init.push_back("Integration steps"); // 10
	Value_init.push_back(0.);	//	10
	Variable_init.push_back("Boundaries (s)"); // 11
	Value_init.push_back(0.);	//	11
	Variable_init.push_back("Acquisition time for electronic offset (s)"); // 12
	Value_init.push_back(20.);	//	12
	Variable_init.push_back("Gaussian to fit"); // 13
	Value_init.push_back(0.);	//	13
	Variable_init.push_back("Smoothing"); // 14
	Value_init.push_back(0.);	//	14
	Variable_init.push_back("Chamber value (Gy)"); // 15
	Value_init.push_back(0.);	//	15

	cout<<endl;
	ifstream pathfile_param(pathname.c_str());
	if(!pathfile_param)
	{
		cout<<"No entry file for the data path"<<endl;
		cout<<"No file: "<<pathname<<endl;
		exit(0);
	}
	else
	{
		getline(pathfile_param,path_file);
	}

	ifstream datafile_param(filename.c_str());
	// ifstream datafile_param("./Entry/Entry_param_1.txt");
	if(!datafile_param)
	{
		cout<<"No entry parameters file for this configuration: "<<config_simu<<endl;
		cout<<"No file: "<<filename<<endl;
		exit(0);
	}
	else
	{
		while(true)
		{
			string buffer;
			string variable;
			double value;
			getline(datafile_param,tmp);
			for(ichar=0;ichar<tmp.size();ichar++)
			{
				next=tmp[ichar];
				if(next!=':')
					variable+=next;
				else
				{	
					for(iint=ichar+2;iint<tmp.size();iint++)
					{	
						next=tmp[iint];
						buffer+=next;
					}
					ichar=tmp.size();	
				}	
			}
			int ind_value=-1;
			for(ivar=0;ivar<Variable_init.size();ivar++)
			{
				if(!variable.compare(Variable_init[ivar]))
					ind_value=ivar;
			}
			if(ind_value==-1)
				cout<<"Variable "<<variable<<" not defined. Maybe check the entry file."<<endl;
			else
			{
				if(buffer.find_first_not_of(' ')!=std::string::npos)
				{
					if(ind_value==0)
						data_file=buffer;
					if(ind_value==1)
						path_file+=buffer;
					if(ind_value==2)
					{
						if(!buffer.compare("default"))
							Value_init[ind_value]=0;
						if(!buffer.compare("yes")||!buffer.compare("Yes"))
							Value_init[ind_value]=1;
						if(!buffer.compare("no sub"))
							Value_init[ind_value]=2;
						if(!buffer.compare("function"))
							Value_init[ind_value]=3;
					}
					if(ind_value==3)
					{
						Value_init[ind_value]=0.;
						calibrage_used=0;
						if(CheckDouble(buffer)==1)
						{
							value=(double)atof(buffer.c_str());
							if(value<=0.)
								cout<<"No valid input data for calibration:"<<value<<endl;
							else
							{
								cout<<Variable_init[ind_value]<<" default value: "<<Value_init[ind_value]<<" fC/part; new value: "<<value<<" fC/part"<<endl;
								Value_init[ind_value]=value;
								calibrage_used=1;
							}
						}
						else
						{
							std::size_t found=buffer.find_first_of(' ');
							std::string bufferofbuffer=buffer.substr(0,found);
							if(!bufferofbuffer.compare("nothing"))
								cout<<"No input data for calibration"<<endl;
							if(!bufferofbuffer.compare("proton_Cyrce"))
								calibrage_used=2;
							if(!bufferofbuffer.compare("proton_Arronax"))
								calibrage_used=3;
							if(!bufferofbuffer.compare("proton_Arronax_68_MeV"))
								calibrage_used=4;
							if(!bufferofbuffer.compare("multiple"))
							{
								calibrage_used=5;
								cout<<"Multiple calibration values for primary particles"<<endl;
								buffer=buffer.substr(found+1);
								ss2<<buffer;
								ind_cal=0;
								do{
									ss2>>multiple_calib[ind_cal];
									ind_cal++;
								}while(!ss2.eof()&&ind_cal<=25);
								ss2.clear();
								// ind_cal--;
							}
						}
					}
					if(ind_value==8)
					{
						energy=0.;
						if(CheckDouble(buffer)==1)
						{
							value=(double)atof(buffer.c_str());
							if(value<=0.)
							{
								cout<<"No valid input data for primary particles energy:"<<value<<endl;
								Value_init[ind_value]=0.;
							}
							else
							{
								cout<<Variable_init[ind_value]<<" default value: "<<Value_init[ind_value]<<" MeV; new value: "<<value<<" MeV"<<endl;
								Value_init[ind_value]=value;
								energy=value;
							}
						}
						else
						{
							Value_init[ind_value]=0.;
							std::size_t found=buffer.find_first_of(' ');
							std::string bufferofbuffer=buffer.substr(0,found);
							if(!bufferofbuffer.compare("nothing"))
								cout<<"No input data for primary particles energy"<<endl;
							if(!bufferofbuffer.compare("multiple"))
							{
								cout<<"Multiple energy values for primary particles"<<endl;
								buffer=buffer.substr(found+1);
								ss2<<buffer;
								ind_en=0;
								do{
									ss2>>multiple_energy[ind_en];
									ind_en++;
								}while(!ss2.eof()&&ind_en<=25);
								ss2.clear();
								// ind_en--;
								Value_init[ind_value]=-1.;
								energy=-1.;
							}
						}
					}
					if(ind_value==4||ind_value==10)
					{
						value=(double)atof(buffer.c_str());
						if(value==Value_init[ind_value])
							cout<<Variable_init[ind_value]<<" default value keeped: "<<Value_init[ind_value]<<endl;
						else	
							cout<<Variable_init[ind_value]<<" default value: "<<Value_init[ind_value]<<"; new value: "<<value<<endl;
						Value_init[ind_value]=value;
					}
					if(ind_value==5)
					{
						ss2<<buffer;
						ss2>>n;
						borne_m_x=n;
						ss2>>n;
						borne_M_x=n;
						ss2.clear();
					}
					if(ind_value==6)
					{
						ss2<<buffer;
						ss2>>n;
						borne_m_y=n;
						ss2>>n;
						borne_M_y=n;
						ss2.clear();
					}
					if(ind_value==7)
					{
						if(!buffer.compare("derivative"))
							Value_init[ind_value]=0;
						if(!buffer.compare("charge"))
							Value_init[ind_value]=1;
						if(!buffer.compare("quanta"))
							Value_init[ind_value]=2;
						if(!buffer.compare("manual"))
							Value_init[ind_value]=3;
					}
					if(ind_value==11)
					{
						ss2<<buffer;
						ss2>>m;
						bound_min=m;
						ss2>>m;
						bound_max=m;
						ss2.clear();
					}
					if(ind_value==9||ind_value==12||ind_value==13)
					{
						value=(double)atof(buffer.c_str());
						if(value==Value_init[ind_value])
							cout<<Variable_init[ind_value]<<" default value keeped: "<<Value_init[ind_value]<<endl;
						else	
							cout<<Variable_init[ind_value]<<" default value: "<<Value_init[ind_value]<<"; new value: "<<value<<endl;
						Value_init[ind_value]=value;
					}
					if(ind_value==14)
					{
						if(!buffer.compare("no")||!buffer.compare("No"))
							Value_init[ind_value]=0;
						if(!buffer.compare("yes")||!buffer.compare("Yes"))
							Value_init[ind_value]=1;
					}
					if(ind_value==15)
					{
						if(CheckDouble(buffer)==1)
						{
							Value_init[ind_value]=(double)atof(buffer.c_str());
							external_dose=(double)atof(buffer.c_str());
						}
						else
						{
							if(!buffer.compare("nothing"))
								Value_init[ind_value]=0;
						}
					}
				}	
				else
					cout<<"Value not define for "<<variable<<". Keeping the default one: "<<Value_init[ind_value]<<". Maybe check the entry file."<<endl;
			}	
			if(datafile_param.eof()) break;
		}
		datafile_param.close();
	}	

	// data_faster_file="../DATA/gaussian_0001.fast";
	filename=path_file+data_file; 
	ifstream datafile_faster(filename.c_str());
	if(!datafile_faster)
	{
		cout<<"No data file: "<<filename<<endl;
		exit(0);
	}
	else
		data_faster_file=filename;
	datafile_faster.close();

	if(borne_m_x>borne_M_x||borne_m_x<0||borne_m_x>32||borne_M_x<0||borne_M_x>32)
	{
		cout<<"Error with strips exclusion for X, "<<borne_m_x<<" - "<<borne_M_x<<" default values used"<<endl;
		borne_m_x=16;
		borne_M_x=17;
	}
	if(borne_m_y>borne_M_y||borne_m_y<0||borne_m_y>32||borne_M_y<0||borne_M_y>32)
	{
		cout<<"Error with strips exclusion for Y, "<<borne_m_y<<" - "<<borne_M_y<<" default values used"<<endl;
		borne_m_y=16;
		borne_M_y=17;
	}

	cout<<endl;
	cout<<"=================================================="<<endl;
	cout<<" Parameters for the initialisation; config: "<<config_simu<<endl;
	cout<<"--------------------------------------------------"<<endl;
	cout<<" File of data: "<<data_faster_file<<endl;

	ivar=2;
	if(Value_init[ivar]==0)
		cout<<" No background extraction, default values used"<<endl;
	if(Value_init[ivar]==1)
		cout<<" Background extraction"<<endl;
	if(Value_init[ivar]==2)
		cout<<" No background substraction"<<endl;
	if(Value_init[ivar]==3)
	{
		cout<<" Background substraction using function"<<endl;
		cout<<" Exclusion strips for X: "<<borne_m_x<<" - "<<borne_M_x<<endl;
		cout<<" Exclusion strips for Y: "<<borne_m_y<<" - "<<borne_M_y<<endl;
	}
	bkgnd_param=Value_init[ivar];
	
	ivar=3;
	if(calibrage_used==5)
	{
		cout<<" Multiple calibration values: "<<ind_cal<<" values"<<endl;
		for(int ii=0;ii<ind_cal;ii++)
			cout<<" "<<multiple_calib[ii]<<" fC/part;";
		cout<<endl;
	}	
	if(calibrage_used==0&&energy==0)
		cout<<" No calibration value"<<endl;
	if(calibrage_used==0&&energy!=0)
	{
		cout<<" Calibration from energy used"<<endl;
		calibrage_used=6;
	}
	if(calibrage_used==1)
		cout<<" Calibration value: "<<Value_init[ivar]<<" fC/part"<<endl;
	if(calibrage_used==2)
		cout<<" Cyrce calibration values used"<<endl;
	if(calibrage_used==3)
		cout<<" Arronax calibration values used"<<endl;
	if(calibrage_used==4)
		cout<<" Arronax calibration value used for 68 MeV protons"<<endl;
	calib_entry=Value_init[ivar];

	ivar=8;
	if(Value_init[ivar]<0.)
	{
		cout<<" Multiple energies for primary particles: "<<ind_en<<" values"<<endl;
		for(int ii=0;ii<ind_en;ii++)
			cout<<" "<<multiple_energy[ii]<<" MeV;";
		cout<<endl;
	}	
	if(Value_init[ivar]==0.)
		cout<<" No energy for the primary particles"<<endl;
	if(Value_init[ivar]>0.)
		cout<<" Energy of the particles: "<<energy<<endl;

	if(ind_en!=ind_cal)
	{
		cout<<" Be aware that the numbers of calibration and energy values are different"<<endl;
		dosedistribution=false;
	}
	if(energy==0.&&calibrage_used==0)
		dosedistribution=false;

	ivar=4;
	cout<<" "<<Variable_init[ivar]<<": "<<Value_init[ivar]<<endl;
	SamplingTime=Value_init[ivar];

	ivar=10;
	cout<<" "<<Variable_init[ivar]<<": "<<(int)Value_init[ivar]<<endl;
	IntegrationStep=(int)Value_init[ivar];

	ivar=12;
	cout<<" "<<Variable_init[ivar]<<": "<<Value_init[ivar]<<endl;
	bound_eoff=Value_init[ivar];

	ivar=13;
	cout<<" "<<Variable_init[ivar]<<": "<<(int)Value_init[ivar]<<endl;
	Gausstofit=(int)Value_init[ivar];

	ivar=7;
	if(Value_init[ivar]==0)
		cout<<" Derivative charge method used with a threshold of "<<Value_init[9]<<endl;
	if(Value_init[ivar]==1)
		cout<<" Charge method used with a threshold of "<<Value_init[9]<<" pC"<<endl;
	if(Value_init[ivar]==2)
		cout<<" Quanta method used with a threshold of "<<Value_init[9]<<endl;
	if(Value_init[ivar]==3)
		cout<<" Irradiation manually defined ["<<bound_min<<" s;"<<bound_max<<" s]"<<endl;
	area_find_param=Value_init[ivar];

	ivar=14;
	if(Value_init[ivar]==0)
		cout<<" No smoothing applied"<<endl;
	if(Value_init[ivar]==1)
	{
		lissage_param=1;
		cout<<" Smoothing applied"<<endl;
	}

	ivar=15;
	if(Value_init[ivar]==0)
		cout<<" No additional dose value used"<<endl;
	else
		cout<<" External dose value of "<<Value_init[ivar]<<" Gy"<<endl;

	// for(ivar=4;ivar<Variable_init.size();ivar++)
	// 	cout<<" "<<Variable_init[ivar]<<": "<<Value_init[ivar]<<endl;
	cout<<"=================================================="<<endl;
	cout<<endl;

	if(logfileprint==true)
	{
		logfile.open("Analysis.log",std::ios::out);

		logfile<<endl;
		logfile<<"=================================================="<<endl;
		logfile<<" Parameters for the initialisation; config: "<<config_simu<<endl;
		logfile<<"--------------------------------------------------"<<endl;
		logfile<<" File of data: "<<data_faster_file<<endl;

		ivar=2;
		if(Value_init[ivar]==0)
			logfile<<" No background extraction, default values used"<<endl;
		if(Value_init[ivar]==1)
			logfile<<" Background extraction"<<endl;
		if(Value_init[ivar]==2)
			logfile<<" No background substraction"<<endl;
		if(Value_init[ivar]==3)
		{
			logfile<<" Background substraction using function"<<endl;
			logfile<<" Exclusion strips for X: "<<borne_m_x<<" - "<<borne_M_x<<endl;
			logfile<<" Exclusion strips for Y: "<<borne_m_y<<" - "<<borne_M_y<<endl;
		}
		
		ivar=3;
		if(calibrage_used==5)
		{
			logfile<<" Multiple calibration values: "<<ind_cal<<" values"<<endl;
			for(int ii=0;ii<ind_cal;ii++)
				logfile<<" "<<multiple_calib[ii]<<" fC/part;";
			logfile<<endl;
		}	
		if(calibrage_used==0&&energy==0)
			logfile<<" No calibration value"<<endl;
		if(calibrage_used==0&&energy!=0)
			logfile<<" Calibration from energy used"<<endl;
		if(calibrage_used==1)
			logfile<<" Calibration value: "<<Value_init[ivar]<<" fC/part"<<endl;
		if(calibrage_used==2)
			logfile<<" Cyrce calibration values used"<<endl;
		if(calibrage_used==3)
			logfile<<" Arronax calibration values used"<<endl;
		if(calibrage_used==4)
			logfile<<" Arronax calibration value used for 68 MeV protons"<<endl;

		ivar=8;
		if(Value_init[ivar]<0.)
		{
			logfile<<" Multiple energies for primary particles: "<<ind_en<<" values"<<endl;
			for(int ii=0;ii<ind_en;ii++)
				logfile<<" "<<multiple_energy[ii]<<" MeV;";
			logfile<<endl;
		}
		if(Value_init[ivar]==0)
			logfile<<" No energy for the primary particles"<<endl;
		if(Value_init[ivar]>0.)
			logfile<<" Energy of the particles: "<<Value_init[ivar]<<endl;

		if(ind_en!=ind_cal)
			logfile<<" Be aware that the numbers of calibration and energy values are different"<<endl;

		ivar=4;
		logfile<<" "<<Variable_init[ivar]<<": "<<Value_init[ivar]<<endl;

		ivar=10;
		logfile<<" "<<Variable_init[ivar]<<": "<<(int)Value_init[ivar]<<endl;

		ivar=12;
		logfile<<" "<<Variable_init[ivar]<<": "<<Value_init[ivar]<<endl;

		ivar=13;
		logfile<<" "<<Variable_init[ivar]<<": "<<(int)Value_init[ivar]<<endl;

		ivar=7;
		if(Value_init[ivar]==0)
			logfile<<" Derivative charge method used with a threshold of "<<Value_init[9]<<endl;
		if(Value_init[ivar]==1)
			logfile<<" Charge method used with a threshold of "<<Value_init[9]<<" pC"<<endl;
		if(Value_init[ivar]==2)
			logfile<<" Quanta method used with a threshold of "<<Value_init[9]<<endl;
		if(Value_init[ivar]==3)
			logfile<<" Irradiation manually defined ["<<bound_min<<" s;"<<bound_max<<" s]"<<endl;

		ivar=14;
		if(Value_init[ivar]==0)
			logfile<<" No smoothing applied"<<endl;
		if(Value_init[ivar]==1)
			logfile<<" Smoothing applied"<<endl;

		ivar=15;
		if(Value_init[ivar]==0)
			logfile<<" No additional dose value used"<<endl;
		else
			logfile<<" External dose value of "<<Value_init[ivar]<<" Gy"<<endl;

		// for(ivar=4;ivar<Variable_init.size();ivar++)
		// 	logfile<<" "<<Variable_init[ivar]<<": "<<Value_init[ivar]<<endl;
		logfile<<"=================================================="<<endl;
		logfile<<endl;
	}

	data_folder="Output/"+data_file;
}

double Extremum(double a,double b,double c,double d,double e)
{
	double extrm=0.;
	if(fabs(a)>fabs(b))
		extrm=a;
	if(fabs(extrm)<fabs(c))
		extrm=c;
	if(fabs(extrm)<fabs(d))
		extrm=d;
	if(fabs(extrm)<fabs(e))
		extrm=e;
	// cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<extrm<<" "<<endl;
	return extrm;
}

double TEL_value(int material,double e_part)
{
	double Z_part=1.;
	double dEdX=0.;
	double density;
	double Zeff_mat;
	double Aeff_mat;
	double Ieff_mat;
	if(material==0)
	{
		// Air
		density=1.2e-3;
		Zeff_mat=737.5;
		Aeff_mat=1480.2;
		Ieff_mat=85.7*1.E-6; // MeV
	}
	if(material==1)
	{
		// Water
		density=1.;
		Zeff_mat=10.;
		Aeff_mat=18.;
		Ieff_mat=69.*1.E-6; // MeV
	}
	double N=Na*density*1.e6*Zeff_mat/Aeff_mat;

	if(e_part>0.)
	{
		double beta=TMath::Sqrt(1-1/TMath::Power(1+e_part/uma_MeV_c2,2));
		double v_part=beta*c;
		double bethe0=4.*TMath::Pi()*N*TMath::Power(Z_part,2)/(me_kg*TMath::Power(v_part,2));
		double bethe1=TMath::Power(TMath::Power(q,2)/(4.*TMath::Pi()*e0),2);
		double bethe2=TMath::Log(2*me_MeV_c2*TMath::Power(beta,2)/(Ieff_mat*(1-TMath::Power(beta,2))))-TMath::Power(beta,2);
		dEdX=bethe0*bethe1*bethe2/(q*1.e9);
	}
	dEdX=TMath::Max(0.,dEdX);
	return dEdX; //in keV.um-1
}

double Loss_value(double e_part,double density)
{
	int A_part=1;
	double Z_part=1.;
	double dEdX=0.;
	double e_loss=0.;

	double gap=gap_Dosion*2.;
	double e_part_loss=e_part;

	double length_left=gap*1000.;
	double dx=1./density; //um
	
	if(dx>1.)
		dx=1.;

	while(length_left>0.&&e_part_loss>0.)
	{
		dEdX=TEL_value(0,e_part_loss)/1000.;	// MeV
		if(length_left<dx)
			dx=length_left;
		if(e_part_loss>dEdX*dx/A_part)
		{
			e_loss+=dEdX*dx; //convert
			e_part_loss-=dEdX*dx/A_part;
		}
		else
		{
			e_loss+=e_part_loss*A_part; //convert
			e_part_loss=0;
		}
		length_left-=dx;
	}
	return e_loss;
}

double Calib_value(int area,int in_area,double e_part)
{
	double calib_polynome;
	if(in_area!=0)
		return 0.;

	if(calibrage_used==0)
		return 0.;
		
	if(calibrage_used==1)
		return calib_entry; 

	if(calibrage_used==2)
	{
		if(e_part==0.)
			return 1000./data_calib[area];
		else
		{
			calib_polynome=data_par[0]+data_par[1]*e_part+data_par[2]*pow(e_part,2)+data_par[3]*pow(e_part,3);
			return 1000./calib_polynome;
		}
	}

	if(calibrage_used==3)
		return 0.028431749; //68 MeV p+ Arronax

	if(calibrage_used==4)
		return 0.028431749; //68 MeV p+ Arronax

	if(calibrage_used==5)
	{
		int indice=area%ind_cal;
		return multiple_calib[area];
	}
	
	if(calibrage_used==6)
	{
		int indice=area%ind_en;
		return multiple_calib_th[area];
	}

	if(calibrage_used==-1)
		return Loss_value(energy,1.2e-3)/W_air*q*1.E15;

	// return .01875; //120 MeV
	// return .01532; //160 MeV
	// return .01339; //196 MeV
	// return .01322; //200 MeV
	// return Loss_value(e_part,1.2e-3)/W_air*q*1.E15;
}

void Calibrage(char *file,double chargeTot_X,double chargeTot_Y)
{
	Vect_calib_factor.clear();
	Vect_calib_charge.clear();
	Vect_calib_quanta.clear();

	Vect_calib_factor.resize(1);
	Vect_calib_charge.resize(1);
	Vect_calib_quanta.resize(1);

	faster_file_reader_p reader;
	faster_data_p data;
	scaler_counter scaler_cnt;
	reader=faster_file_reader_open(file);
	int label;
	int count_quanta=0;
	double t0;
	double fasterTime;
	double chargeTot_pC;
	double calib_factor;
	double calib_factor_X;
	double calib_factor_Y;
	long int last_quanta=-1;
	long int dquanta=-1;
	long int quanta=-1;
	double* vect_time_q=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_quanta=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_dquanta=(double*)malloc(MAX_INTEGR*sizeof(double));

	t0=-1;
	while((data=faster_file_reader_next(reader))!=NULL) 
	{
		label=faster_data_label(data);
		if(label==LabelCount)
		{
			faster_data_load(data,&scaler_cnt);
			if(t0==-1)
				t0=faster_data_clock_sec(data);
			fasterTime=faster_data_clock_sec(data)-t0;
			vect_time_q[count_quanta]=fasterTime;
			if(last_quanta==-1)
				last_quanta=scaler_cnt.quanta;
			dquanta=scaler_cnt.quanta-last_quanta;
			last_quanta=scaler_cnt.quanta;
			quanta=scaler_cnt.quanta;
			vect_quanta[count_quanta]=quanta;
			vect_dquanta[count_quanta]=dquanta;
			count_quanta++;
		}
	}
	if(vect_time_q[0]<0.)
		vect_time_q[0]=0.;
	count_quanta--;
	faster_file_reader_close(reader);
	if(quanta>0)
	{
		TCanvas *cQuanta= new TCanvas("Quanta over time","",2000,1000);
		cQuanta->SetCanvasSize(2000,1000);
		cQuanta->Divide(1,2);

		cQuanta->cd(1);
		TGraph *TG_dQuanta=new TGraph(count_quanta,vect_time_q,vect_dquanta);
		TG_dQuanta->SetMarkerColor(2);
		TG_dQuanta->SetLineColor(2);
		TG_dQuanta->SetLineWidth(1.5);
		TG_dQuanta->SetTitle("Differential quanta over time");
		TG_dQuanta->GetHistogram()->SetTitleSize(0.035);
		TG_dQuanta->GetXaxis()->SetTitle("Time (s)");
		TG_dQuanta->GetXaxis()->SetTickSize(0.01);
		TG_dQuanta->GetXaxis()->SetTitleSize(0.06);
		TG_dQuanta->GetXaxis()->SetLabelSize(0.05);
		TG_dQuanta->GetYaxis()->SetTitle("dQuanta");
		TG_dQuanta->GetYaxis()->SetTickSize(0.01);
		TG_dQuanta->GetYaxis()->SetTitleSize(0.06);
		TG_dQuanta->GetYaxis()->CenterTitle();
		TG_dQuanta->GetYaxis()->SetLabelSize(0.05);
		TG_dQuanta->Write("dQuanta");
		TG_dQuanta->Draw("AL");

		cQuanta->cd(2);
		TGraph *TG_Quanta=new TGraph(count_quanta,vect_time_q,vect_quanta);
		TG_Quanta->SetMarkerColor(4);
		TG_Quanta->SetLineColor(4);
		TG_Quanta->SetLineWidth(1.5);
		TG_Quanta->SetTitle("Quanta over time");
		TG_Quanta->GetHistogram()->SetTitleSize(0.035);
		TG_Quanta->GetXaxis()->SetTitle("Time (s)");
		TG_Quanta->GetXaxis()->SetTickSize(0.01);
		TG_Quanta->GetXaxis()->SetTitleSize(0.06);
		TG_Quanta->GetXaxis()->SetLabelSize(0.05);
		TG_Quanta->GetYaxis()->SetTitle("Quanta");
		TG_Quanta->GetYaxis()->SetTickSize(0.01);
		TG_Quanta->GetYaxis()->SetTitleSize(0.06);
		TG_Quanta->GetYaxis()->CenterTitle();
		TG_Quanta->GetYaxis()->SetLabelSize(0.05);
		TG_Quanta->Write("Quanta");
		TG_Quanta->Draw("AL");

		cQuanta->SaveAs("Picture/Quanta.png");

		TG_Quanta->Delete();
		TG_dQuanta->Delete();
		cQuanta->Destructor();

		chargeTot_pC=(chargeTot_X+chargeTot_Y)/2.;
		calib_factor=chargeTot_pC/quanta;
		calib_factor_X=chargeTot_X/quanta;
		calib_factor_Y=chargeTot_Y/quanta;
		cout<<"Calibrage issu des quanta"<<endl;
		cout<<"Calib.  X : "<<chargeTot_X<<"(pC)/"<<quanta<<"(part.) = "<<calib_factor_X*1000.<<"(fC/part.)"<<endl;
		cout<<"Calib.  Y : "<<chargeTot_Y<<"(pC)/"<<quanta<<"(part.) = "<<calib_factor_Y*1000.<<"(fC/part.)"<<endl;
		cout<<"Calibrage : "<<chargeTot_pC<<"(pC)/"<<quanta<<"(part.) = "<<calib_factor*1000.<<"(fC/part.)"<<endl;

		if(logfileprint==true)
		{
			logfile<<"Calibrage issu des quanta"<<endl;
			logfile<<"Calib.  X : "<<chargeTot_X<<"(pC)/"<<quanta<<"(part.) = "<<calib_factor_X*1000.<<"(fC/part.)"<<endl;
			logfile<<"Calib.  Y : "<<chargeTot_Y<<"(pC)/"<<quanta<<"(part.) = "<<calib_factor_Y*1000.<<"(fC/part.)"<<endl;
			logfile<<"Calibrage : "<<chargeTot_pC<<"(pC)/"<<quanta<<"(part.) = "<<calib_factor*1000.<<"(fC/part.)"<<endl;
		}

		Vect_calib_factor.push_back(calib_factor);
		Vect_calib_charge.push_back(chargeTot_pC);
		Vect_calib_quanta.push_back(quanta);
	}
	else
		cout<<"Pas de données de calibrage, nombre de quanta nul"<<endl;

	free(vect_time_q);
	free(vect_quanta);
	free(vect_dquanta);
}

void Lissage()
{
	ifstream lissage_file("Lissage_dosion_LPC.txt");
	int tmp;
	if(!lissage_file)
	{
		cout<<" No input file for smoothing parameters"<<endl;
		return;
	}
	for(int i=0;i<N_STRIPS;i++)
		lissage_file>>tmp>>lissage_factor[i][0]>>lissage_factor[i][1];
	lissage_file.close();
}

void Scaler(char *file,double decimation,int tot_area,double signal_time[][2],double vect_charge_t[],double vect_charge_x[],double vect_charge_y[])
{
	Vect_calib_factor.clear();
	Vect_calib_charge.clear();
	Vect_calib_quanta.clear();

	Vect_calib_factor.resize(tot_area);
	Vect_calib_charge.resize(tot_area);
	Vect_calib_quanta.resize(tot_area);

	faster_file_reader_p reader;
	faster_data_p data;
	scaler_measurement scaler_meas;
	reader=faster_file_reader_open(file);
	TString name_qth2th;
	int label;
	int j;
	int current_area=0;
	int in_area=-1;
	double t0;
	double fasterTime;
	double Qth2th;
	double maxy;
	long int quanta=0;
	double calib_factor;

	TH1F *hQth2th = new TH1F("hQth2th","Charge threshold to threshold",1000,0.,4000);

	t0=-1;
	// while((data=faster_file_reader_next(reader))!=NULL) 
	while((data=faster_file_reader_next(reader))!=NULL&&current_area<tot_area) 
	{
		label=faster_data_label(data);
		if(label==LabelScaler)
		{
			faster_data_load(data,&scaler_meas);
			if(t0==-1)
				t0=faster_data_clock_sec(data);
			fasterTime=faster_data_clock_sec(data)-t0;
			if(fasterTime>signal_time[current_area][0]&&fasterTime<signal_time[current_area][1])
			{
				in_area=0;
				Qth2th=scaler_meas.qtt;
				hQth2th->Fill(Qth2th);
				j=MAX_QNT-1;
				while(Seuil_quanta[current_area][j]>Qth2th&&j>0)
					j--;
				// if((double)rand()/RAND_MAX>0.99)
				// 	cout<<" "<<j<<" "<<Seuil_quanta[current_area][j]<<" "<<Qth2th<<endl;
				quanta+=j+1;
			}
			else
				if(in_area==0&&fasterTime>signal_time[current_area][1])
					in_area=1;
			if(in_area==1)
			{
				in_area=1;
				name_qth2th="TH1_Qth2th_";
				name_qth2th+=(current_area+1);
				TCanvas *cQth2th= new TCanvas(name_qth2th);
				cQth2th->SetCanvasSize(2000,1600);
				cQth2th->SetLogy();

				hQth2th->SetTitle("Charge threshold to threshold");
				hQth2th->SetTitleSize(0.0);
				hQth2th->SetStats(0);
				hQth2th->GetXaxis()->SetTitle("Charge");
				hQth2th->GetXaxis()->SetTickSize(0.01);
				hQth2th->GetXaxis()->SetTitleSize(0.036);
				hQth2th->GetXaxis()->CenterTitle();
				hQth2th->GetXaxis()->SetLabelSize(0.02);
				hQth2th->GetYaxis()->SetTitle("Count (u.a)");
				hQth2th->GetYaxis()->SetTickSize(0.01);
				hQth2th->GetYaxis()->SetTitleSize(0.036);
				hQth2th->GetYaxis()->CenterTitle();
				hQth2th->GetYaxis()->SetLabelSize(0.02);
				hQth2th->Draw();
				TLine *line= new TLine();
				line->SetLineColor(2);
				line->SetLineWidth(3);
				maxy=hQth2th->GetMaximum();
				for(int i=0;i<MAX_QNT;i++)
					line->DrawLine(Seuil_quanta[current_area][i],0,Seuil_quanta[current_area][i],maxy);
				name_qth2th="Picture/Qth2th_";
				name_qth2th+=(current_area+1);
				name_qth2th+=".png";
				hQth2th->Write(name_qth2th);
				cQth2th->SaveAs(name_qth2th);
				hQth2th->Reset();
				cQth2th->Destructor();
				
				quanta/=decimation;
				calib_factor=vect_charge_t[current_area]/quanta;
				cout<<"Calibrage issu du scaler"<<endl;
				if(tot_area==1)
				{
					cout<<"Calib.  X : "<<vect_charge_x[current_area]<<"(pC)/"<<quanta<<"(part.) = "<<vect_charge_x[current_area]/quanta*1000.<<"(fC/part.)"<<endl;
					cout<<"Calib.  Y : "<<vect_charge_y[current_area]<<"(pC)/"<<quanta<<"(part.) = "<<vect_charge_y[current_area]/quanta*1000.<<"(fC/part.)"<<endl;
				}
				cout<<"Calibrage : "<<vect_charge_t[current_area]<<"(pC)/"<<quanta<<"(part.) = "<<calib_factor*1000.<<"(fC/part.)"<<endl;
				if(logfileprint==true)
				{
					logfile<<"Calibrage issu du scaler"<<endl;
					if(tot_area==1)
					{
						logfile<<"Calib.  X : "<<vect_charge_x[current_area]<<"(pC)/"<<quanta<<"(part.) = "<<vect_charge_x[current_area]/quanta*1000.<<"(fC/part.)"<<endl;
						logfile<<"Calib.  Y : "<<vect_charge_y[current_area]<<"(pC)/"<<quanta<<"(part.) = "<<vect_charge_y[current_area]/quanta*1000.<<"(fC/part.)"<<endl;
					}
					logfile<<"Calibrage : "<<vect_charge_t[current_area]<<"(pC)/"<<quanta<<"(part.) = "<<calib_factor*1000.<<"(fC/part.)"<<endl;
				}

				Vect_calib_factor[current_area]=calib_factor;
				Vect_calib_charge[current_area]=vect_charge_t[current_area];
				Vect_calib_quanta[current_area]=quanta;

				in_area=2;
				current_area+=1;
				quanta=0;
			}
		}
	}
	faster_file_reader_close(reader);
}

void ElectronicOffsetExtraction(char *file,double first_signal,double last_signal,double time_i,double time_f,int beg_on,int end_on)
{
	faster_file_reader_p reader;
	faster_data_p data;
	electrometer_data electro;
	reader=faster_file_reader_open(file);

	int label;
	int signal_eoff=0;
	int count_X=0;
	int count_Y=0;
	double charge;
	double t0=-1;
	double fasterTime;
	double eoff_sample=bound_eoff;
	double EOffX[N_STRIPS];
	double EOffY[N_STRIPS];

	debut_eoff=time_i;
	fin_eoff=time_f;

	if(first_signal>eoff_sample)
		debut_eoff=first_signal-eoff_sample;

	if(last_signal<fin_eoff-eoff_sample)
		fin_eoff=last_signal+eoff_sample;

	if(bkgnd_param==1||bkgnd_param==3)
	{
		cout<<"Extraction du bruit de fond électronique"<<endl;
		for(int i=0;i<N_STRIPS;i++)
		{
			EOffX[i]=0.;
			EOffY[i]=0.;
		}
		reader=faster_file_reader_open(file);
		while((data=faster_file_reader_next(reader))!=NULL) 
		{
			label=faster_data_label(data);
			if(label==LabelX||label==LabelY)
			{
				faster_data_load(data,&electro);
				if(t0==-1)
					t0=faster_data_clock_sec(data);
				fasterTime=faster_data_clock_sec(data)-t0;
				if((fasterTime<(first_signal-1.)&&fasterTime>debut_eoff&&beg_on==0)||(fasterTime>(last_signal+1.)&&fasterTime<fin_eoff&&end_on==0))
				{
					signal_eoff=1;
					for(int j=0;j<N_STRIPS;j++) 
					{
						charge=electrometer_channel_charge_pC(electro,j+1);
						switch(label) 
						{
							case LabelX:
								EOffX[j]+=charge;
								if(j==0)
									count_X++;
							break;
							case LabelY:
								EOffY[j]+=charge;
								if(j==0)
									count_Y++;
							break;
						}
					}
				}
			}
		}
		faster_file_reader_close(reader);
		if(signal_eoff==1)
			for(int i=0;i<N_STRIPS;i++)
			{
				offXY[i][0]=EOffX[i]/count_X;
				offXY[i][1]=EOffY[i]/count_Y;
			}
	}

	if(bkgnd_param==2)
		for(int i=0;i<N_STRIPS;i++)
		{
			offXY[i][0]=0.;
			offXY[i][1]=0.;
		}

	if(bkgnd_param==0)
	{
		ifstream eoff_in_file("Offset.txt");
		int tmp;
		for(int i=0;i<N_STRIPS;i++)
			eoff_in_file>>tmp>>offXY[i][0]>>offXY[i][1];
		eoff_in_file.close();
	}

	TCanvas *cEOff = new TCanvas("Electronique offset");
	cEOff->SetCanvasSize(2000,1000);
	cEOff->Divide(2,2);
	TH1F *hEOffX = new TH1F("hEOffX","Electronique offset for X strips",N_STRIPS,.5,32.5);
	TH1F *hEOffY = new TH1F("hEOffY","Electronique offset for Y strips",N_STRIPS,.5,32.5);
	for(int i=0;i<N_STRIPS;i++)
	{
		// EOffX[i]=offset;
		// EOffY[i]=offset;
		EOffX[i]=offXY[i][0];
		EOffY[i]=offXY[i][1];
		hEOffX->SetBinContent(i+1,EOffX[i]);
		hEOffY->SetBinContent(i+1,EOffY[i]);
		// cout<<EOffX[i]<<" "<<EOffY[i]<<endl;
	}
	TH1F *hEOffXZ=(TH1F*)hEOffX->Clone();
	TH1F *hEOffYZ=(TH1F*)hEOffY->Clone();

	cEOff->cd(1);
	hEOffX->SetFillColor(2);
	hEOffX->GetXaxis()->SetTickSize(0.01);
	hEOffX->GetXaxis()->SetNdivisions(N_STRIPS);
	// hEOffX->GetXaxis()->SetTitle("Strip");
	hEOffX->GetXaxis()->CenterTitle();
	hEOffX->GetXaxis()->SetTickSize(0.01);
	hEOffX->GetXaxis()->SetTitleSize(0.06);
	hEOffX->GetXaxis()->SetLabelSize(0.05);
	hEOffX->GetYaxis()->SetTickSize(0.01);
	hEOffX->GetYaxis()->SetTitle("Charge (pC)");
	hEOffX->GetYaxis()->CenterTitle();
	hEOffX->GetYaxis()->SetTickSize(0.01);
	hEOffX->GetYaxis()->SetTitleSize(0.06);
	hEOffX->GetYaxis()->SetLabelSize(0.05);
	hEOffX->SetBarWidth(0.8);
	hEOffX->SetBarOffset(0.1);
	hEOffX->SetStats(0);
	hEOffX->Draw("b");
	hEOffX->Write("EOffX");

	cEOff->cd(3);
	hEOffXZ->SetFillColor(4);
	hEOffXZ->SetTitle("");
	hEOffXZ->GetXaxis()->SetTickSize(0.01);
	hEOffXZ->GetXaxis()->SetNdivisions(N_STRIPS);
	hEOffXZ->GetXaxis()->SetTitle("Strip");
	hEOffXZ->GetXaxis()->CenterTitle();
	hEOffXZ->GetXaxis()->SetTickSize(0.01);
	hEOffXZ->GetXaxis()->SetTitleSize(0.06);
	hEOffXZ->GetXaxis()->SetLabelSize(0.05);
	hEOffXZ->GetYaxis()->SetTickSize(0.01);
	hEOffXZ->GetYaxis()->SetTitle("Charge (pC)");
	hEOffXZ->GetYaxis()->CenterTitle();
	hEOffXZ->GetYaxis()->SetTickSize(0.01);
	hEOffXZ->GetYaxis()->SetTitleSize(0.06);
	hEOffXZ->GetYaxis()->SetLabelSize(0.05);
	hEOffXZ->SetBarWidth(0.8);
	hEOffXZ->SetBarOffset(0.1);
	hEOffXZ->SetStats(0);
	hEOffXZ->GetYaxis()->SetRangeUser(hEOffXZ->GetBinContent(hEOffXZ->GetMinimumBin())/1.01,hEOffXZ->GetBinContent(hEOffXZ->GetMaximumBin())*1.01);
	hEOffXZ->Draw("b");

	cEOff->cd(2);
	hEOffY->SetFillColor(2);
	hEOffY->GetXaxis()->SetTickSize(0.01);
	hEOffY->GetXaxis()->SetNdivisions(N_STRIPS);
	// hEOffY->GetXaxis()->SetTitle("Strip");
	hEOffY->GetXaxis()->CenterTitle();
	hEOffY->GetXaxis()->SetTickSize(0.01);
	hEOffY->GetXaxis()->SetTitleSize(0.06);
	hEOffY->GetXaxis()->SetLabelSize(0.05);
	hEOffY->GetYaxis()->SetTickSize(0.01);
	hEOffY->GetYaxis()->SetTitle("Charge (pC)");
	hEOffY->GetYaxis()->CenterTitle();
	hEOffY->GetYaxis()->SetTickSize(0.01);
	hEOffY->GetYaxis()->SetTitleSize(0.06);
	hEOffY->GetYaxis()->SetLabelSize(0.05);
	hEOffY->SetBarWidth(0.8);
	hEOffY->SetBarOffset(0.1);
	hEOffY->SetStats(0);
	hEOffY->Draw("b");
	hEOffY->Write("EOffY");

	cEOff->cd(4);
	hEOffYZ->SetFillColor(4);
	hEOffYZ->SetTitle("");
	hEOffYZ->GetXaxis()->SetTickSize(0.01);
	hEOffYZ->GetXaxis()->SetNdivisions(N_STRIPS);
	hEOffYZ->GetXaxis()->SetTitle("Strip");
	hEOffYZ->GetXaxis()->CenterTitle();
	hEOffYZ->GetXaxis()->SetTickSize(0.01);
	hEOffYZ->GetXaxis()->SetTitleSize(0.06);
	hEOffYZ->GetXaxis()->SetLabelSize(0.05);
	hEOffYZ->GetYaxis()->SetTickSize(0.01);
	hEOffYZ->GetYaxis()->SetTitle("Charge (pC)");
	hEOffYZ->GetYaxis()->CenterTitle();
	hEOffYZ->GetYaxis()->SetTickSize(0.01);
	hEOffYZ->GetYaxis()->SetTitleSize(0.06);
	hEOffYZ->GetYaxis()->SetLabelSize(0.05);
	hEOffYZ->SetBarWidth(0.8);
	hEOffYZ->SetBarOffset(0.1);
	hEOffYZ->SetStats(0);
	hEOffYZ->GetYaxis()->SetRangeUser(hEOffYZ->GetBinContent(hEOffYZ->GetMinimumBin())/1.01,hEOffYZ->GetBinContent(hEOffYZ->GetMaximumBin())*1.01);
	hEOffYZ->Draw("b");
	cEOff->SaveAs("Picture/Offset_electronique.png");

	hEOffX->Delete();
	hEOffY->Delete();
	hEOffXZ->Delete();
	hEOffYZ->Delete();
	cEOff->Destructor();

	ofstream eoff_file("Offset.txt",std::ios::out);
	for(int i=0;i<N_STRIPS;i++)
		eoff_file<<i<<" "<<offXY[i][0]<<" "<<offXY[i][1]<<endl;
	eoff_file.close();
}

void DerivativeCharge(int count,double h,double vect_charge_in[],double vect_dcharge_out[])
{
	double yl1,yl2,yr1,yr2;
	for(int i=2;i<count-1;i++)
	{
		if(i<2)
		{	
			yl2=0.;
			yl1=0.;
			if(i>=1)
				yl1=vect_charge_in[i-1];
		}
		else
		{
			yl2=vect_charge_in[i-2];
			yl1=vect_charge_in[i-1];
		}
		if(i>count-2)
		{	
			yr2=0.;
			yr1=0.;
			if(i<=count-1)
				yr1=vect_charge_in[i+1];
		}
		else
		{
			yr2=vect_charge_in[i+2];
			yr1=vect_charge_in[i+1];
		}
		vect_dcharge_out[i]=(8.*(yr1-yl1)-yr2+yl2)/(12.*h);
	}
}

void DerivativeSignalArea(char *file,int *tot_area,double signal_time[][2])
{
	faster_file_reader_p reader;
	faster_data_p data;
	electrometer_data electro;
	reader=faster_file_reader_open(file);
	int label;
	int mvt;
	const int asc=0;
	const int des=1;
	int beg_on=0;
	int end_on=0;
	int isLabelX;
	int isLabelY;
	int count_tot=0;
	int count_int=0;
	int count_moy=0;
	int nb_1sec=0;
	int last_i=0;
	int integration=IntegrationStep;
	double t0;
	double fasterTime;
	double first_signal;
	double last_signal;
	double charge;
	double dcharge_moy;
	double chargeTot_pC=0.;
	double charge_X=0.;
	double charge_X_int=0.;
	double charge_Y=0.;
	double charge_Y_int=0.;
	double yl2,yl1,yr1,yr2,h;
	double dyl2,dyl1,dyr1,dyr2;
	double test_signal;
	// double seuilD=50.;
	double seuilD=Value_init[9];
	double seuilS=5.E-4;
	double max_time;

	double* vect_time=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_charge=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_time_int=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_charge_int=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_dcharge_int=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_time_moy=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_dcharge_moy=(double*)malloc(MAX_INTEGR*sizeof(double));

	t0=-1;
	while((data=faster_file_reader_next(reader))!=NULL) 
	{
		label=faster_data_label(data);
		if(label==LabelX||label==LabelY)
		{
			if(t0==-1)
				t0=faster_data_clock_sec(data);
			faster_data_load(data,&electro);
			fasterTime=faster_data_clock_sec(data)-t0;
			for(int j=0;j<N_STRIPS;j++) 
			{
				charge=electrometer_channel_charge_pC(electro,j+1);
				switch(label) 
				{
					case LabelX:
						charge_X+=charge;
						isLabelX=1;
					break;
					case LabelY:
						charge_Y+=charge;
						isLabelY=1;
					break;
				}
			}
			if(isLabelX==1&&isLabelY==1)
			{
				chargeTot_pC+=(charge_X+charge_Y)/2.;
				charge_X_int+=charge_X;
				charge_Y_int+=charge_Y;
				vect_charge[count_tot]=(charge_X+charge_Y)/2.;
				// vect_charge[count_tot]=charge_X;
				vect_time[count_tot]=fasterTime;
				charge_X=0.;
				charge_Y=0.;
				if(count_tot%integration==(integration-1))
				{
					vect_charge_int[count_int]=(charge_X_int+charge_Y_int)/2.;
					vect_time_int[count_int]=fasterTime;
					charge_X_int=0.;
					charge_Y_int=0.;
					count_int++;
				}
				count_tot++;
				isLabelX=0;
				isLabelY=0;
			}
		}
	}
	count_int--;
	count_tot--;
	faster_file_reader_close(reader);

	mvt=asc;
	first_signal=-1.;
	*tot_area=0;
	h=vect_time_int[1]-vect_time_int[0];
	nb_1sec=1+(int)1./h;
	nb_1sec=2;

	DerivativeCharge(count_int,h,vect_charge_int,vect_dcharge_int);

	for(int i=2;i<count_int-1;i++)
	{
		if(i<2)
		{	
			dyl2=0.;
			dyl1=0.;
			if(i>=1)
				dyl1=vect_dcharge_int[i-1];
		}
		else
		{
			dyl2=vect_dcharge_int[i-2];
			dyl1=vect_dcharge_int[i-1];
		}
		if(i>count_int-2)
		{	
			dyr2=0.;
			dyr1=0.;
			if(i<=count_int-1)
				dyr1=vect_dcharge_int[i+1];
		}
		else
		{
			dyr2=vect_dcharge_int[i+2];
			dyr1=vect_dcharge_int[i+1];
		}

		if(i%5==2)
		{
			dcharge_moy=Extremum(dyl1,dyl2,dyr1,dyr2,vect_dcharge_int[i]);
			// +/-3 pour choisir les valeurs en dehors des 5 testées
			if(i>=3)
				yl2=vect_charge_int[i-3];
			else
				yl2=0.;
			if(i<count_int-3)
				yr2=vect_charge_int[i+3];
			else
				yr2=0.;
			test_signal=(yl2-yr2)/TMath::Max(yl2,yr2);

			if(fabs(dcharge_moy)>seuilD&&fabs(test_signal)>seuilS&&i>(last_i+nb_1sec))
			{
				if(dcharge_moy>0.&&test_signal<0.&&mvt==des)	//double montée
				// if(dcharge_moy>0.&&mvt==des)	//double montée
				{
						cout<<"Début de deuxième montée du signal à "<<vect_time_int[i]<<endl;
						signal_time[*tot_area][1]=vect_time_int[i];
						*tot_area=*tot_area+1;
						signal_time[*tot_area][0]=vect_time_int[i];
						last_i=i;
						// i+=nb_1sec;
				}
				if(dcharge_moy>0.&&test_signal<0.)	//montée
				// if(dcharge_moy>0.&&mvt==asc)	//montée
				{
						cout<<"Début du signal à "<<vect_time_int[i]<<endl;
						if(*tot_area==0)
							first_signal=vect_time_int[i];
						if(i>=nb_1sec)
							signal_time[*tot_area][0]=vect_time_int[i-nb_1sec];
						else
							signal_time[*tot_area][0]=vect_time_int[0];
						last_i=i;
						// i+=nb_1sec;
						mvt=des;
				}
				if(dcharge_moy<0.&&test_signal>0.&&mvt==asc)	//double descente
				// if(dcharge_moy>0.&&mvt==des)	//double montée
				{
						cout<<"Deuxième descente du signal à "<<vect_time_int[i]<<endl;
						signal_time[*tot_area][1]=vect_time_int[i];
						*tot_area=*tot_area+1;
						signal_time[*tot_area][0]=vect_time_int[i];
						last_i=i;
						// i+=nb_1sec;
				}
				if(dcharge_moy<0.&&test_signal>0.)	//descente
				// if(dcharge_moy<0.&&mvt==des&&i>(last_i+nb_1sec))	//descente
				{
					cout<<"Fin du signal à "<<vect_time_int[i]<<endl;
					last_signal=vect_time_int[i];
					signal_time[*tot_area][1]=vect_time_int[i];
					if(i<count_int-nb_1sec)
						signal_time[*tot_area][1]=vect_time_int[i+nb_1sec];
					else
						signal_time[*tot_area][1]=vect_time_int[count_int-1];
					mvt=asc;
					last_i=i;
					// i+=nb_1sec;
					*tot_area=*tot_area+1;
					signal_time[*tot_area][0]=signal_time[*tot_area-1][1]; //valeur refuge pour les double descentes
					mvt=asc;
				}
				if(dcharge_moy<0.&&test_signal>0.&&*tot_area==0)	//cela signifie que le fichier débute sur un signal on
				{
					beg_on=1;
					cout<<"Attention, le fichier débute sur du signal On"<<endl;
					first_signal=vect_time_int[0];
					signal_time[*tot_area][0]=vect_time_int[0];
					if(i<count_int-nb_1sec)
						signal_time[*tot_area][1]=vect_time_int[i+nb_1sec];
					else
						signal_time[*tot_area][1]=vect_time_int[count_int-1];
					last_i=i;
					// i+=nb_1sec;
					mvt=des;
				}
			}
			vect_dcharge_moy[count_moy]=dcharge_moy;
			vect_time_moy[count_moy]=vect_time_int[i];
			count_moy++;
		}
	}
	count_moy--;

	if(mvt==des)
	{
		end_on=1;
		cout<<"Attention, le fichier se termine sur du signal On"<<endl;
		signal_time[*tot_area][1]=vect_time_int[count_int-nb_1sec]-1.;
		*tot_area=*tot_area+1;
		last_signal=-1.;
	}

	if(first_signal<2.&&*tot_area>0)
	{
		cout<<"Attention, il semblerait que le fichier débute sur du signal On, premier signal à : "<<first_signal<<endl;
		signal_time[0][0]=vect_time_int[0];
		first_signal=vect_time_int[0];
		beg_on=1;
	}

	TCanvas *cCharge= new TCanvas("Charge over time");
	// cCharge->SetCanvasSize(1000,750);
	// cCharge->Divide(1,3);
	cCharge->SetCanvasSize(2000,2000);
	cCharge->Divide(1,4);

	cCharge->cd(1);
	TGraph *TG_Charge=new TGraph(count_tot,vect_time,vect_charge);
	TG_Charge->SetMarkerColor(2);
	TG_Charge->SetLineColor(2);
	TG_Charge->SetLineWidth(1.5);
	TG_Charge->SetTitle("Charge total over time");
	TG_Charge->GetXaxis()->SetTitle("Time (s)");
	TG_Charge->GetXaxis()->SetTickSize(0.01);
	TG_Charge->GetXaxis()->SetTitleSize(0.06);
	TG_Charge->GetXaxis()->SetLabelSize(0.05);
	TG_Charge->GetYaxis()->SetTitle("Charge (pC)");
	TG_Charge->GetYaxis()->SetTickSize(0.01);
	TG_Charge->GetYaxis()->SetTitleSize(0.06);
	TG_Charge->GetYaxis()->CenterTitle();
	TG_Charge->GetYaxis()->SetLabelSize(0.05);
	TG_Charge->Write("Charge");
	TG_Charge->Draw("AL");

	cCharge->cd(2);
	TGraph *TG_Charge_int=new TGraph(count_int,vect_time_int,vect_charge_int);
	TG_Charge_int->SetMarkerColor(2);
	TG_Charge_int->SetLineColor(2);
	TG_Charge_int->SetLineWidth(1.5);
	TG_Charge_int->SetTitle("Charge over time");
	TG_Charge_int->GetXaxis()->SetTitle("Time (s)");
	TG_Charge_int->GetXaxis()->SetTickSize(0.01);
	TG_Charge_int->GetXaxis()->SetTitleSize(0.06);
	TG_Charge_int->GetXaxis()->SetLabelSize(0.05);
	TG_Charge_int->GetYaxis()->SetTitle("Charge (pC)");
	TG_Charge_int->GetYaxis()->SetTickSize(0.01);
	TG_Charge_int->GetYaxis()->SetTitleSize(0.06);
	TG_Charge_int->GetYaxis()->CenterTitle();
	TG_Charge_int->GetYaxis()->SetLabelSize(0.05);
	TG_Charge_int->Write("Charge int");
	TG_Charge_int->Draw("AL");

	cCharge->cd(3);
	TGraph *TG_dCharge=new TGraph(count_int,vect_time_int,vect_dcharge_int);
	TG_dCharge->SetMarkerColor(4);
	TG_dCharge->SetLineColor(4);
	TG_dCharge->SetLineWidth(1.5);
	TG_dCharge->SetTitle("Charge derivative over time");
	TG_dCharge->GetXaxis()->SetTitle("Time (s)");
	TG_dCharge->GetXaxis()->SetTickSize(0.01);
	TG_dCharge->GetXaxis()->SetTitleSize(0.06);
	TG_dCharge->GetXaxis()->SetLabelSize(0.05);
	TG_dCharge->GetYaxis()->SetTitle("Derivative");
	TG_dCharge->GetYaxis()->SetTickSize(0.01);
	TG_dCharge->GetYaxis()->SetTitleSize(0.06);
	TG_dCharge->GetYaxis()->CenterTitle();
	TG_dCharge->GetYaxis()->SetLabelSize(0.05);
	TG_dCharge->Write("DCharge");
	TG_dCharge->Draw("AL");

	cCharge->cd(4);
	TGraph *TG_dMCharge=new TGraph(count_moy,vect_time_moy,vect_dcharge_moy);
	TG_dMCharge->SetMarkerColor(4);
	TG_dMCharge->SetLineColor(4);
	TG_dMCharge->SetLineWidth(1.5);
	TG_dMCharge->SetTitle("Mean charge derivative over time");
	TG_dMCharge->GetXaxis()->SetTitle("Time (s)");
	TG_dMCharge->GetXaxis()->SetTickSize(0.01);
	TG_dMCharge->GetXaxis()->SetTitleSize(0.06);
	TG_dMCharge->GetXaxis()->SetLabelSize(0.05);
	TG_dMCharge->GetYaxis()->SetTitle("Mean derivative");
	TG_dMCharge->GetYaxis()->SetTickSize(0.01);
	TG_dMCharge->GetYaxis()->SetTitleSize(0.06);
	TG_dMCharge->GetYaxis()->CenterTitle();
	TG_dMCharge->GetYaxis()->SetLabelSize(0.05);
	TG_dMCharge->Write("DMCharge");
	TG_dMCharge->Draw("AL");
	max_time=vect_time_int[count_int-1];
	TLine *line= new TLine();
	line->SetLineColor(2);
	line->DrawLine(0.,seuilD,max_time,seuilD);
	line->DrawLine(0.,-seuilD,max_time,-seuilD);

	cCharge->SaveAs("Picture/Charge.png");

	ElectronicOffsetExtraction(file,first_signal,last_signal,vect_time[0],vect_time[count_tot],beg_on,end_on);

	TG_Charge->Delete();
	TG_Charge_int->Delete();
	TG_dCharge->Delete();
	TG_dMCharge->Delete();
	cCharge->Destructor();
	free(vect_time);
	free(vect_charge);
	free(vect_time_int);
	free(vect_charge_int);
	free(vect_dcharge_int);
}

void ChargeSignalArea(char *file,int *tot_area,double signal_time[][2])
{
	faster_file_reader_p reader;
	faster_data_p data;
	electrometer_data electro;
	reader=faster_file_reader_open(file);
	int label;
	int mvt;
	const int asc=0;
	const int des=1;
	int beg_on=0;
	int end_on=0;
	int isLabelX;
	int isLabelY;
	int count_tot=0;
	int count_int=0;
	int nb_1sec=0;
	int integration=IntegrationStep;
	double t0;
	double fasterTime;
	double first_signal;
	double last_signal;
	double charge;
	double chargeTot_pC=0.;
	double charge_X=0.;
	double charge_X_int=0.;
	double charge_Y=0.;
	double charge_Y_int=0.;
	double yl2,yl1,yr1,yr2,h;
	// double seuilC=40200.;
	double seuilC=Value_init[9];
	double max_time;

	double* vect_time=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_charge=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_time_int=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_charge_int=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_dcharge_int=(double*)malloc(MAX_INTEGR*sizeof(double));

	t0=-1;
	while((data=faster_file_reader_next(reader))!=NULL) 
	{
		label=faster_data_label(data);
		if(label==LabelX||label==LabelY)
		{
			if(t0==-1)
				t0=faster_data_clock_sec(data);
			faster_data_load(data,&electro);
			fasterTime=faster_data_clock_sec(data)-t0;
			for(int j=0;j<N_STRIPS;j++) 
			{
				charge=electrometer_channel_charge_pC(electro,j+1);
				switch(label) 
				{
					case LabelX:
						charge_X+=charge;
						isLabelX=1;
					break;
					case LabelY:
						charge_Y+=charge;
						isLabelY=1;
					break;
				}
			}
			if(isLabelX==1&&isLabelY==1)
			{
				chargeTot_pC+=(charge_X+charge_Y)/2.;
				charge_X_int+=charge_X;
				charge_Y_int+=charge_Y;
				vect_charge[count_tot]=(charge_X+charge_Y)/2.;
				// vect_charge[count_tot]=charge_X;
				vect_time[count_tot]=fasterTime;
				charge_X=0.;
				charge_Y=0.;
				if(count_tot%integration==(integration-1))
				{
					vect_charge_int[count_int]=(charge_X_int+charge_Y_int)/2.;
					vect_time_int[count_int]=fasterTime;
					charge_X_int=0.;
					charge_Y_int=0.;
					count_int++;
				}
				count_tot++;
				isLabelX=0;
				isLabelY=0;
			}
		}
	}
	count_int--;
	count_tot--;
	faster_file_reader_close(reader);

	mvt=asc;
	first_signal=-1.;
	*tot_area=0;
	h=vect_time_int[1]-vect_time_int[0];
	nb_1sec=1+(int)1./h;
	nb_1sec=2;
	
	DerivativeCharge(count_int,h,vect_charge_int,vect_dcharge_int);
	
	for(int i=2;i<count_int-1;i++)
	{

		if(vect_charge_int[i]>seuilC&&mvt==asc)
		{
			cout<<"Début du signal à "<<vect_time_int[i]<<endl;
			if(*tot_area==0)
				first_signal=vect_time_int[i];
			if(i>=nb_1sec)
				signal_time[*tot_area][0]=vect_time_int[i-nb_1sec];
			else
				signal_time[*tot_area][0]=vect_time_int[0];
			// i+=nb_1sec;
			mvt=des;
		}
		if(vect_charge_int[i]<seuilC&&mvt==des)
		{
			cout<<"Fin du signal à "<<vect_time_int[i]<<endl;
			last_signal=vect_time_int[i];
			signal_time[*tot_area][1]=vect_time_int[i];
			if(i<count_int-nb_1sec+1)
				signal_time[*tot_area][1]=vect_time_int[i+nb_1sec-1];
			else
				signal_time[*tot_area][1]=vect_time_int[count_int-1];
			mvt=asc;
			// i+=nb_1sec;
			*tot_area=*tot_area+1;
			signal_time[*tot_area][0]=signal_time[*tot_area-1][1];
			mvt=asc;
		}
	}

	if(mvt==des)
	{
		cout<<"Attention, le fichier se termine sur du signal On"<<endl;
		signal_time[*tot_area][1]=vect_time_int[count_int-nb_1sec]-1.;
		*tot_area=*tot_area+1;
		last_signal=-1.;
		end_on=1;
	}

	if(first_signal<2.&&*tot_area>0)
	{
		cout<<"Attention, il semblerait que le fichier débute sur du signal On, premier signal à : "<<first_signal<<endl;
		signal_time[0][0]=vect_time_int[0];
		first_signal=vect_time_int[0];
		beg_on=1;
	}

	TCanvas *cCharge= new TCanvas("Charge over time");
	cCharge->SetCanvasSize(2000,1500);
	cCharge->Divide(1,3);

	cCharge->cd(1);
	TGraph *TG_Charge=new TGraph(count_tot,vect_time,vect_charge);
	TG_Charge->SetMarkerColor(2);
	TG_Charge->SetLineColor(2);
	TG_Charge->SetLineWidth(1.5);
	TG_Charge->SetTitle("Charge total over time");
	TG_Charge->GetXaxis()->SetTitle("Time (s)");
	TG_Charge->GetXaxis()->SetTickSize(0.01);
	TG_Charge->GetXaxis()->SetTitleSize(0.06);
	TG_Charge->GetXaxis()->SetLabelSize(0.05);
	TG_Charge->GetYaxis()->SetTitle("Charge (pC)");
	TG_Charge->GetYaxis()->SetTickSize(0.01);
	TG_Charge->GetYaxis()->SetTitleSize(0.06);
	TG_Charge->GetYaxis()->CenterTitle();
	TG_Charge->GetYaxis()->SetLabelSize(0.05);
	TG_Charge->Write("Charge");
	TG_Charge->Draw("AL");

	cCharge->cd(2);
	TGraph *TG_Charge_int=new TGraph(count_int,vect_time_int,vect_charge_int);
	TG_Charge_int->SetMarkerColor(2);
	TG_Charge_int->SetLineColor(2);
	TG_Charge_int->SetLineWidth(1.5);
	TG_Charge_int->SetTitle("Charge over time");
	TG_Charge_int->GetXaxis()->SetTitle("Time (s)");
	TG_Charge_int->GetXaxis()->SetTickSize(0.01);
	TG_Charge_int->GetXaxis()->SetTitleSize(0.06);
	TG_Charge_int->GetXaxis()->SetLabelSize(0.05);
	TG_Charge_int->GetYaxis()->SetTitle("Charge (pC)");
	TG_Charge_int->GetYaxis()->SetTickSize(0.01);
	TG_Charge_int->GetYaxis()->SetTitleSize(0.06);
	TG_Charge_int->GetYaxis()->CenterTitle();
	TG_Charge_int->GetYaxis()->SetLabelSize(0.05);
	TG_Charge_int->Write("Charge int");
	TG_Charge_int->Draw("AL");
	max_time=vect_time_int[count_int-1];
	TLine *line= new TLine();
	line->SetLineColor(4);
	line->DrawLine(0,seuilC,max_time,seuilC);

	cCharge->cd(3);
	TGraph *TG_dCharge=new TGraph(count_int,vect_time_int,vect_dcharge_int);
	TG_dCharge->SetMarkerColor(4);
	TG_dCharge->SetLineColor(4);
	TG_dCharge->SetLineWidth(1.5);
	TG_dCharge->SetTitle("Charge derivative over time");
	TG_dCharge->GetXaxis()->SetTitle("Time (s)");
	TG_dCharge->GetXaxis()->SetTickSize(0.01);
	TG_dCharge->GetXaxis()->SetTitleSize(0.06);
	TG_dCharge->GetXaxis()->SetLabelSize(0.05);
	TG_dCharge->GetYaxis()->SetTitle("Derivative");
	TG_dCharge->GetYaxis()->SetTickSize(0.01);
	TG_dCharge->GetYaxis()->SetTitleSize(0.06);
	TG_dCharge->GetYaxis()->CenterTitle();
	TG_dCharge->GetYaxis()->SetLabelSize(0.05);
	TG_dCharge->Write("DCharge");
	TG_dCharge->Draw("AL");

	cCharge->SaveAs("Picture/Charge.png");

	ElectronicOffsetExtraction(file,first_signal,last_signal,vect_time[0],vect_time[count_tot],beg_on,end_on);

	TG_Charge->Delete();
	TG_Charge_int->Delete();
	TG_dCharge->Delete();
	cCharge->Destructor();
	free(vect_time);
	free(vect_charge);
	free(vect_time_int);
	free(vect_charge_int);
	free(vect_dcharge_int);
}

void QuantaSignalArea(char *file,int *tot_area,double signal_time[][2])
{
	faster_file_reader_p reader;
	faster_data_p data;
	electrometer_data electro;
	scaler_counter scaler_cnt;
	reader=faster_file_reader_open(file);
	int label;
	int mvt;
	const int asc=0;
	const int des=1;
	int beg_on=0;
	int end_on=0;
	int isLabelX;
	int isLabelY;
	int count_tot=0;
	int count_int=0;
	int count_quanta=0;
	int nb_1sec=0;
	int integration=IntegrationStep;
	long int last_quanta=-1;
	long int dquanta=-1;
	double t0;
	double fasterTime;
	double first_signal;
	double last_signal;
	double charge;
	double h;
	double chargeTot_pC=0.;
	double charge_X=0.;
	double charge_X_int=0.;
	double charge_Y=0.;
	double charge_Y_int=0.;
	double seuilQ=Value_init[9];

	double* vect_time=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_time_q=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_charge=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_dquanta=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_time_int=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_charge_int=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_dcharge_int=(double*)malloc(MAX_INTEGR*sizeof(double));

	t0=-1;
	while((data=faster_file_reader_next(reader))!=NULL) 
	{
		label=faster_data_label(data);
		if(label==LabelCount)
		{
			faster_data_load(data,&scaler_cnt);
			if(t0==-1)
				t0=faster_data_clock_sec(data);
			fasterTime=faster_data_clock_sec(data)-t0;
			vect_time_q[count_quanta]=fasterTime;
			if(last_quanta==-1)
				last_quanta=scaler_cnt.quanta;
			dquanta=scaler_cnt.quanta-last_quanta;
			last_quanta=scaler_cnt.quanta;
			vect_dquanta[count_quanta]=dquanta;
			count_quanta++;
		}

		if(label==LabelX||label==LabelY)
		{
			if(t0==-1)
				t0=faster_data_clock_sec(data);
			faster_data_load(data,&electro);
			fasterTime=faster_data_clock_sec(data)-t0;
			for(int j=0;j<N_STRIPS;j++) 
			{
				charge=electrometer_channel_charge_pC(electro,j+1);
				switch(label) 
				{
					case LabelX:
						charge_X+=charge;
						isLabelX=1;
					break;
					case LabelY:
						charge_Y+=charge;
						isLabelY=1;
					break;
				}
			}
			if(isLabelX==1&&isLabelY==1)
			{
				chargeTot_pC+=(charge_X+charge_Y)/2.;
				charge_X_int+=charge_X;
				charge_Y_int+=charge_Y;
				vect_charge[count_tot]=(charge_X+charge_Y)/2.;
				// vect_charge[count_tot]=charge_X;
				vect_time[count_tot]=fasterTime;
				charge_X=0.;
				charge_Y=0.;
				if(count_tot%integration==(integration-1))
				{
					vect_charge_int[count_int]=(charge_X_int+charge_Y_int)/2.;
					vect_time_int[count_int]=fasterTime;
					charge_X_int=0.;
					charge_Y_int=0.;
					count_int++;
				}
				count_tot++;
				isLabelX=0;
				isLabelY=0;
			}
		}
	}
	if(vect_time_q[0]<0.)
		vect_time_q[0]=0.;
	count_quanta--;
	count_int--;
	count_tot--;
	faster_file_reader_close(reader);

	h=vect_time_int[1]-vect_time_int[0];
	DerivativeCharge(count_int,h,vect_charge_int,vect_dcharge_int);

	if(count_quanta<1)
	{
		cout<<"Pas de données de quanta"<<endl;
		*tot_area=0;
		ElectronicOffsetExtraction(file,first_signal,last_signal,vect_time[0],vect_time[count_tot],beg_on,end_on);
		return;
	}

	mvt=asc;
	first_signal=-1.;
	*tot_area=0;
	nb_1sec=2;
	for(int i=0;i<count_quanta;i++)
	{
		if(vect_dquanta[i]>seuilQ&&mvt==asc)
		{
			cout<<"Début du signal à "<<vect_time_q[i]<<endl;
			if(*tot_area==0)
				first_signal=vect_time_q[i];
			if(i>=nb_1sec)
				signal_time[*tot_area][0]=vect_time_q[i-nb_1sec];
			else
				signal_time[*tot_area][0]=vect_time_q[0];
			// i+=nb_1sec;
			mvt=des;
		}
		if(vect_dquanta[i]<seuilQ&&mvt==des)
		{
			cout<<"Fin du signal à "<<vect_time_q[i]<<endl;
			last_signal=vect_time_q[i];
			signal_time[*tot_area][1]=vect_time_q[i];
			if(i<count_quanta-nb_1sec+1)
				signal_time[*tot_area][1]=vect_time_q[i+nb_1sec-1];
			else
				signal_time[*tot_area][1]=vect_time_q[count_quanta-1];
			mvt=asc;
			// i+=nb_1sec;
			*tot_area=*tot_area+1;
			signal_time[*tot_area][0]=signal_time[*tot_area-1][1];
			mvt=asc;
		}
	}

	if(mvt==des)
	{
		cout<<"Attention, le fichier se termine sur du signal On"<<endl;
		signal_time[*tot_area][1]=vect_time_q[count_quanta-nb_1sec]-1.;
		*tot_area=*tot_area+1;
		last_signal=-1.;
		end_on=1;
	}

	if(first_signal<2.&&*tot_area>0)
	{
		cout<<"Attention, il semblerait que le fichier débute sur du signal On, premier signal à : "<<first_signal<<endl;
		signal_time[0][0]=vect_time_int[0];
		first_signal=vect_time_int[0];
		beg_on=1;
	}

	TCanvas *cCharge= new TCanvas("Charge over time");
	cCharge->SetCanvasSize(2000,2000);
	cCharge->Divide(1,4);

	cCharge->cd(1);
	TGraph *TG_Charge=new TGraph(count_tot,vect_time,vect_charge);
	TG_Charge->SetMarkerColor(2);
	TG_Charge->SetLineColor(2);
	TG_Charge->SetLineWidth(1.5);
	TG_Charge->SetTitle("Charge total over time");
	TG_Charge->GetXaxis()->SetTitle("Time (s)");
	TG_Charge->GetXaxis()->SetTickSize(0.01);
	TG_Charge->GetXaxis()->SetTitleSize(0.06);
	TG_Charge->GetXaxis()->SetLabelSize(0.05);
	TG_Charge->GetYaxis()->SetTitle("Charge (pC)");
	TG_Charge->GetYaxis()->SetTickSize(0.01);
	TG_Charge->GetYaxis()->SetTitleSize(0.06);
	TG_Charge->GetYaxis()->CenterTitle();
	TG_Charge->GetYaxis()->SetLabelSize(0.05);
	TG_Charge->Write("Charge");
	TG_Charge->Draw("AL");

	cCharge->cd(2);
	TGraph *TG_Charge_int=new TGraph(count_int,vect_time_int,vect_charge_int);
	TG_Charge_int->SetMarkerColor(2);
	TG_Charge_int->SetLineColor(2);
	TG_Charge_int->SetLineWidth(1.5);
	TG_Charge_int->SetTitle("Charge over time");
	TG_Charge_int->GetXaxis()->SetTitle("Time (s)");
	TG_Charge_int->GetXaxis()->SetTickSize(0.01);
	TG_Charge_int->GetXaxis()->SetTitleSize(0.06);
	TG_Charge_int->GetXaxis()->SetLabelSize(0.05);
	TG_Charge_int->GetYaxis()->SetTitle("Charge (pC)");
	TG_Charge_int->GetYaxis()->SetTickSize(0.01);
	TG_Charge_int->GetYaxis()->SetTitleSize(0.06);
	TG_Charge_int->GetYaxis()->CenterTitle();
	TG_Charge_int->GetYaxis()->SetLabelSize(0.05);
	TG_Charge_int->Write("Charge int");
	TG_Charge_int->Draw("AL");

	cCharge->cd(3);
	TGraph *TG_dCharge=new TGraph(count_int,vect_time_int,vect_dcharge_int);
	TG_dCharge->SetMarkerColor(4);
	TG_dCharge->SetLineColor(4);
	TG_dCharge->SetLineWidth(1.5);
	TG_dCharge->SetTitle("Charge derivative over time");
	TG_dCharge->GetXaxis()->SetTitle("Time (s)");
	TG_dCharge->GetXaxis()->SetTickSize(0.01);
	TG_dCharge->GetXaxis()->SetTitleSize(0.06);
	TG_dCharge->GetXaxis()->SetLabelSize(0.05);
	TG_dCharge->GetYaxis()->SetTitle("Derivative");
	TG_dCharge->GetYaxis()->SetTickSize(0.01);
	TG_dCharge->GetYaxis()->SetTitleSize(0.06);
	TG_dCharge->GetYaxis()->CenterTitle();
	TG_dCharge->GetYaxis()->SetLabelSize(0.05);
	TG_dCharge->Write("DCharge");
	TG_dCharge->Draw("AL");

	cCharge->cd(4);
	TGraph *TG_dQuanta=new TGraph(count_quanta,vect_time_q,vect_dquanta);
	TG_dQuanta->SetMarkerColor(4);
	TG_dQuanta->SetLineColor(4);
	TG_dQuanta->SetLineWidth(1.5);
	TG_dQuanta->SetTitle("Differential quanta over time");
	TG_dQuanta->GetHistogram()->SetTitleSize(0.035);
	TG_dQuanta->GetXaxis()->SetTitle("Time (s)");
	TG_dQuanta->GetXaxis()->SetTickSize(0.01);
	TG_dQuanta->GetXaxis()->SetTitleSize(0.06);
	TG_dQuanta->GetXaxis()->SetLabelSize(0.05);
	TG_dQuanta->GetYaxis()->SetTitle("dQuanta");
	TG_dQuanta->GetYaxis()->SetTickSize(0.01);
	TG_dQuanta->GetYaxis()->SetTitleSize(0.06);
	TG_dQuanta->GetYaxis()->CenterTitle();
	TG_dQuanta->GetYaxis()->SetLabelSize(0.05);
	TG_dQuanta->Write("dQuanta");
	TG_dQuanta->Draw("AL");
	TLine *line= new TLine();
	line->SetLineColor(2);
	line->DrawLine(vect_time_q[0],seuilQ,vect_time_q[count_quanta-1],seuilQ);

	cCharge->SaveAs("Picture/Charge.png");

	ElectronicOffsetExtraction(file,first_signal,last_signal,vect_time[0],vect_time[count_tot],beg_on,end_on);

	TG_Charge->Delete();
	TG_Charge_int->Delete();
	TG_dCharge->Delete();
	TG_dQuanta->Delete();
	cCharge->Destructor();
	free(vect_time);
	free(vect_time_q);
	free(vect_charge);
	free(vect_time_int);
	free(vect_charge_int);
	free(vect_dcharge_int);
	free(vect_dquanta);
}

void ManualSignalArea(char *file,int *tot_area,double signal_time[][2])
{
	faster_file_reader_p reader;
	faster_data_p data;
	electrometer_data electro;
	reader=faster_file_reader_open(file);
	int label;
	int mvt;
	int beg_on=0;
	int end_on=0;
	int isLabelX;
	int isLabelY;
	int count_tot=0;
	int count_int=0;
	int integration=IntegrationStep;
	double t0;
	double fasterTime;
	double first_signal;
	double last_signal;
	double charge;
	double chargeTot_pC=0.;
	double charge_X=0.;
	double charge_X_int=0.;
	double charge_Y=0.;
	double charge_Y_int=0.;

	double* vect_time=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_charge=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_time_int=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_charge_int=(double*)malloc(MAX_INTEGR*sizeof(double));

	t0=-1;
	while((data=faster_file_reader_next(reader))!=NULL) 
	{
		label=faster_data_label(data);
		if(label==LabelX||label==LabelY)
		{
			if(t0==-1)
				t0=faster_data_clock_sec(data);
			faster_data_load(data,&electro);
			fasterTime=faster_data_clock_sec(data)-t0;
			for(int j=0;j<N_STRIPS;j++) 
			{
				charge=electrometer_channel_charge_pC(electro,j+1);
				switch(label) 
				{
					case LabelX:
						charge_X+=charge;
						isLabelX=1;
					break;
					case LabelY:
						charge_Y+=charge;
						isLabelY=1;
					break;
				}
			}
			if(isLabelX==1&&isLabelY==1)
			{
				chargeTot_pC+=(charge_X+charge_Y)/2.;
				charge_X_int+=charge_X;
				charge_Y_int+=charge_Y;
				vect_charge[count_tot]=(charge_X+charge_Y)/2.;
				// vect_charge[count_tot]=charge_X;
				vect_time[count_tot]=fasterTime;
				charge_X=0.;
				charge_Y=0.;
				if(count_tot%integration==(integration-1))
				{
					vect_charge_int[count_int]=(charge_X_int+charge_Y_int)/2.;
					vect_time_int[count_int]=fasterTime;
					charge_X_int=0.;
					charge_Y_int=0.;
					count_int++;
				}
				count_tot++;
				isLabelX=0;
				isLabelY=0;
			}
		}
	}
	count_int--;
	count_tot--;
	faster_file_reader_close(reader);

	if(bound_min<vect_time[0])
	{
		cout<<"Mauvaise borne de début du signal : "<<bound_min<<" < "<<vect_time[0]<<endl;
		cout<<"Redefinition du début du signal"<<endl;
		bound_min=vect_time[0];
		beg_on=1;
	}

	if(bound_max>vect_time[count_tot])
	{
		cout<<"Mauvaise borne de fin du signal : "<<bound_max<<" > "<<vect_time[count_tot]<<endl;
		cout<<"Redefinition de la fin du signal"<<endl;
		bound_max=vect_time[count_tot];
		end_on=1;
	}

	TCanvas *cCharge= new TCanvas("Charge over time");
	cCharge->SetCanvasSize(2000,1000);
	cCharge->Divide(1,2);

	cCharge->cd(1);
	TGraph *TG_Charge=new TGraph(count_tot,vect_time,vect_charge);
	TG_Charge->SetMarkerColor(2);
	TG_Charge->SetLineColor(2);
	TG_Charge->SetLineWidth(1.5);
	TG_Charge->SetTitle("Charge total over time");
	TG_Charge->GetXaxis()->SetTitle("Time (s)");
	TG_Charge->GetXaxis()->SetTickSize(0.01);
	TG_Charge->GetXaxis()->SetTitleSize(0.06);
	TG_Charge->GetXaxis()->SetLabelSize(0.05);
	TG_Charge->GetYaxis()->SetTitle("Charge (pC)");
	TG_Charge->GetYaxis()->SetTickSize(0.01);
	TG_Charge->GetYaxis()->SetTitleSize(0.06);
	TG_Charge->GetYaxis()->CenterTitle();
	TG_Charge->GetYaxis()->SetLabelSize(0.05);
	TG_Charge->Write("Charge");
	TG_Charge->Draw("AL");

	cCharge->cd(2);
	TGraph *TG_Charge_int=new TGraph(count_int,vect_time_int,vect_charge_int);
	TG_Charge_int->SetMarkerColor(2);
	TG_Charge_int->SetLineColor(2);
	TG_Charge_int->SetLineWidth(1.5);
	TG_Charge_int->SetTitle("Charge over time");
	TG_Charge_int->GetXaxis()->SetTitle("Time (s)");
	TG_Charge_int->GetXaxis()->SetTickSize(0.01);
	TG_Charge_int->GetXaxis()->SetTitleSize(0.06);
	TG_Charge_int->GetXaxis()->SetLabelSize(0.05);
	TG_Charge_int->GetYaxis()->SetTitle("Charge (pC)");
	TG_Charge_int->GetYaxis()->SetTickSize(0.01);
	TG_Charge_int->GetYaxis()->SetTitleSize(0.06);
	TG_Charge_int->GetYaxis()->CenterTitle();
	TG_Charge_int->GetYaxis()->SetLabelSize(0.05);
	TG_Charge_int->Write("Charge int");
	TG_Charge_int->Draw("AL");

	cCharge->SaveAs("Picture/Charge.png");

	first_signal=bound_min;
	last_signal=bound_max;
	*tot_area=1;
	signal_time[0][0]=bound_min;
	signal_time[0][1]=bound_max;
	ElectronicOffsetExtraction(file,first_signal,last_signal,vect_time[0],vect_time[count_tot],beg_on,end_on);

	TG_Charge->Delete();
	TG_Charge_int->Delete();
	cCharge->Destructor();
	free(vect_time);
	free(vect_charge);
	free(vect_time_int);
	free(vect_charge_int);
}

void SubFittingBackground(int SFBdraw,int binl,int binr,double min,double max,double time,double *sum_val)
{
	// Gros bordel entre les bin qui partent de 1 et les vecteurs de 0
	// plus le fait que les pistes doivent être représentées en milieu de bin 
	stringstream ss;
	ss<<(int)time;
	string indice=ss.str();
	string name="Picture/SFB_"+indice+".png";

	int i_par=9;
	double scale_value;
	double bdf_SFB;
	double par[i_par];
	double y_min;
	double y_max;

	TH1F* Profile=new TH1F("Profile","Profile in charge",N_STRIPS,.5,32.5);
	TH1F* ProfExc=new TH1F("ProfExc","Profile in charge without excluded points",N_STRIPS,1,33);
	TGraph *TG_Prof_Exc=new TGraph();
	TGraph *TG_Post=new TGraph();
	TGraph *TG_Visu=new TGraph();
	TF1* SinProfile=new TF1("SinProfile","TMath::Sin(x)",2,31);
	TF1* PolNProfile=new TF1("PolNProfile","pol8",2,31);

	for(int i=FIRST_ELEC;i<LAST_ELEC;i++)
	{
		scale_value=PreSFB[i];
		Profile->SetBinContent(i+1,PreSFB[i]);
		if(i+1<=binl||i+1>=binr)
		{
			ProfExc->SetBinContent(i+1,scale_value);
			TG_Prof_Exc->SetPoint(i,i+1,scale_value);
		}
	}
	ProfExc->Fit(PolNProfile,"Q");

	for(int ii=0;ii<i_par;ii++)
		par[ii]=PolNProfile->GetParameter(ii);
	
	*sum_val=0.;
	for(int i=FIRST_ELEC;i<LAST_ELEC;i++)
	{
		double x=i+1.5;
		bdf_SFB=0.;
		for(int ii=0;ii<i_par;ii++)
			bdf_SFB+=par[ii]*pow(x,ii);
		PostSFB[i]=PreSFB[i]-bdf_SFB;
		if(i+1>binl&&i+1<binr)
			*sum_val+=PostSFB[i];
		if(SFBdraw==1)
		{
			TG_Post->SetPoint(i,x-.5,PostSFB[i]);
			TG_Visu->SetPoint(i,x-.5,bdf_SFB);
		}
	}
	if(SFBdraw==1)
	{
		double x=1.5;
		bdf_SFB=par[0]+par[1]*x+par[2]*pow(x,2)+par[3]*pow(x,3)+par[4]*pow(x,4)+par[5]*pow(x,5)+par[6]*pow(x,6)+par[7]*pow(x,7)+par[8]*pow(x,8);
		TG_Visu->SetPoint(0,x-.5,bdf_SFB);
		x=LAST_ELEC+1.5;
		bdf_SFB=par[0]+par[1]*x+par[2]*pow(x,2)+par[3]*pow(x,3)+par[4]*pow(x,4)+par[5]*pow(x,5)+par[6]*pow(x,6)+par[7]*pow(x,7)+par[8]*pow(x,8);
		TG_Visu->SetPoint(LAST_ELEC,x-.5,bdf_SFB);
	}

	if(SFBdraw==1)
	{
		y_min=Profile->GetMinimum()*1.1;
		y_max=Profile->GetMaximum()*1.1;

		TCanvas *cSFB = new TCanvas("Sub fitting background");
		cSFB->SetCanvasSize(2000,1000);
		cSFB->Divide(1,2);

		cSFB->cd(1);
		TG_Prof_Exc->SetLineColor(2);
		TG_Prof_Exc->SetMarkerColor(2);
		TG_Prof_Exc->SetMarkerStyle(4);
		TG_Prof_Exc->SetMarkerSize(1.2);
		TG_Prof_Exc->GetXaxis()->SetTickSize(0.01);
		TG_Prof_Exc->GetXaxis()->SetNdivisions(N_STRIPS);
		TG_Prof_Exc->GetXaxis()->SetTitle("Strip");
		TG_Prof_Exc->GetXaxis()->CenterTitle();
		TG_Prof_Exc->GetXaxis()->SetTickSize(0.01);
		TG_Prof_Exc->GetXaxis()->SetTitleSize(0.06);
		TG_Prof_Exc->GetXaxis()->SetLabelSize(0.05);
		TG_Prof_Exc->GetYaxis()->SetTickSize(0.01);
		TG_Prof_Exc->GetYaxis()->SetTitle("Charge (pC)");
		TG_Prof_Exc->GetYaxis()->CenterTitle();
		TG_Prof_Exc->GetYaxis()->SetTickSize(0.01);
		TG_Prof_Exc->GetYaxis()->SetTitleSize(0.06);
		TG_Prof_Exc->GetYaxis()->SetLabelSize(0.05);
		TG_Prof_Exc->GetXaxis()->SetRangeUser(1,N_STRIPS);
		TG_Prof_Exc->GetYaxis()->SetRangeUser(y_min,y_max);
		TG_Prof_Exc->Draw("AP");
		Profile->SetLineColor(2);
		Profile->SetLineWidth(2);
		Profile->SetStats(0);
		Profile->Draw("same");
		// Profile->Draw();
		// TG_Prof_Exc->Write("TG_Prof_Exc");

		PolNProfile->SetLineColor(4);
		// PolNProfile->Draw("same L");
		TG_Visu->SetLineColor(4);
		TG_Visu->SetLineWidth(2);
		TG_Visu->Draw("same L");

		cSFB->cd(2);
		gStyle->SetBarWidth(0.9);
		gStyle->SetBarOffset(0.1);
		TG_Post->SetLineColor(2);
		TG_Post->SetFillColor(2);
		TG_Post->SetLineWidth(1);
		TG_Post->SetMarkerColor(2);
		TG_Post->SetMarkerStyle(4);
		TG_Post->SetMarkerSize(1.2);
		TG_Post->GetXaxis()->SetTickSize(0.01);
		TG_Post->GetXaxis()->SetNdivisions(N_STRIPS);
		TG_Post->GetXaxis()->SetTitle("Strip");
		TG_Post->GetXaxis()->CenterTitle();
		TG_Post->GetXaxis()->SetTickSize(0.01);
		TG_Post->GetXaxis()->SetTitleSize(0.06);
		TG_Post->GetXaxis()->SetLabelSize(0.05);
		TG_Post->GetYaxis()->SetTickSize(0.01);
		TG_Post->GetYaxis()->SetTitle("Charge (pC)");
		TG_Post->GetYaxis()->CenterTitle();
		TG_Post->GetYaxis()->SetTickSize(0.01);
		TG_Post->GetYaxis()->SetTitleSize(0.06);
		TG_Post->GetYaxis()->SetLabelSize(0.05);
		TG_Post->GetXaxis()->SetRangeUser(1,N_STRIPS);
		// TG_Post->GetYaxis()->SetRangeUser(y_min,y_max);
		TG_Post->Draw("AB");
		TG_Post->Write("TG_Post");

		// cSFB->SaveAs("Picture/SFB.png");
		cSFB->SaveAs(name.c_str());
		cSFB->Destructor();
	}

	Profile->Delete();
	ProfExc->Delete();
	SinProfile->Delete();
	PolNProfile->Delete();
	TG_Prof_Exc->Delete();
	TG_Post->Delete();
	TG_Visu->Delete();
}

void DoseDistribution(int nb_area,double Fluence[N_STRIPS][N_STRIPS])
{
	int count_dose=0;
	double dose_max;
	double mean_dose=0.;
	double rms_dose=0.;
	double calib=Calib_value(0,0,energy)*pow(strip_width,2)/1000.;
	double TEL=TEL_value(1,energy);
	double pC_to_Gy=TEL/calib*q*1.E10;

	// ***********************************************
	// TEL=keV.um-1=1000.eV.1E-6.m-1=1E9.eV.m-1
	// Calib_value=fC.part-1(integrale)=fC
	// calib=1E3.fC.cm2=1E-4.pC.m2
	// kg=dm3(eau)=1E-3.m3
	// J=q.eV
	// Gy=J.kg-1=1E3.q.eV.m-3 <=> eV.m-3=1E-3.Gy.q
	// pC_to_Gy=1E13.eV.pC-1.m-1.m-2=1E13.eV.pC-1.m-3
	// pC_to_Gy=1E13.[eV.m-3].pC-1. <=> 1E10.q.Gy.pC-1
	// ***********************************************

	double Dose[N_STRIPS][N_STRIPS];
	// cout<<TEL<<" "<<calib<<endl;

	TH2F* TH2_Dose=new TH2F("TH2_Dose","Fluency map (particle/cm2)",N_STRIPS,1,33,N_STRIPS,1,33);
	TH2F* TH2_Cont=new TH2F();

	for(int i=0;i<N_STRIPS;i++)
		for(int j=0;j<N_STRIPS;j++)
		{
			Dose[i][j]=Fluence[i][j]*pC_to_Gy;
			// TH2_Dose->SetBinContent(i+1,j+1,Fluence[i][j]/calib);
			TH2_Dose->SetBinContent(i+1,j+1,Dose[i][j]);
		}
	dose_max=TH2_Dose->GetBinContent(TH2_Dose->GetMaximumBin());
	double contours[4];
				 contours[0] = 0.*dose_max;
				 contours[1] = 0.2*dose_max;
				 contours[2] = 0.5*dose_max;
				 contours[3] = 0.9*dose_max;

	TH1F* Dose_dist=new TH1F("Dose_dist","Distribution de dose",25,dose_max*.1,dose_max*1.1);
	for(int i=0;i<N_STRIPS;i++)
		for(int j=0;j<N_STRIPS;j++)
		{
			if(Dose[i][j]>.9*dose_max)
			{
				mean_dose+=Dose[i][j];
				rms_dose+=Dose[i][j]*Dose[i][j];
				count_dose++;
			}
			Dose_dist->Fill(Dose[i][j]);
		}
	mean_dose/=count_dose;
	rms_dose=sqrt(rms_dose/count_dose-(mean_dose*mean_dose));
	
	TCanvas *cDose= new TCanvas("Dose distribution");
	cDose->SetCanvasSize(2100,700);
	cDose->Divide(3,1);

	cDose->cd(1);
	TH2_Dose->SetTitle("");//Fluency map (particle/cm2)");
	TH2_Dose->SetTitleSize(0.0);
	TH2_Dose->SetStats(0);
	TH2_Dose->GetXaxis()->SetTitle("Channel X");
	TH2_Dose->GetXaxis()->SetNdivisions(N_STRIPS);
	TH2_Dose->GetXaxis()->SetTickSize(0.01);
	TH2_Dose->GetXaxis()->SetTitleSize(0.036);
	TH2_Dose->GetXaxis()->CenterTitle();
	TH2_Dose->GetXaxis()->SetLabelSize(0.02);
	TH2_Dose->GetYaxis()->SetTitle("Channel Y");
	TH2_Dose->GetYaxis()->SetNdivisions(N_STRIPS);
	TH2_Dose->GetYaxis()->SetTickSize(0.01);
	TH2_Dose->GetYaxis()->SetTitleSize(0.036);
	TH2_Dose->GetYaxis()->CenterTitle();
	TH2_Dose->GetYaxis()->SetLabelSize(0.02);
	TH2_Dose->Draw("colz");

	TH2_Cont=(TH2F*)TH2_Dose->Clone("TH2_Cont");
	TH2_Cont->SetContour(4, contours);
	TH2_Cont->SetLineColor(kWhite);
	// TH2_Cont->SetLineWidth(1.8);
	TH2_Cont->Draw("same cont3 list");

	cDose->cd(2);
	Dose_dist->SetTitle("");//Fluency map (particle/cm2)");
	Dose_dist->SetTitleSize(0.0);
	Dose_dist->SetStats(0);
	Dose_dist->GetXaxis()->SetTitle("Dose (Gy)");
	Dose_dist->GetXaxis()->SetTickSize(0.01);
	Dose_dist->GetXaxis()->SetTitleSize(0.036);
	Dose_dist->GetXaxis()->CenterTitle();
	Dose_dist->GetXaxis()->SetLabelSize(0.02);
	Dose_dist->GetYaxis()->SetTitle("Nombre pixels");
	Dose_dist->GetYaxis()->SetTickSize(0.01);
	Dose_dist->GetYaxis()->SetTitleSize(0.036);
	Dose_dist->GetYaxis()->CenterTitle();
	Dose_dist->GetYaxis()->SetLabelSize(0.02);
	Dose_dist->Draw("colz");

	cDose->cd(3);
	double xt=.9;
	double yt=.9;
	double ydecal=.05;
	TText* Texte=new TText();
	TString text_tmp;
	Texte->SetTextAlign(31);
	Texte->SetTextFont(43);
	Texte->SetTextSize(30);
	Texte->SetTextColor(1);
	text_tmp.Form("Irradiation %d",nb_area+1);
	Texte->DrawText(xt,yt,text_tmp);
	yt-=ydecal;
	yt-=ydecal;
	if(calibrage_used==1||calibrage_used==2)
	{
		Texte->DrawText(xt,yt,"Particule : proton");
		yt-=ydecal;
	}
	if(calibrage_used==3)
	{
		Texte->DrawText(xt,yt,"Particule : 68 MeV proton");
		yt-=ydecal;
	}
	text_tmp.Form("Energie : %2.2lf MeV",energy);
	Texte->DrawText(xt,yt,text_tmp);
	yt-=ydecal;
	text_tmp.Form("Calibrage : %3.3E fC/part.",Calib_value(0,0,energy));
	Texte->DrawText(xt,yt,text_tmp);
	yt-=ydecal;
	text_tmp.Form("TEL : %3.3E keV/um",TEL);
	Texte->DrawText(xt,yt,text_tmp);
	yt-=ydecal;
	yt-=ydecal;
	text_tmp.Form("Dose maximale %3.3E Gy",dose_max);
	Texte->DrawText(xt,yt,text_tmp);
	yt-=ydecal;
	Texte->DrawText(xt,yt,"Isodoses");
	yt-=ydecal;
	text_tmp.Form("90%% -- %3.3E Gy",contours[3]);
	Texte->DrawText(xt,yt,text_tmp);
	yt-=ydecal;
	text_tmp.Form("50%% -- %3.3E Gy",contours[2]);
	Texte->DrawText(xt,yt,text_tmp);
	yt-=ydecal;
	text_tmp.Form("20%% -- %3.3E Gy",contours[1]);
	Texte->DrawText(xt,yt,text_tmp);
	yt-=ydecal;
	yt-=ydecal;
	text_tmp.Form("Dose moyenne 90%% %3.3E Gy +/- %2.2lf %%",mean_dose,rms_dose/mean_dose*100.);
	Texte->DrawText(xt,yt,text_tmp);
	yt-=ydecal;
	cout<<"Dose moyenne dans les 90% de "<<mean_dose<<" Gy +/- "<<rms_dose/mean_dose*100.<<" %"<<endl;
	logfile<<"Dose moyenne dans les 90% de "<<mean_dose<<" Gy +/- "<<rms_dose/mean_dose*100.<<" %"<<endl;

	cDose->SaveAs("Picture/Dose_distribution.png");
	
	TH2_Dose->Delete();
	TH2_Cont->Delete();
	Dose_dist->Delete();
	cDose->Destructor();
}

int main(int argc, char** argv)
{
	cout.precision(8);
	system("rm -f Picture/*.png");
	system("rm -f Sampling/*.png Sampling/*.pdf Sampling/*.mp4");
	// cout.setf(std::ios_base::fixed | std::ios_base::scientific, std::ios_base::floatfield);
	set_plot_style();
	// char *filename;
	char *filename=(char*)malloc(80);
	
	faster_file_reader_p reader;
	faster_data_p data;
	electrometer_data electro;

	PreSFB.resize(N_STRIPS);
	PostSFB.resize(N_STRIPS);

	TFile* rootfile=new TFile("./PostAnalysis.root","RECREATE");
	TText* Texte=new TText();
	TString name_area;
	TString name_smpl;
	TString text_tmp;
	unsigned short label;
	int data_calib=0;
	int data_meas=0;
	int count_tot=0;
	int count_int=0;
	int count_smpl=0;
	int integration;
	int int_fps;
	int npeak;
	int ii;
	int i_cx;
	int i_cy;
	int in_area=0;
	int isLabelX=0;
	int isLabelY=0;
	int tot_area=0;
	int count_area=0;
	int bin_up=1000;
	int bin_down=2;
	int voxel;
	int bool_print=0;
	int strip_min;
	int strip_max;
	double t0;
	double t1;
	double fasterTime=0.;
	double integralTime=0.;
	double mid_signal;
	double sum_x=0.;
	double sum_y=0.;
	double sum_val=0.;
	double val;
	double charge;
	double ampl;
	double mean_x;
	double mean_y;
	double rms_x;
	double rms_y;
	double xt;
	double yt;
	double ydecal=.04;
	double min;
	double max;
	double calib_factor;
	double chargeTot_pC=0.;
	double chargeTot_signal_X=0.;
	double chargeTot_signal_Y=0.;
	double charge_X_int=0.;
	double charge_Y_int=0.;
	double Threshold=0.005;//500.*(fC_per_particle/1000.); /// signal equivalent to 500 particle passing through
	double factor_eoff=.00001;
	double par[6];
	double EOffX[N_STRIPS];
	double EOffY[N_STRIPS];
	float* PeakX;
	TArrayD* ChX=new TArrayD(N_STRIPS);
	TArrayD* ChY=new TArrayD(N_STRIPS);
	TArrayD* SamplX=new TArrayD(N_STRIPS);
	TArrayD* SamplY=new TArrayD(N_STRIPS);
	TArrayD* AreaX=new TArrayD(N_STRIPS);
	TArrayD* AreaY=new TArrayD(N_STRIPS);
	TArrayD* TotalX=new TArrayD(N_STRIPS);
	TArrayD* TotalY=new TArrayD(N_STRIPS);
	double ProfilX[N_STRIPS];
	double ProfilY[N_STRIPS];
	double vect_time_smpl[MAX_SMPL];
	double vect_charge_t_smpl[MAX_SMPL];
	double vect_mean_x[MAX_SMPL];
	double vect_mean_y[MAX_SMPL];
	double vect_rms_x[MAX_SMPL];
	double vect_rms_y[MAX_SMPL];
	double vect_ampl_area[MAX_SMPL];
	double vect_charge_t_area[MAX_SMPL];
	double vect_charge_x_area[MAX_SMPL];
	double vect_charge_y_area[MAX_SMPL];
	double vect_ampl_x_area[MAX_SMPL];
	double vect_ampl_y_area[MAX_SMPL];
	double vect_mean_x_area[MAX_SMPL];
	double vect_mean_y_area[MAX_SMPL];
	double vect_rms_x_area[MAX_SMPL];
	double vect_rms_y_area[MAX_SMPL];
	double vect_ampl_x2_area[MAX_SMPL];
	double vect_ampl_y2_area[MAX_SMPL];
	double vect_mean_x2_area[MAX_SMPL];
	double vect_mean_y2_area[MAX_SMPL];
	double vect_rms_x2_area[MAX_SMPL];
	double vect_rms_y2_area[MAX_SMPL];
	double vect_fwhm_x_area[MAX_SMPL];
	double vect_fwhm_y_area[MAX_SMPL];
	double signal_time[MAX_SMPL][2];
	double Map[N_STRIPS][N_STRIPS];

	double* vect_time_tot=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_time_int=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_charge_clean=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_charge_cumul=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_fluence_cumul=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_intensity_inst=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* matrix_fluence=(double*)malloc(bin_up*bin_up*sizeof(double));

	ofstream errrel("Output.txt",std::ios::out);

	int config_simu=0;
	if(argc>1)
		config_simu=(int)atof(argv[1]);

	EntryParameters(config_simu);

	if(energy>0.)
	{
		calib_energy=Loss_value(energy,1.2e-3)/W_air*q*1.E15;
		if(calibrage_used==1)
		{
			cout<<"Valeur théorique de calibrage à "<<energy<<" MeV : "<<calib_energy<<" fC/part."<<endl;
			cout<<"Valeur de calibrage donnée : "<<calib_entry<<" fC/part."<<endl;
			if(logfileprint==true)
			{
				logfile<<"Valeur théorique de calibrage à "<<energy<<" MeV : "<<calib_energy<<" fC/part."<<endl;
				logfile<<"Valeur de calibrage donnée : "<<calib_entry<<" fC/part."<<endl;
			}
		}
		else
		{
			cout<<"Valeur théorique de calibrage à "<<energy<<" MeV : "<<calib_energy<<" fC/part."<<endl;
			if(logfileprint==true)
				logfile<<"Valeur théorique de calibrage à "<<energy<<" MeV : "<<calib_energy<<" fC/part."<<endl;
		}
	}
	if(energy<1.)
	{
		for(int i=0;i<ind_en;i++)
		{
			multiple_calib_th[i]=Loss_value(multiple_energy[i],1.2e-3)/W_air*q*1.E15;
			if(calibrage_used==5&&ind_cal==ind_en)
			{
				cout<<"Valeur théorique de calibrage à "<<multiple_energy[i]<<" MeV : "<<multiple_calib_th[i]<<" fC/part."<<endl;
				cout<<"Valeur de calibrage donnée : "<<multiple_calib[i]<<" fC/part."<<endl;
				if(logfileprint==true)
				{
					logfile<<"Valeur théorique de calibrage à "<<multiple_energy[i]<<" MeV : "<<multiple_calib_th[i]<<" fC/part."<<endl;
					logfile<<"Valeur de calibrage donnée : "<<multiple_calib[i]<<" fC/part."<<endl;
				}
			}
			else
			{
				cout<<"Valeur théorique de calibrage à "<<multiple_energy[i]<<" MeV : "<<multiple_calib_th[i]<<" fC/part."<<endl;
				if(logfileprint==true)
					logfile<<"Valeur théorique de calibrage à "<<multiple_energy[i]<<" MeV : "<<multiple_calib_th[i]<<" fC/part."<<endl;
			}
		}
	}

	strcpy(filename,data_faster_file.c_str());
	integration=IntegrationStep;
	int_fps=(int)(1./SamplingTime);
	char command_line[80];
	sprintf(command_line,"faster_disfast %s -I",filename);
	system(command_line);

	reader=faster_file_reader_open(filename);
	
	if(reader==NULL) 
	{
		printf ("Error opening file %s\n", filename);
		faster_file_reader_close(reader);
		exit(0);
	}
	else
		printf("Fichier de données bien ouvert\n");

	for(int j=0;j<N_STRIPS;j++)
	{
		ProfilX[j]=0;
		ProfilY[j]=0;
		lissage_factor[j][0]=1.;
		lissage_factor[j][1]=1.;
	}
	ChX->Reset();
	ChY->Reset();
	SamplX->Reset();
	SamplY->Reset();
	AreaX->Reset();
	AreaY->Reset();
	TotalX->Reset();
	TotalY->Reset();

	if(lissage_param==1)
		Lissage();

	if(area_find_param==0)
		DerivativeSignalArea(filename,&tot_area,signal_time);
	if(area_find_param==1)
		ChargeSignalArea(filename,&tot_area,signal_time);
	if(area_find_param==2)
		QuantaSignalArea(filename,&tot_area,signal_time);
	if(area_find_param==3)
		ManualSignalArea(filename,&tot_area,signal_time);
	
	if(tot_area>MAX_SMPL)
	{
		cout<<"Nombre d'irradiation supérieur à la limite"<<endl;
		tot_area=MAX_SMPL;
	}
	cout<<tot_area<<" période(s) d'irradiation"<<endl;
	for(int i=0;i<tot_area;i++)
	{
		cout<<"Irradiation "<<i+1<<"; début : "<<signal_time[i][0]<<"; fin : "<<signal_time[i][1]<<"; durée : "<<signal_time[i][1]-signal_time[i][0]<<endl;
		if(logfileprint==true)
			logfile<<"Irradiation "<<i+1<<"; début : "<<signal_time[i][0]<<"; fin : "<<signal_time[i][1]<<"; durée : "<<signal_time[i][1]-signal_time[i][0]<<endl;
	}

	nbSummedSample=0;
	count_area=0;
	t0=-1; 

	for(int j=0;j<N_STRIPS;j++)
	{
		EOffX[j]=offXY[j][0];
		EOffY[j]=offXY[j][1];
	}

	int i_tmp=0;
	// do
	// {
	// 	data=faster_file_reader_next(reader);
	// 	i_tmp++;
	// }while(i_tmp<100);
	// while((data=faster_file_reader_next(reader))!=NULL&&i_tmp<5) 
	while((data=faster_file_reader_next(reader))!=NULL) 
	{
		label=faster_data_label(data);
		if(label==LabelCount)
			data_calib=1;
		if(label==LabelScaler)
			data_meas=1;
		if(label==LabelX||label==LabelY)
		{
			i_tmp++;
			if(t0==-1)
			{
				t0=faster_data_clock_sec(data);
				t1=0.;
			}
			faster_data_load(data,&electro);

			fasterTime=faster_data_clock_sec(data)-t0;
			integralTime=fasterTime-t1;
			if(in_area==0&&fasterTime<signal_time[0][0])
				in_area=2;
			if(fasterTime>signal_time[count_area][0]&&fasterTime<signal_time[count_area][1])
				in_area=0;
			if(in_area==0&&fasterTime>signal_time[count_area][1])
				in_area=1;

			mid_signal=signal_time[count_area][0]+(signal_time[count_area][1]-signal_time[count_area][0])/2.;
			if(count_area>=tot_area)
				mid_signal=DBL_MAX;

			if(bkgnd_param==3)
			{
				PreSFB.clear();
				PostSFB.clear();
				min=DBL_MAX;
				max=0.;
				for(int j=FIRST_ELEC;j<LAST_ELEC;j++) 
				{
					charge=electrometer_channel_charge_pC(electro,j+1);
					switch(label) 
					{
						case LabelX:
							val=charge-EOffX[j];
							val=val*lissage_factor[j][0];
							if(val>max)	max=val;
							if(val<min)	min=val;
							PreSFB[j]=val;
							isLabelX=1;
							strip_min=borne_m_x;
							strip_max=borne_M_x;
						break;
						case LabelY:
							val=charge-EOffY[j];
							val=val*lissage_factor[j][1];
							if(val>max)	max=val;
							if(val<min)	min=val;
							PreSFB[j]=val;
							isLabelY=1;
							strip_min=borne_m_y;
							strip_max=borne_M_y;
						break;
					}
				}

				if((fasterTime<mid_signal&&bool_print==2))
					bool_print=0;
				if((fasterTime>mid_signal&&bool_print==0))
					bool_print=1;
				SubFittingBackground(bool_print,strip_min,strip_max,min,max,fasterTime,&sum_val);
				if(bool_print==1)
					bool_print=2;
				// for(int iii=0;iii<N_STRIPS;iii++)
				// 	errrel<<PostSFB[iii]<<" ";
				// errrel<<endl;

				switch(label)
				{
					case LabelX:
						if(in_area==0)
							chargeTot_signal_X+=sum_val;
						for(int j=FIRST_ELEC;j<LAST_ELEC;j++)
							ChX->SetAt(PostSFB[j],j); 
						isLabelX=1;
						// errrel<<ChX->GetSum()<<" "<<sum_val<<endl;
					break;
					case LabelY:
						if(in_area==0)
							chargeTot_signal_Y+=sum_val;
						for(int j=FIRST_ELEC;j<LAST_ELEC;j++)
							ChY->SetAt(PostSFB[j],j);
						isLabelY=1;
						// errrel<<ChY->GetSum()<<" "<<sum_val<<endl;
					break;
				}
			}
			else
			{
				for(int j=FIRST_ELEC;j<LAST_ELEC;j++) 
				{
					charge=electrometer_channel_charge_pC(electro,j+1);
					// val=charge;
					switch(label) 
					{
						case LabelX:
							val=charge-EOffX[j];
							val=val*lissage_factor[j][0];
							if(in_area==0)
							// if(in_area==0&&(j>=borne_m_x&&j<borne_M_x)) // taille du plastique
								chargeTot_signal_X+=val;
							// if(val<EOffX[j]*factor_eoff)
							// 	val=0.; 
							// ChX->SetAt(val*corrXY[j][0],j);
							ChX->SetAt(val,j);
							isLabelX=1;
						break;
						case LabelY:
							val=charge-EOffY[j];
							val=val*lissage_factor[j][1];
							if(in_area==0)
							// if(in_area==0&&(j>=borne_m_y&&j<borne_M_y)) // taille du plastique
								chargeTot_signal_Y+=val;
							// if(val<EOffY[j]*factor_eoff)
							// 	val=0.; 
							// ChY->SetAt(val*corrXY[j][1],j);
							ChY->SetAt(val,j);
							isLabelY=1;
						break;
					}
					// errrel<<val<<" ";
				}
				// errrel<<endl;
			}
			if(isLabelX==1&&isLabelY==1)
			{
				sum_x=0.;
				sum_y=0.;
				for(int j=0;j<N_STRIPS;j++)
				{
					val=ChX->GetAt(j);
					ProfilX[j]+=val;
					if(val>Threshold)
						sum_x+=val;
					val+=SamplX->GetAt(j);
					SamplX->SetAt(val,j);
					// TotalX->SetAt(val,j);
					if(in_area==0)
					{
						val=ChX->GetAt(j)+AreaX->GetAt(j);
						AreaX->SetAt(val,j);
					}

					val=ChY->GetAt(j);
					ProfilY[j]+=val;
					if(val>Threshold)
						sum_y+=val;
					val+=SamplY->GetAt(j);
					SamplY->SetAt(val,j);
					// TotalY->SetAt(val,j);
					if(in_area==0)
					{
						val=ChY->GetAt(j)+AreaY->GetAt(j);
						AreaY->SetAt(val,j);
					}
				}
				vect_time_tot[count_tot]=fasterTime;
				vect_charge_clean[count_tot]=(ChX->GetSum()+ChY->GetSum())/2.;

				if(count_tot>0)
					vect_charge_cumul[count_tot]=vect_charge_cumul[count_tot-1]+(ChX->GetSum()+ChY->GetSum())/2.;
				else
					vect_charge_cumul[count_tot]=(ChX->GetSum()+ChY->GetSum())/2.;

				calib_factor=Calib_value(count_area,in_area,energy)*pow(strip_width,2)/1000.;
				if(calib_factor!=0.)
					calib_factor=1./calib_factor;
				// calib_factor=1.;
	
				if(count_tot>0)
					vect_fluence_cumul[count_tot]=vect_fluence_cumul[count_tot-1]+(ChX->GetSum()+ChY->GetSum())/2.*calib_factor;
				else
					vect_fluence_cumul[count_tot]=(ChX->GetSum()+ChY->GetSum())/2.*calib_factor;

				if(count_tot%integration==(integration-1))
				{
					vect_time_int[count_int]=vect_time_tot[count_tot];
					if(count_int>0)
						vect_intensity_inst[count_int]=(charge_X_int+charge_Y_int)/2.*calib_factor*pow(strip_width,2)*q/(vect_time_int[count_int]-vect_time_int[count_int-1])*1.E15;
					else	
						vect_intensity_inst[count_int]=0.;
					charge_X_int=ChX->GetSum();
					charge_Y_int=ChY->GetSum();
					count_int++;
				}
				else
				{
					charge_X_int+=ChX->GetSum();
					charge_Y_int+=ChY->GetSum();
				}

				chargeTot_pC+=(sum_x+sum_y)/2.;
				count_tot++;

				// errrel<<sum_x<<" "<<sum_y<<endl;
				ChX->Reset();
				ChY->Reset();
				isLabelX=0;
				isLabelY=0;
			}

			if(in_area==1&&tot_area>0)
			{
				// cout<<"------------------------"<<endl;
				// cout<<"Piste et charges totales"<<endl;
				// // double duration=(signal_time[count_area][1]-signal_time[count_area][0]);
				// double duration=1.; // CPO fourni une même quantité en intégrale et pas instantanée 
				// for(int jj=0;jj<N_STRIPS;jj++)
				// 	cout<<jj+1<<" "<<AreaX->GetAt(jj)/duration<<" "<<AreaY->GetAt(jj)/duration<<endl;
				// cout<<"------------------------"<<endl;
				for(int jj=0;jj<N_STRIPS;jj++)
				{
					val=TotalX->GetAt(jj)+AreaX->GetAt(jj);
					TotalX->SetAt(val,jj);
					val=TotalY->GetAt(jj)+AreaY->GetAt(jj);
					TotalY->SetAt(val,jj);
				}

				delete gROOT->FindObject("FWHM");	// Pourquoi ? Parce que Root
				
				name_area="TH2_Area_";
				name_area+=(count_area+1);
				TH2F* TH2_Area=new TH2F(name_area,"Fluency map (particle/cm2)",N_STRIPS,1,33,N_STRIPS,1,33);
				TH1F* FWHM=new TH1F("FWHM","FWHM",bin_up,1,33);
				TH1F* Profil_x_area=new TH1F("Profil_x_area","X profile in number of particles",N_STRIPS,1,33);
				TH1F* Profil_y_area=new TH1F("Profil_y_area","Y profile in number of particles",N_STRIPS,1,33);
				TF1* GaussProfilX=new TF1("GaussProfilX","gaus",2,32);
				TF1* GaussProfilY=new TF1("GaussProfilY","gaus",2,32);
				TF1* g2=new TF1("g2","gaus",1,33);
				TF1* Gauss2ProfilX=new TF1("Gauss2ProfilX","gaus(0)+gaus(3)",1,33);
				TF1* Gauss2ProfilY=new TF1("Gauss2ProfilY","gaus(0)+gaus(3)",1,33);
				GaussProfilX->SetNpx(bin_up);
				GaussProfilY->SetNpx(bin_up);
				Gauss2ProfilX->SetNpx(bin_up);
				Gauss2ProfilY->SetNpx(bin_up);
				g2->SetNpx(bin_up);

				mean_x=0.;
				mean_y=0.;
				rms_x=0.;
				rms_y=0.;
				sum_x=0.;
				sum_y=0.;

				for(int i=0;i<N_STRIPS;i++)
				// for(int i=borne_m_x;i<=borne_M_x;i++)
				{
					ii=i+1;
					Profil_x_area->SetBinContent(i+1,AreaX->GetAt(i));
					Profil_y_area->SetBinContent(i+1,AreaY->GetAt(i));
					if(AreaX->GetAt(i)>Threshold)
					{
						mean_x+=ii*AreaX->GetAt(i);
						rms_x+=ii*ii*AreaX->GetAt(i);
						sum_x+=AreaX->GetAt(i);
					}
					if(AreaY->GetAt(i)>Threshold)
					{
						mean_y+=ii*AreaY->GetAt(i);
						rms_y+=ii*ii*AreaY->GetAt(i);
						sum_y+=AreaY->GetAt(i);
					}
				}

				for(int i=0;i<N_STRIPS;i++)
					for(int k=0;k<N_STRIPS;k++)
					{
						if(sum_x>0.&&sum_y>0.)
							TH2_Area->SetBinContent(i+1,k+1,AreaX->GetAt(i)*AreaY->GetAt(k)/(sum_x*sum_y)*(sum_x+sum_y)/2.);
						else	
							TH2_Area->SetBinContent(i+1,k+1,0.);
					}
				if(sum_x>0.)
				{
					mean_x/=sum_x;
					rms_x=sqrt(rms_x/sum_x-(mean_x*mean_x));
				}
				else
				{
					mean_x=0.;
					rms_x=0.;
				}
				if(sum_y>0.)
				{
					mean_y/=sum_y;
					rms_y=sqrt(rms_y/sum_y-(mean_y*mean_y));
				}
				else
				{
					mean_y=0.;
					rms_y=0.;
				}
				
				vect_charge_t_area[count_area]=(sum_x+sum_y)/2.;
				vect_charge_x_area[count_area]=sum_x;
				vect_charge_y_area[count_area]=sum_y;
				vect_ampl_area[count_area]=TH2_Area->GetBinContent(TH2_Area->GetMaximumBin());

				TCanvas *cArea= new TCanvas("Dose map");
				cArea->SetCanvasSize(2000,2000);
				cArea->Divide(2,2);

				cArea->cd(1);
				TH2_Area->SetTitle("");//Fluency map (particle/cm2)");
				TH2_Area->SetTitleSize(0.0);
				TH2_Area->SetStats(0);
				TH2_Area->GetXaxis()->SetTitle("Channel X");
				TH2_Area->GetXaxis()->SetNdivisions(N_STRIPS);
				TH2_Area->GetXaxis()->SetTickSize(0.01);
				TH2_Area->GetXaxis()->SetTitleSize(0.036);
				TH2_Area->GetXaxis()->CenterTitle();
				TH2_Area->GetXaxis()->SetLabelSize(0.02);
				TH2_Area->GetYaxis()->SetTitle("Channel Y");
				TH2_Area->GetYaxis()->SetNdivisions(N_STRIPS);
				TH2_Area->GetYaxis()->SetTickSize(0.01);
				TH2_Area->GetYaxis()->SetTitleSize(0.036);
				TH2_Area->GetYaxis()->CenterTitle();
				TH2_Area->GetYaxis()->SetLabelSize(0.02);
				TH2_Area->Draw("colz");

				cArea->cd(3);
				i_cx=ceil(mean_x);
				ampl=Profil_x_area->GetBinContent(Profil_x_area->GetBin(i_cx));
				// ampl=Profil_x_area->GetBinContent(Profil_x_area->GetMaximumBin());
				// GaussProfilX->SetParameters(ampl,mean_x,rms_x);

				i_cx=TMath::Max(2,Profil_x_area->GetMaximumBin()-bin_down);
				i_cy=TMath::Min(Profil_x_area->GetNbinsX()-2,Profil_x_area->GetMaximumBin()+bin_down);

				Profil_x_area->SetTitle("");//Fluency map (particle/cm2)");
				Profil_x_area->SetTitleSize(0.0);
				Profil_x_area->SetStats(0);
				Profil_x_area->GetXaxis()->SetTitle("Channel X");
				Profil_x_area->GetXaxis()->SetNdivisions(N_STRIPS);
				Profil_x_area->GetXaxis()->SetTickSize(0.01);
				Profil_x_area->GetXaxis()->SetTitleSize(0.036);
				Profil_x_area->GetXaxis()->CenterTitle();
				Profil_x_area->GetXaxis()->SetLabelSize(0.02);
				Profil_x_area->GetYaxis()->SetTitle("Intensity");
				Profil_x_area->GetYaxis()->SetTickSize(0.01);
				Profil_x_area->GetYaxis()->SetTitleSize(0.036);	
				Profil_x_area->GetYaxis()->CenterTitle();
				Profil_x_area->GetYaxis()->SetLabelSize(0.02);
				Profil_x_area->Draw();
				Profil_x_area->Fit(GaussProfilX,"Q0","",i_cx,i_cy);
				if(Gausstofit==1)
				{
					Profil_x_area->Fit(GaussProfilX,"QR");
					vect_ampl_x_area[count_area]=GaussProfilX->GetParameter(0);
					vect_mean_x_area[count_area]=GaussProfilX->GetParameter(1);
					vect_rms_x_area[count_area]=GaussProfilX->GetParameter(2);
					FWHM=(TH1F*)GaussProfilX->GetHistogram()->Clone();
				}
				if(Gausstofit==2)
				{
					Profil_x_area->Fit(g2,"Q0R+");
					GaussProfilX->GetParameters(&par[0]);
					g2->GetParameters(&par[3]);
					Gauss2ProfilX->SetParameters(par);
					Profil_x_area->Fit(Gauss2ProfilX,"QR+");	
					vect_ampl_x_area[count_area]=Gauss2ProfilX->GetParameter(0);
					vect_mean_x_area[count_area]=Gauss2ProfilX->GetParameter(1);
					vect_rms_x_area[count_area]=Gauss2ProfilX->GetParameter(2);
					vect_ampl_x2_area[count_area]=Gauss2ProfilX->GetParameter(3);
					vect_mean_x2_area[count_area]=Gauss2ProfilX->GetParameter(4);
					vect_rms_x2_area[count_area]=Gauss2ProfilX->GetParameter(5);
					FWHM=(TH1F*)Gauss2ProfilX->GetHistogram()->Clone();
				}
				Profil_x_area->GetYaxis()->SetRangeUser(0,TMath::Max(Profil_x_area->GetBinContent(Profil_x_area->GetMaximumBin()),FWHM->GetMaximum())*1.05);
				vect_fwhm_x_area[count_area]=FWHM->GetBinCenter(FWHM->FindLastBinAbove(FWHM->GetMaximum()/2.))
																		-FWHM->GetBinCenter(FWHM->FindFirstBinAbove(FWHM->GetMaximum()/2.));

				GaussProfilX->Delete();
				Gauss2ProfilX->Delete();
				FWHM->Clear();

				cArea->cd(4);
				i_cy=ceil(mean_y);
				ampl=Profil_y_area->GetBinContent(Profil_y_area->GetBin(i_cy));
				// ampl=Profil_y_area->GetBinContent(Profil_y_area->GetMaximumBin());
				// GaussProfilY->SetParameters(ampl,mean_y,rms_y);

				i_cx=TMath::Max(1,Profil_y_area->GetMaximumBin()-bin_down);
				i_cy=TMath::Min(Profil_y_area->GetNbinsX()-1,Profil_y_area->GetMaximumBin()+bin_down);

				Profil_y_area->SetTitle("");//Fluency map (particle/cm2)");
				Profil_y_area->SetTitleSize(0.0);
				Profil_y_area->SetStats(0);
				Profil_y_area->GetXaxis()->SetTitle("Channel Y");
				Profil_y_area->GetXaxis()->SetNdivisions(N_STRIPS);
				Profil_y_area->GetXaxis()->SetTickSize(0.01);
				Profil_y_area->GetXaxis()->SetTitleSize(0.036);
				Profil_y_area->GetXaxis()->CenterTitle();
				Profil_y_area->GetXaxis()->SetLabelSize(0.02);
				Profil_y_area->GetYaxis()->SetTitle("Intensity");
				Profil_y_area->GetYaxis()->SetTickSize(0.01);
				Profil_y_area->GetYaxis()->SetTitleSize(0.036);
				Profil_y_area->GetYaxis()->CenterTitle();
				Profil_y_area->GetYaxis()->SetLabelSize(0.02);
				Profil_y_area->Draw();
				Profil_y_area->Fit(GaussProfilY,"Q0","",i_cx,i_cy);
				if(Gausstofit==1)
				{
					Profil_y_area->Fit(GaussProfilY,"QR");
					vect_ampl_y_area[count_area]=GaussProfilY->GetParameter(0);
					vect_mean_y_area[count_area]=GaussProfilY->GetParameter(1);
					vect_rms_y_area[count_area]=GaussProfilY->GetParameter(2);
					FWHM=(TH1F*)GaussProfilY->GetHistogram()->Clone();
				}
				if(Gausstofit==2)
				{
					Profil_y_area->Fit(g2,"Q0+");
					GaussProfilY->GetParameters(&par[0]);
					g2->GetParameters(&par[3]);
					Gauss2ProfilY->SetParameters(par);
					Profil_y_area->Fit(Gauss2ProfilY,"QR+");	
					vect_ampl_y_area[count_area]=Gauss2ProfilY->GetParameter(0);
					vect_mean_y_area[count_area]=Gauss2ProfilY->GetParameter(1);
					vect_rms_y_area[count_area]=Gauss2ProfilY->GetParameter(2);
					vect_ampl_y2_area[count_area]=Gauss2ProfilY->GetParameter(3);
					vect_mean_y2_area[count_area]=Gauss2ProfilY->GetParameter(4);
					vect_rms_y2_area[count_area]=Gauss2ProfilY->GetParameter(5);
					FWHM=(TH1F*)Gauss2ProfilY->GetHistogram()->Clone();
				}
				Profil_y_area->GetYaxis()->SetRangeUser(0,TMath::Max(Profil_y_area->GetBinContent(Profil_y_area->GetMaximumBin()),FWHM->GetMaximum())*1.05);
				vect_fwhm_y_area[count_area]=FWHM->GetBinCenter(FWHM->FindLastBinAbove(FWHM->GetMaximum()/2.))
																		-FWHM->GetBinCenter(FWHM->FindFirstBinAbove(FWHM->GetMaximum()/2.));

				GaussProfilY->Delete();
				Gauss2ProfilY->Delete();
				FWHM->Delete();
				g2->Delete();

				cArea->cd(2);
				xt=.9;
				yt=.9;
				Texte->SetTextAlign(31);
				Texte->SetTextFont(43);
				Texte->SetTextSize(30);
				Texte->SetTextColor(1);
				text_tmp.Form("Irradiation %d",count_area+1);
				Texte->DrawText(xt,yt,text_tmp);
				yt-=ydecal;
				yt-=ydecal;
				text_tmp.Form("Debut : %2.2lf sec",signal_time[count_area][0]);
				Texte->DrawText(xt,yt,text_tmp);
				yt-=ydecal;
				text_tmp.Form("Fin : %2.2lf sec",signal_time[count_area][1]);
				Texte->DrawText(xt,yt,text_tmp);
				yt-=ydecal;
				text_tmp.Form("Duree : %2.2lf sec",signal_time[count_area][1]-signal_time[count_area][0]);
				Texte->DrawText(xt,yt,text_tmp);
				yt-=ydecal;
				yt-=ydecal;
				text_tmp.Form("Charge totale X : %2.2lf pC",vect_charge_x_area[count_area]);
				Texte->DrawText(xt,yt,text_tmp);
				yt-=ydecal;
				text_tmp.Form("Charge totale Y : %2.2lf pC",vect_charge_y_area[count_area]);
				Texte->DrawText(xt,yt,text_tmp);
				yt-=ydecal;
				text_tmp.Form("Charge totale : %2.2lf pC",vect_charge_t_area[count_area]);
				Texte->DrawText(xt,yt,text_tmp);
				yt-=ydecal;
				yt-=ydecal;
				Texte->DrawText(xt,yt,"Parametres ajustement");
				yt-=ydecal;
				Texte->DrawText(xt,yt,"Centre; RMS");
				yt-=ydecal;
				text_tmp.Form("X : %3.3lf; %2.2E",vect_mean_x_area[count_area],vect_rms_x_area[count_area]);
				Texte->DrawText(xt,yt,text_tmp);
				yt-=ydecal;
				if(Gausstofit==2)
				{
					text_tmp.Form("X : %3.3lf; %2.2E",vect_mean_x2_area[count_area],vect_rms_x2_area[count_area]);
					Texte->DrawText(xt,yt,text_tmp);
					yt-=ydecal;
				}
				text_tmp.Form("Y : %3.3lf; %2.2E",vect_mean_y_area[count_area],vect_rms_y_area[count_area]);
				Texte->DrawText(xt,yt,text_tmp);
				yt-=ydecal;
				if(Gausstofit==2)
				{
					text_tmp.Form("Y : %3.3lf; %2.2E",vect_mean_y2_area[count_area],vect_rms_y2_area[count_area]);
					Texte->DrawText(xt,yt,text_tmp);
					yt-=ydecal;
				}
				yt-=ydecal;
				text_tmp.Form("FWHM X : %3.3lf (%3.3lf mm)",vect_fwhm_x_area[count_area],vect_fwhm_x_area[count_area]*sizeStrip*10.);
				Texte->DrawText(xt,yt,text_tmp);
				yt-=ydecal;
				text_tmp.Form("FWHM Y : %3.3lf (%3.3lf mm)",vect_fwhm_y_area[count_area],vect_fwhm_y_area[count_area]*sizeStrip*10.);
				Texte->DrawText(xt,yt,text_tmp);
				yt-=ydecal;

				name_area="Picture/Area_";
				name_area+=(count_area+1);
				name_area+=".png";
				cArea->SaveAs(name_area);
				
				// rootfile->Write();
				TH2_Area->Delete();
				Profil_x_area->Delete();
				Profil_y_area->Delete();
				// FWHM->Delete();
				cArea->Destructor();

				AreaX->Reset();
				AreaY->Reset();
				in_area=2;
				count_area++;
			}

			// if(integralTime>DBL_MAX)
			if(integralTime>SamplingTime)
			{
				if(nbSummedSample>=MAX_SMPL)
					cout<<"Maximum number of samples reached"<<endl;
				// else if(in_area==0&&tot_area>0)
				else
				{
					TCanvas *cSmpl= new TCanvas("Dose map");
					cSmpl->SetCanvasSize(1000,1000);
					double x1=.23;
					double x2=.23;
					double y1=.78;
					double y2=.78;
					// TPad * pad1 = new TPad("pad1","pad1",0,0,x1,y1);
					// TPad * pad2 = new TPad("pad2","pad2",0,y2,x1,1);
					// TPad * pad3 = new TPad("pad3","pad3",x2,0,1,y1);
					// TPad * pad4 = new TPad("pad4","pad4",x2,y2,1,1);
					// pad1->SetTopMargin(0.1);
					// pad1->SetBottomMargin(0.1);
					// pad1->Draw();
					// pad2->SetTopMargin(0.1);
					// pad2->SetBottomMargin(0.1);
					// pad2->Draw();
					// pad3->SetTopMargin(0.1);
					// pad3->SetBottomMargin(0.1);
					// pad3->Draw();
					// pad4->SetTopMargin(0.1);
					// pad4->SetBottomMargin(0.1);
					// pad4->Draw();

					TH2F* Map_smpl=new TH2F("Map_smpl","",N_STRIPS,1,33,N_STRIPS,1,33);
					TH1F *hSmplX = new TH1F("hSmplX","",N_STRIPS,.5,32.5);
					TH1F *hSmplY = new TH1F("hSmplY","",N_STRIPS,.5,32.5);
					
					mean_x=0.;
					mean_y=0.;
					rms_x=0.;
					rms_y=0.;
					sum_x=0.;
					sum_y=0.;
					for(int i=0;i<N_STRIPS;i++)
					{
						ii=i+1;
						if(SamplX->GetAt(i)>Threshold)
						{
							hSmplX->SetBinContent(i+1,SamplX->GetAt(i));
							mean_x+=ii*SamplX->GetAt(i);
							rms_x+=ii*ii*SamplX->GetAt(i);
							sum_x+=SamplX->GetAt(i);
						}
						if(SamplY->GetAt(i)>Threshold)
						{
							hSmplY->SetBinContent(i+1,SamplY->GetAt(i));
							mean_y+=ii*SamplY->GetAt(i);
							rms_y+=ii*ii*SamplY->GetAt(i);
							sum_y+=SamplY->GetAt(i);
						}
					}
					// sum_x=SamplX->GetSum();
					// sum_y=SamplY->GetSum();
					if(sum_x>0.)
					{
						mean_x/=sum_x;
						rms_x=sqrt(rms_x/sum_x-(mean_x*mean_x));
					}
					else
					{
						mean_x=0.;
						rms_x=0.;
					}
					if(sum_y>0.)
					{
						mean_y/=sum_y;
						rms_y=sqrt(rms_y/sum_y-(mean_y*mean_y));
					}
					else
					{
						mean_y=0.;
						rms_y=0.;
					}
					vect_time_smpl[nbSummedSample]=fasterTime;
					vect_charge_t_smpl[nbSummedSample]=(sum_x+sum_y)/2.;

					vect_mean_x[nbSummedSample]=mean_x;
					vect_rms_x[nbSummedSample]=rms_x;
					vect_mean_y[nbSummedSample]=mean_y;
					vect_rms_y[nbSummedSample]=rms_y;

					for(int i=0;i<N_STRIPS;i++)
						for(int k=0;k<N_STRIPS;k++)
						{
							if(sum_x>0.&&sum_y>0.)
								Map_smpl->SetBinContent(i+1,k+1,SamplX->GetAt(i)*SamplY->GetAt(k)/(sum_x*sum_y)*(sum_x+sum_y)/2.);
							else	
								Map_smpl->SetBinContent(i+1,k+1,0.);
						}

					if(in_area==0)
					{
						// pad4->cd();
						// hSmplX->SetFillColor(2);
						// hSmplX->GetXaxis()->SetTickSize(0.01);
						// hSmplX->GetXaxis()->SetNdivisions(N_STRIPS);
						// // hSmplX->GetXaxis()->SetTitle("Strip");
						// hSmplX->GetXaxis()->CenterTitle();
						// hSmplX->GetXaxis()->SetTickSize(0.01);
						// hSmplX->GetXaxis()->SetTitleSize(0.0);
						// hSmplX->GetXaxis()->SetLabelSize(0.05);
						// hSmplX->GetYaxis()->SetTickSize(0.01);
						// hSmplX->GetYaxis()->SetTitle("");
						// hSmplX->GetYaxis()->CenterTitle();
						// hSmplX->GetYaxis()->SetTickSize(0.01);
						// hSmplX->GetYaxis()->SetTitleSize(0.0);
						// hSmplX->GetYaxis()->SetLabelSize(0.05);
						// hSmplX->SetBarWidth(0.8);
						// hSmplX->SetBarOffset(0.1);
						// hSmplX->SetStats(0);
						// hSmplX->Draw("b");
						// pad4->SaveAs("Sampling/tmp.pdf","pdf");

						// hSmplY->SetFillColor(4);
						// hSmplY->GetXaxis()->SetTickSize(0.01);
						// hSmplY->GetXaxis()->SetNdivisions(N_STRIPS);
						// // hSmplY->GetXaxis()->SetTitle("Strip");
						// hSmplY->GetXaxis()->CenterTitle();
						// hSmplY->GetXaxis()->SetTickSize(0.01);
						// hSmplY->GetXaxis()->SetTitleSize(0.0);
						// hSmplY->GetXaxis()->SetLabelSize(0.0);
						// hSmplY->GetYaxis()->SetTickSize(0.01);
						// hSmplY->GetYaxis()->SetTitle("");
						// hSmplY->GetYaxis()->CenterTitle();
						// hSmplY->GetYaxis()->SetTickSize(0.01);
						// hSmplY->GetYaxis()->SetTitleSize(0.0);
						// hSmplY->GetYaxis()->SetLabelSize(0.05);
						// hSmplY->SetBarWidth(0.8);
						// hSmplY->SetBarOffset(0.1);
						// hSmplY->SetStats(0);
						// hSmplY->Draw("b");
						
						// pad1->cd();
						// TImage *img = TImage::Open("Sampling/tmp.pdf");
						// img->Flip(90);
						// img->Draw();

						// pad3->cd();
						Map_smpl->SetTitle(Form("Sampling: %d, time: %2.0lf sec",count_smpl+1,round(vect_time_smpl[nbSummedSample])));
						Map_smpl->SetTitleSize(0.0);
						Map_smpl->SetStats(0);
						Map_smpl->SetMinimum(0.);
						Map_smpl->Draw("colz");	
						name_smpl="Sampling/Smpl_";
						name_smpl+=(count_smpl+1);
						name_smpl+=".png";
						cSmpl->SaveAs(name_smpl);
						count_smpl++;
					}
					Map_smpl->Delete();
					hSmplX->Delete();
					hSmplY->Delete();
					cSmpl->Destructor();
					SamplX->Reset();
					SamplY->Reset();
					nbSummedSample++;
				}
				t1=fasterTime;
			}
		}
	}
	count_tot--;
	count_int--;
	count_smpl--;
	
	if(count_smpl>0)
	{
		sprintf(Execution,"ffmpeg -r %d -f image2 -s 1000x1000 -i Sampling/Smpl_%%d.png -vcodec libx264 -crf 25  -pix_fmt yuv420p Sampling/Smpl.mp4",int_fps);
		cout<<"FFMPEG commande: "<<endl;
		cout<<Execution<<endl;
		system(Execution);
	}

	cout<<"Total of samples of "<<SamplingTime<<"(s) : "<<nbSummedSample<<endl;
	for(int i=0;i<tot_area;i++)
	{
		cout<<"Signal "<<i+1<<" Mean X : "<<vect_mean_x_area[i]<<"; Mean Y : "<<vect_mean_y_area[i]
		<<"; RMS X : "<<vect_rms_x_area[i]<<"; RMS Y : "<<vect_rms_y_area[i]<<"; Charge totale (pC) : "<<vect_charge_t_area[i]<<"; Amplitude (%) : "<<vect_charge_t_area[i]/chargeTot_pC*100.<<endl;
		if(logfileprint==true)
			logfile<<"Signal "<<i+1<<" Mean X : "<<vect_mean_x_area[i]<<"; Mean Y : "<<vect_mean_y_area[i]
			<<"; RMS X : "<<vect_rms_x_area[i]<<"; RMS Y : "<<vect_rms_y_area[i]<<"; Charge totale (pC) : "<<vect_charge_t_area[i]<<"; Amplitude (%) : "<<vect_charge_t_area[i]/chargeTot_pC*100.<<endl;
	}
	cout<<"Charge totale : "<<chargeTot_pC<<" pC; charge signal X : "<<chargeTot_signal_X<<" pC; charge signal Y : "<<chargeTot_signal_Y<<" pC"<<endl;

	errrel<<"------------------------"<<endl;
	errrel<<"Piste et charges totales"<<endl;
	// double duration=(signal_time[count_area][1]-signal_time[count_area][0]);
	double duration=1.; // CPO fourni une même quantité en intégrale et pas instantanée 
	for(int jj=0;jj<N_STRIPS;jj++)
		errrel<<jj+1<<" "<<TotalX->GetAt(jj)/duration<<" "<<TotalY->GetAt(jj)/duration<<endl;
	errrel<<"------------------------"<<endl;

	TH2F* TH2_Map=new TH2F("TH2_Map","Fluency map (particle/cm2)",N_STRIPS,1,33,N_STRIPS,1,33);
	TH1F* Profil_x=new TH1F("ProfX","X profile in number of particles",N_STRIPS,1,33);
	TH1F* Profil_y=new TH1F("ProfY","Y profile in number of particles",N_STRIPS,1,33);

	TCanvas *cCCharge= new TCanvas("Charge over time");
	cCCharge->SetCanvasSize(2000,2000);
	cCCharge->Divide(1,4);

	cCCharge->cd(1);
	TGraph *TG_CCharge=new TGraph(count_tot,vect_time_tot,vect_charge_clean);
	TG_CCharge->SetMarkerColor(2);
	TG_CCharge->SetLineColor(2);
	TG_CCharge->SetLineWidth(1.5);
	TG_CCharge->SetTitle("Charge clean over time");
	TG_CCharge->GetXaxis()->SetTitle("Time (s)");
	TG_CCharge->GetXaxis()->SetTickSize(0.01);
	TG_CCharge->GetXaxis()->SetTitleSize(0.06);
	TG_CCharge->GetXaxis()->SetLabelSize(0.05);
	TG_CCharge->GetYaxis()->SetTitle("Charge (pC)");
	TG_CCharge->GetYaxis()->SetTickSize(0.01);
	TG_CCharge->GetYaxis()->SetTitleSize(0.06);
	TG_CCharge->GetYaxis()->CenterTitle();
	TG_CCharge->GetYaxis()->SetLabelSize(0.05);
	TG_CCharge->Write("Charge");
	TG_CCharge->Draw("AL");

	TLine *line= new TLine();
	line->SetLineColor(4);
	line->SetLineWidth(1.5);
	TArrow *arrow= new TArrow();
	arrow->SetLineColor(6);
	arrow->SetLineWidth(3.);
	if(tot_area>0)
	{
		if(bkgnd_param==1||bkgnd_param==3)
		{
			TBox *box= new TBox();
			box->SetFillStyle(0);
			box->SetLineColor(6);
			box->SetLineWidth(2);
			box->SetLineStyle(5);
			box->DrawBox(debut_eoff,TG_CCharge->GetYaxis()->GetXmin()/1.1,signal_time[0][0],TG_CCharge->GetYaxis()->GetXmax()/1.1);
			box->DrawBox(signal_time[tot_area-1][1],TG_CCharge->GetYaxis()->GetXmin()/1.1,fin_eoff,TG_CCharge->GetYaxis()->GetXmax()/1.1);
		}
		for(int i=0;i<tot_area;i++)
		{
			double arrow_y=(TG_CCharge->GetYaxis()->GetXmax()-TG_CCharge->GetYaxis()->GetXmin())/3.+TG_CCharge->GetYaxis()->GetXmin();
			line->DrawLine(signal_time[i][0],TG_CCharge->GetYaxis()->GetXmin()/1.1,signal_time[i][0],TG_CCharge->GetYaxis()->GetXmax()/1.1);
			line->DrawLine(signal_time[i][1],TG_CCharge->GetYaxis()->GetXmin()/1.1,signal_time[i][1],TG_CCharge->GetYaxis()->GetXmax()/1.1);
			arrow->DrawArrow(signal_time[i][0],arrow_y,signal_time[i][1],arrow_y,0.01,"<>");
		}
	}

	cCCharge->cd(2);
	TGraph *TG_CumulCharge=new TGraph(count_tot,vect_time_tot,vect_charge_cumul);
	TG_CumulCharge->SetMarkerColor(2);
	TG_CumulCharge->SetLineColor(2);
	TG_CumulCharge->SetLineWidth(2.);
	TG_CumulCharge->SetTitle("Charge cumul over time");
	TG_CumulCharge->GetXaxis()->SetTitle("Time (s)");
	TG_CumulCharge->GetXaxis()->SetTickSize(0.01);
	TG_CumulCharge->GetXaxis()->SetTitleSize(0.06);
	TG_CumulCharge->GetXaxis()->SetLabelSize(0.05);
	TG_CumulCharge->GetYaxis()->SetTitle("Charge (pC)");
	TG_CumulCharge->GetYaxis()->SetTickSize(0.01);
	TG_CumulCharge->GetYaxis()->SetTitleSize(0.06);
	TG_CumulCharge->GetYaxis()->CenterTitle();
	TG_CumulCharge->GetYaxis()->SetLabelSize(0.05);
	TG_CumulCharge->Write("Charge_cumul");
	TG_CumulCharge->Draw("AL");

	cCCharge->cd(3);
	TGraph *TG_CumulFluence=new TGraph(count_tot,vect_time_tot,vect_fluence_cumul);
	TG_CumulFluence->SetMarkerColor(2);
	TG_CumulFluence->SetLineColor(4);
	TG_CumulFluence->SetLineWidth(2.);
	TG_CumulFluence->SetTitle("Fluence cumul over time");
	TG_CumulFluence->GetXaxis()->SetTitle("Time (s)");
	TG_CumulFluence->GetXaxis()->SetTickSize(0.01);
	TG_CumulFluence->GetXaxis()->SetTitleSize(0.06);
	TG_CumulFluence->GetXaxis()->SetLabelSize(0.05);
	TG_CumulFluence->GetYaxis()->SetTitle("Particles/cm2");
	TG_CumulFluence->GetYaxis()->SetTickSize(0.01);
	TG_CumulFluence->GetYaxis()->SetTitleSize(0.06);
	TG_CumulFluence->GetYaxis()->CenterTitle();
	TG_CumulFluence->GetYaxis()->SetLabelSize(0.05);
	TG_CumulFluence->Write("Fluence_cumul");
	TG_CumulFluence->Draw("AL");

	cCCharge->cd(4);
	TGraph *TG_IntensInst=new TGraph(count_int,vect_time_int,vect_intensity_inst);
	TG_IntensInst->SetMarkerColor(4);
	TG_IntensInst->SetLineColor(4);
	TG_IntensInst->SetLineWidth(1.5);
	TG_IntensInst->SetTitle("Intensity");
	TG_IntensInst->GetXaxis()->SetTitle("Time (s)");
	TG_IntensInst->GetXaxis()->SetTickSize(0.01);
	TG_IntensInst->GetXaxis()->SetTitleSize(0.06);
	TG_IntensInst->GetXaxis()->SetLabelSize(0.05);
	TG_IntensInst->GetYaxis()->SetTitle("Intensity (fA)");
	TG_IntensInst->GetYaxis()->SetTickSize(0.01);
	TG_IntensInst->GetYaxis()->SetTitleSize(0.06);
	TG_IntensInst->GetYaxis()->CenterTitle();
	TG_IntensInst->GetYaxis()->SetLabelSize(0.05);
	TG_IntensInst->Write("Intensity");
	TG_IntensInst->Draw("AL");

	cCCharge->SaveAs("Picture/Charge_clean.png");

	TG_CCharge->Delete();
	TG_CumulCharge->Delete();
	TG_CumulFluence->Delete();
	TG_IntensInst->Delete();
	cCCharge->Destructor();
	free(vect_time_tot);
	free(vect_time_int);
	free(vect_charge_clean);
	free(vect_charge_cumul);
	free(vect_fluence_cumul);
	free(vect_intensity_inst);

	sum_x=0.;
	sum_y=0.;
	for(int j=0;j<N_STRIPS;j++)
	{
		sum_x+=ProfilX[j];
		sum_y+=ProfilY[j];
	}

	for(int j=0;j<N_STRIPS;j++)	
	{
		Profil_x->SetBinContent(j+1,ProfilX[j]);
		Profil_y->SetBinContent(j+1,ProfilY[j]);
		for(int k=0;k<N_STRIPS;k++)
		{
			Map[j][k]=ProfilX[j]*ProfilY[k]/(sum_x*sum_y)*(sum_x+sum_y)/2.;
			TH2_Map->SetBinContent(j+1,k+1,ProfilX[j]*ProfilY[k]/(sum_x*sum_y)*(sum_x+sum_y)/2.);
		}
	}

	if(dosedistribution||calibrage_used!=-1.)
		DoseDistribution(0,Map);

	TCanvas *cMap= new TCanvas("Dose map");
	cMap->SetCanvasSize(2000,2000);
	cMap->Divide(2,2);

	cMap->cd(1);
	TH2_Map->SetTitle("");//Fluency map (particle/cm2)");
	TH2_Map->SetTitleSize(0.0);
	TH2_Map->SetStats(0);
	TH2_Map->GetXaxis()->SetTitle("Channel X");
	TH2_Map->GetXaxis()->SetNdivisions(N_STRIPS);
	TH2_Map->GetXaxis()->SetTickSize(0.01);
	TH2_Map->GetXaxis()->SetTitleSize(0.036);
	TH2_Map->GetXaxis()->CenterTitle();
	TH2_Map->GetXaxis()->SetLabelSize(0.02);
	TH2_Map->GetYaxis()->SetTitle("Channel Y");
	TH2_Map->GetYaxis()->SetNdivisions(N_STRIPS);
	TH2_Map->GetYaxis()->SetTickSize(0.01);
	TH2_Map->GetYaxis()->SetTitleSize(0.036);
	TH2_Map->GetYaxis()->CenterTitle();
	TH2_Map->GetYaxis()->SetLabelSize(0.02);
	// TH2_Map->Fit(f2DGauss);
	TH2_Map->Draw("colz");

	cMap->cd(2);
	// bin_up=N_STRIPS;
	TF2 *f2DGauss = new TF2("f2DGauss","[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])",1,33,1,33);
	TF2 *f2D2Gauss = new TF2("f2D2Gauss","([0]*TMath::Gaus(x,[1],[2])+[3]*TMath::Gaus(x,[4],[5]))*([6]*TMath::Gaus(y,[7],[8])+[9]*TMath::Gaus(y,[10],[11]))*[12]",1,33,1,33);
	// TF2 *f2D2Gauss = new TF2("f2D2Gauss","([0]*TMath::Gaus(x,[1],[2])+[3]*TMath::Gaus(x,[4],[5]))*([6]*TMath::Gaus(y,[7],[8])+TMath::Gaus(y,[9],[10]))",1,33,1,33);
	TH2F* TH2_Map_up=new TH2F("TH2_Map_up","Enhanced fluency map (particle/cm2)",bin_up,1,33,bin_up,1,33);
	TH2F* TH2_Map_tmp=new TH2F();

	f2DGauss->SetNpx(bin_up);
	f2DGauss->SetNpy(bin_up);
	f2D2Gauss->SetNpx(bin_up);
	f2D2Gauss->SetNpy(bin_up);
	for(int i=0;i<tot_area;i++)
	{
		ampl=vect_ampl_area[i];
		mean_x=vect_mean_x_area[i];
		mean_y=vect_mean_y_area[i];
		rms_x=vect_rms_x_area[i];
		rms_y=vect_rms_y_area[i];
		if(Gausstofit==1)
		{
			f2DGauss->SetParameters(ampl,mean_x,rms_x,mean_y,rms_y);
			TH2_Map_tmp=((TH2F*)f2DGauss->GetHistogram());
		}
		if(Gausstofit==2)
		{
			f2D2Gauss->SetParameter(0,vect_ampl_x_area[i]);
			f2D2Gauss->SetParameter(1,vect_mean_x_area[i]);
			f2D2Gauss->SetParameter(2,vect_rms_x_area[i]);
			f2D2Gauss->SetParameter(3,vect_ampl_x2_area[i]);
			f2D2Gauss->SetParameter(4,vect_mean_x2_area[i]);
			f2D2Gauss->SetParameter(5,vect_rms_x2_area[i]);
			f2D2Gauss->SetParameter(6,vect_ampl_y_area[i]);
			f2D2Gauss->SetParameter(7,vect_mean_y_area[i]);
			f2D2Gauss->SetParameter(8,vect_rms_y_area[i]);
			f2D2Gauss->SetParameter(9,vect_ampl_y2_area[i]);
			f2D2Gauss->SetParameter(10,vect_mean_y2_area[i]);
			f2D2Gauss->SetParameter(11,vect_rms_y2_area[i]);
			ampl=vect_ampl_area[i]/((vect_ampl_x_area[i]+vect_ampl_x2_area[i])*(vect_ampl_y_area[i]+vect_ampl_y2_area[i]));
			f2D2Gauss->SetParameter(12,ampl);
			TH2_Map_tmp=((TH2F*)f2D2Gauss->GetHistogram());
		}
		for(int j=0;j<bin_up;j++)	for(int k=0;k<bin_up;k++)
		{
			voxel=j*bin_up+k;
			matrix_fluence[voxel]+=TH2_Map_tmp->GetBinContent(j,k);
			if(i==tot_area-1)
				TH2_Map_up->SetBinContent(TH2_Map_up->GetBin(j,k),matrix_fluence[voxel]);
		}
	}

	TH2_Map_up->SetTitle("");//Fluency map (particle/cm2)");
	TH2_Map_up->SetTitleSize(0.0);
	TH2_Map_up->SetStats(0);
	TH2_Map_up->GetXaxis()->SetTitle("Channel X");
	TH2_Map_up->GetXaxis()->SetNdivisions(N_STRIPS);
	TH2_Map_up->GetXaxis()->SetTickSize(0.01);
	TH2_Map_up->GetXaxis()->SetTitleSize(0.036);
	TH2_Map_up->GetXaxis()->CenterTitle();
	TH2_Map_up->GetXaxis()->SetLabelSize(0.02);
	TH2_Map_up->GetYaxis()->SetTitle("Channel Y");
	TH2_Map_up->GetYaxis()->SetNdivisions(N_STRIPS);
	TH2_Map_up->GetYaxis()->SetTickSize(0.01);
	TH2_Map_up->GetYaxis()->SetTitleSize(0.036);
	TH2_Map_up->GetYaxis()->CenterTitle();
	TH2_Map_up->GetYaxis()->SetLabelSize(0.02);
	TH2_Map_up->Draw("colz");

	TGraph *TG_Centers=new TGraph(tot_area,vect_mean_x_area,vect_mean_y_area);
	TG_Centers->SetMarkerColor(1);
	TG_Centers->SetMarkerStyle(2);
	TG_Centers->SetLineWidth(0.);
	// TG_Centers->Draw("P same");
	TG_Centers->Draw("P same");
	free(matrix_fluence);

	cMap->cd(3);
	Profil_x->SetTitle("");//Fluency map (particle/cm2)");
	Profil_x->SetTitleSize(0.0);
	Profil_x->SetStats(0);
	Profil_x->GetXaxis()->SetTitle("Channel X");
	Profil_x->GetXaxis()->SetNdivisions(N_STRIPS);
	Profil_x->GetXaxis()->SetTickSize(0.01);
	Profil_x->GetXaxis()->SetTitleSize(0.036);
	Profil_x->GetXaxis()->CenterTitle();
	Profil_x->GetXaxis()->SetLabelSize(0.02);
	Profil_x->GetYaxis()->SetTitle("Intensity");
	Profil_x->GetYaxis()->SetTickSize(0.01);
	Profil_x->GetYaxis()->SetTitleSize(0.036);
	Profil_x->GetYaxis()->CenterTitle();
	Profil_x->GetYaxis()->SetLabelSize(0.02);
	Profil_x->GetYaxis()->SetRangeUser(0,Profil_x->GetBinContent(Profil_x->GetMaximumBin())*1.05);
	Profil_x->Draw();

	cMap->cd(4);
	Profil_y->SetTitle("");//Fluency map (particle/cm2)");
	Profil_y->SetTitleSize(0.0);
	Profil_y->SetStats(0);
	Profil_y->GetXaxis()->SetTitle("Channel Y");
	Profil_y->GetXaxis()->SetNdivisions(N_STRIPS);
	Profil_y->GetXaxis()->SetTickSize(0.01);
	Profil_y->GetXaxis()->SetTitleSize(0.036);
	Profil_y->GetXaxis()->CenterTitle();
	Profil_y->GetXaxis()->SetLabelSize(0.02);
	Profil_y->GetYaxis()->SetTitle("Intensity");
	Profil_y->GetYaxis()->SetTickSize(0.01);
	Profil_y->GetYaxis()->SetTitleSize(0.036);
	Profil_y->GetYaxis()->CenterTitle();
	Profil_y->GetYaxis()->SetLabelSize(0.02);
	Profil_y->GetYaxis()->SetRangeUser(0,Profil_y->GetBinContent(Profil_y->GetMaximumBin())*1.05);
	Profil_y->Draw();
	cMap->SaveAs("Picture/Fluence_reconstruction.png");
	
	// rootfile->Write();
	TH2_Map->Delete();
	TH2_Map_up->Delete();
	// TH2_Map_tmp->Delete();
	f2DGauss->Delete();
	f2D2Gauss->Delete();
	Profil_x->Delete();
	Profil_y->Delete();
	cMap->Destructor();

	if(nbSummedSample>0)
	{
		TCanvas *cSamp= new TCanvas("Sampling dose");
		cSamp->SetCanvasSize(2000,2000);
		cSamp->Divide(1,4);

		cSamp->cd(1);
		TGraph *TG_Mean_x=new TGraph(nbSummedSample,vect_time_smpl,vect_mean_x);
		TG_Mean_x->SetMarkerStyle(2);
		TG_Mean_x->SetMarkerColor(2);
		TG_Mean_x->SetLineColor(2);
		TG_Mean_x->SetLineWidth(1.5);
		TG_Mean_x->SetTitle("Mean X position");
		TG_Mean_x->GetXaxis()->SetTitle("Time (s)");
		TG_Mean_x->GetXaxis()->SetTickSize(0.01);
		TG_Mean_x->GetXaxis()->SetTitleSize(0.06);
		TG_Mean_x->GetXaxis()->SetLabelSize(0.05);
		TG_Mean_x->GetYaxis()->SetTitle("Position");
		TG_Mean_x->GetYaxis()->SetTickSize(0.01);
		TG_Mean_x->GetYaxis()->SetTitleSize(0.06);
		TG_Mean_x->GetYaxis()->CenterTitle();
		TG_Mean_x->GetYaxis()->SetLabelSize(0.05);
		TG_Mean_x->Write("Mean X");
		TG_Mean_x->Draw("ALP");

		cSamp->cd(2);
		TGraph *TG_Mean_y=new TGraph(nbSummedSample,vect_time_smpl,vect_mean_y);
		TG_Mean_y->SetMarkerStyle(2);
		TG_Mean_y->SetMarkerColor(2);
		TG_Mean_y->SetLineColor(2);
		TG_Mean_y->SetLineWidth(1.5);
		TG_Mean_y->SetTitle("Mean Y position");
		TG_Mean_y->GetXaxis()->SetTitle("Time (s)");
		TG_Mean_y->GetXaxis()->SetTickSize(0.01);
		TG_Mean_y->GetXaxis()->SetTitleSize(0.06);
		TG_Mean_y->GetXaxis()->SetLabelSize(0.05);
		TG_Mean_y->GetYaxis()->SetTitle("Position");
		TG_Mean_y->GetYaxis()->SetTickSize(0.01);
		TG_Mean_y->GetYaxis()->SetTitleSize(0.06);
		TG_Mean_y->GetYaxis()->CenterTitle();
		TG_Mean_y->GetYaxis()->SetLabelSize(0.05);
		TG_Mean_y->Write("Mean Y");
		TG_Mean_y->Draw("ALP");

		cSamp->cd(3);
		TGraph *TG_Rms_x=new TGraph(nbSummedSample,vect_time_smpl,vect_rms_x);
		TG_Rms_x->SetMarkerStyle(2);
		TG_Rms_x->SetMarkerColor(4);
		TG_Rms_x->SetLineColor(4);
		TG_Rms_x->SetLineWidth(1.5);
		TG_Rms_x->SetTitle("RMS along x-axis");
		TG_Rms_x->GetXaxis()->SetTitle("Time (s)");
		TG_Rms_x->GetXaxis()->SetTickSize(0.01);
		TG_Rms_x->GetXaxis()->SetTitleSize(0.06);
		TG_Rms_x->GetXaxis()->SetLabelSize(0.05);
		TG_Rms_x->GetYaxis()->SetTitle("RMS");
		TG_Rms_x->GetYaxis()->SetTickSize(0.01);
		TG_Rms_x->GetYaxis()->SetTitleSize(0.06);
		TG_Rms_x->GetYaxis()->CenterTitle();
		TG_Rms_x->GetYaxis()->SetLabelSize(0.05);
		TG_Rms_x->Write("RMS X");
		TG_Rms_x->Draw("ALP");

		cSamp->cd(4);
		TGraph *TG_Rms_y=new TGraph(nbSummedSample,vect_time_smpl,vect_rms_y);
		TG_Rms_y->SetMarkerStyle(2);
		TG_Rms_y->SetMarkerColor(4);
		TG_Rms_y->SetLineColor(4);
		TG_Rms_y->SetLineWidth(1.5);
		TG_Rms_y->SetTitle("RMS along y-axis");
		TG_Rms_y->GetXaxis()->SetTitle("Time (s)");
		TG_Rms_y->GetXaxis()->SetTickSize(0.01);
		TG_Rms_y->GetXaxis()->SetTitleSize(0.06);
		TG_Rms_y->GetXaxis()->SetLabelSize(0.05);
		TG_Rms_y->GetYaxis()->SetTitle("RMS");
		TG_Rms_y->GetYaxis()->SetTickSize(0.01);
		TG_Rms_y->GetYaxis()->SetTitleSize(0.06);
		TG_Rms_y->GetYaxis()->CenterTitle();
		TG_Rms_y->GetYaxis()->SetLabelSize(0.05);
		TG_Rms_y->Write("RMS Y");
		TG_Rms_y->Draw("ALP");

		// rootfile->Write();
		cSamp->SaveAs("Picture/Sampling.png");
		cSamp->Destructor();
		TG_Mean_x->Delete();
		TG_Mean_y->Delete();
		TG_Rms_x->Delete();
		TG_Rms_y->Delete();
	}

	if(data_calib==1&&tot_area>0)
		Calibrage(filename,chargeTot_signal_X,chargeTot_signal_Y);
	else
		cout<<"Pas de données de calibrage"<<endl;

	if(data_meas==1&&tot_area>0)
		Scaler(filename,1./100.,tot_area,signal_time,vect_charge_t_area,vect_charge_x_area,vect_charge_y_area);
	else
		cout<<"Pas de données de mesure scaler"<<endl;

	// if(Vect_calib_factor.size()>0)
	// {
	// 	for(int i=0;i<Vect_calib_factor.size();i++)
	// 		cout<<Vect_calib_factor[i]*1000<<" "<<Vect_calib_charge[i]<<" "<<Vect_calib_quanta[i]<<endl;
	// }
	// for(int i=0;i<tot_area;i++)
	// 	cout<<"i "<<vect_charge_t_area[i]<<"; X "<<vect_charge_x_area[i]<<"; Y "<<vect_charge_y_area[i]<<endl;

	printf("Images générées\n");
	faster_file_reader_close(reader);
	free(filename);
	rootfile->Write();
	rootfile->Close();

	errrel.close();
	if(logfileprint==true)
	{
		logfile.close();
		cout<<"Fichier log généré"<<endl;
	}

	char Execution[80];
	sprintf(Execution,"rm -f -r %s",data_folder.c_str());
	system(Execution);
	sprintf(Execution,"mkdir %s",data_folder.c_str());
	system(Execution);
	sprintf(Execution,"cp Picture/*.png %s/",data_folder.c_str());
	system(Execution);
	sprintf(Execution,"cp PostAnalysis.root %s/",data_folder.c_str());
	system(Execution);
	if(logfileprint==true)
	{
		sprintf(Execution,"cp Analysis.log %s/",data_folder.c_str());
		system(Execution);
	}

	return EXIT_SUCCESS;
}