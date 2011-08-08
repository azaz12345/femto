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
 *	@(#)fbs.cpp	11/02/2010
 */

#include "stdhead.h"
#include "external_parameters.h"
#include "fbs.h"
#include "LocationState.h"
#include "SimulatorParameter.h"
#include "Record.h"

//=================Global Parameters===============
BLOCK_RECORD BLOCK[120];
FS_INFO fsdata[FS_NUM];

Record fbsposition("fbsposition");


//=================End Global Paramters============

//************************************************//
//               Uniform distribution             //
//************************************************//

void FBS::fs_position(int Permutation)
{
    int i,j,k=0,index =0;
    double angle  = 0;
    int fs_num =0;
    XYAXIS fs_position;
    COORD FSNODE;
    POLAXIS temp_pol;
    int House_num = (int) (Street_block_size/House_width);
    int block_num =0;
    int house_num =0;
    int x_index =0;
    int y_index =0;
    int block_length=12;
    int fs_OSG_Num ;
    int temp=0;
    int CSG_GROUP ;
    double temp_distance =0;
    double temp_distance1 =0;
    LinkList mbs;




    for(i=0; i<NUM_SECTOR; i++)
    {
        for(k=0; k<FS_Sub_channel_num; k++)
            DL_FS_sub_channel[i][k] =0;
    }

    for(i=0; i<120; i++) // Consider 120 BLOCKs
    {
        BLOCK[i].FS_num_length = 0;
        for(j=0; j<100; j++)
        {
            BLOCK[i].FS_num[j] =0;
        }
    }

    std::ofstream outfile;
    outfile.open("fs_position.xls",std::ios::out);

    angle = 0;

    while(block_num <120)
    {
        house_num = 0;
        while(house_num <100)
        {
            if(Rand(&seed_fs_position) <= Density )
            {
                x_index = block_num % block_length;
                y_index = block_num / block_length;

                if((house_num/House_num)<5)
                    fs_position.x = ((-6+x_index)*block_unit+(House_width/2)+(house_num/House_num)*House_width);
                else
                    fs_position.x = ((-6+x_index)*block_unit+(House_width/2)+(house_num/House_num)*House_width+Street_width);
                if((house_num%House_num)<5)
                    fs_position.y = ((5-y_index)*block_unit-(House_width/2)-(house_num%House_num)*House_width);
                else
                    fs_position.y = ((5-y_index)*block_unit-(House_width/2)-(house_num%House_num)*House_width-Street_width);

                temp_pol = FSNODE.cart2pol(BSOxy[0],fs_position);
                angle = temp_pol.th ;
                if(angle>=PI/3)
                    angle = temp_pol.th - floor(temp_pol.th / (PI/3) )*(PI/3);
                else if (angle < 0)
                    angle = temp_pol.th + (floor(fabs(temp_pol.th) / (PI/3))+1) * (PI/3);
                if( temp_pol.r <= (R * sin(PI/3) / sin((2 * PI / 3)-angle))+0.2)  //MBS½d³ò¦AÃB¥~0.2km©ñ¸mFBS
                {
                    fsdata[fs_num].serial = fs_num ;
                    fsdata[fs_num].position = fs_position;
                    fsdata[fs_num].ms_num = 0;
                    fsdata[fs_num].sub_channel_index[0]= -1; //tch add on 2010/2/11
                    fsdata[fs_num].block_num = block_num;
                    fsdata[fs_num].UserInfo.TotalNumUser = 10;
                    fsdata[fs_num].UserInfo.CurrNumUser = 0;
                    //femtocell根據所處的sector去選擇所要的band   ex:(1,3,3)為例如果sector為1那將會隨機選擇0 or 2
                    fsdata[fs_num].RadioBand = ( FS_sector_mapping(fs_num) + (((int)(Rand()*10))%2+1) ) % 3;
                    fsdata[fs_num].DataChannel = {0,0,0,0,0,0,0,0,0,0};
                    //建立Fingerprint
				 	double RSSIs[NUM_CELL] = {0};
					for(int CellID=0; CellID<=NUM_CELL; CellID++){
						double PathLoss_ = mbs.pathloss( 3, BSOxy_MAI[CellID], fsdata[fs_num].position);
						POLAXIS position_polar =FSNODE.cart2pol(BSOxy_MAI[CellID], fsdata[fs_num].position);
						double Max_DL_power_ = 46.532;
						double Back_off_ = Back_off;
						double Max_DL_EIRP_;        //所有marco都是一樣
						double AntennaGain_ = -20;  //初始化為-20
						Max_DL_EIRP_ = Max_DL_power_ + Back_off_;

						for(int SectorID=0; SectorID<NUM_SECTOR; SectorID++){
							if(AntennaGain_< mbs.antenna_pattern(SECTOR_Steer_dir[SectorID],position_polar.th))
								AntennaGain_ = mbs.antenna_pattern(SECTOR_Steer_dir[SectorID],position_polar.th);
						}

						RSSIs[CellID] = Max_DL_EIRP_ + AntennaGain_ - PathLoss_ - 10*log10((float)All_carriers);

					}


					GenLevelFP<FP_LEVEL > GenFp_(-80.0, -95.0);
					fsdata[fs_num].LevelFingerprint = GenFp_.buildFP(RSSIs);

                    if(temp_pol.r <= R * sin(PI/3) / sin((2 * PI / 3)-angle))
                    {
                        fsdata[fs_num].marco_index = 0;
                        fsdata[fs_num].sector_index = FS_sector_mapping(fs_num);

                        fsdata[fs_num].FS_DL_EIRP = Max_FS_DL_power - Back_off;
                        fsdata[fs_num].FS_MAX_DL_power = Max_FS_DL_power - Back_off + Back_off;
                    }
                    else
                    {
                        temp_distance1 = FSNODE.distance(BSOxy[1],fs_position);
                        temp =1;
                        for(i=2; i<=6; i++)
                        {
                            if(temp_distance1 > FSNODE.distance(BSOxy[i],fs_position))
                            {
                                temp_distance1 = FSNODE.distance(BSOxy[i],fs_position);
                                temp = i;
                            }
                        }
                        fsdata[fs_num].marco_index = temp;
                        fs_position = FSNODE.cart_change(fs_position,BSOxy[temp]);
                        fsdata[fs_num].position = fs_position;
                        fsdata[fs_num].sector_index = FS_sector_mapping(fs_num);
                        fsdata[fs_num].position.x = fs_position.x+BSOxy[temp].x;
                        fsdata[fs_num].position.y = fs_position.y+BSOxy[temp].y;

                        fsdata[fs_num].FS_DL_EIRP = Max_FS_DL_power - Back_off;
                        fsdata[fs_num].FS_MAX_DL_power = Max_FS_DL_power - Back_off + Back_off;
                    }
                    //建立Vector Fingerprint
					GenVectorFP GenVtFP_(fsdata[fs_num].marco_index);
					fsdata[fs_num].VectorFingerprint = GenVtFP_.buildFP(RSSIs);
/*
                    LocationState L;
					printf("\n");
					printf("%s\n", L.IsOnStreet(fsdata[fs_num].position) ? "street":"house");
   		     		system("pause");
*/
                    fs_num ++;
                }//end of if
            }
            house_num ++;
        }
        block_num ++;
    }

    fs_OSG_Num = (int)(fs_OSG_Percent * fs_num) ;
    srand(time(NULL));

    for ( int ind = 0; ind < fs_num ; ind++ )
    {
        fsdata[ind].Power_State = true;
        double y = Rand();
        if ( y <= fs_OSG_Percent && index <= fs_OSG_Num)
        {
            fsdata[ind].fs_mode = 1 ; // OSG
            fsdata[ind].CSG_GROUP = 0 ;
            index ++ ;
        }
        else
        {
            CSG_GROUP = rand()% 99 + 1 ;
            fsdata[ind].fs_mode = 0 ; // CSG
            fsdata[ind].CSG_GROUP = CSG_GROUP ;
        }

        fbsposition.Scatter( fsdata[ind].position.x , fsdata[ind].position.y , fsdata[ind].CSG_GROUP );
    }

        fbsposition.OutputScatter() ;





    FS_INFO temp1;

    for(i=0; i<fs_num; i++) // re-permutation
    {
        temp = (int) (Rand(&seed_fs_position) * fs_num);
        temp1 = fsdata[i];
        fsdata[i] = fsdata[temp];
        fsdata[temp] = temp1;
    }

    FS_NUM_INDEX = fs_num;

    for ( k=0; k<fs_num; k++)
    {
        fsdata[k].serial =k;
        fs_allocation(k,Permutation);
//        outfile<<fsdata[k].serial<<"\t"<<fsdata[k].marco_index <<"\t"<<fsdata[k].sector_index<<"\t"<<fsdata[k].block_num <<"\t"<<fsdata[k].first_sector_index<<"\t"<<fsdata[k].sub_channel_index[0]<<"\t"<<fsdata[k].position.x<<"\t"<<fsdata[k].position.y <<std::endl;
//        outfile<<fsdata[k].serial<<"\t"<<fsdata[k].position.x<<"\t"<<fsdata[k].position.x
    }

}



//============================================================================//
//	        FS sub_channel allocaiton				      //
//============================================================================//
void FBS::fs_allocation(int fs_num, int Permutation)
{
    COORD FSNODE;
    POLAXIS pol = FSNODE.cart2pol(BSOxy[0],fsdata[fs_num].position);
    double angle =0;
    int temp =0;
    int s_block =0;
    int channel =0;
    int temp3 = 0;
    int x_index, y_index;
    int i, j, k;
    int found = 0;
    int a_full =0;
    double threshold = 0.35;


    BLOCK[fsdata[fs_num].block_num].FS_num[BLOCK[fsdata[fs_num].block_num].FS_num_length] = fsdata[fs_num].serial;
    BLOCK[fsdata[fs_num].block_num].FS_num_length ++;
    x_index = fsdata[fs_num].block_num / 10;
    y_index = fsdata[fs_num].block_num % 10;


    int fap_num,temp_value1,fap_ix;
    int fs_block = fsdata[fs_num].block_num;
    double distance =0.0;
    int* a = NULL;
    if(Permutation ==0)
        a = new int[(int) (Num_channel_DL_PUSC/Num_SCH_per_MS_DL)];
    else
        a = new int[Num_channel_DL_FUSC];
    while (threshold > 0 )
    {
        for(k=0; k<Num_channel_DL_PUSC/Num_SCH_per_MS_DL; k++)
            a[k] = 0;
        a_full =0;
        for(i=0; i<=2; i++)
        {
            for(j=11; j<=13; j++)
            {
                s_block = fs_block-(j-(i*12)); //search through surrounding blocks (differ by -11, -12, -13, -1, 0, +1, +13, +12, +11)
                if(s_block>=0 && s_block<120)
                {
                    temp_value1 = fap_ix =0;
                    fap_num = BLOCK[s_block].FS_num_length; //number of active FAPs in s_block
                    while(fap_ix < fap_num)  //index thru all FAPs
                    {
                        distance = FSNODE.distance(fsdata[BLOCK[s_block].FS_num[fap_ix]].position, fsdata[fs_num].position); //calculate the distance
                        if(distance < threshold && distance !=0) //The neighbor FAP is within current threshold distance
                        {
                            if (Permutation==0)
                                temp_value1 =fs_sub_channel_mapping(fsdata[BLOCK[s_block].FS_num[fap_ix]].sector_index, fsdata[BLOCK[s_block].FS_num[fap_ix]].sub_channel_index[fsdata[fs_num].ms_num]); //see the note in fs_sub_channel_mapping def.
                            else
                                temp_value1 = fsdata[BLOCK[s_block].FS_num[fap_ix]].sub_channel_index[fsdata[fs_num].ms_num];
                            a[temp_value1]=1;  //mark the channel used
                        }
                        fap_ix ++;
                    } // end of while
                }
            }//end of for (j)
        } // end of for (i)
        i=0;
        found=0;

        for(k = 0; k < Num_channel_DL_PUSC/Num_SCH_per_MS_DL; k++)
        {
            if(a[k]==0)
            {
                if(Permutation == 0)
                {
                    if(fsdata[fs_num].sector_index ==0 && k>9)
                    {
                        channel = k - 10;
                        found = 1;
                    }
                    else if(fsdata[fs_num].sector_index ==1)
                    {
                        if(k<10)
                        {
                            channel = k;
                            found = 1;
                        }
                        else if(k>19)
                        {
                            channel = k - 10;
                            found = 1;
                        }
                    }
                    else if(fsdata[fs_num].sector_index ==2 && k<20)
                    {
                        channel = k;
                        found = 1;
                    }
                }
                else
                {
                    channel = k;
                    found = 1;
                }
            }
            if(found == 1)
                k = 30;
        }
        if(found == 1)
            threshold = 0;
        else
            threshold -= 0.01;
    } // end of while (threshold > 0 )

    delete a;
//	} //end of (temp2==temp3)
    DL_FS_sub_channel[fsdata[fs_num].sector_index][channel]=1;
    fsdata[fs_num].sub_channel_index[fsdata[fs_num].ms_num] = channel;
    fsdata[fs_num].ms_num ++;
}

//============================================================================//
//			End of FS sub channel allocation  		      //
//============================================================================//


INT16 FBS::FS_sector_mapping(INT16 FS_index)
{
    //both fixed relay or mobile relay need this function to update their relationship with serving BS
    double sector_angle = 360/(double)NUM_SECTOR;//the range each sector covers (in degree)
    double angle = 0;
    INT16  sector_index = 0;
    //calculate which sector that FS belongs to
    //check the target sector
    angle = atan2((fsdata[FS_index].position.y-BSOxy[0].y),(fsdata[FS_index].position.x-BSOxy[0].x));
    if(angle>=0)//-pi~pi => 0~360 degree
        angle=angle*180/PI;
    else
        angle=angle*180/PI+360;
    //choose the serving sector
    sector_index=(INT16)floor((angle+sector_angle/2)/sector_angle);
    if(sector_index>=NUM_SECTOR)
        sector_index=sector_index-(INT16)NUM_SECTOR;//0~NUM_SECTOR-1

    return sector_index;
}


