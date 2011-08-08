#include "Simulator.h"

#include "external_parameters.h"
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include "SimulatorParameter.h"

/***analysis***/
#include "Record.h"
/************/

unsigned int femtoNum=0;
unsigned int macroNum=0;
unsigned int outNum = 0;

unsigned int totalfemtoNum=0;
unsigned int totalmacroNum=0;

double ListMissRateTotal=0;
double outage 		= 0;
double MeanNumInList=0;
long int countList = 0;
double angleMSE = 0;
double OptimumTargetmissCount = 0;
unsigned  int macroUserNum_a = 0;
unsigned  int femtoUserNum_a =0;
double speedDetectionErrorTotal = 0;
double speedDetectionTotalCount = 0;
double speedTotal = 0;
double speedCount = 0;
double NumOutdoorUser=0;
double TotalInOutdoorUser=0;
Record ReSpeed("SPEED");
Record ReSpeedDeteError("SPEED_DETECTION_ERROR");
Record ReEstAngle("EST_ANGLE");
Record ReRealAngle("REAL_ANGLE");
Record ReHandoverTimes("Handover_Times");
Record ReListLength("List_length");
Record ReMissRate("MissRate");
Record ReAlpha("Alpha");
//Record ReListMeanDistanceBtwUserFetmo("DistanceBtwUser_Femto");


/*GenMultiUserParam(int SimuTotalTime,int NumUser,double TickTime,double ScanPeriod,int P)*/

Simulator::Simulator():USERLIST(),P1(3600,300,0.5,2.0,0),MultiUser( &USERLIST, P1)
{
	/*initialization*/

/*
	P1.numuser = 100;
	P1.permutation = 0;
	P1.simulationtotaltime = 3600;
	P1.ticktime = 0.5;
	P1.scanperiod = 2.0;
*/

	outage 		= 0;
	MeanNumInList=0;
	countList = 0;
	angleMSE = 0;
	OptimumTargetmissCount = 0;
	macroUserNum_a = 0;
	femtoUserNum_a =0;


	printf("5555555555555\n");
	//MultiUser = new GenMultiUser( &USERLIST, P1);
	printf("6666666666666666\n");
	vector<WayPoint>* UserPathList_ = MultiUser.getUserPathlist();
	printf("4444444444444\n");

    //ctor
}

Simulator::~Simulator()
{
	printf("3333333333333\n");
	//delete MultiUser;
    //dtor
}


void
Simulator::Start()
{

	for(int i=0; i<=(int)(P1.simulationtotaltime/P1.ticktime); i++){
		//system("PAUSE");
		//_sleep(1000);
		system("cls");
		printf("Time:%f\n",i*(P1.ticktime));


		MultiUser.UpdateAllPosition(USERLIST.pFirst,0);

		MSNODE** arrayMSNODE = new MSNODE*[P1.numuser];
		MSNODE* currMS = USERLIST.pFirst;

		for( int j=0; currMS != NULL; j++){
			arrayMSNODE[j] = currMS;
			currMS = currMS->pNext;
		}

		femtoNum=0;
		macroNum=0;
		outNum = 0;



		//#pragma omp parallel for
		for(int j=0; j<P1.numuser; j++){
			//printf("User%d:\n",j);
#ifdef LEVEL_FP_
			MSwithLevelFP  MS( &USERLIST, &BSOxy_MAI, &fsdata, P1.permutation);
#endif
#ifdef VECTOR_FP_
			MSwithVectorFP  MS( &USERLIST, &BSOxy_MAI, &fsdata, P1.permutation);
#endif
#ifdef FULL_SCAN_
			MobileStationBase MS( &USERLIST, &BSOxy_MAI, &fsdata, P1.permutation);
#endif
			MS.UpdateMSINFO(arrayMSNODE[j], i, P1.ticktime);

			//delete  MS[i];

			//if(i==(P1.simulationtotaltime-0.5)){printf("position(%f,%f)\n", arrayMSNODE[j]->msdata.position.x   , arrayMSNODE[j]->msdata.position.y);}




		}
		printf("Macro:%d\tFemto:%d\tBlocking:%d\n",macroNum,femtoNum,outNum);
		totalfemtoNum+=femtoNum;
		totalmacroNum+=macroNum;
		delete [] arrayMSNODE;

	}

	//system("cls");


	extern vector<double> MeanNumInList_v;
	extern vector<double> MissRate;
	extern vector<double> Outage;
	double total=(double)(P1.numuser*(P1.simulationtotaltime/P1.ticktime));

	printf("femtoNum:%f\tmacroNum:%f\n",totalfemtoNum/total,totalmacroNum/total);
	printf("Blocking Rate:%f %\n",outage/total);
	Outage.push_back(  outage/(double)(P1.numuser*(P1.simulationtotaltime/P1.ticktime))  );
	printf("OptimumTargetSelectionMissRate:%f\n",OptimumTargetmissCount/countList);
	MissRate.push_back(OptimumTargetmissCount/countList);
	printf("MeanNumInList: %f\n",MeanNumInList/countList);
	MeanNumInList_v.push_back(MeanNumInList/countList);
	printf("ListMissRate:%f\n",(ListMissRateTotal/countList));
	printf("MobilityStateDetectionErrorRate:%f\t%f\n",(speedDetectionErrorTotal/speedDetectionTotalCount), ReSpeedDeteError.Average());
	printf("##Difference## Mean :%f m/s\tVariance:%f\n", ReSpeed.Average(), ReSpeed.Variance());
	printf("Handover Times:%f",ReHandoverTimes.Total());


    std::ofstream result_file;
    char tmp[30];
    std::string filename("Sim_Result_");
    sprintf(tmp,"R=%.1f_",CELL_RADIOUS);
    filename+=tmp;
    sprintf(tmp,"D=%.1f_",HOUSE_DENSITY);
    filename+=tmp;
#ifdef FULL_SCAN_
	filename+="FullScan";
#endif
#ifdef VECTOR_FP_
	sprintf(tmp,"%d_",VFP_LENGTH);
	filename+="VectorFP";
	filename+=tmp;
	filename+="_";
	if(VFP_SHIFT_ENABLE==1)
	{

		filename+="shift";
		sprintf(tmp,"%d_",SHIFT_VALUE);
		filename+=tmp;
	}
	else
	{
		filename+="noshift";
	}


#endif
#ifdef LEVEL_FP_
	sprintf(tmp,"%d_",FP_LEVEL);
	filename+="LevelFP";
	filename+=tmp;
#endif
    filename+=".txt";
    result_file.open( filename.c_str(), std::ios::out);
    result_file<<"femtoNum:"<<totalfemtoNum/total<<"macroNum:"<<totalmacroNum/total<<std::endl<<std::endl;
    result_file<<"Blocking Rate:"<<outage/total<<std::endl<<std::endl;
	result_file<<"OptimumTargetSelectionMissRate:"<<OptimumTargetmissCount/countList<<std::endl<<std::endl;
	result_file<<"MeanNumInList:"<<MeanNumInList/countList<<"\t"<<ReListLength.Average()<<std::endl<<std::endl;
	result_file<<"ListMissRate:"<<ListMissRateTotal/countList<<"\t"<<ReMissRate.Average()<<std::endl<<std::endl;
	result_file<<"##MobilityStateDetectionErrorRate:"<<ReSpeedDeteError.Average()<<std::endl<<std::endl;
	result_file<<"##Difference## Mean :"<<ReSpeed.Average()<<"\t"<<"Variance:"<<ReSpeed.Variance()<<std::endl<<std::endl;
    result_file<<"Handover Times:"<<ReHandoverTimes.Total()<<std::endl;
    result_file<<"mean Alpha:"<<ReAlpha.Average()<<std::endl;
    result_file<<"Outdoor Ratio:"<<NumOutdoorUser/TotalInOutdoorUser<<std::endl;
    //result_file<<"mean distance btw Femto & User:"<<ReListMeanDistanceBtwUserFetmo.Average()<<std::endl;
    result_file.close();

	ReSpeed.OutputPDF();
	ReSpeed.OutputCDF();
	ReEstAngle.OutputPDF();
	ReRealAngle.OutputPDF();
	ReListLength.OutputCDF();
	ReListLength.OutputPDF();
	ReMissRate.OutputCDF();
	ReMissRate.OutputPDF();
	ReAlpha.OutputPDF();
	ReAlpha.OutputCDF();

	//ReListMeanDistanceBtwUserFetmo.OutputPDF();



}
