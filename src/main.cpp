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
 *	@(#)main.cpp	11/02/2010
 */

//********************************************************************//
//                   Simulation platform for                          //
//      Next Generation Wireless Communication Networks               //
//                    with Femto cells                                //
//      Developed at Network Swtching Lab., CCU                       //
//      http://www.cn.ee.ccu.edu.tw/                                  //
//      Authors: Pei-Chen Lu, for femto cell simulation               //
//               Ting-Chao Hou, make it make-able on Linux 2010/2/18  //
//               Chih-Yu Lu, Song-Chieh Shen, Feng-YU Tian            //
//                Shi-Ting Tseng, Chien-Shih Chen, make further       //
//                funcational partition  2010/11/02                   //
//      E-mail: ieetch@ccu.edu.tw                                     //
//                                                                    //
//      Derivied from:                                                //
//      OFDMA radio access network simulation platform                //
//      Developed by Broadband Radio Access Systems Lab.              //
//      http://braslab.cm.nctu.edu.tw/                                //
//      Author:I-Kang Fu                                              //
//********************************************************************//
#include  "stdhead.h"
#include "fbs.h"
#include "external_parameters.h"
#include "PathGenerator.h"
#include <vector>
#include <fstream>
#include "GenMultiUser.h"
#include "MobileStation.h"
#include "CSG.h"
#include "Simulator.h"
//#include "TrafficModel.h"


/********* Simulation Setup *********/
int   Sim_terminate = 3000;    // Condition to terminate simulate (in terms of total number of MS arrivals)

//****** Global Parameters ********//
int Femto_mode = 1;      // 0 -> No Femto ; 1 -> Fixed Femto
int Permutation = 0;      // 0 -> PUSC permutation; 1 -> FUSC permutation

const double Sim_resolution       = symbol_duration;

LinkList Mslink;
//*********************************//
extern void  assign_system_parameters();

//---------------------------------//
int main()
{

    //---------------simulation initialization---------------//

    double t_new_call=0.0;
    double t_end = Sim_terminate*Sim_resolution;
    double t_abs=0.0;
    double Load_voice=100;
    double traffic_load=Load_voice*NUM_CELL;
    double progress_counter=0.05;
    double temp_SINR1=0;
    double total_num=0;
    double confid_value = 0;
    INT16  i, j, n =0,z=0;
    long *Seed_sys;
    Seed_sys  = new long;
    *Seed_sys = 8888;
    int channel_num[NUM_CELL];

    assign_system_parameters();

    if(Permutation ==0) //PUSC, subchannels are divided among sectors
    {
        Sub_channel_num = (int)(Num_channel_DL_PUSC / NUM_SECTOR);
        // number of channels for macro-serving MS in a sector
        FS_Sub_channel_num = (int)(Num_channel_DL_PUSC / Num_SCH_per_MS_DL)-Sub_channel_num;
        // number of channels that can be allocated to femto-serving MS
    }
    else   //FUSC, all sectors share the same set of subchannels
    {
        Sub_channel_num = (int)(Num_channel_DL_FUSC / Num_SCH_per_MS_DL);
        FS_Sub_channel_num = (int)(Num_channel_DL_FUSC / Num_SCH_per_MS_DL);
    }

//      Pointer-based multi-dimensional arrays 2010/2/11
//
//      int*** DL_sub_channel_sum;  in class SubChannel

    Mslink.DL_sub_channel_sum = new int**[NUM_CELL];
    for(i = 0; i < NUM_CELL; i++)
        Mslink.DL_sub_channel_sum[i] = new int*[NUM_SECTOR];
    for(i = 0; i < NUM_CELL; i++)
    {
        for(j = 0; j < NUM_SECTOR; j++)
            Mslink.DL_sub_channel_sum[i][j] = new int[Sub_channel_num];
    }


    Mslink.DL_FS_sub_channel = new int*[NUM_SECTOR];
    for(i = 0; i < NUM_SECTOR; i++)
        Mslink.DL_FS_sub_channel[i] = new int[FS_Sub_channel_num];

    n=1; // number of iteration
    double* arry = NULL;
    arry= new double[n];

    Mslink.sys_setup();  //Assign various seeds


    /***************************
        Generator of Path
        先假設模擬時間為3600秒
    ****************************/
    PathGenerator* PathGen = new PathGenerator(3600,0.5,&BSOxy_MAI);
    CSG* _CSG = new CSG() ;
    vector<WayPoint> PathDataBase;


    for (i=0; i<n; i++)
    {
        if(Femto_mode==1)
//            Mslink.fs_position(Permutation);
        //generate femto APs randomly according to density and also
        //assign the channel for one MS

        t_abs = 0.0;
        SUM_ncall =0;
        // generate Sim_terminate (3000) MS

/************************************全面改寫的部分*******************************/
		Simulator smu;

		smu.Start();

        printf("\n---------------段落-------------\n");
        system("pause");
/*
        LinkList USERLIST;

        struct GenMultiUserParam P1;
        P1.numuser = 3000;
        P1.simulationtotaltime = 3600;
        P1.ticktime = 0.5;
        P1.permutation = Permutation;

		GenMultiUser TESTobj(&USERLIST, P1);
        //GenMultiUser TESTobj(&USERLIST,3000, 3600, Permutation);
        //ticktime為0.5
        //simulationtotaltime為3600秒

        for(float i=0.5;i<=3600;i=i+0.5){
            printf("時間:%f\n",i);
            TESTobj.UpdateAllPosition(USERLIST.pFirst,0);

//start for openMP
            MSNODE** arrayMSNODE = new MSNODE*[3000];
            MSNODE* currMS = USERLIST.pFirst;
            for(int i=0;
                currMS != NULL;
                i++ )
            {

                arrayMSNODE[i] = currMS;
                currMS = currMS->pNext;
            }

*/
            //MobileStationBase* MS = new MobileStationBase(&BSOxy,&fsdata, Permutation);
            /*
            MobileStationBase** MS = new MobileStationBase*[3000];
            for(int i=0;i<3000;i++){
                MS[i] = new MobileStationBase(&BSOxy_MAI,&fsdata, Permutation);
            }
            */

/*
            //#pragma omp parallel for
            for(int i=0; i<3000; i++){
                MobileStationBase MS(&BSOxy_MAI,&fsdata, Permutation);
                MS.UpdateMSINFO(arrayMSNODE[i]);
                //delete  MS[i];

            }
            delete [] arrayMSNODE;
            //delete [] MS;
//end of openMP
*/


/*原本的方式
            for(MSNODE* currMS = USERLIST.pFirst;
                currMS != NULL;
                currMS = currMS->pNext )
            {

                MS.UpdateMSINFO(currMS);

            }
*/

 //       }
/***********************************************************************************/
        while(SUM_ncall < Sim_terminate)                                                //double SUM_ncall = 0.0;           number of new call arrival events
        {                                                                               //int   Sim_terminate = 3000;       Condition to terminate simulate (in terms of total number of MS arrivals)
            for(j=0; j<19; j++)
                channel_num[j]=Num_channel_DL_PUSC;
            // Generate one MS
            MS_use_femto_num = Mslink.insertLast(t_new_call,traffic_load,Femto_mode);
            // Check if the MS can be admitted
            Mslink.new_call_CAC(t_abs, Femto_mode, Permutation);
            // Resource allocation and capacity measurement //SINR and Capacity compute
            Mslink.process_event(t_abs,Femto_mode,Permutation);
            // This only removes one node at pFirst?
            Mslink.node_departure();
        }

        temp_SINR1 = temp_SINR-temp_SINR1;
        total_num = Eb_No_measure_counter-total_num;
        arry[i] = temp_SINR1/total_num;
        // Since temp_SINR and Eb_No_measure_counter do not get reset to 0
        // at the beginning of next iteration, so the above deduct the
        // previous counts

        temp_SINR1=temp_SINR;
        total_num=Eb_No_measure_counter;
    } // end for (n=0 ..9)



    confid_value = confid(arry,n);	 //confidence interval
    //temp = (double)(temp_SINR /Eb_No_measure_counter);
    std::ofstream  outfile;
    //outfile.open("STASTICS.txt",std::ios::app);
    outfile.open("STASTICS.txt");
    outfile <<"Femto Density =   "<< Density << std::endl;
    outfile <<"Femto Number =  " << FS_NUM_INDEX<<std::endl;
    outfile <<"Average SINR =  "<< temp_SINR / Eb_No_measure_counter << std::endl;
    outfile <<"Average Marco BS SINR =  " << temp_SINR_BS /SINR_BS_counter << std::endl;
    outfile <<"Average Femto BS SINR =  " << temp_SINR_FS /SINR_FS_counter << std::endl;
    outfile << "Femto Mode Ratio = " << (MS_use_femto_num /Eb_No_measure_counter)*100<<"%"<<std::endl;
    outfile << SINR_BS_counter << "  "<<SINR_FS_counter<<std::endl;
    outfile << "confidence interval =  " << confid_value<< std::endl;
    for(i=0; i<n; i++)
        outfile << arry[i]<<std::endl;
    Frecords(Femto_mode,Permutation);

    return 0;
} // End of Main
