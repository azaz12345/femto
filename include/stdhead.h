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
 *	@(#)stdhead.h	11/16/2010
 */

//
//2010.11.16 modified
//
#include "fbs.h"

#pragma once


#ifndef STDHEAD_H
#define STDHEAD_H

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include  <iostream>
#include  <iomanip>
#include  <fstream>
#include  <math.h>
#include  <time.h>
#include <stdlib.h>
#include <valarray>
#include "GenFingerprintBase.h"
#include "SimulatorParameter.h"
#include "EventQueue.h"
#include "tool.h"

extern  EventQueue EvQue_;


#define    SERVICE_DISRUPTION_TIME       "Service_Disruption_Time.dat"
#define    STASTICS     "Stastics.dat"
#define    T_RADIO      "T_radio.dat"
#define    REENTRY      "Reentry.dat"
#define    Eb_No_CDF    "Eb_No_CDF.dat"
#define    Tx_power_CDF "Tx_power_CDF.dat"
#define    TEST         "TEST.dat"


typedef struct
{
    double x;
    double y;
} XYAXIS;

typedef struct
{
    double th;
    double r;
} POLAXIS;




/*RadioBand分成w1 w2 w3分別為0 1 2
    (目前採用隨機選擇)
 */

typedef struct
{
    int serial;
    int marco_index;
    int sector_index;
    int block_num; //block
    int first_sector_index;

    struct UserInfo
    {

        int TotalNumUser;   //femtocell人數上限
        int CurrNumUser;    //當前使用者人數

    } UserInfo;
    int RadioBand;				//0~2
    int DataChannel[10];    //有使用則設為1

    LevelFP<FP_LEVEL > 	LevelFingerprint;
    VectorFP		VectorFingerprint;

    int sub_channel_index[2];
    int ms_num;
    int fs_mode ;
    int CSG_GROUP;
    bool Power_State ;
    double FS_DL_EIRP;
    double FS_MAX_DL_power;
    XYAXIS position;
} FS_INFO;

typedef struct
{
    int FS_num[100];
    int FS_num_length;
} BLOCK_RECORD;

enum  SERVICE  {RT,NRT};                         //service types -> real-time (RT), non real-time

#define CELL7                 7               // number of first tier cells
#define NUM_CELL             19               // number of total cells
#define NUM_SECTOR            3               // number of sectors in each cell
#define Pathloss_model        1               // 0 -> ITU; 1 -> EMD

#define All_carriers              840           //total number of sub-carriers
#define Num_channel_DL_PUSC       30   //number of sub-channels
#define Carriers_perCH_DL_PUSC    28             //number of sub-carriers of each sub-channel
#define Data_Carriers_perCH_DL_PUSC	  24         //2010.11.16 modified
#define Num_channel_DL_FUSC       32             //number of sub-channels(per sector); why is it 32? tch 2010/2/11
#define Carriers_perCH_DL_FUSC    48             //number of sub-carriers of each sub-channel

#define FS_NUM                    10000            // number of relay stations (FS) in each cell
#define FS_SECTOR_NUM             4               // number of beams of each FS
#define NEAR_FS_NUM               2000



//   parameters

const double BW					= 10.0;            //(MHz)channl bandwidth
const double Noise_power        = -174+10*log10(BW*1000000); //(dBm) thermal noise power within whole band
const double Noise_power_density    = -174;        //(dBm/Hz)
const double A_subcarrier_frequency = 10.9375*1000;   //Hz

const double Back_off           = 5;              //(dB)amplifier backoff of OFDM transmitter
const double BS_Noise_figure    = 5;              //(dB)noise figure at receiver (BS)
const double MS_Nosie_figure	= 7;			  //(dB)noise figure at receiver (MS)
const double BS_antenna_gain    = 17;             //(dBi)antenna gain at BS

const double Max_FS_DL_power    = 20.3;          //(dBm)Max. downlink transmit power at FS

const double Street_block_size  = 0.2;            //(km) the distance from one cross raod to another
const double Street_width   = 0.03;           //(km) the width of the street
const double block_unit     = Street_block_size+Street_width;
const double House_width	= 0.02;
const double Real_House_width	= 0.014;

//--------traffic model----------------//
const double RT_prob            = 1;             //prob. on new arrival call to use real-time (RT) services
const double RT_time            = 200;           //(sec) mean service time of real-time services

const double  PI                 = 3.14159266;
const double  SECTOR_Steer_dir[NUM_SECTOR]        = {0, 2*PI/3, 4*PI/3};  //steering direction of the BS antennas
const double  K                   = 1;            // reuse factor
const double  R                   = 0.6;            //(km) cell radious


//  class_def
//
class COORD
{
    friend class MSNODE;
public:
    XYAXIS   cart;
    POLAXIS  pol;
public:
    XYAXIS   pol2cart(POLAXIS pol);
    POLAXIS  cart2pol(XYAXIS reference, XYAXIS target);
    XYAXIS   cart_change(XYAXIS origin , XYAXIS destination);
    XYAXIS   init_position(long *seed_position_x,  long *seed_position_y, double R);
    double   distance(XYAXIS c1, XYAXIS c2);
};

//*******************************************************************************//
//   Define the class "MSNODE", which generate the object for mobile station     //
//*******************************************************************************//
//   Class "MSINFO" generate the sub-object among "MSNODE" to record all MS      //
//   related parameters                                                          //
//*******************************************************************************//

class MSINFO:public COORD
{
    friend class MSNODE;
public:

    int            ID;
    double         session;              //service time for circuit switched service
    XYAXIS         position;             //relative axis to serving BS
    INT16          scell;                //current serving BS index
    INT16          ssector;              //current serving sector index in BS
    INT16		 on_street;			   // 0 -> MS stay indoor; 1-> MS is on street.
    INT16		 first_sector_index;   // MS¬O¸first_sector_index
    INT16		 fs_near_num;	       // Fetmo
    INT16		 sub_channel_index;    // BS¸Sub_channel
    int		 temp_test;            // flag indicating whether the MS is inside tag macro cell
    int		 block_index;



    double         proploss[NUM_CELL];    //dB
    INT16          MCS_DL;               //Modulation and Coding Scheme (MCS) for downlink
    double         DL_Eb_No;             //dB,for traffic channel
//--------------------for femto mode----------------------------//
    INT16          femto_mode;           //0 -> MS connect to BS, 1 -> MS connect to FS
    INT16          sFS;                  //current serving FS index
    double         FS_RSSI[NEAR_FS_NUM];

    INT16		 FS_NEAR[NEAR_FS_NUM]; //record the neighbor fs id
//--------------------for mobility model----------------------------//
    double         direction;              // direction of user(angle in pi)
    double         speed;                  // speed of user(km/hr)
//--------------------for traffic model---------------------//
    enum SERVICE   con_type;
//--------------------for shadow fading----------------------------//
    INT16          serial;
//--------------------for Handoff algorithms------------------//
    double         RSSI[NUM_CELL];
//-------------------Call time------------------------------//
    bool           CallState;
    double         CallTime;
    double         Call_Interval;

//---------------------------CSG Whitelist---------------------------------//

    int CSG_GROUP;
//-------------------------------SCAN---------------------------------------//
	double ScanPeriod;
	double ScanStartTickTime;
//------------------------------Fingerprint--------------------------------------//
	LevelFP<FP_LEVEL > LevelFingerprint;
    VectorFP		VectorFingerprint;


//--------------------------------------//
    MSINFO() //constructor to initialize
    {
        INT16 i,j;
        session=0.0;
        fs_near_num;
        sub_channel_index = 0;
        cart.x =0.0;
        cart.y = 0.0;
        pol.r = 0.0;
        pol.th =0.0;
        temp_test=0;
        scell=0;
        ssector=0;
        femto_mode=0;
        sFS=0;
        DL_Eb_No=0;
        MCS_DL=1;
        CallState = false;
        CallTime = 0.0;

        ScanPeriod=0;
        ScanStartTickTime=0;

        for(i=0; i<NUM_CELL; i++)
        {
            RSSI[i]=0;
            proploss[i]=0;
        }
        for(j=0; j<NEAR_FS_NUM; j++)
        {
            FS_RSSI[j]=0;
            FS_NEAR[j]=0;
        }
        direction=0.0;
        serial=0;
    }
    ~MSINFO() {}
};

class MSNODE
{
public:

    MSNODE*  pPrevious;
    MSNODE*  pNext;

    MSINFO   msdata;
    std::ofstream  out_file;
//-------------------------------------//
    MSNODE()
    {
        pNext=NULL;
        pPrevious=NULL;
    } //constructor
    ~MSNODE() {}
//-------------------------------------//
}; //end of class MSNODE





//****************************************************************************//
//   Define the class "LinkList", which manage operation between BS and MS    //
//   Most system operation and RRM functions are included in this class       //
//****************************************************************************//
class LinkList:public FBS
{
protected:
    long   *seed_arrival;
    long   *seed_session;
    long   seed_log_normal1;   //generate longterm fading
    long   seed_log_normal2;   //generate longterm fading
    long   *seed_cell;
    long   seed_position_x ;
    long   seed_position_y ;

public:
    MSNODE* pFirst;
    MSNODE* pLast;
    INT16   length;

    LinkList()
    {
        pFirst=NULL;
        pLast=NULL;
        length=0;
        //------event timer initialization-------//
        //--------------------------------------//
        seed_arrival=new long;
        seed_session=new long;
        seed_cell = new long;
    }  //constructor
    //-------------------------------------//
    ~LinkList()  //destructor
    {
        //(deletes all links)
        MSNODE* pCurrent=pFirst;       //start at beginning
        while(pCurrent != NULL)        //until end of list
        {
            MSNODE* pTemp=pCurrent;   //remember current
            pCurrent=pCurrent->pNext; //move to next link
            delete pTemp;             //delete old current
        } //while//

        delete seed_arrival;
        delete seed_session;
        delete seed_cell;
    } //destructor
    //----------------------------------------//
    void     sys_setup();
    bool     isEmpty();
    double   insertLast(double t_new, double traffic_load, int Femto_mode);  //insert new MS at end of linklist
    double   init_msnode(double t_new, MSNODE* pCurr, int Femto_mode);
    MSNODE*  rmv_msnode(MSNODE* pCurr);
    void     new_call_CAC(double current_time, int Femto_mode, int Permutation);
    void		node_departure();
    void     DL_sub_channel_initial();
    //---------------------------------------//
//   void     SINR(int Femto_mode,int Permutation);
    void     LIF_assign_channel(int Femto_mode,int Permutation,MSNODE* pCurr);
//    void     DL_signal_calculation(int Femto_mode,int Permutation,MSNODE* pCurr);
//    void     interfer_calculation(int Femto_mode,int Permutation,MSNODE* pCurr);
//   void     SINR_calculation(int Permutation,MSNODE* pCurr);
    double   antenna_pattern(double steer_dir, double target_dir);
    double   pathloss(int channel_index, XYAXIS TX_location, XYAXIS RX_location);
    //---------------------------------------//
    void     process_event(double current_time, int Femto_mode,int Permutation);
    //--- Statistic Function ----------------//
    void     reset_sys();
    void     capacity_measurement(int Permutation, int Femto_mode);
    void		Eb_CDF_measurement(double Eb_No_value, int mode);
    void		Throughput_CDF();
    //--- Handoff ---------------------------//
    double   SINR_to_BER(double Eb_No, int Mod_order);
    //---------------------------------------//
}; //End of class LinkList




class LinkListEvPrcs:public EventProcess{

public:
    LinkListEvPrcs(LinkList* linklist){
        linklist_ = linklist;
    };

    virtual void ProcessEvent(int type, void* msnode){
        switch(type){
            case INSER_NODE:
            {

                linklist_->insertLast(0,0,1);


                extern long Seed;
                Event event;
                double ServTime = ExpRand(200,&Seed);
                event.EventType_= RMV_NODE;
                event.time_     = EvQue_.getClock()+ServTime;
                event.node_     = linklist_->pLast;
                event.EvProc_   = this;
                printf("##event.time_:%f\tServTime:%f\n",event.time_,ServTime);
                EvQue_.pushEvent(event);


                break;
            }
            case RMV_NODE:
            {
                //printf("RMV_NODE\n");
                //printf("Memmory addr:%d\n",msnode);
                linklist_->rmv_msnode((MSNODE*)msnode);

                break;

            }

        }

    };


private:

    LinkList* linklist_;

};


#endif
