#ifndef DATACHANNELASSIGNMENTDL_H
#define DATACHANNELASSIGNMENTDL_H

#include "stdhead.h"
//#include "channel_allocation.h"



class DataChannelAssignmentDL
{
	public:
		DataChannelAssignmentDL( XYAXIS (*BSOxy)[NUM_CELL], FS_INFO (*fsdata__)[FS_NUM]);

		DataChannelAssignmentDL();

		void MacroChannelNum();

		virtual ~DataChannelAssignmentDL();

		virtual void RunCompute( MSNODE* msnode_1,int Permutation );

		void RunCompute( MSINFO& msdata , int Permutation );

		void CopyList(vector<MSNODE*> LNode);

		void AssignDataChannel(MSINFO& msdata,int Permutation);


	protected:


	private:

		XYAXIS (*BSOxy_)[NUM_CELL];
		FS_INFO (*fsdata_)[FS_NUM];

		vector<MSNODE*>* ListCall;


};

#endif // DATACHANNELASSIGNMENTDL_H
