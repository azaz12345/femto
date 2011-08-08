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
 *	@(#)sinr.cpp	11/02/2010
 */

#include "stdhead.h"
#include "external_parameters.h"


////////////////////////////////////////////////////////////////////
double  total_carrier=(double)All_carriers;
double  temp_angle=0;
double  angle=0;

////////////////////////////////////////////////////////////////////
POLAXIS temp_polar;
double  temp_prop_loss=0;
INT16   i,j,k;
double  antenna_gain=0;
double  sector_num=(double)NUM_SECTOR;
double  N_DL=0; //thermal noise power
double temp_Interference =0;
double  Eb_No_DL=0; //received Eb/No
double  S_DL=0; //received signal power
double  I_distribute_DL=0;//received interference power (FS)
double  temp_I=0;
double  I_charge_DL=0; //received interference power (BS)

/*
void LinkList::SINR(int Femto_mode,int Permutation)
{
    MSNODE* pCurr=pFirst;

    ///////////------------------Eb_No calculation-------------------///////////
    //---check sub-channel usage---//

    pCurr=pFirst;

    while(pCurr!=NULL)
    {
        S_DL=0;
        I_charge_DL=0;
        I_distribute_DL=0;

        DL_signal_calculation(Femto_mode , Permutation,pCurr);
        interfer_calculation(Femto_mode , Permutation,pCurr);
        SINR_calculation(Permutation,pCurr);

        pCurr=pCurr->pNext;
    }//end of while

    //////////------------End of Eb_No calculation----------------/////////////
}
*/
/*
void LinkList::DL_signal_calculation(int Femto_mode,int Permutation,MSNODE* pCurr)
{

    //-------calculate the received signal power-------//
    if(pCurr->msdata.femto_mode==0)//MS directly served by BS
    {
        temp_polar=pCurr->msdata.cart2pol(BSOxy_MAI[0],pCurr->msdata.position);
        antenna_gain=antenna_pattern(SECTOR_Steer_dir[pCurr->msdata.ssector],temp_polar.th);

        if(pCurr->msdata.on_street==1)
            temp_prop_loss=pathloss(0, BSOxy[0], pCurr->msdata.position);
        else
            temp_prop_loss=pathloss(3, BSOxy[0], pCurr->msdata.position);

        if(Permutation == 0) // PUSC
            S_DL = Max_DL_EIRP + antenna_gain - temp_prop_loss - 10*log10(total_carrier);
        else
            S_DL = Max_DL_EIRP + antenna_gain - temp_prop_loss - 10*log10(total_carrier);
    }//if
    else//MS served by FS
    {
        temp_polar=pCurr->msdata.cart2pol(fsdata[MS_FS_id].position, pCurr->msdata.position);

        if(pCurr->msdata.on_street ==1)
            temp_prop_loss=pathloss(2, fsdata[pCurr->msdata.sFS].position, pCurr->msdata.position);
        else
            temp_prop_loss=pathloss(1, fsdata[pCurr->msdata.sFS].position, pCurr->msdata.position);

        if(Permutation == 0)
            S_DL = fsdata[pCurr->msdata.sFS].FS_DL_EIRP - temp_prop_loss - 10*log10(total_carrier) + 10*log10(30.0) - 10*log10(2.0);
        else
            S_DL = fsdata[pCurr->msdata.sFS].FS_DL_EIRP - temp_prop_loss - 10*log10(total_carrier) + 10*log10(30.0) - 10*log10(2.0);

    }// else

}//end of DL signal calculation

*/


/*
void LinkList::interfer_calculation(int Femto_mode,int Permutation,MSNODE* pCurr)
{

    if(pCurr==pFirst)//第一次先計算出N_DL
    {
        if(Permutation == 0) // PUSC
            N_DL = Noise_power_density + 10*log10(A_subcarrier_frequency);            // Noise_power is for the entire bandwidth. Since we only use
        // portions of subcarriers, need to subtract those noise power
        // that is outside of our band.
        // Carrier_perCH_DL_PUSC = 28, Num_channel_DL_PUSC=20
        // total_carrier = 2048
        // So this is the noise power for a subchannel
        else
            N_DL = Noise_power_density + 10*log10(A_subcarrier_frequency);
        // Carrier_perCH_DL_FUSC = 48, Num_channel_DL_FUSC=32
        // This is not consistent with PUSC above;
        // should reduce by the subchannel number (32) in dB
    }




    //ofstream outfile;
    std::ofstream outfile;
    outfile.open("inter.xls",std::ios::app);

    //-------calculate the received interference power-------//
    // The original code has flaws.
    // It considers either MS connected to MacroBS under PUSC,
    // (regardless of Femto_mode, no interference from FS;
    //  it would not work for macroMS under FUSC with femtoBS nearby)
    // or MS connected FemtoBS (Femto_mode must be 1 and has
    // interference from both neighbor macroBS and femtoBS

    // Should correct above error by considering all possible cases
    // thoroughly. E.g., if(Femto.mode==0) ... 2010/2/12  !!!


    if(pCurr->msdata.femto_mode==0)//MS連到Macro
    {
        if(Permutation==1) //FUSC: intra-cell, inter-sector interference
        {
            temp_polar = pCurr->msdata.cart2pol(BSOxy[0],pCurr->msdata.position);
            temp_prop_loss=pathloss(0,BSOxy[0],pCurr->msdata.position);
            for(j=0; j<NUM_SECTOR; j++)
            {
                antenna_gain = antenna_pattern(SECTOR_Steer_dir[j],temp_polar.th);
                if(j!=pCurr->msdata.ssector)
                {
                    //intra-cell interference from antenna side lobe
                    temp_I = Max_DL_EIRP + antenna_gain - temp_prop_loss - 10*log10(total_carrier);
                    I_charge_DL+=pow(10,temp_I/10);//(mW)linear
                }// if
            }// for
        } // end of Permutation=1

        for(i=1; i<NUM_CELL; i++) //interference from other BS
        {
            temp_polar=pCurr->msdata.cart2pol(BSOxy_MAI[i],pCurr->msdata.position);
            if(pCurr->msdata.on_street==1)
                temp_prop_loss=pathloss(0,BSOxy[i] ,pCurr->msdata.position);
            else
                temp_prop_loss=pathloss(3,BSOxy[i],pCurr->msdata.position);

            // cell sub_channel
            for(j=0; j<NUM_SECTOR; j++)
            {
                antenna_gain=antenna_pattern(SECTOR_Steer_dir[j],temp_polar.th);
                if(Permutation==0)
                {
                    if(j == pCurr->msdata.ssector )
                    {
                        temp_I = Max_DL_EIRP + antenna_gain - temp_prop_loss - 10*log10(total_carrier);
                        I_charge_DL+=pow(10,temp_I/10);//(mW)linear

                    }// if
                }// if(Permutation==0)
                else  //FUSC
                {
                    temp_I = Max_DL_EIRP + antenna_gain - temp_prop_loss - 10*log10(total_carrier);
                    I_charge_DL+=pow(10,temp_I/10);//(mW)linear
                }// else
            } // for(j=0;
        } //end of for(i...)

        //----------interference of near femtocell----------//
        for(i=0; i<pCurr->msdata.fs_near_num; i++)
        {
            if(pCurr->msdata.on_street ==1)
                temp_prop_loss = pathloss(2,fsdata[pCurr->msdata.FS_NEAR[i]].position,pCurr->msdata.position);
            else
                temp_prop_loss = pathloss(1,fsdata[pCurr->msdata.FS_NEAR[i]].position,pCurr->msdata.position);
            j=0;
            while(j<fsdata[pCurr->msdata.FS_NEAR[i]].ms_num && j<3)
            {
                if(mbs_sub_channel_mapping(pCurr->msdata.ssector,pCurr->msdata.sub_channel_index) == fs_sub_channel_mapping(fsdata[pCurr->msdata.FS_NEAR[i]].sector_index,fsdata[pCurr->msdata.FS_NEAR[i]].sub_channel_index[j]))
                {
                    if(Permutation == 0)
                        temp_I = fsdata[pCurr->msdata.FS_NEAR[i]].FS_DL_EIRP - temp_prop_loss - 10*log10(total_carrier) + 10*log10(30.0) - 10*log10(2.0);
                    else
                        temp_I = fsdata[pCurr->msdata.FS_NEAR[i]].FS_DL_EIRP - temp_prop_loss - 10*log10(total_carrier) + 10*log10(30.0) - 10*log10(2.0);
                    I_distribute_DL += pow(10,temp_I/10);
                } // if(fs_sub_channel_mapping...
                j ++;
            }// while(j<fsdata....
        } //end for all neighbor FS ...

        temp_Interference=10*log10(I_charge_DL + I_distribute_DL + pow(10,N_DL/10));

    }//end of Femto_mode = 0
    else if(pCurr->msdata.femto_mode > 0)  // MS connected to FS
    {
        int temp_sector = 0;
        // calculate the interference from co-channel Macro BSs
        for(i=0; i<NUM_CELL; i++)
        {
            temp_polar=pCurr->msdata.cart2pol(BSOxy_MAI[i],pCurr->msdata.position);
            if(pCurr->msdata.on_street==1)
                temp_prop_loss=pathloss(0,BSOxy[i] ,pCurr->msdata.position);
            else
                temp_prop_loss=pathloss(3,BSOxy[i],pCurr->msdata.position);

            for(j=0; j<NUM_SECTOR; j++)
            {
                antenna_gain=antenna_pattern(SECTOR_Steer_dir[j],temp_polar.th);
                if(Permutation ==0)
                {
                    temp_sector = (int) floor((float)fs_sub_channel_mapping(pCurr->msdata.ssector,pCurr->msdata.sub_channel_index) / Sub_channel_num );
                    if(j==temp_sector)
                    {
                        temp_I = Max_DL_EIRP + antenna_gain - temp_prop_loss - 10*log10(total_carrier);
                        I_distribute_DL+=pow(10,temp_I/10);//(mW);dB->linear

                        outfile<< temp_I<< "\t";
                    }
                }// if(Permutation ==0)
                else //FUSC
                {
                    temp_I = Max_DL_EIRP + antenna_gain - temp_prop_loss - 10*log10(total_carrier);
                    I_distribute_DL+=pow(10,temp_I/10);//(mW);dB->linear
                }
            }//end of for(j)
        } // end for {i...}

        //calculate the interference from inter-cell FSs
        for(i=0; i<pCurr->msdata.fs_near_num; i++)
        {
            temp_polar = pCurr->msdata.cart2pol(fsdata[pCurr->msdata.FS_NEAR[i]].position,pCurr->msdata.position);
            if(pCurr->msdata.on_street ==1)
                temp_prop_loss = pathloss(2,fsdata[pCurr->msdata.FS_NEAR[i]].position,pCurr->msdata.position);
            else
                temp_prop_loss = pathloss(1,fsdata[pCurr->msdata.FS_NEAR[i]].position,pCurr->msdata.position);
            j=0;
            while(j<fsdata[pCurr->msdata.FS_NEAR[i]].ms_num && j<3)
            {
                if(fs_sub_channel_mapping(pCurr->msdata.ssector,pCurr->msdata.sub_channel_index) == fs_sub_channel_mapping(fsdata[pCurr->msdata.FS_NEAR[i]].sector_index,fsdata[pCurr->msdata.FS_NEAR[i]].sub_channel_index[j]))
                {
                    if(Permutation == 0)
                        temp_I = fsdata[pCurr->msdata.FS_NEAR[i]].FS_DL_EIRP - temp_prop_loss - 10*log10(total_carrier) + 10*log10(30.0) - 10*log10(2.0);
                    else
                        temp_I = fsdata[pCurr->msdata.FS_NEAR[i]].FS_DL_EIRP - temp_prop_loss - 10*log10(total_carrier) + 10*log10(30.0) - 10*log10(2.0);
                    I_distribute_DL += pow(10,temp_I/10);
                } // if(fs_sub_channel_mapping...
                j ++;
            }// while(j<fsdata....
        } //end for all neighbor FS ...
        outfile<<temp_I<<std::endl;

        temp_Interference= 10*log10(I_charge_DL+I_distribute_DL+pow(10,N_DL/10));

    } //end of DL interference calculation

} // end of interfer_calculation
*/
/*
void LinkList::SINR_calculation(int Permutation,MSNODE* pCurr)
{
    if(pCurr->msdata.femto_mode==0)
    {
        if(Permutation == 0)
            Eb_No_DL=S_DL - temp_Interference - MS_Nosie_figure;
        else
            Eb_No_DL=S_DL+10*log10(Proc_gain_DL_FUSC)-temp_Interference - MS_Nosie_figure;
        pCurr->msdata.DL_Eb_No=Eb_No_DL;
    }
    else
    {
        Eb_No_DL=S_DL-temp_Interference - MS_Nosie_figure;
        pCurr->msdata.DL_Eb_No=Eb_No_DL;
    }


    if(pCurr->msdata.temp_test==1)
    {
        temp_SINR +=pCurr->msdata.DL_Eb_No;
        // temp_SINR has no use
        std::ofstream outfile;
        outfile.open("Eb_No_total.xls",std::ios::app);
        outfile <<pCurr->msdata.femto_mode<<"\t"<<(pCurr->msdata.position.x)*1000<<"\t"<<pCurr->msdata.position.y<<"\t"<<S_DL<<"\t"<<temp_Interference<<"\t"<<pCurr->msdata.DL_Eb_No<<std::endl;

        if(pCurr->msdata.femto_mode ==0)
        {
            temp_SINR_BS +=pCurr->msdata.DL_Eb_No;
            // temp_SINR_BS has no use
            std::ofstream outfile;
            outfile.open("Eb_No_0.xls",std::ios::app);
            outfile <<pCurr->msdata.scell<<"\t"<<pCurr->msdata.on_street<<"\t"<<pCurr->msdata.distance(BSOxy[0],pCurr->msdata.position)<<"\t"<<S_DL<< "\t"<<temp_Interference<<"\t"<<pCurr->msdata.DL_Eb_No<<"\t"<<pCurr->msdata.sub_channel_index<<"\t"<<mbs_sub_channel_mapping(pCurr->msdata.ssector,pCurr->msdata.sub_channel_index)<<std::endl;
            SINR_BS_counter ++;
        }
        else
        {
            temp_SINR_FS +=pCurr->msdata.DL_Eb_No;
            // temp_SINR_FS has no use
            std::ofstream outfile;
            outfile.open("Eb_No_1.xls",std::ios::app);
            outfile <<pCurr->msdata.sFS<<"\t"<<pCurr->msdata.sub_channel_index<<"\t"<<pCurr->msdata.on_street<<"\t"<<pCurr->msdata.distance(fsdata[pCurr->msdata.sFS].position, pCurr->msdata.position)<<"\t"<<pCurr->msdata.position.x<<"\t"<<pCurr->msdata.position.y<<"\t"<<S_DL<<"\t"<<temp_Interference<<"\t"<<pCurr->msdata.DL_Eb_No<<std::endl;
            SINR_FS_counter ++;
        }

        Eb_No_measure_counter ++;
        Eb_CDF_measurement(pCurr->msdata.DL_Eb_No, pCurr->msdata.femto_mode);
    }
} // end of SINR_calculation

*/
