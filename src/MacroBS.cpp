#include "MacroBS.h"
#include "external_parameters.h"
#include "GenFingerprintBase.h"

MacroBS::MacroBS()
{
	//ctor
}

MacroBS::~MacroBS()
{
	//dtor
}

void
MacroBS::buildAllFP(){

	COORD FSNODE;
	LinkList mbs; //use for FP

	for(int mCellID=0; mCellID<NUM_CELL; mCellID++){

		double RSSIs[NUM_CELL] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		for(int CellID=0; CellID<NUM_CELL; CellID++){
			double PathLoss_ = mbs.pathloss( 0, BSOxy_MAI[CellID], BSOxy_MAI[mCellID]);
			POLAXIS position_polar =FSNODE.cart2pol(BSOxy_MAI[CellID], BSOxy_MAI[mCellID]);
			double Max_DL_power_ = 46.532;
			double Back_off_ = Back_off;
			double Max_DL_EIRP_;        //所有marco都是一樣
			double AntennaGain_ = -20;  //初始化為-20
			Max_DL_EIRP_ = Max_DL_power_ - Back_off_;

			for(int SectorID=0; SectorID<NUM_SECTOR; SectorID++){
				if(AntennaGain_< mbs.antenna_pattern(SECTOR_Steer_dir[SectorID],position_polar.th))
					AntennaGain_ = mbs.antenna_pattern(SECTOR_Steer_dir[SectorID],position_polar.th);
			}

			if(mCellID==CellID)
			RSSIs[CellID] = Max_DL_EIRP_;//Max_DL_power_  - 10*log10((float)All_carriers);//Max_DL_EIRP_ - 10*log10((float)All_carriers); //17 is max antenna gain
			else
			RSSIs[CellID] = Max_DL_EIRP_ + AntennaGain_- PathLoss_ ;//- 10*log10((float)All_carriers);
			//RSSIs[CellID] = Max_DL_EIRP_ - PathLoss_ - 10*log10((float)All_carriers);






		}

		GenVectorFP GenVtFP_(mCellID);
		BSVectorFP[mCellID] = GenVtFP_.buildFP(RSSIs);

		for(vector<VectorElement>::iterator it=BSVectorFP[mCellID].Elements.begin();it!=BSVectorFP[mCellID].Elements.end();it++)
		{
			printf("Cell %d\t RSSI:%f\n",it->CellID,it->ElementValue);
		}
		//system("pause");
	}


}
