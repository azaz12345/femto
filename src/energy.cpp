/************************************************************
Hardware Component               Energy Consumption[Watt]
--------------------------------------------------------------
Microprocessor                          1.7
Associated Memory                       0.5
--------------------------------------------------------------
FPGA                                    2.0
Associated Memory                       0.5
--------------------------------------------------------------
Other Circuitry                         2.0
--------------------------------------------------------------
RF Transmitter                          1.0
RF Receiver                             0.5
--------------------------------------------------------------
RF Power Amplifier                      2.0
*************************************************************/
//Other = Microprocessor + Associated Memory  + FPGA + Associated Memory  + Other Circuitry
#include "energy.h"
#define Better 0
#define Vector 1



energy::energy():
   RF_Transmitter(1.0),
   RF_Receiver(0.5),
   RF_Power_Amplifier(2.0),
   Other_Circuitry(2.0),
   Other(4.7) {

   Energy_vec = new vector<Energy_INFO> (FS_NUM_INDEX) ;

   }

energy::~energy()
{
  delete Energy_vec ;
};

double energy::Active_Power()
{
    double Active_Power = RF_Transmitter + RF_Receiver + RF_Power_Amplifier + Other ;

    return Active_Power ;
};

double energy::Saving_Power()
{
    double Saving_Power = RF_Transmitter + RF_Receiver + RF_Power_Amplifier ;

    return Saving_Power ;
};

void energy::NoUserFemto(int fs_number, double ticktime,LinkList* Userlist_,
											int Permutation__,CSG* csg_,vector<vector<int>* >* FemtoListByFP_v)
{

    MSwithVectorFP MS_ (&USERLIST, &BSOxy_MAI, &fsdata, Permutation__,csg_);
    vector<int>::iterator it ;

#if Better
    bool T_F = false;

    if ( fsdata[fs_number].UserInfo.CurrNumUser == 0 && fsdata[fs_number].Power_State == true )
    {
        Energy_vec->at(fs_number).NoUserTime_full+=1;
    }
    else if ( fsdata[fs_number].UserInfo.CurrNumUser != 0 )
    {
        fsdata[fs_number].Power_State = true ;
        Energy_vec->at(fs_number).NoUserTime_full = 0 ;
    }
    else
        T_F = true;

     ComputeFullNoUserTime(fs_number, ticktime,T_F);
#endif
//===========================================================================================================================
#if Vector
    for ( vector< vector<int>* >::iterator inter = FemtoListByFP_v->begin(); inter!=FemtoListByFP_v->end();inter++)
    {
        it = find( (*inter)->begin(),(*inter)->end(), fs_number);

        if ( it != (*inter)->end() )
        {
            ComputeListNoUserTime(1,fs_number,ticktime);
            return;
        }
    }

    ComputeListNoUserTime(0,fs_number,ticktime);
#endif
};

//====================================================================================
void energy::ComputeListNoUserTime(int finding, int fs_number,double ticktime)
{
    bool except = false ;
    if (  finding == 1)  //找到在LIST內
    {
        if( fsdata[fs_number].UserInfo.CurrNumUser == 0 )
        {
            Energy_vec->at(fs_number).NoUserTime_list +=1 ;
        }
        else
        {
            fsdata[fs_number].Power_State = true ;
            Energy_vec->at(fs_number).NoUserTime_list = 0;
        }

    }
    else
    {
        if ( fsdata[fs_number].UserInfo.CurrNumUser == 0 )
            Energy_vec->at(fs_number).NoUserTime_list +=1 ;
        else
            Energy_vec->at(fs_number).NoUserTime_list = 0;
    }
//=====================================================================================
    if( Energy_vec->at(fs_number).NoUserTime_list > TimeToClose/ticktime )  //無MS使用
    {
        fsdata[fs_number].Power_State = false ;
        Energy_vec->at(fs_number).List_Power += Other/ticktime ;
        Energy_vec->at(fs_number).A_Power += Active_Power()/ticktime ;
    }
    else                                                      //有MS使用
    {
        Energy_vec->at(fs_number).List_Power += Active_Power()/ticktime ;
        Energy_vec->at(fs_number).A_Power += Active_Power()/ticktime ;
    }

};
//=======================================================================================
void energy::ComputeFullNoUserTime(int fs_number,double ticktime,bool T_F_)
{
    if( Energy_vec->at(fs_number).NoUserTime_full > TimeToClose/ticktime || T_F_ == true )  //無MS使用
    {
        fsdata[fs_number].Power_State = false;
        Energy_vec->at(fs_number).Save_Power += Other/ticktime ;
        Energy_vec->at(fs_number).A_Power += Active_Power()/ticktime ;
    }
    else                                                      //有MS使用
    {
        Energy_vec->at(fs_number).Save_Power += Active_Power()/ticktime ;
        Energy_vec->at(fs_number).A_Power += Active_Power()/ticktime ;
    }

};
//=======================================================================================


void energy::TotalEnergy()
{
     double TotalEnergy =0.0,A_TotalEnergy = 0.0, L_TotalEnergy = 0.0;

     for(vector<Energy_INFO>::iterator it = Energy_vec->begin(); it != Energy_vec->end(); it++)
     {
         TotalEnergy+=it->Save_Power ;
         L_TotalEnergy+=it->List_Power ;
         A_TotalEnergy+=it->A_Power;
     }

     printf("\nGoodSaveEnergyRate%f",1.0-TotalEnergy/A_TotalEnergy);
     printf("\nSaveEnergyRate%f",1.0-L_TotalEnergy/A_TotalEnergy);
     printf("\nTotalEnergy%f",TotalEnergy);
     printf("\nL_TotalEnergy%f", L_TotalEnergy);
     printf("\nA_TotalEnergy%f\n",A_TotalEnergy);

};



