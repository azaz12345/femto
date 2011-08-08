#include "MobileStation.h"
#include "external_parameters.h"
#include "SimulatorParameter.h"
#include <algorithm>
#include <cmath>

MSwithVectorFP::MSwithVectorFP( LinkList* Userlist_, XYAXIS (*BSOxy)[NUM_CELL],
											FS_INFO (*fsdata__)[FS_NUM],
											int Permutation__,CSG* csg_):
	MobileStationBase(Userlist_,BSOxy,fsdata__,Permutation__,csg_)
{
    FemtoListByFP = new vector<int>;
}


MSwithVectorFP::~MSwithVectorFP()
{

}


double realAngle(XYAXIS curr,XYAXIS pre, int servingCell){

	XYAXIS V1,V2;

	V1.x = pre.x - BSOxy_MAI[servingCell].x;
	V1.y = pre.y - BSOxy_MAI[servingCell].y;

	V2.x = curr.x - BSOxy_MAI[servingCell].x;
	V2.y = curr.y - BSOxy_MAI[servingCell].y;

	return acos((V1.x*V2.x + V1.y*V2.y) / (sqrt((V1.x*V1.x+V1.y*V1.y)*(V2.x*V2.x+V2.y*V2.y))));

}

void
MSwithVectorFP::UpdateMSINFO( MSNODE* msnode_, double numticktime, double ticktime){

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

/*************************SINR*********************************/


/***********************掃描&換手決策***************************/
	VectorFP PreFP,currFP;
	XYAXIS PreLocation,currLocation;
	BSINFO optimumFemtoTarget;
	//Serving is macro & Period scan
	if( ( (int)(numticktime - msnode->msdata.ScanStartTickTime)%((int)(msnode->msdata.ScanPeriod/ticktime))==0 && msnode->msdata.femto_mode==0 ))
	{

		//printf("Serving:Macro\tPeriod Scan\n");
		MacroScan();

		//PreFP = msnode->msdata.VectorFingerprint;
		UpdateFP();
		//currFP = msnode->msdata.VectorFingerprint;
		//GenVectorFP genVFP(msnode->msdata.scell);
		//printf("################estAngle:%f\t%f\t%f\n",genVFP.EstAngle(currFP,PreFP)/PI*180,realAngle(msnode->msdata.position ,msnode->msdata.preposition ,msnode->msdata.scell )/PI*180,genVFP.EstAngle(currFP,PreFP)/PI*180-realAngle(msnode->msdata.position ,msnode->msdata.preposition ,msnode->msdata.scell )/PI*180);



		extern double MeanNumInList;
		extern long int countList;
		getNeighborFemtoList();
		MeanNumInList+= getFemtoListByFP(VFP_LENGTH);
		countList++;                                 //triger HO count

		optimumFemtoScan();                          //0.35 femtocell scanlist
		optimumFemtoTarget = DecideTargetCell(2);    //check RSSI , SINR , UserNumber , CSG
		Femtolist.clear();

		FemtoScan();                                 //check RSSI

		HandoverDecision();                          //assign channel to MS and HO to MS

		extern double missCount;
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
		MeanNumInList+= getFemtoListByFP(VFP_LENGTH);
		countList++;

		optimumFemtoScan();
		optimumFemtoTarget = DecideTargetCell(2);
		Femtolist.clear();

		FemtoScan();

		HandoverDecision();

		extern double missCount;
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
		MeanNumInList+= getFemtoListByFP(VFP_LENGTH);
		countList++;

		optimumFemtoScan();
		optimumFemtoTarget = DecideTargetCell(2);
		Femtolist.clear();

		FemtoScan();

		HandoverDecision();

		extern double missCount;
		if(optimumFemtoTarget.BSTYPE!=0)
		if(find_if( Femtolist.begin(), Femtolist.end(),findBSID(optimumFemtoTarget.BSID) ) == Femtolist.end())
		{missCount++;}

	}
	//analysis
	extern double outage;

	if((msnode->msdata.femto_mode==0) && (MacroRSSI(msnode->msdata.ssector,msnode->msdata.scell,1) < -100))
	{
		//printf("OutageM!!!!!%f\n",outage);
		outage++;
	}
	else if( (msnode->msdata.femto_mode==1) && (FemtoRSSI(msnode->msdata.sFS,2) < -100))
	{
		//printf("OutageF!!!!!%f\n",outage);
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
}


struct FemtoDifference{
	int FemtoID;
	double difference;
};

struct sortByDistance{

	bool operator () (const FemtoDifference& b1, const FemtoDifference& b2) const{

		return (int)(b1.difference*100000) < (int)(b2.difference*100000); //小排到大
	}

};







int
MSwithVectorFP::getFemtoListByFP(int length)
{
	int FsListNum=length;

	//multiset<FemtoDifference,sortByDistance> allFemto;
	vector<FemtoDifference> allFemto;
	sortByDistance sortOBJ;


	//GenVtFP_.decideCoordBase(msnode->msdata.VectorFingerprint);

	for(int FemtoCellID=0; FemtoCellID<FS_NUM_INDEX; FemtoCellID++){


		FemtoDifference femtodifference;



		femtodifference.FemtoID	= FemtoCellID;

		femtodifference.difference = GenVtFP_.difference(msnode->msdata.VectorFingerprint, (*fsdata_)[FemtoCellID].VectorFingerprint, false,0);



		allFemto.push_back(femtodifference);
		//allFemto.insert(femtodifference);

	}
	///
	sort( allFemto.begin(), allFemto.end(), sortOBJ);
#if VFP_SHIFT_ENABLE
	if(allFemto[0].difference>5)
	{
		allFemto.clear();

		for(int FemtoCellID=0; FemtoCellID<FS_NUM_INDEX; FemtoCellID++){


		FemtoDifference femtodifference;



		femtodifference.FemtoID	= FemtoCellID;

		femtodifference.difference = GenVtFP_.difference(msnode->msdata.VectorFingerprint, (*fsdata_)[FemtoCellID].VectorFingerprint, true, SHIFT_VALUE);//(int)(allFemto[0].difference) 5



		allFemto.push_back(femtodifference);
		//allFemto.insert(femtodifference);

		}
		sort( allFemto.begin(), allFemto.end(), sortOBJ);
		///
	}
#endif
/*
	int testcount=0;
	for(int cellid=0;cellid<msnode->msdata.fs_near_num;cellid++)
	{

		if(msnode->msdata.distance(msnode->msdata.position,(*fsdata_)[msnode->msdata.FS_NEAR[cellid]].position)<=0.1)
		{

			printf("---------------0.05 sys-------Femto %d----(%f,%f)-----\n",msnode->msdata.FS_NEAR[cellid],(*fsdata_)[msnode->msdata.FS_NEAR[cellid]].position.x,(*fsdata_)[msnode->msdata.FS_NEAR[cellid]].position.y);
			for(vector<VectorElement>::iterator FPit=(*fsdata_)[msnode->msdata.FS_NEAR[cellid]].VectorFingerprint.Elements.begin();FPit!=(*fsdata_)[msnode->msdata.FS_NEAR[cellid]].VectorFingerprint.Elements.end();FPit++)
			{
				printf("Cell %d\tValue:%f\n",FPit->CellID,FPit->ElementValue);

			}
			testcount++;
		}
	}
	printf("%d\n",testcount);
*/
/*
	int xxx=0;
	for( vector<FemtoDifference>::iterator it = allFemto.begin(); it!=(allFemto.end()); it++)
	{
		printf("Femto:%d value: %f\n",it->FemtoID,it->difference);
		if(xxx%20==0) system("pause");
		xxx++;
	}
	printf("=========\n");
*/
//	int outconunt=0;
	int i_length=0;
	for( vector<FemtoDifference>::iterator it = allFemto.begin(); it<(allFemto.begin()+length); it++)
//	for( multiset<FemtoDifference,sortByDistance>::iterator it = allFemto.begin(); i_length<=length; it++)
	{
		//printf("Femto:%d value: %f\n",it->FemtoID,(*it).difference);

		//extern Record ReListMeanDistanceBtwUserFetmo;
		//ReListMeanDistanceBtwUserFetmo.InsertData(msnode->msdata.distance(msnode->msdata.position,(*fsdata_)[it->FemtoID].position)*1000);


		FemtoListByFP->push_back((*it).FemtoID);
/*
		if(msnode->msdata.distance(msnode->msdata.position,(*fsdata_)[it->FemtoID].position)>0.35)
		{

			printf("Femto %d out of 0.35km:%f\n",it->FemtoID,msnode->msdata.distance(msnode->msdata.position,(*fsdata_)[it->FemtoID].position));
			printf("---------------------------------\n");
			for(vector<VectorElement>::iterator FPit=(*fsdata_)[it->FemtoID].VectorFingerprint.Elements.begin();FPit!=(*fsdata_)[it->FemtoID].VectorFingerprint.Elements.end();FPit++)
			{
				printf("Cell %d\tValue:%f\n",FPit->CellID,FPit->ElementValue);

			}
			printf("\n");

			outconunt++;
		}
*/
		i_length++;
	}
/*
	printf("==========MS==========(%f,%f)\n",msnode->msdata.position.x,msnode->msdata.position.y);
	for(vector<VectorElement>::iterator FPit=msnode->msdata.VectorFingerprint.Elements.begin();FPit!=msnode->msdata.VectorFingerprint.Elements.end();FPit++)
	{
		printf("Cell %d\tValue:%f\n",FPit->CellID,FPit->ElementValue);

	}

	//printf("FemtoListByFP.size: %d\n",FemtoListByFP.size());
	system("pause");
*/
	//printf("miss number in List:%d\n",outconunt);
	//system("pause");

	return FsListNum;
}


void
MSwithVectorFP::FemtoScan(){

	sortByRSSI SortObj;

	for(vector<int>::iterator femtoit=FemtoListByFP->begin(); femtoit!=FemtoListByFP->end();femtoit++)
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
MSwithVectorFP::optimumFemtoScan(){

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

