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
 *	@(#)mbs.cpp	11/02/2010
 */

#include "stdhead.h"
#include "external_parameters.h"
#include "SimulatorParameter.h"
#include "GenFingerprintBase.h"

//============================Global Parameters=======================================
double Density		  = HOUSE_DENSITY;
int Num_SCH_per_MS_DL  = 1;             //Max. number of DL sub-channel can be allocated for each MS
int FS_Sub_channel_num =0;                       //number of subchannels available to Femtocells within a sector
int Sub_channel_num; //PUSC channelnumber
double SUM_ncall = 0.0;            // number of new call arrival events
INT16 Serial_seed = 0; // ID of each simulated MS
double Max_speed = 30;             // (km/
int FS_NUM_INDEX = 0;   //
double MS_use_femto_num = 0.0;//用紀錄有多少個MS是使用Femto
double temp_SINR=0;   // Sum of SINR of all MS
double temp_SINR_BS=0;
double temp_SINR_FS =0;
long seed_sub_channel = 675808621;
long seed_fs_position = 134547324;

double R1=R*(pow(3*K,0.5));  // the distance between co-channel cells depends on reuse factor


XYAXIS BSOxy[NUM_CELL] =
{
    {R*0.0  ,R*0.0      },  {R*0.0  ,R*1.732    },  {R*1.5  ,R*0.866    },  {R*1.5  ,R*-0.866   },
    {R*0.0  ,R*-1.732   },  {R*-1.5 ,R*-0.866   },  {R*-1.5 ,R*0.866    },  {R*0.0  ,R*3.464    },
    {R*1.5  ,R*2.598    },  {R*3.0  ,R*1.732    },  {R*3.0  ,R*0.0      },  {R*3.0  ,R*-1.732   },
    {R*1.5  ,R*-2.598   },  {R*0.0  ,R*-3.464   },  {R*-1.5 ,R*-2.598   },  {R*-3.0 ,R*-1.732   },
    {R*-3.0 ,R*0.0      },  {R*-3.0 ,R*1.732    },  {R*-1.5 ,R*2.598    }
};

XYAXIS BSOxy_MAI[NUM_CELL] =
{
    {R1* 0.0  ,R1* 0.0}, {R1*0.0   ,R1*1.0 }, {R1*0.866 ,R1*0.5 }, {R1*0.866 ,R1*-0.5}, //0--3
    {R1* 0.0  ,R1*-1.0}, {R1*-0.866,R1*-0.5}, {R1*-0.866,R1*0.5 }, {R1*0.0   ,R1*2.0 }, //4--7
    {R1*0.866 ,R1*1.5 }, {R1*1.732 ,R1*1.0 }, {R1*1.732 ,R1*0.0 }, {R1*1.732 ,R1*-1.0}, //8--11
    {R1*0.866 ,R1*-1.5}, {R1* 0.0  ,R1*-2.0}, {R1*-0.866,R1*-1.5}, {R1*-1.732,R1*-1.0}, //12--15
    {R1*-1.732,R1*-0.0}, {R1*-1.732,R1*1.0 }, {R1*-0.866,R1*1.5}
};                      //16--18


VectorFP BSVectorFP[NUM_CELL];


//-----Adjacent cell identification for wrap around-----//
//----------the matrix start from adjcent[0][0]---------//
INT16 adjcent[NUM_CELL][NUM_CELL] =
{
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18},
    {1,7,8,2,0,6,18,11,15,14,9,10,3,4,5,16,17,13,12},
    {2,8,9,10,3,0,1,15,14,13,17,16,11,12,4,5,6,18,7},
    {3,2,10,11,12,4,0,8,9,17,16,15,7,18,13,14,5,6,1},
    {4,0,3,12,13,14,5,1,2,10,11,7,18,17,9,8,15,16,6},
    {5,6,0,4,14,15,16,18,1,2,3,12,13,9,8,7,11,10,17},
    {6,18,1,0,5,16,17,12,7,8,2,3,4,14,15,11,10,9,13},
    {7,11,15,8,1,18,12,10,16,5,14,9,2,0,6,17,13,4,3},
    {8,15,14,9,2,1,7,16,5,4,13,17,10,3,0,6,18,12,11},
    {9,14,13,17,10,2,8,5,4,12,18,6,16,11,3,0,1,7,15},
    {10,9,17,16,11,3,2,14,13,18,6,5,15,7,12,4,0,1,8},
    {11,10,16,15,7,12,3,9,17,6,5,14,8,1,18,13,4,0,2},
    {12,3,11,7,18,13,4,2,10,16,15,8,1,6,17,9,14,5,0},
    {13,4,12,18,17,9,14,0,3,11,7,1,6,16,10,2,8,15,5},
    {14,5,4,13,9,8,15,6,0,3,12,18,17,10,2,1,7,11,16},
    {15,16,5,14,8,7,11,17,6,0,4,13,9,2,1,18,12,3,10},
    {16,17,6,5,15,11,10,13,18,1,0,4,14,8,7,12,3,2,9},
    {17,13,18,6,16,10,9,4,12,7,1,0,5,15,11,3,2,8,14},
    {18,12,7,1,6,17,13,3,11,15,8,2,0,5,16,10,9,14,4},
};
