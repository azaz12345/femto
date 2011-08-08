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
 *	@(#)pathloss.cpp	11/02/2010
 */

#include "stdhead.h"
#include "external_parameters.h"


//***********************************************************************************************//
//***********************************************************************************************//
//                                    Propagation loss Module (mainly EMD )                                   //
//***********************************************************************************************//
//***********************************************************************************************//
double LinkList::pathloss(int channel_index, XYAXIS TX_location, XYAXIS RX_location)
{
    double path_loss=0;
    double distance=0;
    double d1=0;
    double d2=0;
//      int temp=0;
    double temp=0;
    double temp1=0;
    POLAXIS temp_polar;
    double angle =0;
    double street_index = 0;
    double BS_first_wall = 0; //BS
    int house_index = 0;
    double pl_urban_micro=0;
    double temp_house_d = 0;
    int floor_num = 0;
    double d_in = 0; // the distance from wall to inside termial
    double d_out = 0;// the distance between the outside termianl and closet point of the wall to the inside terminal
    COORD CAL;


    d1=fabs(TX_location.x-RX_location.x);
    d2=fabs(TX_location.y-RX_location.y);

    distance=pow((pow(d1,2)+pow(d2,2)),0.5);

    // convert from km to cm?
    d1 = (d1+0.000005) * 100000;
    d2 = (d2+0.000005) * 100000;

    temp_polar=CAL.cart2pol(TX_location,RX_location);
    // change range from (-pi, pi) to (0, 2pi)
    if(temp_polar.th < 0)
        angle = temp_polar.th + 2*PI;
    else
        angle = temp_polar.th;

    if(channel_index ==2) //
    {
        //  temp stores the longer dimension?
        if((angle <=PI/4 || angle > 7*PI/4) || (angle > 3*PI/4 && angle <= 5*PI/4))
            temp =  d1;
        else
            temp =  d2;
        floor_num =1;
        // House_width = 0.02km
        temp=temp - (House_width*100000);
        // Real_House_width = 0.014km
        while(temp > (Real_House_width / 2)*100000)
        {
            floor_num = floor_num +2;
            temp = temp- (House_width*100000);
        }

        /***************
        這裡多算一道牆??
        ****************/


    }//end of if

    if(channel_index ==1) //
    {
        if((angle <=PI/4 || angle > 7*PI/4) || (angle > 3*PI/4 && angle <= 5*PI/4))
            temp = d1;
        else
            temp = d2;
        if( temp > (Real_House_width / 2)*100000)
        {
            floor_num ++;
            temp=temp-(0.006*100000);
        }
//		if(abs((Real_House_width / 2)-temp)<0.00000000000000001)
//			temp = Real_House_width / 2;

        if( temp > (Real_House_width / 2)*100000)
        {
            floor_num ++;
            temp=temp-(House_width*100000);
            channel_index = 2; // why?
        }
        while(temp > (Real_House_width / 2)*100000)
        {
            floor_num = floor_num + 2;
            temp = temp- (House_width*100000);
        }
    }//end of if

    if(Pathloss_model ==1)  //EMD
    {
        if(channel_index == 3)
        {
            // figure out din and dout based on relative location
            if(angle <=PI/4 || angle > 7*PI/4)
            {
                street_index = floor(RX_location.x /block_unit);
                if(RX_location.x< ( street_index * block_unit)+(block_unit/2))
                {
                    house_index = (int) ((RX_location.x - (street_index * block_unit))/House_width);
                    BS_first_wall = street_index * block_unit + house_index * House_width + 0.003;

                }
                else
                {
                    house_index = (int) ((RX_location.x - (street_index * block_unit)-Street_width)/House_width);
                    BS_first_wall = street_index * block_unit + house_index * House_width + Street_width+0.003;
                }

                d_in = fabs(RX_location.x - BS_first_wall);
                RX_location.x = RX_location.x - d_in;
                d_out = CAL.distance(TX_location, RX_location);
            }
            else if(angle >PI/4 && angle <= 3*PI/4)
            {
                if(angle>PI/2)
                    angle = PI-angle;
                street_index = floor(RX_location.y / block_unit);
                if (RX_location.y < (street_index * block_unit + block_unit/2))
                {
                    house_index = (int) ((RX_location.y -(street_index * block_unit)) / House_width);
                    BS_first_wall = street_index * block_unit + house_index*House_width + 0.003;
                }
                else
                {
                    house_index = (int) ((RX_location.y - (street_index * block_unit) -Street_width) / House_width);
                    BS_first_wall = street_index * block_unit + house_index * House_width + Street_width + 0.003;
                }
                d_in = fabs(RX_location.y - BS_first_wall);
                RX_location.y = RX_location.y - d_in;
                d_out =  CAL.distance(TX_location, RX_location);
            }
            else if(angle > 3*PI/4 && angle <= 5*PI/4)
            {
                if(angle < PI)
                    angle = PI-angle;
                else
                    angle = angle - PI;
                street_index = floor(RX_location.x /block_unit);
                if (RX_location.x < ( street_index * block_unit)+(block_unit/2))
                {
                    house_index = (int) ((RX_location.x - (street_index * block_unit))/House_width);
                    BS_first_wall = street_index * block_unit + house_index * House_width + 0.003 + Real_House_width;
                }
                else
                {
                    house_index = (int) ((RX_location.x - (street_index * block_unit) - Street_width) / House_width);
                    BS_first_wall = street_index * block_unit + house_index * House_width + Street_width + 0.003 + Real_House_width;
                }

                d_in = fabs(RX_location.x - BS_first_wall) ;
                RX_location.x = RX_location.x + d_in;
                d_out = CAL.distance(TX_location, RX_location);
            }
            else if(angle >5*PI/4 && angle <=7*PI/4)
            {
                if(angle > 2/3*PI)
                    angle = (2/3*PI) - angle;
                else
                    angle = angle - (2/3*PI);
                street_index = floor(RX_location.y / block_unit);
                if(RX_location.y < (street_index * block_unit + block_unit/2))
                {
                    house_index = (int) ((RX_location.y - (street_index * block_unit))/House_width);
                    BS_first_wall = street_index * block_unit + house_index * House_width + 0.003 +  Real_House_width;
                }
                else
                {
                    house_index = (int) ((RX_location.y - (street_index * block_unit) - Street_width) / House_width);
                    BS_first_wall = street_index * block_unit + house_index * House_width + Street_width + 0.003 + Real_House_width  ;
                }

                d_in = fabs(RX_location.y - BS_first_wall) ;
                RX_location.y = RX_location.y + d_in;
                d_out = CAL.distance(TX_location, RX_location);
            }

        }//end of if (channel_index ==3)
    }// end of Pathloss_model ==1


    if(channel_index==0)  // outdoors
    {
        //Baseline Model; BS and MS ;distance (km), f_carrier (MHz)
        path_loss=38*log10(distance)+60.26+21*log10(f_carrier);
    }
    else if(channel_index==1) // FS to MS inside
    {
        //Indoor Small Office (Room to  Corridor); LOS; FS and MS
        if(Pathloss_model ==0)
            path_loss = 37+30*log10(1000*distance)+18.3*pow(floor_num,(double)(floor_num+2)/(floor_num+1)-0.46);
        else
            path_loss=46.4+20*log10(1000*distance)+12*floor_num+20*log10(f_carrier/5000);
    }
    else if(channel_index==2) // FS to MS outside
    {
        //Indoor Small Office (Through-wall); NLOS; FS and MS
        if(Pathloss_model ==0)
            path_loss = 49+40*log10(distance)+30*log10(f_carrier)+18.3*pow(floor_num,(double)(floor_num+2)/(floor_num+1)-0.46);
        else
            path_loss=36.8*log10(1000*distance)+43.8+20*log10(f_carrier/5000)+12*floor_num;
    }
    else if(channel_index==3) //MACRO BS to indoor MS
    {
        //Outdoor to Indoor;(Marco BS and MS) (Femto BS and MS)
        if(Pathloss_model ==0)  //ITU
            path_loss = 49+40*log10(distance)+30*log10(f_carrier);
        else //WiMAX EMD
        {
            double a = 13*(d_in+d_out)/(f_carrier/1000);
            double b = 1;
            temp1 = a > b ? a:b;
            pl_urban_micro = 32.4418 + 20*log10(f_carrier/1000)+20*log10(1000*(d_in+d_out))+17.4*(d_in+d_out)+20*log10(temp1);
            path_loss=pl_urban_micro + 15.29 + 500*d_in; // default angle = 45;
        }

    }
    return path_loss;
}

double LinkList::antenna_pattern(double steer_dir, double target_dir)
{
    double Max_antenna_gain=BS_antenna_gain;
    double antenna_gain=0;
    double angle=fabs((steer_dir-target_dir)/PI)*180;//degree
    if(angle>180)
        angle=360-angle;
    else if(angle<-180)
        angle=360+angle;

    antenna_gain=Max_antenna_gain-(12*(pow((angle/70),2)));
    if(antenna_gain<Max_antenna_gain-20)
        antenna_gain=Max_antenna_gain-20;

    return antenna_gain;
}
