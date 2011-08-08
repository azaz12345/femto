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
 *	@(#)tools.cpp	11/02/2010
 */

#include "stdhead.h"
#include "tool.h"

//*******************************************//
//   exponential random number generators    //
//*******************************************//
// Random Number Generator (RNG) for U(0,1)  //
//*******************************************//
double Rand(void)
{
    static int     flag=0;
    static long    seed;           /* seed of RNG */
    double  A=16807.0,
              P=2147483647.0,
                C=4.6566129E-10;
    static double  x;

    //printf("RADN()的x:%f\n",x);

    if (flag==0)
    {
        /* randoM seed from clock */
        time( &seed );
        seed = (seed % 10000)*10;
    }
    x = (double) seed;
    x = fmod(x=x*A,P)+(double)0.5;
    seed = (long) x;
    flag = 1;
    return (double) (x*C);
}

double Rand(long *seed)
{
    long   tmp;
    double num;
    double A=16807.0,
             P=2147483647.0,
               C=4.6566129E-10;
    double x;

    num=0.0;
    tmp= *seed;
    /* make the uniformly distributed random var "num" */
    x=(double)tmp;
    x=fmod(x=x*A,P)+(double)0.5;
    *seed=(long) x;
    num=x*C;
    return num;
}

double ExpRand(double mean,  long* seed_session)
{
    return (-mean*log(Rand()));
}

//   Macro cell:
//   sector 0 uses channels 0-9, sector 1 uses channel 10-19,
//   sector 2 uses channels 20-29
//   Orthogonal channel assignment is assumed between macro and femto
//   Therefore femto in sector 1 uses channels 10-29, femto in sector 1
//   uses channels 0-9 and 20-29, femto in sector 2 uses channel 0-19
//
int fs_sub_channel_mapping(int sector, int sub_channel_index)
{
    if(sector ==0)
        return (10+sub_channel_index);
    else if (sector ==1)
    {
        if(sub_channel_index < 10)
            return sub_channel_index;
        else
            return (10+sub_channel_index);
    }
    else
        return sub_channel_index;

}

int mbs_sub_channel_mapping(int sector,int sub_channel_index)
{
    if(sector ==1)
        return (10+sub_channel_index);
    else if (sector ==2)
        return (20+sub_channel_index);
    else
        return sub_channel_index;

}

double confid(double *arry, int n )
{
    /*
    *
    *	c o n f i d
    *
    *	Overall Point estimate and confidence interval calculation
    *
    */
//	 double	conmul[11] = {10000.0, 1000.0, 63.657, 9.925, 5.841, 4.604, 4.032, 3.707, 3.499, 3.355, 3.25}; /* 99 percent */
    double	conmul[11] = {10000.0, 1000.0, 12.706, 4.303, 3.182, 2.776, 2.571, 2.447, 2.365, 2.306, 2.262}; /* 95 percent */
//	 double	conmul[11] = {10000.0, 1000.0,  6.314, 2.920, 2.353, 2.132, 2.015, 1.943, 1.895, 1.860, 1.833}; /* 90 percent */

    int 	i;
    double	smpl_var, temp;
    double  smpl_mean;

    smpl_mean = 0.0;
    for (i=0; i<n; i++)
        smpl_mean += arry[i];

    smpl_mean /= n;

    smpl_var = 0.0;
    for (i=0; i<n; i++)
        smpl_var += ( (arry[i] - smpl_mean) * (arry[i] - smpl_mean) );
    smpl_var /= (n-1);

    temp = conmul[n] * sqrt(smpl_var / n);
    return(temp);

}
//-----------------------------//
// intervals between new calls //
//-----------------------------//
//inline double New_call_rand(double mean, long *seed_arrival)
double New_call_rand(double mean, long *seed_arrival)
{
    return (-mean*log(Rand(seed_arrival)));
}

//----------------------------//
//       call session time    //
//----------------------------//
//inline double Session_rand(double mean,  long *seed_session)
double Session_rand(double mean,  long *seed_session)
{
    return (-mean*log(Rand(seed_session)));
}






