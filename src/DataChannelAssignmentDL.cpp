#include "DataChannelAssignmentDL.h"

DataChannelAssignmentDL::DataChannelAssignmentDL( XYAXIS (*BSOxy)[NUM_CELL], FS_INFO (*fsdata__)[FS_NUM]):
BSOxy_(BSOxy),
fsdata_(fsdata__)
{
	//ctor
}

DataChannelAssignmentDL::~DataChannelAssignmentDL()
{
	//dtor
}


/*
	return�����G�N�Ochannel num in some RadioBand (channel num not channel index)
	�bRadioBand2����5��channel = 2*10+5 =25 (channel index in allBand)
	ex:
	In(1,3,3),
	a femtocell in sector0, and it maybe choose RadioBand2 to work, and it has 10 channel (num:0~9) in RadioBand2
*/
int
DataChannelAssignmentDL::AssignDataChannel(MSINFO& msdata){






}
