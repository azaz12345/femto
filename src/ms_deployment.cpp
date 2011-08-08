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
 *	@(#)ms_deployment.cpp	11/02/2010
 */

#include "stdhead.h"
#include "external_parameters.h"


//***********************************************************************************************//
//***********************************************************************************************//
//                                      MS arrival Module                                        //
//***********************************************************************************************//
//***********************************************************************************************//
double LinkList::insertLast(double t_new, double traffic_load, int Femto_mode) //insert at end of list
{
    double  t_next_new;
    double  interval;
    double  ptemp;

    MSNODE* pNewNode = new MSNODE;      //(MSINFO *msd); //make new link

    interval = RT_time/traffic_load;   //average inter-arrival time of RT service
    if(RT_prob != 0)
        interval = interval*RT_prob;   //average inter-arrival time of all services
    //Initilization of New Msnode//
    ptemp=Rand();
    if(ptemp < RT_prob) //new arrival MS use RT service
    {
        pNewNode->msdata.session = t_new + Session_rand(RT_time, seed_session);
    }
    else//new arrival MS use NRT service
    {
        //*****depends on Non-realtime traffic model*****//
        //pNewNode->msdata.data_rate = 10;//10kbps forVoIP traffic
        //serving time for data user depends on data traffic model
    }
    MS_use_femto_num = init_msnode(t_new, pNewNode, Femto_mode);
    if (isEmpty())                   //check if linklist is empty
        pFirst = pNewNode;            //first --> newLink
    else
    {
        pLast->pNext = pNewNode;      //old last --> newLink
        pNewNode->pPrevious = pLast;  //old last --> newLink
    }
    pLast = pNewNode;                 //newLink <-- last
    length++;
    t_next_new = t_new + New_call_rand(interval, seed_arrival); //
    return MS_use_femto_num;
}

//     Randomly place MS in cell 0 and find best RSSI BS as serving BS
//
double LinkList::init_msnode(double t_new, MSNODE* pCurr, int Femto_mode)
{
    INT16    ccell;
    XYAXIS   cposition;
    XYAXIS   block_position;
    double   cdmin,cdistance,crssmax,crss[NUM_CELL],clongterm[NUM_CELL];
    double   cproploss[NUM_CELL]; //temp propagation_loss
    double   longterm_abs[NUM_CELL]; //shadowing to each BS with absolute serial number
    int      nearest_x_street_index=0;
    int      nearest_y_street_index=0;
    double   x_offset=0;
    double   y_offset=0;
    bool     re_position;
    INT16    re_cell=0;
    INT16    i,j;
    POLAXIS  temp_polar;
    XYAXIS   temp_axis;
    double   antenna_gain=0;
    double   temp_distance=0;
    int	 house_index =0;

    double   angle=0;
    double   Rx_power=0;
    INT16    temp=0;
    int      s_block=0;
    int	 fap_ix = 0;
    double   sector_angle=2*180/(double)NUM_SECTOR;
    double   temp_prop_loss=0;
    double   Best_FS_Quality=0;
    INT16    Best_FS_Index_FS=0;
    bool	 y1=0;
    bool	 y2=0;
    int	 temp_block=0;

    cposition = pCurr->msdata.init_position(&seed_position_x, &seed_position_y, R);

    temp_polar = pCurr->msdata.cart2pol(BSOxy[0], cposition);
    angle = temp_polar.th;
    if(angle>=PI/3)
        angle = temp_polar.th - floor(temp_polar.th / (PI/3) )*(PI/3);
    else if (angle < 0)
        angle = temp_polar.th + (floor(fabs(temp_polar.th) / (PI/3))+1) * (PI/3);

    if(temp_polar.r <= R * sin(PI/3) / sin((2 * PI / 3)-angle))
        //check if the position is inside the cell;
        //use the triangle proportion rule: sin(theta) is propotional
        //to the length of facing side
        pCurr->msdata.temp_test = 1;
    else
        pCurr->msdata.temp_test = 0;
    // MS

    nearest_x_street_index=(int) floor(cposition.x/block_unit);
    // block_unit is 230m
    nearest_y_street_index=(int) floor(cposition.y/block_unit);
    // Street_block_size = 200m
    y1 = (nearest_x_street_index * block_unit+Street_block_size/2) < cposition.x && ((nearest_x_street_index+1) * block_unit-Street_block_size/2)> cposition.x;
    y2 = (nearest_y_street_index * block_unit+Street_block_size/2) < cposition.y && ((nearest_y_street_index+1) * block_unit-Street_block_size/2)> cposition.y;

    if (y1==1 || y2==1)
        pCurr->msdata.on_street = 1;
    else
        pCurr->msdata.on_street = 0;

    if(pCurr->msdata.on_street ==0)
    {
        block_position.x = nearest_x_street_index * block_unit;
        block_position.y = nearest_y_street_index * block_unit;
        if((cposition.x - block_position.x) < (block_unit/2))
        {
            house_index = (int) ((cposition.x -block_position.x ) / House_width);
            temp_distance = cposition.x - block_position.x - (house_index * House_width);
            temp_distance = (temp_distance * 100000)/100000;
            if(temp_distance > 0.003 && temp_distance < 0.017)
                y1=0;
            else
                y1=1;
        }
        else
        {
            house_index = (int) ((cposition.x - block_position.x-Street_width)/ House_width);
            temp_distance = cposition.x - block_position.x - Street_width - (house_index * House_width);
            if(temp_distance >= 0.003 && temp_distance < 0.017)
                y1=0; //outdoor
            else
                y1=1;
        }

        if((cposition.y - block_position.y) < (block_unit/2))
        {
            house_index = (int) ((cposition.y -block_position.y) / House_width);
            temp_distance = cposition.y - block_position.y - (house_index * House_width);
            if(temp_distance > 0.003 && temp_distance < 0.017)
                y2=0;
            else
                y2=1;
        }
        else
        {
            house_index = (int) ((cposition.y - block_position.y-Street_width)/ House_width);
            temp_distance = cposition.y - block_position.y - Street_width - (house_index * House_width);
            if(temp_distance > 0.003 && temp_distance < 0.017)
                y2=0;
            else
                y2=1;
        }
        if (y1==1 || y2==1)
            pCurr->msdata.on_street = 1;
        else
            pCurr->msdata.on_street = 0;

    } // end of if(pCurr->msdata.on_street ==0)

    ccell =0;

    // pre-set cell0 as the cell with max. RSSI  //Sector RSSI
    if(pCurr->msdata.on_street == 1)
        cproploss[0] = pathloss(0,BSOxy[0], cposition);
    else
        cproploss[0] = pathloss(3,BSOxy[0], cposition);
    antenna_gain=-20;//reset to a minimum at -20

    for(j=0; j<NUM_SECTOR; j++)
    {
        if(antenna_gain<antenna_pattern(SECTOR_Steer_dir[j],temp_polar.th))
            antenna_gain=antenna_pattern(SECTOR_Steer_dir[j],temp_polar.th);
    }
    pCurr->msdata.RSSI[0] = Max_DL_EIRP + antenna_gain - cproploss[0] - 10*log10((float)All_carriers);;

    // pre-set cell0 as the cell with max. RSSI
    if(pCurr->msdata.on_street == 1)
        cproploss[0] = pathloss(0,BSOxy[0],cposition);
    else
        cproploss[0] = pathloss(3,BSOxy[0],cposition);

    antenna_gain=-20;//reset to a minimum at -20

    temp_polar=pCurr->msdata.cart2pol(BSOxy[0],cposition);
    for(j=0; j<NUM_SECTOR; j++)
    {
        if(antenna_gain<antenna_pattern(SECTOR_Steer_dir[j],temp_polar.th)) //antenna gain
            antenna_gain=antenna_pattern(SECTOR_Steer_dir[j],temp_polar.th);
    }
    crssmax = Max_DL_EIRP + antenna_gain - cproploss[0] - 10*log10((float)All_carriers);;
    crss[0]=crssmax;


    //****************** FS ************************//
    int fs_near_num =0;

    nearest_x_street_index= (int) floor(cposition.x/block_unit)+6;
    nearest_y_street_index= abs( (int) floor(cposition.y/block_unit)-4);
    temp_block = (int) (nearest_y_street_index * 12 + nearest_x_street_index);

    nearest_x_street_index = temp_block / 10 ;
    nearest_y_street_index = temp_block % 10 ;
    for(i=0; i<=2; i++) //
    {
        for(j=11; j<=13; j++)
        {
            fap_ix =0;
            s_block = temp_block - (j-(i*12));
            if(s_block>=0 && s_block <120)
            {
                while(fap_ix < BLOCK[s_block].FS_num_length)
                {
                    temp_distance = pCurr->msdata.distance(fsdata[BLOCK[s_block].FS_num[fap_ix]].position, cposition);
                    if(temp_distance < 0.35) //
                    {
                        // record all neighbor FS (within 0.35)
                        pCurr->msdata.FS_NEAR[fs_near_num] = BLOCK[s_block].FS_num[fap_ix];
                        fs_near_num ++;
                    }
                    fap_ix ++;
                } //end of while
            } //end of if
        }
    }

//============================================================================//
    pCurr->msdata.fs_near_num = fs_near_num;
    for(j=0; j<pCurr->msdata.fs_near_num; j++)
    {
        if(pCurr->msdata.on_street ==1)
            temp_prop_loss=pathloss(2,fsdata[pCurr->msdata.FS_NEAR[j]].position, cposition);
        else
            temp_prop_loss=pathloss(1,fsdata[pCurr->msdata.FS_NEAR[j]].position, cposition);
        pCurr->msdata.FS_RSSI[j]=fsdata[pCurr->msdata.FS_NEAR[j]].FS_DL_EIRP - temp_prop_loss - 10*log10((float)All_carriers)+ 10*log10(30.0) - 10*log10(2.0);	//全向型天線
    }

    //If the best RSSI doesn't belong to nearest cell, then change serving cell.
    if(re_cell > 0)	 //re_cell is 0
    {
        pCurr->msdata.scell=adjcent[ccell][re_cell]; // decide the serving cell after cell search
        pCurr->msdata.position=pCurr->msdata.cart_change(cposition,BSOxy[re_cell]);
        //--------------------------------------------------------------//
        for ( i=0; i<NUM_CELL; i++)
        {
            if(pCurr->msdata.on_street==1)
                pCurr->msdata.proploss[i]=pathloss(0,BSOxy[i],pCurr->msdata.position);
            else
                pCurr->msdata.proploss[i]=pathloss(3,BSOxy[i],pCurr->msdata.position);
            antenna_gain=-20;//reset
            temp_polar=pCurr->msdata.cart2pol(BSOxy[i],pCurr->msdata.position);
            for(j=0; j<NUM_SECTOR; j++)
            {
                if(antenna_gain<antenna_pattern(SECTOR_Steer_dir[j],temp_polar.th))
                    antenna_gain=antenna_pattern(SECTOR_Steer_dir[j],temp_polar.th);
            }
            pCurr->msdata.RSSI[i]= Max_DL_EIRP + antenna_gain - pCurr->msdata.proploss[i] - 10*log10((float)All_carriers);//dB
        }
    }
    else //serving macro BS is cell 0
    {
        pCurr->msdata.scell=ccell;
        pCurr->msdata.position=cposition;
        for(i=0; i<NUM_CELL; i++)
        {
            pCurr->msdata.proploss[i]=cproploss[i];
            pCurr->msdata.RSSI[i]=crss[i];

        }
    }

    if(pCurr->msdata.fs_near_num !=0)
    {
        Best_FS_Quality=pCurr->msdata.FS_RSSI[0];
        Best_FS_Index_FS = pCurr->msdata.FS_NEAR[0];
        //--------------------------------------//
        //------- find the Best FS within home cell----------//
        for(j=0; j<pCurr->msdata.fs_near_num; j++)
        {
            if( pCurr->msdata.FS_RSSI[j] > Best_FS_Quality)
            {
                Best_FS_Quality = pCurr->msdata.FS_RSSI[j];
                Best_FS_Index_FS=pCurr->msdata.FS_NEAR[j];
            }
        }// end of for
    }

    //std::ofstream outfile;
   // outfile.open("RSSI.xls", std::ios::app);
    if( pCurr->msdata.RSSI[0]>Best_FS_Quality || Femto_mode==0 || fs_near_num ==0 )
    {
        // connect to macro
        pCurr->msdata.femto_mode=0;
        pCurr->msdata.sFS=Best_FS_Index_FS;
        pCurr->msdata.block_index = temp_block;
    }
    else  //connect to femto
    {
        pCurr->msdata.position = cposition;
        pCurr->msdata.block_index = temp_block;
        pCurr->msdata.femto_mode=1;
        pCurr->msdata.sFS=Best_FS_Index_FS;
        if(pCurr->msdata.temp_test==1)
            MS_use_femto_num ++;
    } // end of else

   // outfile<<pCurr->msdata.femto_mode<<"\t"<<pCurr->msdata.RSSI[0]<<"\t"<<Best_FS_Quality<<std::endl;

    angle = atan2(pCurr->msdata.position.y,pCurr->msdata.position.x);

    if(angle>=0)//-pi~pi => 0~360 degree
        angle=angle*180/PI;
    else
        angle=angle*180/PI+360;
    //choose the serving sector, sector_angle = 120
    temp=(INT16)floor((angle+sector_angle/2)/sector_angle);

    if(temp>=NUM_SECTOR)
        temp=temp-(INT16)NUM_SECTOR;
    pCurr->msdata.ssector = temp;//0~NUM_SECTOR-1

    //printf("###角度:%f\nsector:%d\n",angle,temp);


    return MS_use_femto_num;
}

//****************************************************************************//
//****************************************************************************//
//                   End (MS arrival Module)                                  //
//****************************************************************************//
//****************************************************************************//


//****************************************************************************//
//****************************************************************************//
//                   MS departure Module                                      //
//****************************************************************************//
//****************************************************************************//
void LinkList::node_departure()
{
    MSNODE* pCurr=pFirst;
    pCurr = rmv_msnode(pCurr);
}


//remove the msnode at pCurrent
MSNODE* LinkList::rmv_msnode(MSNODE* pCurr)
{
    MSNODE* pRmv = pCurr;

    if (pCurr== pFirst && pCurr ==pLast)
    {
        pFirst = NULL;
        pLast = NULL;
    }

    length --;
    delete pRmv;
    return pCurr;
}

//---------------------------------------------//


//****************************************************************************//
//****************************************************************************//
//                   End (MS departure Module)                                //
//****************************************************************************//
//****************************************************************************//
