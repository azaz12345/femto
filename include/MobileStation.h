#ifndef MOBILESTATION_H
#define MOBILESTATION_H

#include <vector>
#include <algorithm>

#include "stdhead.h"
#include "LocationState.h"
#include "CSG.h"


#define		UNKNOW_STATE		0x0000

#define		HIGH_VELOCITY_STATE	0x0001
#define		LOW_VELOCITY_STATE		0x0002

#define		HIGH_VELOCITY_SERVING_MACRO		0x0003
#define 		HIGH_VELOCITY_SERVING_FEMTO		0x0004
#define		LOW_VELOCITY_SERVING_MACRO		0x0005
#define 		LOW_VELOCITY_SERVING_FEMTO		0x0006

using namespace std;





struct BSINFO{

    int BSTYPE;
    int BSID;
    int SECTOR; //when BSTYPE =1 ,it will be used.
    double RSSI;

};

/*
Special search function
vector for struct
*/
struct findBSID{
	int BSID_;
	findBSID(int BSID):BSID_(BSID){}

	bool operator () (const BSINFO& b) const{

		return b.BSID == BSID_;
	}

};

struct sortByRSSI{

	bool operator () (const BSINFO& b1, const BSINFO& b2) const{

		return b1.RSSI > b2.RSSI;
	}

};



/*MobileStation的基礎功能*/
class MobileStationBase: public LocationState, private LinkList
{
public:

    MobileStationBase( LinkList* Userlist_, XYAXIS (*BSOxy)[NUM_CELL], FS_INFO (*fsdata__)[FS_NUM], int Permutation__,CSG* csg_);

    virtual ~MobileStationBase();

    virtual void UpdateMSINFO( MSNODE* msnode_, double numticktime, double ticktime);

protected:

	virtual void UpdateFP();

    virtual void MacroScan();
    virtual void FemtoScan();



    virtual bool Handover(BSINFO& TargetBS);

	virtual void DepartureServingCell();

	virtual BSINFO DecideTargetCell( int BSType);

    //return BSID
    virtual int HandoverDecision();
    //return 0 -> #ERROR# msdata = NULL
    virtual int getNeighborFemtoList();


    /*
      BSID      =>femto是0~1999
      BSType    =>1:Marco, 2:Femto
    */
    double CrtChMacroSINR(int SectorID,int BSID,int BSType);
    double CrtChFemtoSINR(int BSID,int BSType);



    /*
    大基地台並指定哪個sector
    不適用於Femtocell
    */
    double MacroRSSI(int SectorID,int BSID,int BSType);
    double FemtoRSSI(int BSID,int BSType);


    double PathLoss(int BSID,int BSType);

    double NoiseDL(int Permutation, double subcarrierBW);

    MSNODE* msnode;
    XYAXIS (*BSOxy_)[NUM_CELL];
    FS_INFO (*fsdata_)[FS_NUM];

	vector<BSINFO > Macrolist;
    vector<BSINFO > Femtolist;
    CSG* CSG_;

private:

    int UserStateDicision();

    //POLAXIS  position_polar;

    int PermutationMode;

    LinkList* UserList;


};


class MSwithLevelFP : public MobileStationBase,private LinkList{

public:
	MSwithLevelFP( LinkList* Userlist_, XYAXIS (*BSOxy)[NUM_CELL], FS_INFO (*fsdata__)[FS_NUM], int Permutation__, CSG* csg_);

	~MSwithLevelFP();

	virtual void UpdateMSINFO( MSNODE* msnode_, double numticktime, double ticktime);

protected:

	//virtual int getNeighborFemtoList();
	int getFemtoListByFP();

	void FemtoScan();

	void optimumFemtoScan();


private:
	vector<int> FemtoListByFP;


};

class MSwithVectorFP:public MobileStationBase, private LinkList {

public:
	MSwithVectorFP( LinkList* Userlist_, XYAXIS (*BSOxy)[NUM_CELL], FS_INFO (*fsdata__)[FS_NUM], int Permutation__,CSG* csg_);

	~MSwithVectorFP();

	virtual void UpdateMSINFO( MSNODE* msnode_, double numticktime, double ticktime);

	vector<int>* FemtoListByFP;

    vector<int>* GetFemtoListByFPrf(){
        return FemtoListByFP;
    }


protected:
	int getFemtoListByFP(int length);

	void FemtoScan();

	void optimumFemtoScan();

private:

     GenVectorFP GenVtFP_;
//     vector<int> FemtoListByFP;


};



#endif // MOBILESTATION_H
