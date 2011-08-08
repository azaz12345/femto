#include "DataChannelAssignmentDL.h"
#include <queue>

LinkList LL ;
//extern std::priority_queue <MSNODE* , vector<MSNODE*> ,CompareTime > PQ ;


DataChannelAssignmentDL::DataChannelAssignmentDL( XYAXIS (*BSOxy)[NUM_CELL], FS_INFO (*fsdata__)[FS_NUM]):
BSOxy_(BSOxy),
fsdata_(fsdata__)
{
     MacroChannelNum();

//     LL.DL_sub_channel_initial();
	//ctor
}

DataChannelAssignmentDL::DataChannelAssignmentDL(){}


DataChannelAssignmentDL::~DataChannelAssignmentDL()
{
	//dtor
}

void
DataChannelAssignmentDL::RunCompute( MSNODE* msnode_1,int Permutation ){

    if(msnode_1->msdata.sub_channel_index == 0)
    {
        LL.LIF_assign_channel( msnode_1->msdata.femto_mode ,Permutation , msnode_1 );
    }

//    LL.SINR( msnode_1->msdata.femto_mode , Permutation);
}



void
DataChannelAssignmentDL::AssignDataChannel(MSINFO& msdata,int Permutation){

}




//-------------------MacroChannel-------------------//
void DataChannelAssignmentDL::MacroChannelNum()
{
    int channel_num[NUM_CELL];

    for(int j=0; j<NUM_CELL; j++)
        channel_num[j]=Num_channel_DL_PUSC;
}
