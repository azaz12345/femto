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
 *	@(#)external_parameters.h	11/02/2010
 */


#include "stdhead.h"
#include "GenFingerprintBase.h"

/////--------Global variable----------/////
extern VectorFP BSVectorFP[NUM_CELL];
extern XYAXIS BSOxy[NUM_CELL];
extern BLOCK_RECORD BLOCK[120];
extern FS_INFO fsdata[FS_NUM];
extern INT16 adjcent[NUM_CELL][NUM_CELL];
extern XYAXIS BSOxy_MAI[NUM_CELL];
extern double Max_DL_EIRP;
extern double  SUM_dropping;
extern double Proc_gain_DL_FUSC;            //processing gain
extern double f_carrier;
extern int MS_FS_id; //serving FS id for the MS (pCurr->msdata.sFS)


extern double symbol_duration;
extern int Sub_channel_num; //PUSC channelnumber
extern int FS_Sub_channel_num;                       //number of subchannels available to Femtocells within a sector
extern int Num_SCH_per_MS_DL;             //Max. number of DL sub-channel can be allocated for each MS


extern double Max_speed;             // (km/
extern double Density;
extern int FS_NUM_INDEX;   //

extern INT16        Serial_seed; // ID of each simulated MS
extern long	      seed_fs_position; // Femto location

extern double  SUM_ncall;            // number of new call arrival events
extern double  MS_use_femto_num;
//---------------------------------------//
/////--- Global Function Declartion ---/////
extern double  Rand(void);                               // uniform r.v. generator within [0,1]
extern double  Rand(long *seed);                         // uniform r.v. generator within [0,1]

// defined at tools.cpp
extern int fs_sub_channel_mapping(int sector, int sub_channel_index);
extern int mbs_sub_channel_mapping(int sector,int sub_channel_index);

//----------------------------------------//
extern double  New_call_rand(double mean, long *seed_arrival);
extern double  Session_rand(double mean, long *seed_session);
extern void    Frecords(int Femto_mode, int Permutation);
extern double  confid(double *arry, int n);


//-------------------------------//

extern int fs_first_sector_num;



extern double SINR_BS_counter;
extern double SINR_FS_counter;
extern double Eb_No_measure_counter;
extern double temp_SINR;   // Sum of SINR of all MS
extern double temp_SINR_BS;
extern double temp_SINR_FS;
extern long seed_sub_channel;

