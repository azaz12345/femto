#include "GenFingerprintBase.h"
#include "external_parameters.h"
#include <cmath>
#include <set>
#include <algorithm>

/***analysis***/
#include "Record.h"
/************/

/***********************************

				The fingerprint base class

***********************************/


/************************************

				The Vector fingerprint class

*************************************/

GenVectorFP::GenVectorFP()
{

	CoordSys =
	{
		{	0,		1,		2,		3,		4,		5,		6		},
		{	1,		7,		8,		2,		0,		6,		18	},
		{	2,		8,		9,		10,	3,		0,		1		},
		{	3,		2,		10,	11,	12,	4,		0		},
		{	4,		0,		3,		12,	13,	14,	5		},
		{	5,		6,		0,		4,		14,	15,	16	},
		{	6,		18,	1,		0,		5,		16,	17	},
		{	7,		11,	15,	8,		1,		18,	12	},
		{	8,		15,	14,	9,		2,		1,		7		},
		{	9,		14,	13,	17,	10,	2,		8		},
		{	10,	9,		17,	16,	11,	3,		2		},
		{	11,	10,	16,	15,	7,		12,	3		},
		{	12,	3,		11,	7,		18,	13,	4		},
		{	13,	4,		12,	18,	17,	9,		14	},
		{	14,	5,		4,		13,	9,		8,		15	},
		{	15,	16,	5,		14,	8,		7,		11	},
		{	16,	17,	6,		5,		15,	11,	10	},
		{	17,	13,	18,	6,		16,	10,	9		},
		{	18,	12,	7,		1,		6,		17,	13	}

	};

}


GenVectorFP::GenVectorFP(int ServingMacroCell):
	ServingMacroCell_(ServingMacroCell)
{

	CoordSys =
	{
		{	0,		1,		2,		3,		4,		5,		6		},
		{	1,		7,		8,		2,		0,		6,		18	},
		{	2,		8,		9,		10,	3,		0,		1		},
		{	3,		2,		10,	11,	12,	4,		0		},
		{	4,		0,		3,		12,	13,	14,	5		},
		{	5,		6,		0,		4,		14,	15,	16	},
		{	6,		18,	1,		0,		5,		16,	17	},
		{	7,		11,	15,	8,		1,		18,	12	},
		{	8,		15,	14,	9,		2,		1,		7		},
		{	9,		14,	13,	17,	10,	2,		8		},
		{	10,	9,		17,	16,	11,	3,		2		},
		{	11,	10,	16,	15,	7,		12,	3		},
		{	12,	3,		11,	7,		18,	13,	4		},
		{	13,	4,		12,	18,	17,	9,		14	},
		{	14,	5,		4,		13,	9,		8,		15	},
		{	15,	16,	5,		14,	8,		7,		11	},
		{	16,	17,	6,		5,		15,	11,	10	},
		{	17,	13,	18,	6,		16,	10,	9		},
		{	18,	12,	7,		1,		6,		17,	13	}

	};


	//set<int> CoordBase_ =
	CoordBase = set<int>(CoordSys[ServingMacroCell_], CoordSys[ServingMacroCell_]+7);

}




VectorFP
GenVectorFP::buildFP(double* RSSIs){

	VectorFP fp;

	if(ServingMacroCell_>=7)
	{
		//printf("##%d##In simulation System,ms choose the further mBS(two-tier mBS) to be serving due to the other suitable mBS is overloading.\n",ServingMacroCell_);
		//system("pause");
	}

	for(int CellID=0; CellID<NUM_CELL; CellID++){


		if(RSSIs[CellID]<-100) continue;

		VectorElement elem;

		elem.CellID	=	CellID;
		elem.ElementValue	=	RSSIs[elem.CellID];

		fp.Elements.push_back(elem);

	}




/*
	for(vector<VectorElement>::iterator it=fp.Elements.begin(); it!=fp.Elements.end(); it++)
	{

		printf("CELL %d : %f\n",it->CellID,it->ElementValue);
	}
	system("pause");
*/
/*
	if(fp.Elements.size()>=3)
	{
		printf("FP成員不足三個\n");
		system("pause");
	}
*/
	sort(fp.Elements.begin(), fp.Elements.end(),sortByElementValue());
	return fp;

}

VectorFP
GenVectorFP::buildFP(double* RSSIs, double time){

	VectorFP fp;

	if(ServingMacroCell_>=7)
	{
		//printf("##%d##In simulation System,ms choose the further mBS(two-tier mBS) to be serving due to the other suitable mBS is overloading.\n",ServingMacroCell_);
		//system("pause");
	}
	fp.time = time;
	for(int CellID=0; CellID<NUM_CELL; CellID++){


		if(RSSIs[CellID]<-100) continue;

		VectorElement elem;

		elem.CellID	=	CellID;
		elem.ElementValue	=	RSSIs[elem.CellID];

		fp.Elements.push_back(elem);

	}

	sort(fp.Elements.begin(), fp.Elements.end(),sortByElementValue());
	return fp;
}


void
GenVectorFP::decideNearCell(VectorFP& M){



}



/*
return true 	CoorBase has been changed.
return false	CoorBase is not changed.
*/

bool
GenVectorFP::decideCoordBase(VectorFP& M){

}



double
GenVectorFP::difference(VectorFP& M, VectorFP& B){

	double difference_ = 0;
	//findByVectorElement findOBJ;

	for(vector<VectorElement>::iterator Mit=M.Elements.begin(); Mit!=M.Elements.end(); Mit++){
		vector<VectorElement>::iterator Bit;
		if(  (Bit = find_if(B.Elements.begin(), B.Elements.end(), findByVectorElement(Mit->CellID)))  ==  B.Elements.end()   )
		{
			difference_+=pow(Mit->ElementValue-(-100),2);
			//printf("Mit->CellID:%d\n",Mit->CellID);
			continue;
		}


		difference_+=pow(Mit->ElementValue-Bit->ElementValue,2);
		//printf("Mit->CellID:%d\n",Mit->CellID);


	}
	//printf("\n");
	//system("pause");
	return sqrt(difference_);
}

double
GenVectorFP::difference(VectorFP& M, VectorFP& B, bool outside,  double shift){

	double difference_ = 0;
	//findByVectorElement findOBJ;

	double ShiftValuse=0;

	if(outside==true)
		ShiftValuse = shift;



	for(vector<VectorElement>::iterator Mit=M.Elements.begin(); Mit!=M.Elements.end(); Mit++){
		vector<VectorElement>::iterator Bit;
		if(  (Bit = find_if(B.Elements.begin(), B.Elements.end(), findByVectorElement(Mit->CellID)))  ==  B.Elements.end()   )
		{
			difference_+=pow(Mit->ElementValue-ShiftValuse-(-100),2);
			//printf("Mit->CellID:%d\n",Mit->CellID);
			continue;
		}


		difference_+=pow(Mit->ElementValue-ShiftValuse-Bit->ElementValue,2);
		//printf("Mit->CellID:%d\n",Mit->CellID);


	}
	//printf("\n");
	//system("pause");
	return sqrt(difference_);
}



double
GenVectorFP::DotProduct(VectorFP& V1, VectorFP& V2){
	double DotProductValue_=0;
/*
	for(int i=0; i<7; i++)
	{
		//檢查element中CellID是否有在CoorBase set中
		if(CoordBase.find(V1.Elements[i].CellID)==CoordBase.end())
			continue;

		//逐一Check找出B(Fingerprint)中與此CellID一樣的element並將其所帶值相減計算
		for(int j=0;j<7;j++)
		{
			if(V1.Elements[i].CellID!=V2.Elements[j].CellID) continue;

			DotProductValue_ += (V1.Elements[i].ElementValue)*(V2.Elements[j].ElementValue);
			break;
		}
	}
*/
/*
	printf("V1\n");
	for(vector<VectorElement>::iterator itV1=V1.Elements.begin(); itV1!=V1.Elements.end(); itV1++)
	{
		printf("CellID:%d\tRSSI:%f\n",itV1->CellID,itV1->ElementValue);

	}
	printf("V2\n");
	for(vector<VectorElement>::iterator itV2=V2.Elements.begin(); itV2!=V2.Elements.end(); itV2++)
	{
		printf("CellID:%d\tRSSI:%f\n",itV2->CellID,itV2->ElementValue);

	}
*/
	//if(V1.Elements.size()==0 || V2.Elements.size()==0) return 0;

	for(vector<VectorElement>::iterator itV1=V1.Elements.begin(); itV1!=V1.Elements.end(); itV1++)
	{
		for(vector<VectorElement>::iterator itV2=V2.Elements.begin(); itV2!=V2.Elements.end(); itV2++)
		{
			if(itV1->CellID!=itV2->CellID) continue;
			DotProductValue_ += (itV1->ElementValue)*(itV2->ElementValue);

		}

	}


	return DotProductValue_;
}


/*
High mobility state return 1
Low mobility state return 2
Velocity(km/hr)
*/

double
GenVectorFP::EstVelocityState(VectorFP& currM, VectorFP& preM, double scanRange, double VelocityThr){
	double VelocityThreshold_PathLoss=0;
	double Sdifference3 = 0;
	double Sdifference1 =0;
	double Sdifference2 = 0;

	double Cosine = EstCosine(currM , preM);


	int found=1;
	for(vector<VectorElement>::iterator itPreM=preM.Elements.begin(); itPreM!=preM.Elements.end(); itPreM++)
	{
		if(ServingMacroCell_!=itPreM->CellID) continue;
		found=0;
		Sdifference1 = 41.532 - itPreM->ElementValue;
	}
	if(found==1) Sdifference1 = 41.532 - (-100);

	found=1;
	for(vector<VectorElement>::iterator itCurrM=currM.Elements.begin(); itCurrM!=currM.Elements.end(); itCurrM++)
	{
		if(ServingMacroCell_!=itCurrM->CellID) continue;
		found=0;
		Sdifference2 = 41.532 - itCurrM->ElementValue;
	}
	if(found==1) Sdifference2 = 41.532 - (-100);




	Sdifference1 = pow(10,(Sdifference1 - 60.26 - 21*log10(f_carrier))/38);
	Sdifference2 = pow(10,(Sdifference2 - 60.26 - 21*log10(f_carrier))/38);

	//Sdifference1 = pow(10,(Sdifference1/10));
	//Sdifference2 = pow(10,(Sdifference2)/10);


//	double Sdifference1 = BSVectorFP[ServingMacroCell_].Elements[0].ElementValue - preM.Elements[0].ElementValue;
//	double Sdifference2 = BSVectorFP[ServingMacroCell_].Elements[0].ElementValue - currM.Elements[0].ElementValue;

	double Sdifference3_square = pow(Sdifference1,2) + pow(Sdifference2,2) -2*Sdifference1*Sdifference2*Cosine;
	if( (int)(Sdifference3_square*1000000000) <= 0 ) Sdifference3_square=0.000000000000001;
	//Value(Sdifference3_square) must be larger than 0. Because sqrt. so we set Sdifference3_square to 0.00000001.
	//This is very small value.


	Sdifference3 = sqrt( Sdifference3_square);
	/////////////////Analysis///////////////////////


	Sdifference3 = 60.26+38*log10(Sdifference3/( currM.time - preM.time ))+21*log10(f_carrier);
	//Sdifference3 = 60.26+38*log10(Sdifference3)+21*log10(f_carrier);

	//VelocityThreshold_PathLoss = 60.26+38*log10((VelocityThr/3600)*scanRange)+21*log10(f_carrier);
	VelocityThreshold_PathLoss = 60.26+38*log10(VelocityThr/3600)+21*log10(f_carrier);

	//printf("Sdifference1:%f\nSdifference2:%f\nCosine:%f\nscanRange:%f\n",Sdifference1,Sdifference2,Cosine,scanRange);

	//printf("pre_Sdifference3:%f\nSdifference3:%f\nVelocityThreshold_PathLoss:%f\n", pow(Sdifference1,2) + pow(Sdifference2,2) -2*Sdifference1*Sdifference2*Cosine,Sdifference3,VelocityThreshold_PathLoss);
	//system("pause");


	//if(Sdifference3/( currM.time - preM.time ) < 1.47*0.001) return 2;
	if(Sdifference3 < 0.50*VelocityThreshold_PathLoss) return 2;
	//if(Sdifference3 < (VelocityThr/3600)*scanRange) return 2;
	return 1;
}

/*
double
GenVectorFP::EstVelocityState(VectorFP& currM, VectorFP& preM, double scanRange, double VelocityThr){
	double VelocityThreshold_PathLoss=0;
	double Sdifference3 = 0;
	double Sdifference1 =0;
	double Sdifference2 = 0;


	double Cosine = EstCosine(currM , preM);
/*
	VectorFP V1,V2;


	for(vector<VectorElement>::iterator itCurr=currM.Elements.begin(); itCurr!=currM.Elements.end(); itCurr++)
	{
		vector<VectorElement>::iterator itMB;
		//printf("%d\t%f\n",itCurr->CellID, itCurr->ElementValue);
		if((itMB=find_if( BSVectorFP[ServingMacroCell_].Elements.begin(), BSVectorFP[ServingMacroCell_].Elements.end(),findByVectorElement(itCurr->CellID)))==BSVectorFP[ServingMacroCell_].Elements.end())
		{
			//printf("1KKKKKKKKKKKKKKKKKKKKKKKKKKKKK\n");
			VectorElement elem;

			elem.CellID = itCurr->CellID;
			elem.ElementValue = itCurr->ElementValue - (-100);

			V1.Elements.push_back(elem);
			continue;
		}
		//printf("1HHHHHHHHHHHHHHHHHHHHHHHHHHH\n");
		VectorElement elem;

		elem.CellID = itCurr->CellID;
		elem.ElementValue = itCurr->ElementValue - (itMB->ElementValue);

		V1.Elements.push_back(elem);
	}

	for(vector<VectorElement>::iterator sBSit=BSVectorFP[ServingMacroCell_].Elements.begin(); sBSit!=BSVectorFP[ServingMacroCell_].Elements.end(); sBSit++)
	{
		vector<VectorElement>::iterator itV1;

		if((itV1=find_if(V1.Elements.begin(),V1.Elements.end(),findByVectorElement(sBSit->CellID)))==V1.Elements.end())
		{
			VectorElement elem;

			elem.CellID = sBSit->CellID;
			elem.ElementValue = (-100) - sBSit->ElementValue;
			V1.Elements.push_back(elem);
			continue;
		}

	}

	//printf("\n");

	//if(currM.Elements.size()>0)
	for(vector<VectorElement>::iterator itPre=preM.Elements.begin(); itPre!=preM.Elements.end(); itPre++)
	{
		vector<VectorElement>::iterator itMB;
		//printf("%d\t%f\n",itPre->CellID, itPre->ElementValue);
		if((itMB=find_if( BSVectorFP[ServingMacroCell_].Elements.begin(), BSVectorFP[ServingMacroCell_].Elements.end(),findByVectorElement(itPre->CellID)))==BSVectorFP[ServingMacroCell_].Elements.end())
		{
			//printf("2KKKKKKKKKKKKKKKKKKKKKKKKKKKKK\n");
			VectorElement elem;

			elem.CellID = itPre->CellID;
			elem.ElementValue = itPre->ElementValue - (-100);

			V2.Elements.push_back(elem);
			continue;
		}
		//printf("2HHHHHHHHHHHHHHHHHHHHHHHHHHH\n");
		VectorElement elem;

		elem.CellID = itPre->CellID;
		elem.ElementValue = itPre->ElementValue - (itMB->ElementValue);

		V2.Elements.push_back(elem);
	}

	for(vector<VectorElement>::iterator sBSit=BSVectorFP[ServingMacroCell_].Elements.begin(); sBSit!=BSVectorFP[ServingMacroCell_].Elements.end(); sBSit++)
	{
		vector<VectorElement>::iterator itV2;

		if((itV2=find_if(V2.Elements.begin(),V2.Elements.end(),findByVectorElement(sBSit->CellID)))==V2.Elements.end())
		{
			VectorElement elem;

			elem.CellID = sBSit->CellID;
			elem.ElementValue = (-100) - sBSit->ElementValue;

			V2.Elements.push_back(elem);
			continue;
		}

	}




	//取共同的部分作為新的Vector
	for(vector<VectorElement>::iterator itV1=V1.Elements.begin(); itV1!=V1.Elements.end(); itV1++)
	{
		//vector<VectorElement>::iterator tmpit;
		if((find_if(V2.Elements.begin(),V2.Elements.end(),findByVectorElement(itV1->CellID)))==V2.Elements.end())
		{
			//printf("1111111111111111111111111##%d\n",V1.Elements.size());
			//printf("itV1->CellID:%d\titV1->ElementValue:%f\n",itV1->CellID,itV1->ElementValue);

			itV1 = V1.Elements.erase(itV1);
			itV1--;

		}
	}

	for(vector<VectorElement>::iterator itV2=V2.Elements.begin(); itV2!=V2.Elements.end(); itV2++)
	{
		//vector<VectorElement>::iterator tmpit;
		if((find_if(V1.Elements.begin(),V1.Elements.end(),findByVectorElement(itV2->CellID)))==V1.Elements.end())
		{
			//printf("RRRRRRRRRRRRRRRRRRRRRRRRR\n");

			itV2 = V2.Elements.erase(itV2);
			itV2--;

		}
	}


//	Sdifference1=sqrt(DotProduct(V1,V1));
//	Sdifference2=sqrt(DotProduct(V2,V2));


//	double Sdifference3_square = pow(Sdifference1,2) + pow(Sdifference2,2) -2*Sdifference1*Sdifference2*Cosine;

	VectorFP V3;

	for(vector<VectorElement>::iterator itV1=V1.Elements.begin(); itV1!=V1.Elements.end(); itV1++)
	{
		//vector<VectorElement>::iterator tmpit;
		vector<VectorElement>::iterator itV2;
		if((itV2 = find_if(V2.Elements.begin(),V2.Elements.end(),findByVectorElement(itV1->CellID)))==V2.Elements.end())
		{
			continue;

		}
		VectorElement elem;

		elem.CellID=itV1->CellID;
		elem.ElementValue = itV2->ElementValue - itV1->ElementValue;

		V3.Elements.push_back(elem);

	}


	double Sdifference3_square = DotProduct(V3,V3);
	if( (int)(Sdifference3_square*1000000000) <= 0 ) Sdifference3_square=0.0000000001;
	//Value(Sdifference3_square) must be larger than 0. Because sqrt. so we set Sdifference3_square to 0.00000001.
	//This is very small value.


	Sdifference3 = difference(preM,currM);//sqrt( Sdifference3_square);
	/////////////////Analysis///////////////////////
	extern double speedTotal,speedCount;
	extern Record ReSpeed;
	ReSpeed.InsertData( 1000*(Sdifference3  )/( currM.time - preM.time ) );
	speedTotal += Sdifference3/( currM.time - preM.time );
	speedCount+=1;
	//////////////////////////////////////////////////
	//Sdifference3 = 60.26+38*log10(Sdifference3)+21*log10(f_carrier);

	VelocityThreshold_PathLoss = 60.26+38*log10((VelocityThr/3600)*scanRange)+21*log10(f_carrier);

	//printf("Sdifference1:%f\nSdifference2:%f\nCosine:%f\nscanRange:%f\n",Sdifference1,Sdifference2,Cosine,scanRange);

	//printf("pre_Sdifference3:%f\nSdifference3:%f\nVelocityThreshold_PathLoss:%f\n", pow(Sdifference1,2) + pow(Sdifference2,2) -2*Sdifference1*Sdifference2*Cosine,Sdifference3,VelocityThreshold_PathLoss);
	//system("pause");

	//if(Sdifference3 < VelocityThreshold_PathLoss) return 2;
	if(Sdifference3 < 0.75*VelocityThreshold_PathLoss) return 2;
	//if(Sdifference3 < (VelocityThr/3600)*scanRange) return 2;
	return 1;
}

*/


/*
	使用此func一定要用GenVectorFP(int ServingMacroCell)此建構子來init CoordBase
	因為distance一定要使用CoorBase來能順利計算

*/
double
GenVectorFP::EstCosine(VectorFP& currM, VectorFP& preM){

	double EstCosine_ = 0;
	VectorFP V1,V2;


	for(vector<VectorElement>::iterator itCurr=currM.Elements.begin(); itCurr!=currM.Elements.end(); itCurr++)
	{
		vector<VectorElement>::iterator itMB;
		//printf("%d\t%f\n",itCurr->CellID, itCurr->ElementValue);
		if((itMB=find_if( BSVectorFP[ServingMacroCell_].Elements.begin(), BSVectorFP[ServingMacroCell_].Elements.end(),findByVectorElement(itCurr->CellID)))==BSVectorFP[ServingMacroCell_].Elements.end())
		{
			//printf("1KKKKKKKKKKKKKKKKKKKKKKKKKKKKK\n");
			VectorElement elem;

			elem.CellID = itCurr->CellID;
			elem.ElementValue = itCurr->ElementValue - (-100);

			V1.Elements.push_back(elem);
			continue;
		}
		//printf("1HHHHHHHHHHHHHHHHHHHHHHHHHHH\n");
		VectorElement elem;

		elem.CellID = itCurr->CellID;
		elem.ElementValue = itCurr->ElementValue - (itMB->ElementValue);

		V1.Elements.push_back(elem);
	}

	for(vector<VectorElement>::iterator sBSit=BSVectorFP[ServingMacroCell_].Elements.begin(); sBSit!=BSVectorFP[ServingMacroCell_].Elements.end(); sBSit++)
	{
		vector<VectorElement>::iterator itV1;

		if((itV1=find_if(V1.Elements.begin(),V1.Elements.end(),findByVectorElement(sBSit->CellID)))==V1.Elements.end())
		{
			VectorElement elem;

			elem.CellID = sBSit->CellID;
			elem.ElementValue = (-100) - sBSit->ElementValue;
			V1.Elements.push_back(elem);
			continue;
		}
/*
		VectorElement elem;

		elem.CellID = sBSit->CellID;
		elem.ElementValue = (itV1->ElementValue) - sBSit->ElementValue;
		V1.Elements.push_back(elem);
*/
	}

	//printf("\n");

	//if(currM.Elements.size()>0)
	for(vector<VectorElement>::iterator itPre=preM.Elements.begin(); itPre!=preM.Elements.end(); itPre++)
	{
		vector<VectorElement>::iterator itMB;
		//printf("%d\t%f\n",itPre->CellID, itPre->ElementValue);
		if((itMB=find_if( BSVectorFP[ServingMacroCell_].Elements.begin(), BSVectorFP[ServingMacroCell_].Elements.end(),findByVectorElement(itPre->CellID)))==BSVectorFP[ServingMacroCell_].Elements.end())
		{
			//printf("2KKKKKKKKKKKKKKKKKKKKKKKKKKKKK\n");
			VectorElement elem;

			elem.CellID = itPre->CellID;
			elem.ElementValue = itPre->ElementValue - (-100);

			V2.Elements.push_back(elem);
			continue;
		}
		//printf("2HHHHHHHHHHHHHHHHHHHHHHHHHHH\n");
		VectorElement elem;

		elem.CellID = itPre->CellID;
		elem.ElementValue = itPre->ElementValue - (itMB->ElementValue);

		V2.Elements.push_back(elem);
	}

	for(vector<VectorElement>::iterator sBSit=BSVectorFP[ServingMacroCell_].Elements.begin(); sBSit!=BSVectorFP[ServingMacroCell_].Elements.end(); sBSit++)
	{
		vector<VectorElement>::iterator itV2;

		if((itV2=find_if(V2.Elements.begin(),V2.Elements.end(),findByVectorElement(sBSit->CellID)))==V2.Elements.end())
		{
			VectorElement elem;

			elem.CellID = sBSit->CellID;
			elem.ElementValue = (-100) - sBSit->ElementValue;

			V2.Elements.push_back(elem);
			continue;
		}
/*
		VectorElement elem;

		elem.CellID = sBSit->CellID;
		elem.ElementValue = (itV2->ElementValue) - sBSit->ElementValue;
		V2.Elements.push_back(elem);
*/
	}




	//取共同的部分作為新的Vector
	for(vector<VectorElement>::iterator itV1=V1.Elements.begin(); itV1!=V1.Elements.end(); itV1++)
	{
		//vector<VectorElement>::iterator tmpit;
		if((find_if(V2.Elements.begin(),V2.Elements.end(),findByVectorElement(itV1->CellID)))==V2.Elements.end())
		{
			//printf("1111111111111111111111111##%d\n",V1.Elements.size());
			//printf("itV1->CellID:%d\titV1->ElementValue:%f\n",itV1->CellID,itV1->ElementValue);

			itV1 = V1.Elements.erase(itV1);
			itV1--;

		}
	}

	for(vector<VectorElement>::iterator itV2=V2.Elements.begin(); itV2!=V2.Elements.end(); itV2++)
	{
		//vector<VectorElement>::iterator tmpit;
		if((find_if(V1.Elements.begin(),V1.Elements.end(),findByVectorElement(itV2->CellID)))==V1.Elements.end())
		{
			//printf("RRRRRRRRRRRRRRRRRRRRRRRRR\n");

			itV2 = V2.Elements.erase(itV2);
			itV2--;

		}
	}

	//printf("V1.Elements.size():%d\nV2.Elements.size():%d\nBSVectorFP[ServingMacroCell_].Elements.size():%d\tServing%d\n",V1.Elements.size(),V2.Elements.size(),BSVectorFP[ServingMacroCell_].Elements.size(),ServingMacroCell_);
	//system("pause");

	EstCosine_ = DotProduct(V1,V2)/( sqrtl(DotProduct(V1,V1)) * sqrtl(DotProduct(V2,V2)) );

	return EstCosine_;
}

