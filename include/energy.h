#ifndef ENERGY_H
#define ENERGY_H

#include "stdhead.h"
#include "external_parameters.h"
#include "MobileStation.h"



#define TimeToClose 2.5 //sec


struct Energy_INFO{

    int NoUserTime_list ;
    int NoUserTime_full ;
    double Save_Power ;
    double List_Power ;
    double A_Power ;
};

using namespace std;
class energy
{
    public :
        energy() ;
        ~energy();
        double Active_Power();
        double Saving_Power();
        void NoUserFemto(int fs_number, double ticktime,LinkList* Userlist_,
											int Permutation__,CSG* csg_,vector<vector<int>* >* FemtoListByFP_v);

        void ComputeListNoUserTime(int finding,int fs_number,double ticktime);
        void ComputeFullNoUserTime(int fs_number,double ticktime, bool T_F_);


        void TotalEnergy();

    private :

        double RF_Transmitter;
        double RF_Receiver;
        double RF_Power_Amplifier;
        double Other_Circuitry;
        double Other;

        vector<Energy_INFO>* Energy_vec;
            CSG* _CSG ;

    LinkList USERLIST;

};
#endif  // ENERGY_H
