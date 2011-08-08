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
 *	@(#)channel_allocation.h	11/02/2010
 */ 
//

#pragma once
#ifndef ALLOCATION_H
#define ALLOCATION_H


//---------------------------------------------------------------------------//
//                  IEEE 802.16e OFDMA   mode                                 //
//----------------------------------------------------------------------------//
/////--------system parameters--------/////
double frame_duration;           //(sec)frame duration
double symbol_duration;      //(sec)(consider Tg=Tb/8)
//--Link Budget Related--//
double Max_DL_power;	  //(dBm)Max. downlink transmit power at BS (45 Watts)
double Max_UL_power;             //(dBm)Max. uplink transmit power at MS (0.25 Watts)
const double Max_FS_UL_power    = 10.47;          //(dBm)Max. uplink transmit power at FS

double Max_DL_EIRP;//(dBm)equivalent isotropic radiated power (EIRP) of BS
double Max_UL_EIRP;

double f_carrier;           //(MHz)carrier frequency

double Num_SCH_per_MS_UL;             //Max. number of UL sub-channel can be allocated for each MS
double Proc_gain_DL_FUSC;            //processing gain
double Proc_gain_DL_PUSC;            //processing gain

double  SUM_block;            // number of block events 
double  SUM_dropping;            // dropping events

double DL_capacity_measure_counter;
double DL_Throughput_measure_counter;
double DL_capacity_measure_add;
double DL_Throughput_measure_add;
double Eb_No_measure[150];
double SINR_PDF[150];
double SINR_BS[150];
double SINR_FS[150];
double SINR_BS_counter;
double SINR_FS_counter;
double Eb_No_measure_counter;
double DL_Throughput[150];

int MCS;

#endif
