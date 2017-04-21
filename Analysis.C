#include "Analysis.h"

using namespace std;

void EntryParameters(int config_simu)
{
	stringstream ss;
	ss<<config_simu;
	string indice=ss.str();

	int iint;
	int ichar;
	int ivar;
	char next;
	string data_file;
	string path_file;
	string filename="./Entry/Entry_param_"+indice+".txt";
	// string filename="./Entry/Entry_param_1.txt";
	string tmp;

	Variable_init.clear();
	Value_init.clear();
	Variable_init.push_back("File");
	Value_init.push_back(0.);	//	0
	Variable_init.push_back("Path for the file");
	Value_init.push_back(0.);	//	1
	Variable_init.push_back("Background extraction (yes/no sub/function/default)");
	Value_init.push_back(0.);	//	2
	Variable_init.push_back("Calibrate particle");
	Value_init.push_back(0.);	//	3
	Variable_init.push_back("Time sampling (s)");
	Value_init.push_back(0.);	//	4

	cout<<endl;
	// ifstream datafile_param("./Entry/Entry_param_1.txt");
	ifstream datafile_param(filename.c_str());
	if(!datafile_param)
	{
		cout<<"No entry parameters file for this configuration: "<<config_simu<<endl;
		cout<<"No file: "<<filename<<endl;
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
						path_file=buffer;
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
						if(!buffer.compare("proton")||!buffer.compare("Proton")||!buffer.compare("p"))
							Value_init[ind_value]=calib_data[2];
					if(ind_value>3)
					{
						value=(double)atof(buffer.c_str());
						if(value==Value_init[ind_value])
							cout<<Variable_init[ind_value]<<" default value keeped: "<<Value_init[ind_value]<<endl;
						else	
							cout<<Variable_init[ind_value]<<" default value: "<<Value_init[ind_value]<<"; new value: "<<value<<endl;
						Value_init[ind_value]=value;
					}
				}	
				else
					cout<<"Value not define for "<<variable<<". Keeping the default one: "<<Value_init[ind_value]<<". Maybe check the entry file."<<endl;
			}	
			if(datafile_param.eof()) break;
		}
		datafile_param.close();
	}	

	data_faster_file="../DATA/gaussian_0001.fast";
	filename=path_file+data_file; 
	ifstream datafile_faster(filename.c_str());
	if(!datafile_faster)
	{
		cout<<"No data file: "<<filename<<endl;
		cout<<"Keeping the default one: "<<data_faster_file<<". Maybe check the entry file."<<endl;
	}
	else
		data_faster_file=filename;
	datafile_faster.close();

	cout<<endl;
	cout<<"=================================================="<<endl;
	cout<<" Parameters for the initialisation; config: "<<config_simu<<endl;
	cout<<"--------------------------------------------------"<<endl;
	cout<<" File of data: "<<data_faster_file<<endl;
	if(Value_init[2]==0)
		cout<<" No background extraction, default values used"<<endl;
	if(Value_init[2]==1)
		cout<<" Background extraction"<<endl;
	if(Value_init[2]==2)
		cout<<" No background substraction"<<endl;
	if(Value_init[2]==3)
		cout<<" Background substraction using function"<<endl;
	cout<<" Calibration value: "<<Value_init[3]<<"(fC/part.)"<<endl;
	for(ivar=4;ivar<Variable_init.size();ivar++)
		cout<<" "<<Variable_init[ivar]<<": "<<Value_init[ivar]<<endl;
	cout<<"=================================================="<<endl;
	cout<<endl;

	bkgnd_param=Value_init[2];
	SamplingTime=Value_init[4];
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

void Calibrage(char *file,double chargeTot_X,double chargeTot_Y)
{
	faster_file_reader_p reader;
	faster_data_p data;
	electrometer_data electro;
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
	count_quanta--;
	faster_file_reader_close(reader);
	if(quanta>0)
	{
		TCanvas *cQuanta= new TCanvas("Quanta over time","",1000,500);
		cQuanta->SetCanvasSize(1000,500);
		cQuanta->Divide(1,2);

		cQuanta->cd(1);
		TGraph *TG_dQuanta=new TGraph(count_quanta,vect_time_q,vect_dquanta);
		TG_dQuanta->SetMarkerColor(2);
		TG_dQuanta->SetLineColor(2);
		TG_dQuanta->SetLineWidth(1.5);
		TG_dQuanta->SetTitle("Quanta and differential quanta over time");
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
		TG_Quanta->SetTitle("quanta over time");
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
		cout<<"Calib.  X : "<<chargeTot_X<<"(pC)/"<<quanta<<"(part.) = "<<calib_factor_X*1000.<<"(fC/part.)"<<endl;
		cout<<"Calib.  Y : "<<chargeTot_Y<<"(pC)/"<<quanta<<"(part.) = "<<calib_factor_Y*1000.<<"(fC/part.)"<<endl;
		cout<<"Calibrage : "<<chargeTot_pC<<"(pC)/"<<quanta<<"(part.) = "<<calib_factor*1000.<<"(fC/part.)"<<endl;
	}
	else
		cout<<"Pas de données de calibrage, nombre de quanta nul"<<endl;

	free(vect_time_q);
	free(vect_quanta);
	free(vect_dquanta);
}

void SignalArea(char *file,int *tot_area,double signal_time[][2])
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
	int count_Y=0;
	int count_X=0;
	int count_tot=0;
	int count_int=0;
	int count_moy=0;
	int nb_1sec=0;
	int last_i=0;
	int integration=100;
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
	double seuilD=100.;
	double seuilS=5.E-4;
	double bdfX[N_STRIPS];
	double bdfY[N_STRIPS];

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
	for(int i=2;i<count_int-1;i++)
	{
		if(i<2)
		{	
			yl2=0.;
			yl1=0.;
			if(i>=1)
				yl1=vect_charge_int[i-1];
		}
		else
		{
			yl2=vect_charge_int[i-2];
			yl1=vect_charge_int[i-1];
		}
		if(i>count_int-2)
		{	
			yr2=0.;
			yr1=0.;
			if(i<=count_int-1)
				yr1=vect_charge_int[i+1];
		}
		else
		{
			yr2=vect_charge_int[i+2];
			yr1=vect_charge_int[i+1];
		}
		vect_dcharge_int[i]=(8.*(yr1-yl1)-yr2+yl2)/(12.*h);
	}
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

			if(i>=nb_1sec)
				yl2=vect_charge_int[i-nb_1sec];
			else
				yl2=0.;
			if(i<count_int-nb_1sec)
				yr2=vect_charge_int[i+nb_1sec];
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
					last_signal=vect_time_int[i];
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
	if(mvt==des)	//cela signifie que le fichier se termine sur un signal on
	{
		end_on=1;
		cout<<"Attention, le fichier se termine sur du signal On"<<endl;
		signal_time[*tot_area][1]=vect_time_int[count_int-1];
		*tot_area=*tot_area+1;
		last_signal=-1.;
	}
	count_moy--;

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
	cCharge->SetCanvasSize(1000,1000);
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

	cCharge->SaveAs("Picture/Charge.png");

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

	int k=0;
	int signal_bdf=0;

	if(bkgnd_param==1||bkgnd_param==3)
	{
		for(int i=0;i<N_STRIPS;i++)
		{
			bdfX[i]=0.;
			bdfY[i]=0.;
		}
		reader=faster_file_reader_open(file);
		while((data=faster_file_reader_next(reader))!=NULL) 
		{
			label=faster_data_label(data);
			if(label==LabelX||label==LabelY)
			{
				faster_data_load(data,&electro);
				fasterTime=faster_data_clock_sec(data)-t0;
				if((fasterTime<(first_signal-1.)&&beg_on==0)||(fasterTime>(last_signal+1.)&&end_on==0))
				{
					signal_bdf=1;
					for(int j=0;j<N_STRIPS;j++) 
					{
						charge=electrometer_channel_charge_pC(electro,j+1);
						switch(label) 
						{
							case LabelX:
								bdfX[j]+=charge;
								if(j==0)
									count_X++;
							break;
							case LabelY:
								bdfY[j]+=charge;
								if(j==0)
									count_Y++;
							break;
						}
					}
				}
			}
		}
		faster_file_reader_close(reader);
		if(signal_bdf==1)
			for(int i=0;i<N_STRIPS;i++)
			{
				offXY[i][0]=bdfX[i]/count_X;
				offXY[i][1]=bdfY[i]/count_Y;
			}
	}

	if(bkgnd_param==2)
		for(int i=0;i<N_STRIPS;i++)
		{
			offXY[i][0]=0.;
			offXY[i][1]=0.;
		}

	TCanvas *cBDF = new TCanvas("Bruit de fond");
	cBDF->SetCanvasSize(1000,500);
	cBDF->Divide(1,2);
	TH1F *hBdfX = new TH1F("hBdfX","Background for X strips",N_STRIPS,.5,32.5);
	TH1F *hBdfY = new TH1F("hBdfY","Background for Y strips",N_STRIPS,.5,32.5);
	for(int i=0;i<N_STRIPS;i++)
	{
		// bdfX[i]=offset;
		// bdfY[i]=offset;
		bdfX[i]=offXY[i][0];
		bdfY[i]=offXY[i][1];
		hBdfX->SetBinContent(i+1,bdfX[i]);
		hBdfY->SetBinContent(i+1,bdfY[i]);
		// cout<<bdfX[i]<<" "<<bdfY[i]<<endl;
	}
	cBDF->cd(1);
	hBdfX->SetFillColor(2);
	hBdfX->GetXaxis()->SetTickSize(0.01);
	hBdfX->GetXaxis()->SetNdivisions(N_STRIPS);
	hBdfX->GetXaxis()->SetTitle("Strip");
	hBdfX->GetXaxis()->CenterTitle();
	hBdfX->GetXaxis()->SetTickSize(0.01);
	hBdfX->GetXaxis()->SetTitleSize(0.06);
	hBdfX->GetXaxis()->SetLabelSize(0.05);
	hBdfX->GetYaxis()->SetTickSize(0.01);
	hBdfX->GetYaxis()->SetTitle("Charge (pC)");
	hBdfX->GetYaxis()->CenterTitle();
	hBdfX->GetYaxis()->SetTickSize(0.01);
	hBdfX->GetYaxis()->SetTitleSize(0.06);
	hBdfX->GetYaxis()->SetLabelSize(0.05);
	hBdfX->SetBarWidth(0.8);
	hBdfX->SetBarOffset(0.1);
	hBdfX->SetStats(0);
	hBdfX->Draw("b");
	hBdfX->Write("BdfX");
	cBDF->cd(2);
	hBdfY->SetFillColor(4);
	hBdfY->GetXaxis()->SetTickSize(0.01);
	hBdfY->GetXaxis()->SetNdivisions(N_STRIPS);
	hBdfY->GetXaxis()->SetTitle("Strip");
	hBdfY->GetXaxis()->CenterTitle();
	hBdfY->GetXaxis()->SetTickSize(0.01);
	hBdfY->GetXaxis()->SetTitleSize(0.06);
	hBdfY->GetXaxis()->SetLabelSize(0.05);
	hBdfY->GetYaxis()->SetTickSize(0.01);
	hBdfY->GetYaxis()->SetTitle("Charge (pC)");
	hBdfY->GetYaxis()->CenterTitle();
	hBdfY->GetYaxis()->SetTickSize(0.01);
	hBdfY->GetYaxis()->SetTitleSize(0.06);
	hBdfY->GetYaxis()->SetLabelSize(0.05);
	hBdfY->SetBarWidth(0.8);
	hBdfY->SetBarOffset(0.1);
	hBdfY->SetStats(0);
	hBdfY->Draw("b");
	hBdfY->Write("BdfY");
	cBDF->SaveAs("Picture/Bruit_de_fond.png");
	cBDF->Destructor();
}

void SubFittingBackground(int SFBdraw,double min,double max,double *sum_val)
{
	int binl=12;
	int binr=23;
	double scale_value;
	double bdf_SFB;
	double par[6];
	double y_min;
	double y_max;

	TH1F* Profile=new TH1F("Profile","Profile in charge",N_STRIPS,1,33);
	TH1F* ProfExc=new TH1F("ProfExc","Profile in charge without excluded points",N_STRIPS,1,33);
	TGraph *TG_Prof=new TGraph();
	TGraph *TG_Post=new TGraph();
	TF1* SinProfile=new TF1("SinProfile","TMath::Sin(x)",2,32);
	TF1* PolNProfile=new TF1("PolNProfile","pol5",2,32);
	// TF1* PolNProfile=new TF1("PolNProfile",fline,2,32);

	for(int i=0;i<N_STRIPS;i++)
	{
		// scale_value=(2.*(PreSBF[i]-min)/(max-min)-1.);
		scale_value=PreSBF[i];
		Profile->SetBinContent(i+1,PreSBF[i]);
		// ProfExc->SetBinContent(i+1,scale_value);
		if(i<binl||i>binr)
		{
			// PolNProfile->RejectPoint(kTRUE);
			ProfExc->SetBinContent(i+1,scale_value);
			TG_Prof->SetPoint(i,i+1.5,scale_value);
			// ProfExc->SetBinError(i+1,0);
		}
	}
	ProfExc->Fit(PolNProfile,"QR");

	par[0]=PolNProfile->GetParameter(0);
	par[1]=PolNProfile->GetParameter(1);
	par[2]=PolNProfile->GetParameter(2);
	par[3]=PolNProfile->GetParameter(3);
	par[4]=PolNProfile->GetParameter(4);
	par[5]=PolNProfile->GetParameter(5);
	
	for(int i=0;i<N_STRIPS;i++)
	{
		double x=i+1.5;
		bdf_SFB=par[0]+par[1]*x+par[2]*pow(x,2)+par[3]*pow(x,3)+par[4]*pow(x,4)+par[5]*pow(x,5);
		PostSBF[i]=PreSBF[i]-bdf_SFB;
		if(SFBdraw==1)
			TG_Post->SetPoint(i,x,PostSBF[i]);
	}

	if(SFBdraw==1)
	{
		y_min=Profile->GetMinimum()*1.1;
		y_max=Profile->GetMaximum()*1.1;

		TCanvas *cSFB = new TCanvas("Sub fitting background");
		cSFB->SetCanvasSize(1000,750);
		cSFB->Divide(1,3);
		cSFB->cd(1);
		Profile->SetLineColor(2);
		Profile->GetXaxis()->SetTickSize(0.01);
		Profile->GetXaxis()->SetNdivisions(N_STRIPS);
		Profile->GetXaxis()->SetTitle("Strip");
		Profile->GetXaxis()->CenterTitle();
		Profile->GetXaxis()->SetTickSize(0.01);
		Profile->GetXaxis()->SetTitleSize(0.06);
		Profile->GetXaxis()->SetLabelSize(0.05);
		Profile->GetYaxis()->SetTickSize(0.01);
		Profile->GetYaxis()->SetTitle("Charge (pC)");
		Profile->GetYaxis()->CenterTitle();
		Profile->GetYaxis()->SetTickSize(0.01);
		Profile->GetYaxis()->SetTitleSize(0.06);
		Profile->GetYaxis()->SetLabelSize(0.05);
		Profile->GetXaxis()->SetRangeUser(1,N_STRIPS+1);
		Profile->GetYaxis()->SetRangeUser(y_min,y_max);
		Profile->SetBarWidth(0.8);
		Profile->SetBarOffset(0.1);
		Profile->SetStats(0);
		Profile->Draw();
		
		cSFB->cd(2);

		TG_Prof->SetLineColor(2);
		TG_Prof->SetMarkerColor(2);
		TG_Prof->SetMarkerStyle(4);
		TG_Prof->SetMarkerSize(1.2);
		TG_Prof->GetXaxis()->SetTickSize(0.01);
		TG_Prof->GetXaxis()->SetNdivisions(N_STRIPS+1);
		TG_Prof->GetXaxis()->SetTitle("Strip");
		TG_Prof->GetXaxis()->CenterTitle();
		TG_Prof->GetXaxis()->SetTickSize(0.01);
		TG_Prof->GetXaxis()->SetTitleSize(0.06);
		TG_Prof->GetXaxis()->SetLabelSize(0.05);
		TG_Prof->GetYaxis()->SetTickSize(0.01);
		TG_Prof->GetYaxis()->SetTitle("Charge (pC)");
		TG_Prof->GetYaxis()->CenterTitle();
		TG_Prof->GetYaxis()->SetTickSize(0.01);
		TG_Prof->GetYaxis()->SetTitleSize(0.06);
		TG_Prof->GetYaxis()->SetLabelSize(0.05);
		TG_Prof->GetXaxis()->SetRangeUser(1,N_STRIPS+1);
		TG_Prof->GetYaxis()->SetRangeUser(y_min,y_max);
		TG_Prof->Draw("AP");
		TG_Prof->Write("TG_Prof");

		PolNProfile->SetLineColor(4);
		PolNProfile->Draw("same L");
		// Profile->Fit(SinProfile,"QR");
		// SinProfile->SetLineColor(4);
		// SinProfile->Draw("same L");

		cSFB->cd(3);
		TG_Post->SetLineColor(2);
		TG_Post->SetMarkerColor(2);
		TG_Post->SetMarkerStyle(4);
		TG_Post->SetMarkerSize(1.2);
		TG_Post->GetXaxis()->SetTickSize(0.01);
		TG_Post->GetXaxis()->SetNdivisions(N_STRIPS+1);
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
		TG_Post->GetXaxis()->SetRangeUser(1,N_STRIPS+1);
		// TG_Post->GetYaxis()->SetRangeUser(y_min,y_max);
		TG_Post->Draw("AP");
		PolNProfile->SetLineColor(4);
		// PolNProfile->Draw("same L");
		// TG_Post->Write("TG_Post");

		cSFB->SaveAs("Picture/SBF.png");
		cSFB->Destructor();
	}

	Profile->Delete();
	ProfExc->Delete();
	SinProfile->Delete();
	PolNProfile->Delete();
	TG_Prof->Delete();
	TG_Post->Delete();
}

int main(int argc, char** argv)
{
	cout.precision(8);
	system("rm -f Picture/*.png");
	// cout.setf(std::ios_base::fixed | std::ios_base::scientific, std::ios_base::floatfield);
	set_plot_style();
	// char *filename;
	char *filename=(char*)malloc(80);
	
	faster_file_reader_p reader;
	faster_data_p data;
	electrometer_data electro;

	PreSBF.resize(N_STRIPS);
	PostSBF.resize(N_STRIPS);

	TFile* rootfile=new TFile("Output/PostAnalysis.root","RECREATE");
	TString name_area;
	unsigned short label;
	int data_calib=0;
	int count_tot=0;
	int npeak;
	int i_cx;
	int i_cy;
	int area_end=0;
	int isLabelX=0;
	int isLabelY=0;
	int isBoth=0;
	int nIntegration=0;
	int nbdfX=0;
	int nbdfY=0;
	int signal=0;
	int tot_area=0;
	int count_area=0;
	int bin_up=1000;
	int voxel;
	double t0;
	double t1;
	double fasterTime=0.;
	double integralTime=0.;
	double sum_x=0.;
	double sum_y=0.;
	double val;
	double sum_val;
	double charge;
	double ampl;
	double mean_x;
	double mean_y;
	double rms_x;
	double rms_y;
	double min;
	double max;
	double chargeInt;
	double chargeTot_pC=0.;
	double chargeTot_signal_X=0.;
	double chargeTot_signal_Y=0.;
	double chargeOverT=0.;
	double Threshold=0.;//500.*(fC_per_particle/1000.); /// signal equivalent to 500 particle passing through
	double offset=25.;
	double factor_bdf=.001;
	double bdfX[N_STRIPS];
	double bdfY[N_STRIPS];
	float* PeakX;
	TArrayD* ChX=new TArrayD(N_STRIPS);
	TArrayD* ChY=new TArrayD(N_STRIPS);
	TArrayD* SamplX=new TArrayD(N_STRIPS);
	TArrayD* SamplY=new TArrayD(N_STRIPS);
	TArrayD* AreaX=new TArrayD(N_STRIPS);
	TArrayD* AreaY=new TArrayD(N_STRIPS);
	double Map[N_STRIPS][N_STRIPS];
	double ProfilX[N_STRIPS];
	double ProfilY[N_STRIPS];
	double vect_time_spl[MAX_SMPL];
	double vect_charge_t_spl[MAX_SMPL];
	double vect_ampl_spl[MAX_SMPL];
	double vect_mean_x[MAX_SMPL];
	double vect_mean_y[MAX_SMPL];
	double vect_rms_x[MAX_SMPL];
	double vect_rms_y[MAX_SMPL];
	double vect_ampl_area[MAX_SMPL];
	double vect_charge_t_area[MAX_SMPL];
	double vect_mean_x_area[MAX_SMPL];
	double vect_mean_y_area[MAX_SMPL];
	double vect_rms_x_area[MAX_SMPL];
	double vect_rms_y_area[MAX_SMPL];
	double signal_time[MAX_SMPL][2];

	double* vect_time_tot=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* vect_charge_clean=(double*)malloc(MAX_INTEGR*sizeof(double));
	double* matrix_fluence=(double*)malloc(bin_up*bin_up*sizeof(double));

	int config_simu=1;
	if(argc>1)
		config_simu=(int)atof(argv[1]);

	EntryParameters(config_simu);
	strcpy(filename,data_faster_file.c_str());
	
	char command_line[80];
	sprintf(command_line,"faster_disfast %s -I",filename);
	system(command_line);

	reader=faster_file_reader_open(filename);
	
	if(reader==NULL) 
	{
		printf ("Error opening file %s\n", filename);
		faster_file_reader_close(reader);
		return 0;
	}
	else
		printf("Fichier de données bien ouvert\n");

	for(int j=0;j<N_STRIPS;j++)
	{
		ProfilX[j]=0;
		ProfilY[j]=0;
		for(int k=0;k<N_STRIPS;k++)
			Map[j][k]=0.;
	}
	ChX->Reset();
	ChY->Reset();
	SamplX->Reset();
	SamplY->Reset();
	AreaX->Reset();
	AreaY->Reset();

	SignalArea(filename,&tot_area,signal_time);
	if(tot_area>MAX_SMPL)
	{
		cout<<"Nombre d'irradiation supérieur à la limite"<<endl;
		tot_area=MAX_SMPL;
	}
	cout<<tot_area<<" période(s) d'irradiation"<<endl;
	for(int i=0;i<tot_area;i++)
		cout<<"Irradiation "<<i+1<<"; début : "<<signal_time[i][0]<<"; fin : "<<signal_time[i][1]<<endl;

	nbSummedSample=0;
	count_area=0;
	chargeInt=0.;
	t0=-1; 

	for(int j=0;j<N_STRIPS;j++)
	{
		bdfX[j]=offXY[j][0];
		bdfY[j]=offXY[j][1];
	}

	int i_tmp=0;
	// do
	// {
	// 	data=faster_file_reader_next(reader);
	// 	i_tmp++;
	// }while(i_tmp<100);
	// while((data=faster_file_reader_next(reader))!=NULL&&i_tmp<2) 
	while((data=faster_file_reader_next(reader))!=NULL) 
	{
		i_tmp++;
		label=faster_data_label(data);
		if(label==LabelCount)
			data_calib=1;
		if(label==LabelX||label==LabelY)
		{
			if(t0==-1)
			{
				t0=faster_data_clock_sec(data);
				t1=0.;
			}
			faster_data_load(data,&electro);

			fasterTime=faster_data_clock_sec(data)-t0;
			integralTime=fasterTime-t1;
			if(fasterTime>signal_time[count_area][0]&&fasterTime<signal_time[count_area][1])
				area_end=0;
			if(area_end==0&&fasterTime>signal_time[count_area][1])
				area_end=1;

			if(bkgnd_param==3)
			{
				PreSBF.clear();
				PostSBF.clear();
				min=DBL_MAX;
				max=0.;
				for(int j=FIRST_ELEC;j<LAST_ELEC;j++) 
				{
					charge=electrometer_channel_charge_pC(electro,j+1);
					switch(label) 
					{
						case LabelX:
							val=charge-bdfX[j];
							if(val>max)	max=val;
							if(val<min)	min=val;
							PreSBF[j]=val;
							isLabelX=1;
						break;
						case LabelY:
							val=charge-bdfY[j];
							if(val>max)	max=val;
							if(val<min)	min=val;
							PreSBF[j]=val;
							isLabelY=1;
						break;
					}
				}
				sum_val=0.;
				SubFittingBackground(0,min,max,&sum_val);
				switch(label)
				{
					case LabelX:
						if(area_end==0)
							chargeTot_signal_X+=sum_val;
						for(int j=FIRST_ELEC;j<LAST_ELEC;j++)
							ChX->SetAt(PostSBF[j],j); 
						isLabelX=1;
					break;
					case LabelY:
						if(area_end==0)
							chargeTot_signal_Y+=sum_val;
						for(int j=FIRST_ELEC;j<LAST_ELEC;j++)
							ChY->SetAt(PostSBF[j],j);
						isLabelY=1;
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
							val=charge-bdfX[j];
							if(area_end==0)
								chargeTot_signal_X+=val;
							// val=TMath::Max(charge-bdfX[j],0.);
							if(val<bdfX[j]*factor_bdf)
								val=0.; 
							// ChX->SetAt(val*corrXY[j][0],j);
							ChX->SetAt(val,j);
							isLabelX=1;
						break;
						case LabelY:
							val=charge-bdfY[j];
							if(area_end==0)
								chargeTot_signal_Y=val;
							// val=TMath::Max(charge-bdfY[j],0.);
							if(val<bdfY[j]*factor_bdf)
								val=0.; 
							// ChY->SetAt(val*corrXY[j][1],j);
							ChY->SetAt(val,j);
							isLabelY=1;
						break;
					}
				}
			}
			if(isLabelX==1&&isLabelY==1)
			{
				for(int j=0;j<N_STRIPS;j++)
				{
					val=ChX->GetAt(j);
					ProfilX[j]+=val;
					val=ChY->GetAt(j);
					ProfilY[j]+=val;

					val=ChX->GetAt(j)+SamplX->GetAt(j);
					SamplX->SetAt(val,j);
					val=ChY->GetAt(j)+SamplY->GetAt(j);
					SamplY->SetAt(val,j);
					
					if(area_end==0)
					{
						val=ChX->GetAt(j)+AreaX->GetAt(j);
						AreaX->SetAt(val,j);
						val=ChY->GetAt(j)+AreaY->GetAt(j);
						AreaY->SetAt(val,j);
					}
				}
				sum_x=ChX->GetSum();
				sum_y=ChY->GetSum();
				// sum_x=ChX->GetSum()-ChX->GetAt(0)-ChX->GetAt(N_STRIPS-1);
				// sum_y=ChY->GetSum()-ChY->GetAt(0)-ChY->GetAt(N_STRIPS-1);
				vect_time_tot[count_tot]=fasterTime;
				vect_charge_clean[count_tot]=(sum_x+sum_y)/2.;
				chargeTot_pC+=(sum_x+sum_y)/2.;
				// if(area_end==0)
				// 	chargeTot_signal+=(sum_x+sum_y)/2.;
					// chargeTot_signal+=sum_y;
				count_tot++;
				if(sum_x>Threshold&&sum_y>Threshold)
				{
					for(int j=0;j<N_STRIPS;j++)
					{
						for(int k=0;k<N_STRIPS;k++)
						{
							val=ChX->GetAt(j)*ChY->GetAt(k)/(sum_x*sum_y)*(sum_x+sum_y)/2.;
							Map[j][k]+=val;
						}
					}
					chargeOverT+=(sum_x+sum_y)/2.;
				}
				// else
				// {
				// 	for(int j=0;j<N_STRIPS;j++)
				// 	{
				// 		ProfilX_left[j]+=ChX->GetAt(j);
				// 		ProfilY_left[j]+=ChY->GetAt(j);
				// 	}
				// }
				ChX->Reset();
				ChY->Reset();
				isLabelX=0;
				isLabelY=0;
			}
			// cout<<fasterTime<<" "<<" "<<t1<<" "<<integralTime<<" "<<SamplingTime<<endl;
			if(area_end==1&&tot_area>0)
			{
				name_area="TH2_Area_";
				name_area+=(count_area+1);
				TH2F* TH2_Area=new TH2F(name_area,"Fluency map (particle/cm2)",N_STRIPS,1,33,N_STRIPS,1,33);
				TH1F* Profil_x_area=new TH1F("Profil_x_area","X profile in number of particles",N_STRIPS,1,33);
				TH1F* Profil_y_area=new TH1F("Profil_y_area","Y profile in number of particles",N_STRIPS,1,33);
				TF1* GaussProfilX=new TF1("GaussProfilX","[0]*TMath::Gaus(x,[1],[2])",2,32);
				TF1* GaussProfilY=new TF1("GaussProfilY","[0]*TMath::Gaus(x,[1],[2])",2,32);
				GaussProfilX->SetNpx(1000);
				GaussProfilY->SetNpx(1000);

				mean_x=0.;
				mean_y=0.;
				rms_x=0.;
				rms_y=0.;
				sum_x=AreaX->GetSum();
				sum_y=AreaY->GetSum();
				for(int i=0;i<N_STRIPS;i++)
				{
					Profil_x_area->SetBinContent(i+1,AreaX->GetAt(i));
					Profil_y_area->SetBinContent(i+1,AreaY->GetAt(i));
					// cout<<i<<" "<<AreaX->GetAt(i)<<endl;
					mean_x+=i*AreaX->GetAt(i);
					rms_x+=i*i*AreaX->GetAt(i);
					mean_y+=i*AreaY->GetAt(i);
					rms_y+=i*i*AreaY->GetAt(i);
					for(int k=0;k<N_STRIPS;k++)
					{
						if(sum_x>0.&&sum_y>0.)
							TH2_Area->SetBinContent(i+1,k+1,AreaX->GetAt(i)*AreaY->GetAt(k)/(sum_x*sum_y)*(sum_x+sum_y)/2.);
						else	
							TH2_Area->SetBinContent(i+1,k+1,0.);
					}
				}
				if(sum_x>0.)
				{
					// cout<<mean_x<<" "<<sum_x<<endl;
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
				vect_ampl_area[count_area]=TH2_Area->GetBinContent(TH2_Area->GetMaximumBin());

				TCanvas *cArea= new TCanvas("Dose map");
				cArea->SetCanvasSize(1000,1000);
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

				cArea->cd(2);
				// TText* Text=new TText();
				// TPaveText *pt = new TPaveText(.05,.1,.95,.8);
				// pt->AddText("A TPaveText can contain severals line of text.");
				// pt->AddText("They are added to the pave using the AddText method.");
				// pt->AddLine(.0,.5,1.,.5);
				// pt->AddText("Even complex TLatex formulas can be added:");
				// TText *t1 = pt->AddText("F(t) = #sum_{i=-#infty}^{#infty}A(i)cos#[]{#frac{i}{t+i}}");
				// t1->SetTextColor(kBlue);
				// pt->Draw();
				// TText *t2 = pt->GetLineWith("Even");
				// t2->SetTextColor(kOrange+1);

				cArea->cd(3);
		
				TSpectrum* spect_x_area=new TSpectrum();
				npeak=spect_x_area->Search(Profil_x_area,2,"",0.10);
				if(npeak==1)
				{
					PeakX=spect_x_area->GetPositionX();
					mean_x=PeakX[0];
				}
				else
					mean_x=Profil_x_area->GetMaximumBin();
				i_cx=ceil(mean_x);
				ampl=Profil_x_area->GetBinContent(Profil_x_area->GetBin(i_cx));
				// ampl=Profil_x_area->GetBinContent(Profil_x_area->GetMaximumBin());
				GaussProfilX->SetParameters(ampl,mean_x,rms_x);
				// cout<<ampl<<" "<<mean_x<<" "<<rms_x<<endl;

				i_cx=TMath::Max(2,Profil_x_area->GetMaximumBin()-5);
				i_cy=TMath::Min(Profil_x_area->GetNbinsX()-2,Profil_x_area->GetMaximumBin()+5);

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
				Profil_x_area->Fit(GaussProfilX,"Q","",i_cx,i_cy);
				Profil_x_area->Fit(GaussProfilX,"QR");

				vect_mean_x_area[count_area]=GaussProfilX->GetParameter(1);
				vect_rms_x_area[count_area]=GaussProfilX->GetParameter(2);
				GaussProfilX->Delete();
				spect_x_area->Delete();

				cArea->cd(4);

				TSpectrum* spect_y_area=new TSpectrum();
				npeak=spect_y_area->Search(Profil_y_area,2,"",0.10);
				if(npeak==1)
				{
					PeakX=spect_y_area->GetPositionX();
					mean_y=PeakX[0];
				}
				else
					mean_y=Profil_y_area->GetMaximumBin();
				i_cy=ceil(mean_y);
				ampl=Profil_y_area->GetBinContent(Profil_y_area->GetBin(i_cy));
				// ampl=Profil_y_area->GetBinContent(Profil_y_area->GetMaximumBin());
				GaussProfilY->SetParameters(ampl,mean_y,rms_y);

				i_cx=TMath::Max(1,Profil_y_area->GetMaximumBin()-6);
				i_cy=TMath::Min(Profil_y_area->GetNbinsX()-1,Profil_y_area->GetMaximumBin()+6);

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
				Profil_y_area->Fit(GaussProfilY,"Q","",i_cx,i_cy);
				Profil_y_area->Fit(GaussProfilY,"QR");
	
				vect_mean_y_area[count_area]=GaussProfilY->GetParameter(1);
				vect_rms_y_area[count_area]=GaussProfilY->GetParameter(2);
				GaussProfilY->Delete();
				spect_y_area->Delete();

				name_area="Picture/Area_";
				name_area+=(count_area+1);
				name_area+=".png";
				cArea->SaveAs(name_area);
				
				rootfile->Write();
				TH2_Area->Delete();
				Profil_x_area->Delete();
				Profil_y_area->Delete();
				cArea->Destructor();

				AreaX->Reset();
				AreaY->Reset();
				area_end=2;
				count_area++;
			}

			// if(integralTime>DBL_MAX)
			if(integralTime>SamplingTime)
			{
				if(nbSummedSample>=MAX_SMPL)
					cout<<"Maximum number of samples reached"<<endl;
				else
				{
					TH1F* Profil_x_smpl=new TH1F("Profil_x_area","X profile in number of particles",N_STRIPS,1,33);
					TH1F* Profil_y_smpl=new TH1F("Profil_y_area","Y profile in number of particles",N_STRIPS,1,33);
					TF1* GaussProfilX=new TF1("GaussProfilX","[0]*TMath::Gaus(x,[1],[2])",2,32);
					TF1* GaussProfilY=new TF1("GaussProfilY","[0]*TMath::Gaus(x,[1],[2])",2,32);
					GaussProfilX->SetNpx(1000);
					GaussProfilY->SetNpx(1000);
					
					mean_x=0.;
					mean_y=0.;
					rms_x=0.;
					rms_y=0.;
					for(int i=0;i<N_STRIPS;i++)
					{
						Profil_x_smpl->SetBinContent(i+1,SamplX->GetAt(i));
						Profil_y_smpl->SetBinContent(i+1,SamplY->GetAt(i));
						mean_x+=i*SamplX->GetAt(i);
						rms_x+=i*i*SamplX->GetAt(i);
						mean_y+=i*SamplY->GetAt(i);
						rms_y+=i*i*SamplY->GetAt(i);
					}
					sum_x=SamplX->GetSum();
					sum_y=SamplY->GetSum();
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
						mean_x=0.;
						rms_x=0.;
					}
					vect_time_spl[nbSummedSample]=fasterTime;
					vect_charge_t_spl[nbSummedSample]=(sum_x+sum_y)/2.;

					TSpectrum* spect_x_smpl=new TSpectrum();
					npeak=spect_x_smpl->Search(Profil_x_smpl,2,"",0.10);
					if(npeak==1)
					{
						PeakX=spect_x_smpl->GetPositionX();
						mean_x=PeakX[0];
					}
					vect_mean_x[nbSummedSample]=mean_x;
					vect_rms_x[nbSummedSample]=rms_x;
					// ampl=Profil_x_smpl->GetBinContent(Profil_x_smpl->GetMaximumBin());
					// GaussProfilX->SetParameters(ampl,mean_x,rms_x);
					// i_cx=TMath::Max(2,Profil_x_smpl->GetMaximumBin()-5);
					// i_cy=TMath::Min(Profil_x_smpl->GetNbinsX()-2,Profil_x_smpl->GetMaximumBin()+5);
					// Profil_x_smpl->Fit(GaussProfilX,"Q","",i_cx,i_cy);
					// Profil_x_smpl->Fit(GaussProfilX,"QR");
					// vect_mean_x[nbSummedSample]=GaussProfilX->GetParameter(1);
					// vect_rms_x[nbSummedSample]=GaussProfilX->GetParameter(2);
					GaussProfilX->Delete();
					spect_x_smpl->Delete();

					TSpectrum* spect_y_smpl=new TSpectrum();
					npeak=spect_y_smpl->Search(Profil_y_smpl,2,"",0.10);
					if(npeak==1)
					{
						PeakX=spect_y_smpl->GetPositionX();
						mean_y=PeakX[0];
					}
					vect_mean_y[nbSummedSample]=mean_y;
					vect_rms_y[nbSummedSample]=rms_y;
					// ampl=Profil_y_smpl->GetBinContent(Profil_y_smpl->GetMaximumBin());
					// GaussProfilY->SetParameters(ampl,mean_y,rms_y);
					// i_cx=TMath::Max(2,Profil_y_smpl->GetMaximumBin()-5);
					// i_cy=TMath::Min(Profil_y_smpl->GetNbinsX()-2,Profil_x_smpl->GetMaximumBin()+5);
					// Profil_y_smpl->Fit(GaussProfilY,"Q","",i_cx,i_cy);
					// Profil_y_smpl->Fit(GaussProfilY,"QR");
					// vect_mean_y[nbSummedSample]=GaussProfilY->GetParameter(1);
					// vect_rms_y[nbSummedSample]=GaussProfilY->GetParameter(2);
					GaussProfilY->Delete();
					spect_y_smpl->Delete();

					SamplX->Reset();
					SamplY->Reset();
					Profil_x_smpl->Delete();
					Profil_y_smpl->Delete();
					nbSummedSample++;
					// vect_mean_x[nbSummedSample]=mean_x;
					// vect_rms_x[nbSummedSample]=rms_x;
					// vect_mean_y[nbSummedSample]=mean_y;
					// vect_rms_y[nbSummedSample]=rms_y;
				}
				t1=fasterTime;
			}
		}
	}
	count_tot--;

	cout<<"Total of samples of "<<SamplingTime<<"(s) : "<<nbSummedSample<<endl;
	for(int i=0;i<tot_area;i++)
		cout<<"Signal "<<i+1<<" Mean X : "<<vect_mean_x_area[i]<<"; Mean Y : "<<vect_mean_y_area[i]
		<<"; RMS X : "<<vect_rms_x_area[i]<<"; RMS Y : "<<vect_rms_y_area[i]<<"; Amplitude (%) : "<<vect_charge_t_area[i]/chargeTot_pC*100.<<endl;

	cout<<"Charge totale "<<chargeTot_pC<<" charge partielle "<<chargeOverT<<" charge signal X "<<chargeTot_signal_X<<" charge signal Y "<<chargeTot_signal_Y<<endl;
	// cout<<nIntegration<<endl;

	double FullTablePosition[N_STRIPS]; 											// table of possible position of the beam
	for(int i=0;i<N_STRIPS;i++)
		FullTablePosition[i]=-46.5+i*3; //+118.5

	// TH2F* TH2_Map=new TH2F("TH2_Map","Fluency map (particle/cm2)",N_STRIPS-2,FullTablePosition[1]-1.5,FullTablePosition[N_STRIPS-2]+1.5,N_STRIPS-2,FullTablePosition[1]-1.5,FullTablePosition[N_STRIPS-2]+1.5);
	// TH2F* TH2_Map=new TH2F("TH2_Map","Fluency map (particle/cm2)",N_STRIPS,FullTablePosition[0]-1.5,FullTablePosition[N_STRIPS-1]+1.5,N_STRIPS,FullTablePosition[0]-1.5,FullTablePosition[N_STRIPS-1]+1.5);
	TH2F* TH2_Map=new TH2F("TH2_Map","Fluency map (particle/cm2)",N_STRIPS,1,33,N_STRIPS,1,33);
	// TH1F* Profil_x=new TH1F("ProfX","X profile in number of particles",N_STRIPS,FullTablePosition[0]-1.5,FullTablePosition[N_STRIPS-1]+1.5);
	// TH1F* Profil_y=new TH1F("ProfY","Y profile in number of particles", N_STRIPS,FullTablePosition[0]-1.5,FullTablePosition[N_STRIPS-1]+1.5);
	TH1F* Profil_x=new TH1F("ProfX","X profile in number of particles",N_STRIPS,1,33);
	TH1F* Profil_y=new TH1F("ProfY","Y profile in number of particles",N_STRIPS,1,33);

	TCanvas *cCCharge= new TCanvas("Charge over time");
	cCCharge->SetCanvasSize(1000,250);
	cCCharge->Divide(1,1);

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

	cCCharge->SaveAs("Picture/Charge_clean.png");

	TG_CCharge->Delete();
	cCCharge->Destructor();
	free(vect_time_tot);
	free(vect_charge_clean);

	for(int j=0;j<N_STRIPS;j++)	
	{
		Profil_x->SetBinContent(j+1,ProfilX[j]);
		Profil_y->SetBinContent(j+1,ProfilY[j]);
		for(int k=0;k<N_STRIPS;k++)
			TH2_Map->SetBinContent(j+1,k+1,Map[j][k]);
	}

	ampl=TH2_Map->GetBinContent(TH2_Map->GetMaximumBin());
	mean_x=TH2_Map->GetMean(1);
	mean_y=TH2_Map->GetMean(2);
	rms_x=TH2_Map->GetRMS(1);
	rms_y=TH2_Map->GetRMS(2);
	// cout<<TH2_Map->GetBinContent(TH2_Map->GetMaximumBin())<<endl;
	// cout<<TH2_Map->GetMean(1)<<" "<<Profil_x->GetMean(1)<<endl;
	// cout<<TH2_Map->GetMean(2)<<" "<<Profil_y->GetMean(1)<<endl;
	// cout<<TH2_Map->GetRMS(1)<<" "<<Profil_x->GetRMS(1)<<endl;
	// cout<<TH2_Map->GetRMS(2)<<" "<<Profil_y->GetRMS(1)<<endl;

	TCanvas *cMap= new TCanvas("Dose map");
	cMap->SetCanvasSize(1000,1000);
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
	TH2F* TH2_Map_up=new TH2F("TH2_Map_up","Enhanced fluency map (particle/cm2)",bin_up,1,33,bin_up,1,33);
	TH2F* TH2_Map_tmp=new TH2F();

	f2DGauss->SetNpx(bin_up);
	f2DGauss->SetNpy(bin_up);
	for(int i=0;i<tot_area;i++)
	{
		ampl=vect_ampl_area[i];
		mean_x=vect_mean_x_area[i];
		mean_y=vect_mean_y_area[i];
		rms_x=vect_rms_x_area[i];
		rms_y=vect_rms_y_area[i];
		f2DGauss->SetParameters(ampl,mean_x,rms_x,mean_y,rms_y);
		TH2_Map_tmp=((TH2F*)f2DGauss->GetHistogram());
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
	Profil_y->Draw();
	cMap->SaveAs("Picture/Image.png");
	
	rootfile->Write();
	TH2_Map->Delete();
	TH2_Map_up->Delete();
	// TH2_Map_tmp->Delete();
	f2DGauss->Delete();
	Profil_x->Delete();
	Profil_y->Delete();
	cMap->Destructor();

	TCanvas *cSamp= new TCanvas("Sampling dose");
	cSamp->SetCanvasSize(1000,1000);
	cSamp->Divide(1,4);

	cSamp->cd(1);
	TGraph *TG_Mean_x=new TGraph(nbSummedSample,vect_time_spl,vect_mean_x);
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
	TGraph *TG_Mean_y=new TGraph(nbSummedSample,vect_time_spl,vect_mean_y);
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
	TGraph *TG_Rms_x=new TGraph(nbSummedSample,vect_time_spl,vect_rms_x);
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
	TGraph *TG_Rms_y=new TGraph(nbSummedSample,vect_time_spl,vect_rms_y);
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

	rootfile->Write();
	cSamp->SaveAs("Picture/Sampling.png");
	cSamp->Destructor();
	TG_Mean_x->Delete();
	TG_Mean_y->Delete();
	TG_Rms_x->Delete();
	TG_Rms_y->Delete();

	if(data_calib==1)
		// Calibrage(filename,chargeTot_pC);
		Calibrage(filename,chargeTot_signal_X,chargeTot_signal_Y);
	else
		cout<<"Pas de données de calibrage"<<endl;

	printf("Images générées\n");
	faster_file_reader_close(reader);
	free(filename);
	rootfile->Close();
}
