#include "Simulator.h"

#include "external_parameters.h"
#include <stdlib.h>
#include "SimulatorParameter.h"
#include "TrafficModel.h"
#include "DataChannelAssignmentDL.h"
#include "EventQueue.h"
#include "Record.h"

double outage =0.0;
double MeanNumInList=0.0;
long int countList = 0;
double angleMSE = 0.0;
double missCount = 0.0 ;
int ToLowSinr = 0;
int LowSinr = 0;
int MacroLowSinr = 0;
int FemtoLowSinr = 0;
int MacroNum  = 0;
int FemtoNum  = 0;
int femtoNum_=0;
int macroNum_=0;

Record ReMacroSINR("Macro_SINR");
Record ReFemtoSINR("Femto_SINR");
Record ReALLSINR("ALL_SINR");



EventQueue EvQue_;
long Seed;
//Record TaMacroSINR("Ta_Macro_SINR");
//Record TaFemtoSINR("Ta_Femto_SINR");
//Record TaALLSINR("Ta_ALL_SINR");



Simulator::Simulator()
{
	/*initialization*/

	P1.numuser = 2000;
	P1.permutation = 0;
	P1.simulationtotaltime = 3600;
	P1.ticktime = 0.5;
	P1.scanperiod = 2.0;

//	MultiUser = new GenMultiUser( &USERLIST, P1);
//    VQ = new vector<MSNODE*>;

//    vector<WayPoint>* UserPathList_ = MultiUser->getUserPathlist();


/*
   _CSG = new CSG();
    for ( int MS = 0; MS < P1.numuser ; MS++)
    {
        _CSG->NeighborFemto( &UserPathList_[MS] );
        _CSG->NeighborCSG_OSG();
    }
*/
    //ctor
}

Simulator::~Simulator()
{
    //dtor
}


void
Simulator::Start()
{

    int numbertest = 0;
    double SimulationTime = 0;
    energy save ;
    DataChannelAssignmentDL DCA( &BSOxy_MAI , &fsdata);
    LinkListEvPrcs LkEvPrcs(&USERLIST);

    //insert node event
    while( SimulationTime < P1.simulationtotaltime )
    {
        Event event;
        double tmptime = ExpRand(2,&Seed);//exponetial

        SimulationTime=SimulationTime+tmptime;

        event.EventType_= INSER_NODE;
        event.time_     = SimulationTime;
        event.node_     = 0;
        event.EvProc_   = &LkEvPrcs;

        EvQue_.pushEvent(event);
    }

    EvQue_.Process();
    system("pause");
    /*
    for(multiset< Event ,sortByTime>::iterator it=EvQue_.EvQueue.begin();it!=EvQue_.EvQueue.end();it++){
        printf("%f\n",it->time_);
        system("pause");

    }
    */



	for(int i=0; i<=P1.simulationtotaltime/P1.ticktime; i++){

		system("cls");
		printf("®É¶¡:%f\n",i*(P1.ticktime));






//		MultiUser->UpdateAllPosition(USERLIST.pFirst,0);

//		MSNODE** arrayMSNODE = new MSNODE*[P1.numuser];
		MSNODE* currMS = USERLIST.pFirst;

        FemtoListByFP_v = new vector<vector<int>* >;

//        TrafficModel T_M(i*(P1.ticktime),P1.simulationtotaltime, P1.ticktime ,P1.permutation ,&USERLIST ,VQ);
/*
		for( int j=0; currMS != NULL; j++){
			arrayMSNODE[j] = currMS;
			currMS = currMS->pNext;
		}

        T_M.inital( arrayMSNODE ,P1.numuser );    // inital all user aren't on call
*/
		for(int j=0; j<P1.numuser; j++){

//		   if( T_M.CheckMS(j)==false) {continue;}

#ifdef LEVEL_FP_
			MSwithLevelFP  MS( &USERLIST, &BSOxy_MAI, &fsdata, P1.permutation,_CSG);
#endif

#ifdef VECTOR_FP_
			MSwithVectorFP  MS( &USERLIST, &BSOxy_MAI, &fsdata, P1.permutation,_CSG);
#endif

#ifdef FULL_SCAN_
			MobileStationBase MS( &USERLIST, &BSOxy_MAI, &fsdata, P1.permutation,_CSG);
#endif

//            T_M.TrafficList( j );
            numbertest ++ ;

//          DCA.RunCompute( arrayMSNODE[j] , P1.permutation );
//			MS.UpdateMSINFO(arrayMSNODE[j], i*(P1.ticktime), P1.ticktime);

//			FemtoListByFP_v->push_back(MS.GetFemtoListByFPrf());

//			T_M.DataSINR();
			//delete  MS[i];
		}
/************************************Power Saving************************************************/
		for(int j = 0 ; j<FS_NUM_INDEX;j++)
            save.NoUserFemto(j,P1.ticktime, &USERLIST, P1.permutation,_CSG , FemtoListByFP_v);

//		delete [] arrayMSNODE;

		for(vector<vector<int>* >::iterator it = FemtoListByFP_v->begin(); it != FemtoListByFP_v->end(); it++){delete *it;}

		delete FemtoListByFP_v;

	}

    save.TotalEnergy();

    double total=(double)(P1.numuser*(P1.simulationtotaltime/P1.ticktime));

 //   printf("\nFemtoRatio%f\nMacroRatio%f",(double)femtoNum_/total ,(double) macroNum_/total);
 //   printf("\nOutage%f",outage/(( P1.simulationtotaltime/ P1.ticktime)*P1.numuser));
    printf("\nFemtoRatio%f\nMacroRatio%f",(double)femtoNum_/numbertest ,(double) macroNum_/numbertest);
    printf("\nOutage%f",outage/numbertest);

    ReMacroSINR.OutputPDF();
    ReMacroSINR.OutputCDF();
//    TaMacroSINR.OutputPDF();
//    TaMacroSINR.OutputCDF();

    ReFemtoSINR.OutputPDF();
    ReFemtoSINR.OutputCDF();
//    TaFemtoSINR.OutputPDF();
//    TaFemtoSINR.OutputCDF();

    ReALLSINR.OutputPDF();
    ReALLSINR.OutputCDF();
//    TaALLSINR.OutputPDF();
//    TaALLSINR.OutputCDF();






//    printf("\nOutage%f",outage);

}



