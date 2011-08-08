#include "MobileStation.h"
#include "external_parameters.h"
#include "SimulatorParameter.h"
#include "CSG.h"

MSwithLevelFP::MSwithLevelFP( LinkList* Userlist_, XYAXIS (*BSOxy)[NUM_CELL],
											FS_INFO (*fsdata__)[FS_NUM],
											int Permutation__,CSG* csg_):
	MobileStationBase(Userlist_,BSOxy,fsdata__,Permutation__,csg_)
{

}

MSwithLevelFP::~MSwithLevelFP()
{

}

void
MSwithLevelFP::UpdateMSINFO( MSNODE* msnode_, double numticktime, double ticktime){

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
	BSINFO optimumFemtoTarget;
	//Serving is macro & Period scan
	if( ( (int)(numticktime - msnode->msdata.ScanStartTickTime)%((int)(msnode->msdata.ScanPeriod/ticktime))==0 && msnode->msdata.femto_mode==0 ))
	{

		//printf("Serving:Macro\tPeriod Scan\n");
		MacroScan();

		UpdateFP();

		extern double MeanNumInList;
		extern long int countList;
		getNeighborFemtoList();
		MeanNumInList+= getFemtoListByFP();
		if(FemtoListByFP.size()!=0)
		countList++;

		optimumFemtoScan();
		optimumFemtoTarget = DecideTargetCell(2);
		Femtolist.clear();


		//printf("getNeighborFemtoList:%d\n",getFemtoListByFP());

		FemtoScan();

		HandoverDecision();

		extern double missCount;
		if(FemtoListByFP.size()!=0)
		if(optimumFemtoTarget.BSTYPE!=0)
		if(find_if( Femtolist.begin(), Femtolist.end(),findBSID(optimumFemtoTarget.BSID) ) == Femtolist.end())
		{missCount++;}

	//Serving is macro && treigger scan
	}else if ( (msnode->msdata.femto_mode==0) && (MacroRSSI(msnode->msdata.ssector,msnode->msdata.scell,1) < -100) ){

		//printf("Serving:macro\ttrigger Scan\tRSSI:%f\n",MacroRSSI(msnode->msdata.ssector,msnode->msdata.scell,1));
		MacroScan();

		UpdateFP();

		extern double MeanNumInList;
		extern long int countList;
		getNeighborFemtoList();
		MeanNumInList+= getFemtoListByFP();
		if(FemtoListByFP.size()!=0)
		countList++;

		optimumFemtoScan();
		optimumFemtoTarget = DecideTargetCell(2);
		Femtolist.clear();


		//printf("getNeighborFemtoList:%d\n",getFemtoListByFP());

		FemtoScan();

		HandoverDecision();

		extern double missCount;
		if(FemtoListByFP.size()!=0)
		if(optimumFemtoTarget.BSTYPE!=0)
		if(find_if( Femtolist.begin(), Femtolist.end(),findBSID(optimumFemtoTarget.BSID) ) == Femtolist.end())
		{missCount++;}

	//Serving is femto && trigger scan
	}else if( (msnode->msdata.femto_mode==1) && (FemtoRSSI(msnode->msdata.sFS,2) < -95)){


		//printf("Serving:femto\ttrigger Scan\tRSSI:%f\n",FemtoRSSI(msnode->msdata.sFS,2));
		MacroScan();

		UpdateFP();

		extern double MeanNumInList;
		extern long int countList;
		getNeighborFemtoList();
		MeanNumInList+= getFemtoListByFP();
		if(FemtoListByFP.size()!=0)
		countList++;

		optimumFemtoScan();
		optimumFemtoTarget = DecideTargetCell(2);
		Femtolist.clear();


		//printf("getNeighborFemtoList:%d\n",getFemtoListByFP());

		FemtoScan();

		HandoverDecision();

		extern double missCount;
		if(FemtoListByFP.size()!=0)
		if(optimumFemtoTarget.BSTYPE!=0)
		if(find_if( Femtolist.begin(), Femtolist.end(),findBSID(optimumFemtoTarget.BSID) ) == Femtolist.end())
		{missCount++;}


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


    //msdata




    //Macrolist.clear();
	//Femtolist.clear();




}

/*
int
MSwithLevelFP::getNeighborFemtoList(){

	int FsNearNum=0;


	for(int FemtoCellID=0; FemtoCellID<FS_NUM_INDEX; FemtoCellID++){

		GenLevelFP<4 > GenLvFp_(-80.0,-95.0);

		if(GenLvFp_.matchingFP( msnode->msdata.LevelFingerprint, (*fsdata_)[FemtoCellID].LevelFingerprint )==1)
		{
			msnode->msdata.FS_NEAR[FsNearNum] =  FemtoCellID;

			FsNearNum++;
		}



	}

	msnode->msdata.fs_near_num = FsNearNum;
	//printf("matching result:%d\n",FsNearNum);


	return FsNearNum;
}
*/

int
MSwithLevelFP::getFemtoListByFP(){
	int FsListNum=0;

	for(int FemtoCellID=0; FemtoCellID<FS_NUM_INDEX; FemtoCellID++){

		GenLevelFP<FP_LEVEL > GenLvFp_(-60.0,-100.0);


		if(GenLvFp_.matchingFP( msnode->msdata.LevelFingerprint, (*fsdata_)[FemtoCellID].LevelFingerprint )==1)
		{
			FemtoListByFP.push_back(FemtoCellID);

			FsListNum++;
		}

	}

	return FsListNum;
}


void MSwithLevelFP::FemtoScan(){

	sortByRSSI SortObj;

	for(vector<int>::iterator femtoit=FemtoListByFP.begin(); femtoit!=FemtoListByFP.end();femtoit++)
	{
		BSINFO fbs;

		fbs.BSID = (*femtoit);
		fbs.BSTYPE = 2;
		fbs.RSSI =  FemtoRSSI( (*femtoit), 2);

		if(fbs.RSSI>-100) Femtolist.push_back(fbs);

	}

	sort( Femtolist.begin(), Femtolist.end(), SortObj);
}


void
MSwithLevelFP::optimumFemtoScan(){

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

