/*
可能預見的問題:
1.在MP環境下,因為多重存取改變BS info時可能會出現未知問題.

*/


#include "MobileStation.h"
#include "stdhead.h"
#include "external_parameters.h"
#include "Record.h"

#include "GenFingerprintBase.h"
#include "CSG.h"
#include <omp.h>





MobileStationBase::MobileStationBase( LinkList* Userlist_, XYAXIS (*BSOxy)[NUM_CELL], FS_INFO (*fsdata__)[FS_NUM], int Permutation__,CSG* csg_):
	BSOxy_(BSOxy),
	fsdata_(fsdata__),
	PermutationMode(Permutation__),
	UserList(Userlist_),
	CSG_(csg_)
{

    //ctor
}

MobileStationBase::~MobileStationBase()
{

	Macrolist.clear();
	Femtolist.clear();

    //dtor
}

void
MobileStationBase::UpdateMSINFO(MSNODE* msnode_, double numticktime, double ticktime){
    msnode = msnode_;
/**************************************************/
    POLAXIS  temp_polar = msnode->msdata.cart2pol((*BSOxy_)[0], msnode->msdata.position);
    double angle = temp_polar.th;
    if(angle>=PI/3)
        angle = temp_polar.th - floor(temp_polar.th / (PI/3) )*(PI/3);
    else if (angle < 0)
        angle = temp_polar.th + (floor(fabs(temp_polar.th) / (PI/3))+1) * (PI/3);

    if(temp_polar.r <= R * sin(PI/3) / sin((2 * PI / 3)-angle))
        //check if the position is inside the cell;
        //use the triangle proportion rule: sin(theta) is propotional
        //to the length of facing side
        msnode->msdata.temp_test = 1;
    else
        msnode->msdata.temp_test = 0;
/**************************************************/
    if( IsOnStreet(msnode->msdata.position) ){
        msnode->msdata.on_street = 1;
    }else if( IsOnStreet(msnode->msdata.position) ){
        msnode->msdata.on_street = 0;
    }
/*********************check sector*****************************/


/*********************加快程式運作*****************************/


/***********************掃描&換手決策***************************/
	//Serving is macro & Period scan
	if( ( (int)(numticktime - msnode->msdata.ScanStartTickTime)%((int)(msnode->msdata.ScanPeriod/ticktime))==0 && msnode->msdata.femto_mode==0 ))
	{

		//printf("Serving:Macro\tPeriod Scan\n");
		MacroScan();

		UpdateFP();

		extern double MeanNumInList;
		MeanNumInList+=getNeighborFemtoList();

		FemtoScan();

		HandoverDecision();

	//Serving is macro && treigger scan
	}else if ( (msnode->msdata.femto_mode==0) && (MacroRSSI(msnode->msdata.ssector,msnode->msdata.scell,1) < -100) ){

		//printf("Serving:macro\ttrigger Scan\tRSSI:%f\n",MacroRSSI(msnode->msdata.ssector,msnode->msdata.scell,1));
		MacroScan();

		UpdateFP();

		extern double MeanNumInList;
		MeanNumInList+=getNeighborFemtoList();

		FemtoScan();

		HandoverDecision();

	//Serving is femto && trigger scan
	}else if( (msnode->msdata.femto_mode==1) && (FemtoRSSI(msnode->msdata.sFS,2) < -95)){


		//printf("Serving:femto\ttrigger Scan\tRSSI:%f\n",FemtoRSSI(msnode->msdata.sFS,2));
		MacroScan();

		UpdateFP();

		extern double MeanNumInList;
		MeanNumInList+=getNeighborFemtoList();

		FemtoScan();

		HandoverDecision();


	}

	extern double outage;

	if((msnode->msdata.femto_mode==0) && (MacroRSSI(msnode->msdata.ssector,msnode->msdata.scell,1) < -100))
	{
		//printf("Outage!!!!!%f\n",outage);
		outage++;
	}
	else if( (msnode->msdata.femto_mode==1) && (FemtoRSSI(msnode->msdata.sFS,2) < -100))
	{
		//printf("Outage!!!!!%f\n",outage);
		outage++;
	}

    if((msnode->msdata.femto_mode==0) && (MacroRSSI(msnode->msdata.ssector,msnode->msdata.scell,1) >= -100))
	{
		extern int macroNum_;
		macroNum_++;
	}
	if( (msnode->msdata.femto_mode==1) && (FemtoRSSI(msnode->msdata.sFS,2) >= -100))
	{
		extern int femtoNum_;
		femtoNum_++;
	}



    //msdata




    //Macrolist.clear();
	//Femtolist.clear();

}

void
MobileStationBase::MacroScan()
{


	sortByRSSI SortObj;

    //The RSSI of marcocells
    for(int i = 0; i < NUM_CELL; i++ ){


    	BSINFO mbs;

        POLAXIS position_polar = msnode->msdata.cart2pol((*BSOxy_)[i], msnode->msdata.position);
        int BEST_SECTORID_ = 0;//存放AntennaGain最強的sector的ID
        double BEST_AntennaGain_ = antenna_pattern(SECTOR_Steer_dir[0],position_polar.th);

        for(int j = 0; j < 3; j++){

            if( BEST_AntennaGain_ <= antenna_pattern(SECTOR_Steer_dir[j],position_polar.th) ){

                BEST_SECTORID_ = j;
                BEST_AntennaGain_ = antenna_pattern(SECTOR_Steer_dir[j],position_polar.th);
            }

        }

        msnode->msdata.RSSI[i] = MacroRSSI( BEST_SECTORID_, i, 1);
        //msnode->msdata.SINR[i] = CrtChMacroSINR( BEST_SECTORID_, i, 1);



        //tmp store to list in this class
        mbs.BSID 		= i;
        mbs.BSTYPE		= 1;
        mbs.SECTOR		= BEST_SECTORID_;
        mbs.RSSI			= msnode->msdata.RSSI[i];

        if(mbs.RSSI>-100){
			Macrolist.push_back( mbs);
        }

    }




    /*Sorting the RSSI of femtocell & macrocell*/
	sort( Macrolist.begin(), Macrolist.end(), SortObj);


/*
	POLAXIS position_pol = msnode->msdata.cart2pol((*BSOxy_)[0], msnode->msdata.position);
	double angle_ = ((180.0-60.0-((int )((abs(position_pol.th)/PI)*180)%60))/180*PI);

	for(vector<BSINFO>::iterator macroit=Femtolist.begin(); macroit!=Femtolist.end(); macroit++){
		printf("BSID:%d\tRSSI:%f\tPosition:(%f,%f)%s\n",macroit->BSID,macroit->RSSI, msnode->msdata.position.x, msnode->msdata.position.y, position_pol.r > R*sin(PI/3)/sin(angle_) ? "超出":"在內");


	}

	system("pause");
*/

}


void MobileStationBase::FemtoScan(){


	sortByRSSI SortObj;


	// The RSSI of fmetocells
    for(int i = 0; i < msnode->msdata.fs_near_num; i++){

    	BSINFO fbs;

        msnode->msdata.FS_RSSI[i] = FemtoRSSI( msnode->msdata.FS_NEAR[i], 2);
        //msnode->msdata.FS_RSSI[i] = CrtChFemtoSINR( msnode->msdata.FS_NEAR[i], 2);



        //tmp store to list in this class
        fbs.BSID		= msnode->msdata.FS_NEAR[i];
        fbs.BSTYPE	= 2;
        fbs.RSSI		= msnode->msdata.FS_RSSI[i];

		if(fbs.RSSI>-100){
			Femtolist.push_back( fbs);
		}

    }
	sort( Femtolist.begin(), Femtolist.end(), SortObj);

}



void
MobileStationBase::UpdateFP(){

		GenLevelFP<FP_LEVEL > GenLvFp_(-60.0,-100.0);
		msnode->msdata.LevelFingerprint		= GenLvFp_.buildFP(msnode->msdata.RSSI);


		if(msnode->msdata.femto_mode==0){

			GenVectorFP  GenVtFP_(msnode->msdata.scell);

			msnode->msdata.VectorFingerprint	= GenVtFP_.buildFP(msnode->msdata.RSSI);

			//printf("TEST:%d\n",GenVtFP_.distance(msnode->msdata.VectorFingerprint,(*fsdata_)[35].VectorFingerprint));

		}else{

			GenVectorFP GenVtFP_( (*fsdata_)[msnode->msdata.sFS].marco_index );

			msnode->msdata.VectorFingerprint	= GenVtFP_.buildFP(msnode->msdata.RSSI);

			//printf("TEST:%d\n",GenVtFP_.distance(msnode->msdata.VectorFingerprint,(*fsdata_)[35].VectorFingerprint));
		}
		//system("pause");
/*
		//if(msnode->msdata.VectorFingerprint.Elements[0].CellID>0){
			if(msnode->msdata.femto_mode==0) printf("ServingMacroCell: %d\n\n",msnode->msdata.scell);
			else printf("ServingFemtoCell: %d\n\n",msnode->msdata.sFS);
			for(int i=0;i<7;i++){

				printf("Cell %d:%f\n",msnode->msdata.VectorFingerprint.Elements[i].CellID,msnode->msdata.VectorFingerprint.Elements[i].ElementValue);

			}
			printf("\n");
			printf("%s\n",IsOnStreet(msnode->msdata.position)?"街道":"房屋");
			if(msnode->msdata.VectorFingerprint.Elements[0].ElementValue==-120)
			system("pause");
		//}
*/
}



/*未完成

	return false 表示輸入參數的BSTYPE!=1 or 2
	可能參數有問題 請檢查
*/
bool
MobileStationBase::Handover(BSINFO& TargetBS)
{

	//選擇Macrocell為Serving
	if(TargetBS.BSTYPE==1){

		msnode->msdata.femto_mode	= 0;
		msnode->msdata.scell				= TargetBS.BSID;
		msnode->msdata.ssector			= TargetBS.SECTOR;

		//隨機選擇某個CHANNEL  如果無適合的就+1直到找到能用的為止

		int channel_ = (int) floor(Rand(&seed_sub_channel) * Sub_channel_num);
		for( int i=0; i<Sub_channel_num; i++){

			if(UserList->DL_sub_channel_sum[msnode->msdata.scell][msnode->msdata.ssector][(channel_+i)%Sub_channel_num] == 1){
				continue;
			}

			UserList->DL_sub_channel_sum[msnode->msdata.scell][msnode->msdata.ssector][(channel_+i)%Sub_channel_num] = 1;
			msnode->msdata.sub_channel_index = (channel_+i)%Sub_channel_num;
			break;


		}

		//printf("換手至大基地台:%d\tsector:%d\tRSSI:%f\n\n",TargetBS.BSID,TargetBS.SECTOR,TargetBS.RSSI);
		//system("PAUSE");

	//選擇Femtocell為Serving
	}else if(TargetBS.BSTYPE==2){

		msnode->msdata.femto_mode 	= 1;
		msnode->msdata.sFS					= TargetBS.BSID;

		//msnode->msdata.sub_channel_index

		(*fsdata_)[TargetBS.BSID].UserInfo.CurrNumUser += 1;

		//printf("換手至極微小基地台:%d\tRSSI:%f\n\n",TargetBS.BSID,TargetBS.RSSI);
		//system("PAUSE");

	}else{
		return false;

	}


    return true;
}

/*
	依據各種情況如果判斷出MS此時是與Serving BS斷線or離開的狀況 就執行此函數
	將原Serving的記錄更新(channel state, currNumUser)

*/
void
MobileStationBase::DepartureServingCell(){


	//將目前此ServingBS的channel的使用狀況設定為0    ,0->沒使用 1->有使用

	if( msnode->msdata.femto_mode==0){

		UserList->DL_sub_channel_sum[msnode->msdata.scell][msnode->msdata.ssector][msnode->msdata.sub_channel_index] = 0;


	}else if(msnode->msdata.femto_mode==1){

		int ChIndex = (*fsdata_)[msnode->msdata.sFS].RadioBand + msnode->msdata.sub_channel_index;

		(*fsdata_)[msnode->msdata.sFS].DataChannel[ChIndex] = 0;
		//將當前使用人數減1
		(*fsdata_)[msnode->msdata.sFS].UserInfo.CurrNumUser -= 1;

	}


}



/*
DecideTargetCell(int BSType)

return BSTYPE =0, the system has no suitable BS to be used for MS.

*/
BSINFO
MobileStationBase::DecideTargetCell( int BSType){

    CSG* _CSG = CSG_;
    extern int LowSinr;
    extern int MacroLowSinr;
    extern int FemtoLowSinr;
    extern int MacroNum ;
    extern int FemtoNum ;


	if(BSType==1 && !Macrolist.empty()){
		for(vector<BSINFO>::iterator macroit=Macrolist.begin(); macroit!=Macrolist.end(); macroit++){

            MacroNum +=1 ;
			int sector_ = (*macroit).SECTOR;

			//RSSI sensitvity
			if( (int)((*macroit).RSSI) < -100){
				continue;
			}
			//SINR -6dB是最低限度
			if((int)CrtChMacroSINR( sector_, (*macroit).BSID, BSType) <-6){
			    MacroLowSinr +=1 ;
			    LowSinr +=1 ;
				continue;
			}

			//檢查人數上限
			int currNumUEinBS=0;
			for(int j=0; j<Sub_channel_num; j++){

				currNumUEinBS += UserList->DL_sub_channel_sum[(*macroit).BSID][sector_][j];
			}
			if(currNumUEinBS >= Sub_channel_num){
				continue;
			}


			return (*macroit);

		}

		//Macrolist[i]

	//BSType==2
	}else if(BSType == 2 && !Femtolist.empty()){

		for(vector<BSINFO>::iterator femtoit=Femtolist.begin(); femtoit!=Femtolist.end(); femtoit++){

            FemtoNum +=1 ;
			//RSSI sensitvity
/*
			printf("\n(*femtoit).RSSI)%f",(*femtoit).RSSI);
			printf("\nCrtChFemtoSINR( (*femtoit).BSID, BSType )%f",CrtChFemtoSINR( (*femtoit).BSID, BSType ));
			system("pause");
*/

			if((int)((*femtoit).RSSI) < -100){
				continue;
			}
			//SINR -6dB是最低限度
			if((int)CrtChFemtoSINR( (*femtoit).BSID, BSType ) <-6){
			    FemtoLowSinr +=1 ;
			    LowSinr +=1 ;
				continue;
			}

			//OSG/CSG
			if ( _CSG->Femto_Use((*femtoit).BSID, msnode->msdata.CSG_GROUP)== false){
                continue;
			}
			//檢查人數上限
			if( (*fsdata_)[(*femtoit).BSID].UserInfo.CurrNumUser >= (*fsdata_)[(*femtoit).BSID].UserInfo.TotalNumUser){

				continue;
			}


			return (*femtoit);
		}
	}

	BSINFO noTargetCell;

	noTargetCell.BSTYPE	= 0;
	noTargetCell.BSID		= 0;
	noTargetCell.SECTOR	= 0;
	noTargetCell.RSSI		= 0;

	return noTargetCell;



}




/*
	return 	0 no suitable Target Macrocell.(no handover)
				1 no suitable Target Femtocell & Macrocell.(no handover)
				2 Macro-to-Macro handover for High Velocity.
				3 Femto-to-Macro handover for High Velocity.
				4 Macro-to-Macro handover when femtolist is empty.
				5 Macro-to-Femto handover when femtolist is not empty.
				6 Femto-to-Femto handover when Femtolist is not empty.
				7 Femto-to-Macro handover when Femtolist is empty.
				8 don't need handover.(no handover)


	The detail method is described in "The Periodic Scan and Velocity Dependent Handover Scheme for Next Generation Femtocell/Macrocell Overlay Networks".
*/

int
MobileStationBase::HandoverDecision()
{

    //未來會加入以fingerprint決定速度狀態的程序
    //速度20km/h以上為高速移動狀態
	switch( UserStateDicision() ){
		case HIGH_VELOCITY_SERVING_MACRO:
		{
			//printf("LOW_VELOCITY_SERVING_MACRO\n");
			BSINFO TargetCell = DecideTargetCell(1);
			if(TargetCell.BSTYPE == 0)
			{
				return 0;
			}

			double ServingMarcoRSSI = MacroRSSI(msnode->msdata.ssector,msnode->msdata.scell,1);
			double hysteressis = 2;

			if( TargetCell.RSSI <= ( ServingMarcoRSSI + hysteressis) )
			{
				return 8;
			}

			DepartureServingCell();
			Handover(TargetCell);
			return 2;


		}
		case HIGH_VELOCITY_SERVING_FEMTO:
		{
			//printf("HIGH_VELOCITY_SERVING_FEMTO\n");
			//Decide a suitable macrocell TargetCell
			BSINFO TargetCell = DecideTargetCell(1);
			if(TargetCell.BSTYPE == 0)
			{
				return 0;
			}

			DepartureServingCell();
			Handover(TargetCell);

			return 3;

		}
		case LOW_VELOCITY_SERVING_MACRO:
		{
			//printf("LOW_VELOCITY_SERVING_MACRO\n");
			//Decide a suitable femtocell TargetCell
			BSINFO TargetCell = DecideTargetCell(2);
			if(TargetCell.BSTYPE==0)//femtolist is empty.
			{
					TargetCell = DecideTargetCell(1);
					if(TargetCell.BSTYPE==0)//macrolist is empty.
					{
						//printf("TargetCell.BSTYPE==0\n");
						return 2;
					}

					double ServingMarcoRSSI = MacroRSSI(msnode->msdata.ssector,msnode->msdata.scell,1);
					double hysteressis = 2;

					/*There are no suitable femto,so macro-to-macro handover*/
					if( TargetCell.RSSI <= ( ServingMarcoRSSI + hysteressis) )
					{
						//printf("TargetCell.RSSI:%f <= ( ServingMarcoRSSI:%f + hysteressis)\n\n", TargetCell.RSSI, ServingMarcoRSSI);
						return 8;
					}

					//printf("原ServingCellID:%d\tSector:%d\n",msnode->msdata.scell,msnode->msdata.ssector);
					//printf("TargetCell.RSSI:%f >= ( ServingMarcoRSSI:%f + hysteressis)\n", TargetCell.RSSI, ServingMarcoRSSI);
					DepartureServingCell();
					Handover(TargetCell);

					return 4;

			}

			//There are a suitable femto,so macro-to-femto handover
			if(TargetCell.RSSI <= -95)
			{
				//printf("TargetCell.RSSI <= -95\n");
				return 8;
			}


			double ServingMarcoRSSI = MacroRSSI(msnode->msdata.ssector,msnode->msdata.scell,1);
			//printf("原ServingCellID:%d\tSector:%d\n",msnode->msdata.scell,msnode->msdata.ssector);
			//printf("TargetCell.RSSI:%f >= -95 ( ServingMarcoRSSI:%f )\n", TargetCell.RSSI, ServingMarcoRSSI);
			DepartureServingCell();
			Handover(TargetCell);

			return 5;

		}
		case LOW_VELOCITY_SERVING_FEMTO:
		{

			//printf("LOW_VELOCITY_SERVING_FEMTO\n");

			//Decide a suitable femtocell TargetCell
			BSINFO TargetCell = DecideTargetCell(2);
			if(TargetCell.BSTYPE == 0){

				TargetCell = DecideTargetCell(1);
				if(TargetCell.BSTYPE == 0)
				{
					//printf("Macrolist[0]:%f\n",Macrolist[0].RSSI);
					//printf("SINR:%f\n",CrtChMacroSINR(Macrolist[0].SECTOR,Macrolist[0].BSID,1));
					//printf("TargetCell.BSTYPE == 0\n");
					//system("PAUSE");
					return 2;
				}
                double ServingFemtoRSSI = FemtoRSSI(msnode->msdata.sFS,2);

				double hysteressis = 2;


				if(TargetCell.RSSI <= (ServingFemtoRSSI+hysteressis))
				{
					//printf("TargetCell.RSSI:%f <= (ServingMarcoRSSI:%f+hysteressis)\n\n", TargetCell.RSSI, ServingMarcoRSSI);
					return 8;
				}

				//printf("原ServingCellID:%d\tSector:%d\n",msnode->msdata.scell,msnode->msdata.ssector);
				//printf("TargetCell.RSSI:%f >= (ServingMarcoRSSI:%f+hysteressis)\n", TargetCell.RSSI, ServingMarcoRSSI);
				DepartureServingCell();
				Handover(TargetCell);

				return 7;

			}

			double ServingFemtoRSSI = FemtoRSSI(msnode->msdata.sFS,2);
			double hysteressis = 2;

			if(TargetCell.RSSI <= (ServingFemtoRSSI+hysteressis))
			{
				//printf("TargetCell.RSSI:%f <= (ServingFemtoRSSI:%f+hysteressis)\n\n", TargetCell.RSSI, ServingFemtoRSSI);
				return 8;
			}

			//printf("原ServingCellID:%d\n",msnode->msdata.sFS);
			//printf("TargetCell.RSSI:%f >= (ServingFemtoRSSI:%f+hysteressis)\n", TargetCell.RSSI, ServingFemtoRSSI);
			DepartureServingCell();
			Handover(TargetCell);

			return 6;


		}
		case UNKNOW_STATE:
		{
			printf("###ERROR###:In HandoverDecision()");
			system("PAUSE");
			break;
		}

	}


}


/************************************************
    SectorID    =>0~2 for macro
    BSID        =>femto是0~10000(最多,但實際沒用這麼多)
                =>marco是0~18
    BSType      =>1:Marco, 2:Femto
************************************************/


/*Control Channel的Macro的RSSI*/

double
MobileStationBase::CrtChMacroSINR(int SectorID,int BSID,int BSType){

    extern Record Re;
    double SINR_        = 0;

    double mWInterference_m_    = 0;
    double mWInterference_f_         = 0;
    double mWInterference_all_           =0;
    double dBInterference_all_           =0;

        //計算marco interference
        /*
        跟指定大基地台是不同大基地台且同sector都是干擾源

        */

        for( int i = 0; i <  NUM_CELL; i++){
            for(int j=0; j < 3; j++){//sector0~2
//                if( i != BSID && j == SectorID ){
                if((i == BSID && j != SectorID ) || i !=SectorID )
                {
                    double dBInterference_m_= 0;       //marco

                    dBInterference_m_ = MacroRSSI(j,i,1);
                    mWInterference_m_ = pow(10,dBInterference_m_/10);
                }
  //            }

            }
        }


        //計算femto interference
        /*
        跟指定大基地台使用相同頻段的femto都是干擾源
        目前femtocell屬於哪個sector是採用座標(eg.位置)決定
        但現實不太可能使用位置  極有可能利用訊號強度來決定(所以此部分尚需根據情況修改)
        */

        for(int i = 0; i < msnode->msdata.fs_near_num; i++)
        if( (*fsdata_)[msnode->msdata.FS_NEAR[i]].Power_State == true )
        {
//           if( (*fsdata_)[msnode->msdata.FS_NEAR[i]].RadioBand == SectorID){
                double dBInterference_f_= 0;       //femto

                dBInterference_f_= FemtoRSSI(msnode->msdata.FS_NEAR[i],2);

                mWInterference_f_+= pow(10,dBInterference_f_/10);
//           }

        }


        //把femto跟marco的interference term(mW)相加起來
        mWInterference_all_ = mWInterference_m_ + mWInterference_f_ + pow(10,NoiseDL(0, A_subcarrier_frequency)/10);


        dBInterference_all_ = 10*log10(mWInterference_all_);


        SINR_ = MacroRSSI(SectorID, BSID, BSType) - dBInterference_all_ - MS_Nosie_figure;

        extern int ToLowSinr;
/*
        if( SINR_ < -6 )
        {
            ToLowSinr +=1;
        }
*/
    extern Record ReMacroSINR;
    extern Record ReALLSINR;
    ReMacroSINR.InsertData(SINR_);
    ReALLSINR.InsertData(SINR_);

    return SINR_;
}


/*Control Channel(include preamble or sychronization signal)的Femto的RSSI*/

double
MobileStationBase::CrtChFemtoSINR(int BSID, int BSType){

    double SINR_        = 0;

    double mWInterference_m_         = 0;
    double mWInterference_f_         = 0;
    double mWInterference_all_           =0;
    double dBInterference_all_           =0;


    //計算marco interference
    /*
        跟指定FEMTOCELL是同一個扇區的其他macrocell都是干擾源
    */

    for( int i = 0; i <  NUM_CELL; i++){
        for(int j=0; j < 3; j++)//sector0~2
        {
//            if( (*fsdata_)[BSID].RadioBand == j ){

                    double dBInterference_m_= 0;       //marco

                    dBInterference_m_ = MacroRSSI(j,i,1);
                    mWInterference_m_ += pow(10,dBInterference_m_/10);

//            }

        }
    }



    //計算femto interference
    /*
    跟所要量測SINR的femtocell為同一個RadioBand的周遭femtocell都視為干擾源
    以及防止計算到自己的RSSI
    */

    for(int i = 0; i < msnode->msdata.fs_near_num; i++)
    if( (*fsdata_)[msnode->msdata.FS_NEAR[i]].Power_State == true )
    {

/*
    	if((*fsdata_)[msnode->msdata.FS_NEAR[i]].RadioBand != (*fsdata_)[BSID].RadioBand)
			continue;
*/
		if(msnode->msdata.FS_NEAR[i]==BSID)
			continue;


		double dBInterference_f_= 0;       //femto

		dBInterference_f_ = FemtoRSSI(msnode->msdata.FS_NEAR[i],BSType);

		mWInterference_f_ += pow(10,dBInterference_f_/10);


    }

    //把femto跟marco的interference term(mW)相加起來
    mWInterference_all_ = mWInterference_m_ + mWInterference_f_ + pow(10,NoiseDL(0, A_subcarrier_frequency)/10);


    dBInterference_all_ = 10*log10(mWInterference_all_);


    SINR_ = FemtoRSSI(BSID, BSType) - dBInterference_all_ - MS_Nosie_figure;

    extern int ToLowSinr;

    if( SINR_ < -6 )
    {
        ToLowSinr +=1;
    }

    extern Record ReFemtoSINR;
    extern Record ReALLSINR;
    ReFemtoSINR.InsertData(SINR_);
    ReALLSINR.InsertData(SINR_);

    return SINR_;
}

/*Control Channel的RSSI*/

double
MobileStationBase::FemtoRSSI(int BSID,int BSType){

    double RSSI_ = 0;//輸出參數

        double PathLoss_;
        PathLoss_ = PathLoss( BSID, BSType);

        RSSI_ = (*fsdata_)[BSID].FS_DL_EIRP - PathLoss_; //- 10*log10((float)All_carriers)+ 10*log10(30.0) - 10*log10(2.0);	//全向型天線

    return RSSI_;
}

/*

return 0 代表參數輸入錯誤

*/
double
MobileStationBase::MacroRSSI(int SectorID,int BSID,int BSType){

    double RSSI_ = 0;//輸出參數

        POLAXIS position_polar = msnode->msdata.cart2pol((*BSOxy_)[BSID], msnode->msdata.position);
        double Max_DL_power_ = 46.532;
        double Back_off_ = Back_off;
        double Max_DL_EIRP_;        //所有marco都是一樣
        double AntennaGain_ = -20;  //初始化為-20
        double PathLoss_;
        //計算Max_DL_EIRP
        Max_DL_EIRP_ = Max_DL_power_ - Back_off_;

        //計算指向性天線的AntennaGain
        if(AntennaGain_< antenna_pattern(SECTOR_Steer_dir[SectorID],position_polar.th))
            AntennaGain_ = antenna_pattern(SECTOR_Steer_dir[SectorID],position_polar.th);

        //計算指定的marcocell的pathloss
        PathLoss_ = PathLoss( BSID, BSType);

        RSSI_ = Max_DL_EIRP_ + AntennaGain_ - PathLoss_ ;//- 10*log10((float)All_carriers);

       //printf("PathLoss:%f\nBSID:%d\tSectorID:%d\tBSTYPE:%d\tAntennaGain_:%f\n",PathLoss_,BSID,SectorID,BSType,AntennaGain_);
        //printf("(*BSOxy_)[BSID]:(%f,%f)",(*BSOxy_)[BSID].x,(*BSOxy_)[BSID].y);

    return RSSI_;
}


double
MobileStationBase::PathLoss(int BSID,int BSType){

    double PathLoss_ = 0;

    if( BSType == 1 ){
        if( IsOnStreet(msnode->msdata.position) == true ){// on street

            PathLoss_ = pathloss( 0, (*BSOxy_)[BSID], msnode->msdata.position);
        }else if( IsOnStreet(msnode->msdata.position) == false ){

            PathLoss_ = pathloss( 3, (*BSOxy_)[BSID], msnode->msdata.position);
        }else{printf("BSType1判斷street有bug\n");system("PAUSE");}


    }else if( BSType == 2){
        if( IsOnStreet(msnode->msdata.position) == true ){ // on street

            PathLoss_ = pathloss( 2, (*fsdata_)[BSID].position, msnode->msdata.position);
        }else if( IsOnStreet(msnode->msdata.position) == false ){

            PathLoss_ = pathloss( 1, (*fsdata_)[BSID].position, msnode->msdata.position);
        }else{printf("BSType2判斷street有bug\n");system("PAUSE");}
    }


    return PathLoss_;
}

/*
350m範圍內的femtocell
return 1代表沒有MS節點

*/

int
MobileStationBase::getNeighborFemtoList(){

    if(msnode==NULL){ return 0; }

    int currBaseBlock = WhichBlock(msnode->msdata.position);
    int FsNearNum = 0;

    for(int i = 0; i <= 2; i++){
        for(int j = 11; j <= 13; j++){

            int currBlock = currBaseBlock - (j-(i*12));

            if(currBlock >= 0 && currBlock < 120){


                for(int FemtoInArray = 0;
                    FemtoInArray < BLOCK[currBlock].FS_num_length;
                    FemtoInArray++ )
                {
                    double distance = msnode->msdata.distance(fsdata[BLOCK[currBlock].FS_num[FemtoInArray]].position,
                                                       msnode->msdata.position);
                    if(distance < 0.35){

                        msnode->msdata.FS_NEAR[FsNearNum] = BLOCK[currBlock].FS_num[FemtoInArray];
                        FsNearNum++;
                    }//end of if(distance < 0.35)

                }

            }//end of if(currBlock >= 0 && currBlock < 120)

        }
        msnode->msdata.fs_near_num = FsNearNum;
    }

    return FsNearNum;
}



double
MobileStationBase::NoiseDL(int Permutation, double subcarrierBW){

    //輸出參數
    double N_DL_;

    if(Permutation == 0) // PUSC
        N_DL_ = Noise_power_density + 10*log10(subcarrierBW);            // Noise_power is for the entire bandwidth. Since we only use
        // portions of subcarriers, need to subtract those noise power
        // that is outside of our band.
        // Carrier_perCH_DL_PUSC = 28, Num_channel_DL_PUSC=20
        // total_carrier = 2048
        // So this is the noise power for a subchannel
    else
        N_DL_ = Noise_power_density + 10*log10(subcarrierBW);
        // Carrier_perCH_DL_FUSC = 48, Num_channel_DL_FUSC=32
        // This is not consistent with PUSC above;
        // should reduce by the subchannel number (32) in dB
    return N_DL_;
}

/*
目前直接採用系統參數
未來此部分會改用某方法來估測速度

*/
int
MobileStationBase::UserStateDicision(){

    int ServingCellType = msnode->msdata.femto_mode;
    double MSVelocity = msnode->msdata.speed;

    if( ServingCellType==0 && MSVelocity >= 20)
    {
		return HIGH_VELOCITY_SERVING_MACRO;
    }

    if( ServingCellType==1 && MSVelocity >= 20)
    {
		return HIGH_VELOCITY_SERVING_FEMTO;
    }

	if( ServingCellType==0 && MSVelocity < 20)
    {
		return LOW_VELOCITY_SERVING_MACRO;
    }

    if( ServingCellType==1 && MSVelocity < 20)
    {
		return LOW_VELOCITY_SERVING_FEMTO;
    }


	return UNKNOW_STATE;

}

