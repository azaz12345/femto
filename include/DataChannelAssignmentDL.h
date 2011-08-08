#ifndef DATACHANNELASSIGNMENTDL_H
#define DATACHANNELASSIGNMENTDL_H

#include "stdhead.h"

class DataChannelAssignmentDL
{
	public:
		DataChannelAssignmentDL( XYAXIS (*BSOxy)[NUM_CELL], FS_INFO (*fsdata__)[FS_NUM]);
		virtual ~DataChannelAssignmentDL();

		virtual int AssignDataChannel(MSINFO& msdata);

	protected:


	private:

		XYAXIS (*BSOxy_)[NUM_CELL];
		FS_INFO (*fsdata_)[FS_NUM];

};

#endif // DATACHANNELASSIGNMENTDL_H
