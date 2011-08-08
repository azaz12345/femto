#ifndef TRAFFICMODEL_H
#define TRAFFICMODEL_H

#include "stdhead.h"
#include "EventQueue.h"
#include <queue>


/*
typedef struct {

    double Call_time ;
    double Call_duration ;
}user;
*/


using namespace std;

class TrafficModel
{

    public:
        TrafficModel (double CurrentT , int Time, double ticktime ,int permutation ,LinkList *USERLIST ,  vector<MSNODE*>* VQ );
        TrafficModel();

        ~TrafficModel ();

        void inital( MSNODE** __msnode , int NumMS_ );

        void New_call( int Femto_mode , MSNODE* NewNode);

        void rmv_call( int Femto_mode , MSNODE* RmvNode);

        void TrafficList( int index_x );

        bool CheckMS( int index);

        double ExpRand(double Mean);
        double StateDuration(int UsageGrade);
        void Past(int _index);

        void DL_sub_channel_initial();

//        TrafficModel( int index_x , vector<MSNODE*>* VQ );

        void LIF_assign_channel(int Permutation,MSNODE* pCurr);

        double DataSINR();

        double DL_signal_calculation_(int Permutation,MSNODE* pCurr);

        double interfer_calculation_(int Permutation,MSNODE* pCurr);

        double SINR_calculation_(int Permutation,MSNODE* pCurr,double S_DL_1 , double I_charge_DL_1);

        double Macro_Interference( int Permutation,MSNODE* pCurr );

        double Femto_Interference( int Permutation,MSNODE* pCurr );

        void Record_SINR(double SINR__,  MSNODE* pCurr_);

//        priority_queue<MSNODE , vector<MSNODE*> ,CompareTime >* PQ;


//        bool TestMS(int users);

        MSNODE** node ;

    protected:
//       MSNODE* _msnode;
//       MSNODE* msnode_;




    private:

    int SimulationTime  ;
    double TickTime ;
    double CurrentTime ;
    int Permutation ;

    LinkList *_USERLIST ;

    vector<MSNODE*>* _VQ;


};

class CompareTime {
    public:

        bool operator()(MSNODE* MS1, MSNODE* MS2)
        {
            if (MS2->msdata.CallTime > MS1->msdata.CallTime) return true;

            return false;
        }
};




#endif
