#include "TrafficModel.h"
#include "DataChannelAssignmentDL.h"
#include "GenMultiUser.h"
#include "external_parameters.h"

#include <vector>



//int MS_FS_id =0; //serving FS id for the MS (pCurr->msdata.sFS)

//priority_queue <MSNODE* , vector<MSNODE*> ,CompareTime > PQ ;
double  total_FS_channel_sum = 0;
double  sub_channel_sum[NUM_SECTOR] = {0};
double  FS_sub_channel_sum[FS_NUM] = {0};
double  Num_sub_channel = 0;
int No_Channel_use = 0;
POLAXIS temp_polar_;



TrafficModel::TrafficModel (double CurrentT , int Time, double ticktime , int permutation ,LinkList *USERLIST , vector<MSNODE*>* VQ)
{
    CurrentTime = CurrentT ;
    SimulationTime  =   Time;
    TickTime = ticktime;
    Permutation = permutation;
    _USERLIST = USERLIST ;
    _VQ = VQ;



    //cdor
}
TrafficModel::TrafficModel(){}
TrafficModel::~TrafficModel()
{
    //dtor
}

//inital Traffic Model
void TrafficModel::inital( MSNODE** __msnode , int NumMS_ ){

    node = __msnode ;

    if ( CurrentTime ==0 )
    {
        for( int i = 0 ;i<NumMS_;i++ )
        {
            node[i]->msdata.CallState = false ;
            node[i]->msdata.CallTime = 0;
            node[i]->msdata.Call_Interval = StateDuration(1);
        }
        if(Permutation==0)  // PUSC
            Num_sub_channel=Num_channel_DL_PUSC/NUM_SECTOR;
        else
            Num_sub_channel=Num_channel_DL_FUSC;

    }
    return ;
}


void TrafficModel::New_call( int Femto_mode , MSNODE* NewNode)
{
    bool    CAC_decision;          //record the SIR is enough or not

    extern double  total_FS_channel_sum;
    extern double  sub_channel_sum[NUM_SECTOR];
    extern double  FS_sub_channel_sum[FS_NUM];
    extern double  Num_sub_channel;

//Compute Channel use number
    for( vector<MSNODE*>::iterator iter = _VQ->begin() ; iter!=_VQ->end();iter++)
    {
        if((*iter)->msdata.scell == NewNode->msdata.scell )
        {
            if(NewNode->msdata.femto_mode==1) // connected to femto
            {
                total_FS_channel_sum++;     //total femtocell channel use
                FS_sub_channel_sum[NewNode->msdata.sFS]+=Num_SCH_per_MS_DL;  //Max. number of DL sub-channel can be allocated for each MS = 1
            }
            else // connected macro
                sub_channel_sum[NewNode->msdata.ssector]+=Num_SCH_per_MS_DL;
        }
    }

/*--------------------------------------------------------------------------------------
    Num_sub_channel:Macro can use channel num
    sub_channel_sum: Macro has been used channel num

--------------------------Compute Channel is enough or not---------------------------*/
    if(NewNode->msdata.femto_mode==0) // pLast (is the new MS) connected to macro
    {
        if(Num_SCH_per_MS_DL < (Num_sub_channel-sub_channel_sum[NewNode->msdata.ssector]) )
            CAC_decision = true;
        else
            CAC_decision = false;//block
    }

    else // Femto MS is not likely to get blocked
    {
        if( Num_SCH_per_MS_DL < (Num_sub_channel-FS_sub_channel_sum[NewNode->msdata.sFS]) )
            CAC_decision = true;
        else
            CAC_decision = false;//block
    }

//----------------------------------------------------------------------------------------

    if ( CAC_decision == true )//admission of new call arrival
    {
        NewNode->msdata.con_type=RT; //是否on call
        NewNode->msdata.CallState=true;

        LIF_assign_channel(Permutation,NewNode);

    }
    else //new call blocking occur!
    {
        _VQ->pop_back();
        No_Channel_use += 1;
    }
}

void TrafficModel::rmv_call( int Femto_mode , MSNODE* RmvNode)
{
    RmvNode->msdata.sub_channel_index = 0;
    int channel_ind = fsdata[RmvNode->msdata.sFS].sub_channel_index[fsdata[RmvNode->msdata.sFS].ms_num];

//    _VQ->pop_back();

    if(RmvNode->msdata.femto_mode==1) // connected to femto
    {
        total_FS_channel_sum-=1;     //total femtocell channel use
        fsdata[RmvNode->msdata.sFS].UserInfo.CurrNumUser -= 1;
        FS_sub_channel_sum[RmvNode->msdata.sFS]-=Num_SCH_per_MS_DL;  //Max. number of DL sub-channel can be allocated for each MS = 1
        _USERLIST->DL_FS_sub_channel[fsdata[RmvNode->msdata.sFS].sector_index][channel_ind] =0;
    }

    else // connected macro
        sub_channel_sum[RmvNode->msdata.ssector]-=Num_SCH_per_MS_DL;
}



void
TrafficModel::TrafficList( int index_x )
{

      vector<MSNODE*>::iterator it ;
      vector<MSNODE*>::iterator its ;
      CompareTime CT ;

      _VQ->push_back( node[index_x] );
      New_call(  FEMTOMODE , node[index_x] );

      sort(_VQ->begin(), _VQ->end(),CT);
      it = unique(_VQ->begin(), _VQ->end());
      _VQ->resize( it - _VQ->begin() );

    while( !_VQ->empty() )
    {
        its = _VQ->end()-1;

        if( (*its)->msdata.CallTime <= 0 )
        {
            (*its)->msdata.sub_channel_index = 0 ;  //release datachannel

//            rmv_call(FEMTOMODE , *its);
            _VQ->pop_back();                             //pop no call MS
        }

        else {break;}
    }

}

/*=====================================================
UseageGrade: 0 (通話時間)
             1 Low
             2 Medium
             3 High
/======================================================*/

bool TrafficModel::CheckMS( int index)
{
    Past(index);
    if ( SimulationTime / 3 > CurrentTime )
    {
        if( node[index]->msdata.CallTime <0 && node[index]->msdata.CallState==true)
        {
            rmv_call(FEMTOMODE , node[index]);
            node[index]->msdata.con_type = NRT ;
            node[index]->msdata.CallState = false ;
            node[index]->msdata.Call_Interval = StateDuration(1);

        }
    }

    else if( 2* SimulationTime / 3 > CurrentTime)
    {
        if( node[index]->msdata.CallTime <0 && node[index]->msdata.CallState==true)
        {
            rmv_call(FEMTOMODE , node[index]);
            node[index]->msdata.con_type = NRT ;
            node[index]->msdata.CallState = false ;
            node[index]->msdata.Call_Interval = StateDuration(2);
        }
    }
    else
    {
        if( node[index]->msdata.CallTime <0 && node[index]->msdata.CallState==true)
        {
            rmv_call(FEMTOMODE , node[index]);
            node[index]->msdata.con_type = NRT ;
            node[index]->msdata.CallState = false ;
            node[index]->msdata.Call_Interval = StateDuration(3);
        }
    }

    if( node[index]->msdata.Call_Interval <0 && node[index]->msdata.CallState==false )
    {
        node[index]->msdata.con_type = RT ;
        node[index]->msdata.CallState = true ;
        node[index]->msdata.CallTime = StateDuration(0);
    }

    return node[index]->msdata.CallState ;

}





double TrafficModel::StateDuration(int UseageGrade)
{
    int Mean ;
    switch(UseageGrade)
    {
        case 0:
            Mean = SimulationTime/480 ;//3*60(sec).
            break;
        case 1:
            Mean = SimulationTime/6;
            break;
        case 2:
            Mean = SimulationTime/24 ;
            break;
        case 3:
            Mean = SimulationTime/48 ;
            break;
    }
    return ExpRand(Mean);

}


double TrafficModel::ExpRand(double Mean)
{
    double P_M =0.0;

    P_M= (-1)*Mean * log((double)rand() /(double)RAND_MAX );

    return P_M ;
}

void TrafficModel::Past(int _index)
{
    if( node[_index]->msdata.CallState == true )
    {
        node[_index]->msdata.CallTime -=TickTime ;
    }
    else
    {
        node[_index]->msdata.Call_Interval -=TickTime ;
    }
}

void TrafficModel::DL_sub_channel_initial()
{
    INT16   i,j,k;

    for(i=0; i<NUM_CELL; i++)
    {
        for(j=0; j<NUM_SECTOR; j++)
        {
            for(k=0; k<Sub_channel_num; k++)
                _USERLIST->DL_sub_channel_sum[i][j][k]=0;
        }
    }

}// end of DL_sub_channel_initial




void TrafficModel::LIF_assign_channel(int Permutation,MSNODE* pCurr)
{
    int temp_sub_channel = 0;
    int channel_upper= 0;
    int temp=0;
    INT16   i,j,k;
    double threshold1 = 0.35; //search for FS within 0.35km=350m
    int found=0;
    int* a = NULL;

    if(pCurr->msdata.femto_mode ==0)  //connected to macro
    {
        temp_sub_channel = (int) floor(Rand(&seed_sub_channel) * Sub_channel_num);//There are Sub_channel_num (10) channels per sector for PUSC; should rename to Channel_num
        channel_upper = temp_sub_channel +Sub_channel_num; //specify the range of searching
        while(_USERLIST->DL_sub_channel_sum[pCurr->msdata.scell][pCurr->msdata.ssector][temp_sub_channel % Sub_channel_num] ==1 && temp_sub_channel < channel_upper)
        {
            temp_sub_channel ++;
        }
        // did not consider when channel cannot be found!!!
        _USERLIST->DL_sub_channel_sum[pCurr->msdata.scell][pCurr->msdata.ssector][temp_sub_channel % Sub_channel_num] = 1;
        pCurr->msdata.sub_channel_index = temp_sub_channel % Sub_channel_num;
    } // end of if

    else  // connected to femto
    {
        temp_sub_channel = (int) floor(Rand(&seed_sub_channel) * FS_Sub_channel_num);  //FS_Sub_channel_num is (30-10=20) for PUSC
        MS_FS_id = pCurr->msdata.sFS;  //serving FS index

        if(Permutation ==0) //PUSC
            a = new int[(int) (Num_channel_DL_PUSC/Num_SCH_per_MS_DL)];
        else  //FUSC
            a = new int[Num_channel_DL_FUSC];

        while(threshold1 != 0)
        {
            //inital femtocell channel
            for(k = 0; k < Num_channel_DL_PUSC/Num_SCH_per_MS_DL; k++)
                a[k] =0;

            //find near femtcell num
            for(i=0; i<pCurr->msdata.fs_near_num; i++)
            {
                j = 0;


                while(j<fsdata[pCurr->msdata.FS_NEAR[i]].ms_num && j<2 && pCurr->msdata.distance(fsdata[pCurr->msdata.FS_NEAR[i]].position, pCurr->msdata.position) < threshold1)
                {
                    temp = fs_sub_channel_mapping(fsdata[pCurr->msdata.FS_NEAR[i]].sector_index,fsdata[pCurr->msdata.FS_NEAR[i]].sub_channel_index[j]);
                    a[temp] = 1;
                    j ++;
                }
            }
            for(k = 0; k < Num_channel_DL_PUSC/Num_SCH_per_MS_DL; k++)
            {
                if(a[k]==0)
                {
                    if(Permutation == 0)
                    {
                        if(pCurr->msdata.ssector ==0 && k>9)
                        {
                            temp_sub_channel = k - 10;
                            found = 1;
                        }
                        else if(pCurr->msdata.ssector ==1)
                        {
                            if(k<10)
                            {
                                temp_sub_channel = k;
                                found = 1;
                            }
                            else if(k>19)
                            {
                                temp_sub_channel = k - 10;
                                found = 1;
                            }
                        }
                        else if(pCurr->msdata.ssector ==2 && k<20)
                        {
                            temp_sub_channel = k;
                            found = 1;
                        }
                    }
                    else
                    {
                        temp_sub_channel = k;
                        found = 1;
                    }
                }
                if(found == 1)
                    k = 30;
            }
            if(found == 1)
                threshold1 = 0;
            else
                threshold1 -= 0.01;
        }
        delete a;

        _USERLIST->DL_FS_sub_channel[fsdata[MS_FS_id].sector_index][temp_sub_channel] =1;
        fsdata[MS_FS_id].sub_channel_index[fsdata[MS_FS_id].ms_num] = temp_sub_channel;
        pCurr->msdata.sub_channel_index = temp_sub_channel;

    } // end of else

} // end of LIF_assign_channel



double
TrafficModel::DataSINR()
{

    for( vector<MSNODE*>::iterator iter = _VQ->begin() ; iter!=_VQ->end();iter++)
    {
        double _S_DL , _I_charge_DL ,_I_distribute_DL , SINR_;

        MSNODE* pCurr=*iter;
        _S_DL=0.0;
        _I_charge_DL=0.0;
        _I_distribute_DL=0.0;
        SINR_ = 0.0 ;

        _S_DL = DL_signal_calculation_(Permutation,pCurr);

        _I_charge_DL = interfer_calculation_(Permutation,pCurr);

        SINR_ = SINR_calculation_(Permutation,pCurr,_S_DL , _I_charge_DL);

        if((*iter)->msdata.femto_mode ==0)
        {
            Record_SINR(SINR_,pCurr);
        }


    }
}

double TrafficModel::DL_signal_calculation_(int Permutation,MSNODE* pCurr)
{
    double antenna_gain , temp_prop_loss , S_DL;

    //-------calculate the received signal power-------//
    if(pCurr->msdata.femto_mode==0)//MS directly served by BS
    {
        temp_polar_=pCurr->msdata.cart2pol(BSOxy_MAI[0],pCurr->msdata.position);
        antenna_gain=_USERLIST->antenna_pattern(SECTOR_Steer_dir[pCurr->msdata.ssector],temp_polar_.th);

        if(pCurr->msdata.on_street==1)
            temp_prop_loss=_USERLIST->pathloss(0, BSOxy[0], pCurr->msdata.position);
        else
            temp_prop_loss=_USERLIST->pathloss(3, BSOxy[0], pCurr->msdata.position);

        if(Permutation == 0) // PUSC
            S_DL = Max_DL_EIRP + antenna_gain - temp_prop_loss - 10*log10(All_carriers);
        else
            S_DL = Max_DL_EIRP + antenna_gain - temp_prop_loss - 10*log10(All_carriers);
    }//if
    else//MS served by FS
    {
        temp_polar_=pCurr->msdata.cart2pol(fsdata[MS_FS_id].position, pCurr->msdata.position);

        if(pCurr->msdata.on_street ==1)
            temp_prop_loss=_USERLIST->pathloss(2, fsdata[pCurr->msdata.sFS].position, pCurr->msdata.position);
        else
            temp_prop_loss=_USERLIST->pathloss(1, fsdata[pCurr->msdata.sFS].position, pCurr->msdata.position);

        if(Permutation == 0)
            S_DL = fsdata[pCurr->msdata.sFS].FS_DL_EIRP - temp_prop_loss - 10*log10(All_carriers) + 10*log10(30.0) - 10*log10(2.0);
        else
            S_DL = fsdata[pCurr->msdata.sFS].FS_DL_EIRP - temp_prop_loss - 10*log10(All_carriers) + 10*log10(30.0) - 10*log10(2.0);

    }// else

    return S_DL ;

}//end of DL signal calculation


double TrafficModel::interfer_calculation_(int Permutation,MSNODE* pCurr)
{
        double I_charge_DL_ , I_distribute_DL_ , temp_Interference , N_DL;


        if(Permutation == 0) // PUSC
            N_DL = Noise_power_density + 10*log10(A_subcarrier_frequency);            // Noise_power is for the entire bandwidth. Since we only use

        else
            N_DL = Noise_power_density + 10*log10(A_subcarrier_frequency);

        I_charge_DL_ = Macro_Interference( Permutation , pCurr );
        I_distribute_DL_ = Femto_Interference( Permutation , pCurr );

//        temp_Interference=10*log10(I_charge_DL_ + I_distribute_DL_ + pow(10,N_DL/10));

        temp_Interference=10*log10(I_charge_DL_ + I_distribute_DL_) + N_DL;

        return temp_Interference;
}


double TrafficModel::SINR_calculation_(int Permutation,MSNODE* pCurr,double S_DL_1 , double I_charge_DL_1)
{
    double Eb_No_DL ;

    if(pCurr->msdata.femto_mode==0)
    {
        if(Permutation == 0)
            Eb_No_DL=S_DL_1 - I_charge_DL_1 - MS_Nosie_figure;
        else
            Eb_No_DL=S_DL_1+10*log10(Proc_gain_DL_FUSC)-I_charge_DL_1 - MS_Nosie_figure;
        pCurr->msdata.DL_Eb_No=Eb_No_DL;
    }
    else
    {
        Eb_No_DL=S_DL_1-I_charge_DL_1 - MS_Nosie_figure;
        pCurr->msdata.DL_Eb_No=Eb_No_DL;
    }
    return Eb_No_DL ;
}





//----------interference of other BS----------//
double TrafficModel::Macro_Interference( int Permutation,MSNODE* pCurr )
{

     double I_charge_DL , temp_prop_loss , temp_I , antenna_gain;
     I_charge_DL = 0.0 ;
     for(int i=1; i<NUM_CELL; i++)
    {
            temp_polar_=pCurr->msdata.cart2pol(BSOxy_MAI[i],pCurr->msdata.position);

            if(pCurr->msdata.on_street==1)
                temp_prop_loss=_USERLIST->pathloss(0,BSOxy[i] ,pCurr->msdata.position);
            else
                temp_prop_loss=_USERLIST->pathloss(3,BSOxy[i],pCurr->msdata.position);

            // cell sub_channel
            for(int j=0; j<NUM_SECTOR; j++)
            {
                antenna_gain=_USERLIST->antenna_pattern(SECTOR_Steer_dir[j],temp_polar_.th);
                if(Permutation==0 && j == pCurr->msdata.ssector )
                {
                    temp_I = Max_DL_EIRP + antenna_gain - temp_prop_loss - 10*log10(All_carriers);
                    I_charge_DL+=pow(10.0,temp_I/10);//(mW)linear

                }// if(Permutation==0)
                else if ( Permutation==1 ) //FUSC
                {
                    temp_I = (int)(Max_DL_EIRP + antenna_gain - temp_prop_loss - 10*log10(All_carriers));
                    I_charge_DL+=pow(10.0,temp_I/10);//(mW)linear
                }// else
            } // for(j=0;
        } //end of for(i...)


        return I_charge_DL;
}
//----------interference of near femtocell----------//
double TrafficModel::Femto_Interference( int Permutation,MSNODE* pCurr )
{
    double temp_I , temp_prop_loss , I_distribute_DL ;
    int j = 0;

    for(int i=0; i<pCurr->msdata.fs_near_num; i++)
    {
            if(pCurr->msdata.on_street ==1)
                temp_prop_loss = _USERLIST->pathloss(2,fsdata[pCurr->msdata.FS_NEAR[i]].position,pCurr->msdata.position);
            else
                temp_prop_loss = _USERLIST->pathloss(1,fsdata[pCurr->msdata.FS_NEAR[i]].position,pCurr->msdata.position);

            while(j<fsdata[pCurr->msdata.FS_NEAR[i]].ms_num && j<NUM_SECTOR)
            {
                if(mbs_sub_channel_mapping(pCurr->msdata.ssector,pCurr->msdata.sub_channel_index) == fs_sub_channel_mapping(fsdata[pCurr->msdata.FS_NEAR[i]].sector_index,fsdata[pCurr->msdata.FS_NEAR[i]].sub_channel_index[j]))
                {
                    if(Permutation == 0)
                        temp_I = fsdata[pCurr->msdata.FS_NEAR[i]].FS_DL_EIRP - temp_prop_loss - 10*log10(All_carriers) + 10*log10(30.0) - 10*log10(fsdata[pCurr->msdata.FS_NEAR[i]].ms_num);
                    else
                        temp_I = fsdata[pCurr->msdata.FS_NEAR[i]].FS_DL_EIRP - temp_prop_loss - 10*log10(All_carriers) + 10*log10(30.0) - 10*log10(fsdata[pCurr->msdata.FS_NEAR[i]].ms_num);

                    I_distribute_DL += pow(10,temp_I/10);
                } // if(fs_sub_channel_mapping...
                j ++;
            }// while(j<fsdata....
    } //end for all neighbor FS ...

    return I_distribute_DL ;
}

void TrafficModel::Record_SINR(double SINR__,MSNODE* pCurr_)
{
    double temp_SINR , temp_SINR_BS , temp_SINR_FS ;
    int *i = 0 ;

    temp_SINR = 0.0 ;

    std::ofstream outfile;
    outfile.open("Eb_No_total.xls",std::ios::app);

    if(pCurr_->msdata.femto_mode ==0)
    {
        outfile <<i++<<"\t"<<SINR__<<"\t"<<1<<std::endl;
    }
    else
    {
        outfile <<i++<<"\t"<<SINR__<<"\t"<<2<<std::endl;
    }

}



