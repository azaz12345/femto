#ifndef GENFINGERPRINTBASE_H
#define GENFINGERPRINTBASE_H
//#include "stdhead.h"

#include <algorithm>
#include <vector>
#include <set>
#include <cstdio>

using namespace std;

/*****************************

		The fingerprint base class
*****************************/

template <typename FP>
class GenFingerprintBase
{
	public:
		GenFingerprintBase();
		virtual ~GenFingerprintBase();

		virtual FP buildFP(double* RSSIs) = 0;

	protected:


	private:

};




template <typename FP>
GenFingerprintBase<FP>::GenFingerprintBase()
{
	//ctor
}


template <typename FP>
GenFingerprintBase<FP>::~GenFingerprintBase()
{
	//dtor
}

template <typename FP> FP
GenFingerprintBase<FP>::buildFP(double* RSSIs){

}

/*************************************
				 GenLevelFP class

**************************************/



typedef struct{

	int CellID;
	double Value;

} LevelElement;


struct sortByValue{

	bool operator () (const LevelElement& b1, const LevelElement& b2) const{

		return b1.Value > b2.Value;
	}

};


template <int numlevel>
struct LevelFP{

	//LevelFP():Level(new vector<int>[numlevel]){ }
	//~LevelFP(){ delete [] Level;}

	vector<int> Level[numlevel];
	//vector<int>* Level;

};



template <int numlevel, typename FP = LevelFP<numlevel> >
class GenLevelFP:public GenFingerprintBase< FP >{

	public:
		GenLevelFP(double upbound, double lowbound);

		virtual FP buildFP(double* RSSIs);

		int matchingFP(FP& M,FP& B);

		int getNumLevel()	{ return NumLevel_;};
		int getUpBound()	{return upbound_;};
		int getLowBound()	{return lowbound_;};

	protected:

	private:
		int NumLevel_;
		double upbound_;
		double lowbound_;

};

/*************************************
				class GenLevelFP Implementation

**************************************/


template <int numlevel, typename FP>
GenLevelFP<numlevel, FP>::GenLevelFP(double upbound, double lowbound):
	NumLevel_(numlevel),
	upbound_(upbound),
	lowbound_(lowbound)
{

}



template <int numlevel, typename FP> FP
GenLevelFP<numlevel, FP>::buildFP(double* RSSIs)
{
	FP fp;


	int MaxMacroCellID=7;
	double LevelRange;

	//檢查Level設定多少  如果是1就另外處理
	if(NumLevel_ !=1	) 	LevelRange = (upbound_-lowbound_)/(NumLevel_-1);
	if(NumLevel_ ==1	)	LevelRange = (upbound_-lowbound_);

	vector<LevelElement> allMBS;
	for(int CellID=0; CellID<19; CellID++)
	{
		LevelElement elem;
		elem.CellID = CellID;
		elem.Value = RSSIs[CellID];
		allMBS.push_back(elem);
	}
	sortByValue sortOBJ;
	sort(allMBS.begin(), allMBS.end(), sortOBJ);


	for(int Cell=0; Cell<=MaxMacroCellID; Cell++){


		for(int level=0; level<NumLevel_; level++){

			if(NumLevel_<=0	) printf("#ERROR#NumLevel_ MUST be greater than 0.\n");

			if(NumLevel_ !=1	)
			if( (int)(allMBS[Cell].Value*1000) < (int)(( upbound_- LevelRange*level )*1000) )
			{
				continue;

			}

			if(NumLevel_ ==1	)
			if( (int)(allMBS[Cell].Value*1000) < (int)(( lowbound_ )*1000) )
			{
				continue;

			}

			//printf("level:%d\tCellID:%d\n",level,CellID);
			fp.Level[level].push_back(allMBS[Cell].CellID);
			break;

		}

	}
/*
	//printf("0:%f\t1:%f\t2:%f\t3:%f\t4:%f\t5:%f\t6:%f\t7:%f\n",RSSIs[0],RSSIs[1],RSSIs[2],RSSIs[3],RSSIs[4],RSSIs[5],RSSIs[6],RSSIs[7]);
	for(int level=0; level<NumLevel_; level++){
		printf("-----------------------------------------------\n");
		printf("Level %d:\t",level);


		for(vector<int>::iterator it=fp.Level[level].begin(); it!=fp.Level[level].end(); it++){

			printf("%d\t",(*it));

		}
		printf("\n");

	}
	system("pause");
*/
	return fp;

}

/*
	return 0 these(A and B) are not matching.

	return 1 there(A and B) are matching.
*/

template <int numlevel, typename FP> int
GenLevelFP<numlevel, FP>::matchingFP(FP& M,FP& B){

	//vector<int> *aFPLevel = A.Level;
	//vector<int> *bFPLevel = B.Level;

	//if(aFPLevel==NULL ){ printf("aFPLevel==NULL\n"); system("pause");}
	//if(bFPLevel==NULL ){ printf("bFPLevel==NULL\n"); system("pause");}

	//檢查FP間彼此同LEVEL中的CELL數目是否一致

	for(int level=0; level<NumLevel_; level++){
		if(M.Level[level].size() !=  B.Level[level].size())
		{

			return 0;
		}

	}

	//同層互相比對是否一致

	for(int level=0; level<NumLevel_; level++)
	{
		for(vector<int>::iterator itM=M.Level[level].begin(); itM!=M.Level[level].end(); itM++)
		{

			if(	find(  B.Level[level].begin(),  B.Level[level].end(), (*itM) ) ==  B.Level[level].end()	)
			{

				return 0;
			}

		}

	}

	return 1;


}



/*******************************************

					VectorFP Class

使用buildFP一定要使用GenVectorFP(int ServingMacroCell);建構子
distance()則不一定
********************************************/





typedef struct{

	int CellID;
	double ElementValue;

} VectorElement;




struct findByVectorElement{
	int BSID_;
	findByVectorElement(int BSID):BSID_(BSID){}

	bool operator () (const VectorElement& b) const{

		return b.CellID == BSID_;
	}

};


struct sortByElementValue{

	bool operator () (const VectorElement& b1, const VectorElement& b2) const{

		return b1.ElementValue > b2.ElementValue;
	}

};

struct VectorFP{

	double time;

	vector<VectorElement> Elements;

};


class GenVectorFP{
public:

	GenVectorFP();
	GenVectorFP(int ServingMacroCell);

	VectorFP buildFP(double* RSSIs);
	VectorFP buildFP(double* RSSIs, double time);

	double difference(VectorFP& M, VectorFP& B);
	double difference(VectorFP& M, VectorFP& B, bool outside,double shif);

	//Veloicity estimation
/*
	High mobility state return 1
	Low mobility state return 2
*/
	double EstVelocityState(VectorFP& currM, VectorFP& preM, double scanRange, double VelocityThr);

	double EstCosine(VectorFP& currM, VectorFP& preM);

	int getServingCellID(){return ServingMacroCell_;};

/*
return true 	CoorBase has been changed.
return false	CoorBase is not changed.
*/
	void decideNearCell(VectorFP& M);

	bool decideCoordBase(VectorFP& M);

	vector<int> NearCells;

protected:
	double DotProduct(VectorFP& M, VectorFP& B);

private:

	set<int> CoordBase;



	int CoordSys[19][7];

	int ServingMacroCell_;

};


#endif // GENFINGERPRINTBASE_H
