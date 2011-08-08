/*-
 * Copyright (c) 2010
 *    Department of Communications Engineering, National Chung Cheng University.
 *    (COMM/CCU) All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the National Chung Cheng
 *      University, Chia-Yi, Taiwan, and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COMM/CCU AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COMM/CCU OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)operations.cpp	11/16/2010
 */

//
//2010.11.16 modified
//
#include "stdhead.h"
#include "channel_allocation.h"
#include "external_parameters.h"

extern BLOCK_RECORD BLOCK[120];
extern FS_INFO fsdata[FS_NUM];

extern XYAXIS BSOxy[NUM_CELL];
extern XYAXIS BSOxy_MAI[NUM_CELL];
extern INT16 adjcent[NUM_CELL][NUM_CELL];
int MS_FS_id =0; //serving FS id for the MS (pCurr->msdata.sFS)

void assign_system_parameters()
{

    frame_duration     = 0.005;           //(sec)frame duration
    symbol_duration    = 0.00010286;      //(sec)(consider Tg=Tb/8)
    //--Link Budget Related--//
    Max_DL_power       = 46.532;	  //(dBm)Max. downlink transmit power at BS (45 Watts)
    Max_UL_power       = 23;             //(dBm)Max. uplink transmit power at MS (0.25 Watts)

    Max_DL_EIRP        = Max_DL_power-Back_off;//(dBm)equivalent isotropic radiated power (EIRP) of BS
    Max_UL_EIRP        = Max_UL_power-Back_off;

    f_carrier          = 2500;           //(MHz)carrier frequency
#define      MCS_order            3              //number of Modulation and Coding Schemes (MCS) able to be used

    Num_SCH_per_MS_UL  = 2;             //Max. number of UL sub-channel can be allocated for each MS
    Proc_gain_DL_FUSC  = 32;            //processing gain
    Proc_gain_DL_PUSC  = 60;            //processing gain

    int fs_near_num		=0; //


//-----------------------------------------//

    SUM_block                      = 0.0;            // number of block events
    SUM_dropping                   = 0.0;            // dropping events
    int i;
    int fs_first_sector_num = 30;
    DL_capacity_measure_counter=0;
    DL_Throughput_measure_counter=0;
    DL_capacity_measure_add=0;
    DL_Throughput_measure_add=0;
    for (i=0; i<150; i++)
    {
        Eb_No_measure[i]=0;
        SINR_PDF[i]=0;
        SINR_BS[i]=0;
        SINR_FS[i]=0;
        DL_Throughput[i] = 0;
    }
    for (i=0; i<100; i++)
        SINR_BS_counter=0;
    SINR_FS_counter =0;
    Eb_No_measure_counter=0;
    MCS=0;
}

void LinkList::process_event(double current_time, int Femto_mode,int Permutation)
{
    SINR(Femto_mode,Permutation);
    capacity_measurement(Permutation,Femto_mode);
}

//****************************************************************************//
//****************************************************************************//
//                 Call Admission Control Module                              //
//****************************************************************************//
//****************************************************************************//
void LinkList::new_call_CAC(double current_time, int Femto_mode,int Permutation)
{
    bool    CAC_decision;          //record the SIR is enough or not
    MSNODE* pCurr=pFirst;
    INT16   i;
    double  total_FS_channel_sum=0;
    double  sub_channel_sum[NUM_SECTOR]= {0};
    double  FS_sub_channel_sum[FS_NUM]= {0};
    double  Num_sub_channel=0;
    double  temp=0;
    double  temp1=0;
    double  temp2=0;


    for(i=0; i<NUM_SECTOR; i++)
        sub_channel_sum[i]=0;
    for(i=0; i<FS_NUM; i++)
        FS_sub_channel_sum[i]=0;

    // new arrival MS will be blocked if no sub-channel is available
    while(pCurr!=pLast)
    {
        if(pCurr->msdata.scell==pLast->msdata.scell) //consider MS belonging to the same serving cell as the new MS (in pLast)
        {
            if(pCurr->msdata.femto_mode==1) // connected to femto
            {
                total_FS_channel_sum++;
                FS_sub_channel_sum[pCurr->msdata.sFS]+=Num_SCH_per_MS_DL;
            }
            else // connected macro
                sub_channel_sum[pCurr->msdata.ssector]+=Num_SCH_per_MS_DL;
        }
        pCurr=pCurr->pNext;
    } //end of while

    //Admission control algorithm
    // Note that we are dealing with subchannels, not channels !!!
    if(Permutation==0)  // PUSC
        Num_sub_channel=Num_channel_DL_PUSC/NUM_SECTOR;
    else
        Num_sub_channel=Num_channel_DL_FUSC;

    if(pLast->msdata.femto_mode==0) // pLast (is the new MS) connected to macro
    {
        if(Num_SCH_per_MS_DL < (Num_sub_channel-sub_channel_sum[pLast->msdata.ssector]) )
            CAC_decision = true;
        else
            CAC_decision = false;//block
    }
    else // Femto MS is not likely to get blocked
    {
        if( Num_SCH_per_MS_DL < (Num_sub_channel-FS_sub_channel_sum[pLast->msdata.sFS]) )
            CAC_decision = true;
        else
            CAC_decision = false;//block
    }

    SUM_ncall += 1.0; //new call arrivals

    if(length==1)//prevent linklist empty
    {
        CAC_decision = true;
    }
    if ( CAC_decision == true )//admission of new call arrival
    {
        Serial_seed+=1;
        pLast->msdata.serial=Serial_seed;
        pCurr->msdata.MCS_DL=1;
        pLast->msdata.con_type=RT;
        pLast->msdata.speed=Max_speed;
        pLast->msdata.direction=PI*0.5*floor(4*Rand());// MS turn in rectangular way

        pCurr=pFirst;
        DL_sub_channel_initial();
        while(pCurr!=NULL)
        {
            LIF_assign_channel(Femto_mode,Permutation,pCurr);
            pCurr=pCurr->pNext;
        } // end while (pCurr)
        // end of 1012 modify

    }
    else //new call blocking occur!
    {
        pCurr = rmv_msnode(pLast);
        SUM_block += 1.0;
    }
}

void LinkList::DL_sub_channel_initial()
{
    INT16   i,j,k;
    for(i=0; i<NUM_CELL; i++)
    {
        for(j=0; j<NUM_SECTOR; j++)
        {
            for(k=0; k<Sub_channel_num; k++)
                DL_sub_channel_sum[i][j][k]=0;
        }
    }

}// end of DL_sub_channel_initial


void LinkList::LIF_assign_channel(int Femto_mode,int Permutation,MSNODE* pCurr)
{
    int temp_sub_channel = 0;
    int channel_upper= 0;
    int temp=0;
    INT16   i,j,k;
    if(pCurr->msdata.femto_mode ==0)  //connected to macro
    {
        temp_sub_channel = (int) floor(Rand(&seed_sub_channel) * Sub_channel_num);//There are Sub_channel_num (10) channels per sector for PUSC; should rename to Channel_num
        channel_upper = temp_sub_channel +Sub_channel_num; //specify the range of searching
        while(DL_sub_channel_sum[pCurr->msdata.scell][pCurr->msdata.ssector][temp_sub_channel % Sub_channel_num] ==1 && temp_sub_channel < channel_upper)
        {
            temp_sub_channel ++;
        }
        // did not consider when channel cannot be found!!!
        DL_sub_channel_sum[pCurr->msdata.scell][pCurr->msdata.ssector][temp_sub_channel % Sub_channel_num] = 1;
        pCurr->msdata.sub_channel_index = temp_sub_channel % Sub_channel_num;
    } // end of if
    else  // connected to femto
    {
        temp_sub_channel = (int) floor(Rand(&seed_sub_channel) * FS_Sub_channel_num);  //FS_Sub_channel_num is (30-10=20) for PUSC
        MS_FS_id = pCurr->msdata.sFS;  //serving FS index
        double threshold1 = 0.35; //search for FS within 0.35km=350m
        int found=0;
        int* a = NULL;
        if(Permutation ==0) //PUSC
            a = new int[(int) (Num_channel_DL_PUSC/Num_SCH_per_MS_DL)];
        else  //FUSC
            a = new int[Num_channel_DL_FUSC];
        while(threshold1 != 0)
        {
            for(k = 0; k < Num_channel_DL_PUSC/Num_SCH_per_MS_DL; k++)
                a[k] =0;
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
        delete [] a;

        DL_FS_sub_channel[fsdata[MS_FS_id].sector_index][temp_sub_channel] =1;
        fsdata[MS_FS_id].sub_channel_index[fsdata[MS_FS_id].ms_num] = temp_sub_channel;
        pCurr->msdata.sub_channel_index = temp_sub_channel;
    } // end of else
} // end of LIF_assign_channel



//---------------------------------------------//
bool LinkList::isEmpty()  //true if no MSNODE
{
    return pFirst==NULL;
}

//---------------------------------------------//
//****************************************************************************//
//****************************************************************************//
//                 End (Call Admission Control Module)                        //
//****************************************************************************//
//****************************************************************************//

//****************************************************************************//
//****************************************************************************//
//                 Performance Measurement Module                             //
//****************************************************************************//
//****************************************************************************//

// The mapping between SINR and BER
// The mapping between SINR and BER
double LinkList::SINR_to_BER(double SINR, int Mod_order)
{
    double BER=0.0;
    double value = 0;
    if(Mod_order ==1) //Modulation_Order=1; QPSK+135/256 coding rate;block length 3840bit
    {
        if(SINR >= 2)
            value =2;
        else if (SINR <=0.5)
            value =0.5;
        else
            value = floor(SINR*10)/10;

        if(value ==2)
            BER = 0.00023;
        else if (value <2 && value >=1.5)
            BER = 0.159500;
        else if (value <1.5 && value >=1)
            BER = 0.999000;
        else
            BER = 1.0;
    }
    else if(Mod_order ==2) //Modulation_Order=2;16QAM+102/256 coding rate
    {
        if(SINR >= 7.5)
            value =7.5;
        else if (SINR <=6)
            value =6;
        else
            value = floor(SINR*10)/10;

        if(value == 7.5)
            BER = 0.000249;
        else if(value<7.5 && value >= 7)
            BER = 0.000338;
        else if (value <7 && value >= 6.5)
            BER = 0.216500  ;
        else if (value <6.5 && value >=6)
            BER = 0.999000;
        else
            BER = 1;

    }
    else if(Mod_order ==3) //Modulation_Order=3;64QAM+135/256 coding rate
    {
        if(SINR >= 13)
            value =13;
        else if (SINR <=11)
            value =11;
        else
            value = floor(SINR*10)/10;

        if(value  == 13)
            BER = 0.000389;
        else if (value <13 && value >=12.5)
            BER = 0.002655 ;
        else if (value <12.5 && value >= 12)
            BER = 0.184000 ;
        else if (value <12 && value >= 11.5)
            BER = 0.960000 ;
        else
            BER = 1;

    }
    return BER;
}

void LinkList::capacity_measurement(int Permutation, int Femto_mode)  //2010.11.16 modified
{
    MSNODE* pCurr=pFirst;
    double  Eb_No=0;
    double  temp=0;
    double  BLER=0;
    double  Tx_power=0;
    double	five_percentile = 0;
    double	coding_rate = 0.0;
    int value = 0;

    while(pCurr!=NULL)
    {
        if(MCS==0) //Adaptive modulation
        {
            if(pCurr->msdata.DL_Eb_No > 11)
                pCurr->msdata.MCS_DL=3;
            else if (pCurr->msdata.DL_Eb_No<6)
                pCurr->msdata.MCS_DL=1;
            else
                pCurr->msdata.MCS_DL=2;
        }

        if(pCurr->msdata.MCS_DL==1) //coding rate
            coding_rate= (double)135/256;
        else if (pCurr->msdata.MCS_DL==2)
            coding_rate = (double)128/256;
        else if (pCurr->msdata.MCS_DL==3)
            coding_rate = (double)135/256;

        if(Permutation==0) //PUSC
        {
            if(Femto_mode==0)
                temp=pCurr->msdata.MCS_DL*2*coding_rate*Data_Carriers_perCH_DL_PUSC*Num_SCH_per_MS_DL*23;//transmitted bits  //2010.11.16 modified
            //transmitted bits per frame
            else if(Femto_mode==1)
                temp=pCurr->msdata.MCS_DL*2*coding_rate*Data_Carriers_perCH_DL_PUSC*Num_SCH_per_MS_DL*23;//transmitted bits  //2010.11.16 modified
            //transmitted bits
        }
        else //FUSC
        {
            if(Femto_mode==0)
                temp=pCurr->msdata.MCS_DL*2*coding_rate*Data_Carriers_perCH_DL_PUSC*Num_SCH_per_MS_DL*23;//transmitted bits  //2010.11.16 modified
            else if(Femto_mode==1)
                temp=pCurr->msdata.MCS_DL*2*coding_rate*Data_Carriers_perCH_DL_PUSC*Num_SCH_per_MS_DL*23;//transmitted bits  //2010.11.16 modified
        }

        BLER=SINR_to_BER(pCurr->msdata.DL_Eb_No, pCurr->msdata.MCS_DL);
        temp=temp*(1-BLER);//effective throughput
        DL_capacity_measure_add+=temp/frame_duration;
        DL_Throughput_measure_add = temp/ frame_duration;
        DL_Throughput_measure_counter ++;
        Throughput_CDF();
        pCurr=pCurr->pNext;
    }//end of while
    DL_capacity_measure_counter++;
}

void LinkList::Throughput_CDF()
{
    int temp = 0;
    temp = (int) ((DL_Throughput_measure_add*10)/((BW*1000000*0.5)/60*2));

    DL_Throughput[temp] ++;
    DL_Throughput_measure_add = 0;

}

void Frecords(int Femto_mode, int Permutation)
{

    std::ofstream  out_file;
    out_file.open("STASTICS.txt", std::ios::app);
    out_file << "Max FS transmit power = " << Max_FS_DL_power <<std::endl;
    out_file << "Frequency Reuse Factor = " << K << std::endl;
    if(Femto_mode==0)
        out_file << "No Femto" << std::endl;
    else if(Femto_mode==1)
        out_file << "Femto Mode"<< std::endl;


    if(Permutation==0)
        out_file << "PUSC permutation for each sector" << std::endl;
    else
        out_file << "FUSC permutation for each sector" << std::endl;

    out_file << "Blocking Probability = " << SUM_block/SUM_ncall << std::endl;
    out_file << "Dropping Probability = " << SUM_dropping/SUM_ncall << std::endl;
    out_file.close();


    INT16  j;
    double temp;
    std::ofstream  out_file1;
    std::ofstream out_file2;
    std::ofstream out_file3;
    std::ofstream out_file4;
    std::ofstream out_file5;
    out_file1.open("SINR_CDF.xls", std::ios::app);
    out_file2.open("SINR_BS.xls",std::ios::app);
    out_file3.open("SINR_FS.xls",std::ios::app);
    out_file4.open("SINR_PDF.xls",std::ios::app);
    out_file5.open("Throughput.xls",std::ios::app);
    for (int k=-20; k<130; k++)
    {
        out_file1 << "\t"<< k ;
        out_file2 << "\t"<< k ;
        out_file3 << "\t"<< k ;
        out_file4 << "\t"<< k ;
        out_file5 << "\t"<< (double)(k+20)/(double)10 ;
    }
    out_file1 << std::endl <<"\t";
    out_file2 << std::endl <<"\t";
    out_file3<< std::endl <<"\t";
    out_file4 << std::endl<<"\t";
    out_file5 << std::endl <<"\t";

    out_file1 << Eb_No_measure[0]/Eb_No_measure_counter << "\t";
    out_file2 << SINR_BS[0]/SINR_BS_counter<< "\t";
    out_file3 << SINR_FS[0]/SINR_FS_counter << "\t";
    out_file4 << Eb_No_measure[0]/Eb_No_measure_counter <<"\t";
    out_file5 << DL_Throughput[0]/DL_Throughput_measure_counter <<"\t";
    temp = Eb_No_measure[0]/Eb_No_measure_counter;
    for(j=1; j<150; j++)
    {
        temp = Eb_No_measure[j]+temp;
        SINR_BS[j]+= SINR_BS[j-1];
        SINR_FS[j]+= SINR_FS[j-1];
        DL_Throughput[j] += DL_Throughput[j-1];
        out_file1 << temp/Eb_No_measure_counter << "\t";
        out_file2 << SINR_BS[j]/SINR_BS_counter << "\t";
        out_file3 << SINR_FS[j]/SINR_FS_counter << "\t";
        out_file4 << (Eb_No_measure[j]/Eb_No_measure_counter) << "\t";
        out_file5 << DL_Throughput[j]/DL_Throughput_measure_counter << "\t";

    }
    out_file1.close();
    out_file2.close();
    out_file3.close();
    out_file4.close();
    out_file5.close();
}

//***********************************************************************************************//
//***********************************************************************************************//
//                                    End (Performance Measurement Module)                       //
//***********************************************************************************************//
//***********************************************************************************************//



//****************************************************************************//
//****************************************************************************//
//               Simulation initialization Module                             //
//****************************************************************************//
//****************************************************************************//
void LinkList::sys_setup()
{

    *seed_arrival=(long int) (Rand()+1);
    *seed_session=(long int) (Rand()+1);
    *seed_cell        =(long int)  Rand()+1;
    seed_position_x  = 376810349;
    seed_position_y   = 1847817841;



} //end of sys_setup
/*----------------------------------*/

//--------------------------------------------//

//****************************************************************************//
//****************************************************************************//
//               End (Simulation initialization Module)                       //
//****************************************************************************//
//****************************************************************************//

//************************************************//
//                 Eb_No_CDF                      //
//************************************************//

void LinkList::Eb_CDF_measurement(double Eb_No_value, int mode) //-10 > value > -20)
{
    int value =0;
    value = floor(Eb_No_value+20);  //2010.11.16 modified from (int) to floor()

    if(value >= 130)
        value = 149;
    else if (value <=0)
        value =0;

    Eb_No_measure[value] ++;

    if(mode ==0)
        SINR_BS[value]++;
    else
        SINR_FS[value]++;
}


/*************************End of Eb_No_CDF*********************/

