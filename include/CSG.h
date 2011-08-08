#ifndef CSG_H
#define CSG_H



#include "fbs.h"
#include "stdhead.h"
#include "PathGenerator.h"
#include "external_parameters.h"
#include <vector>

#define fs_CSG_Percent 0.1

using namespace std;

class CSG
{
    public :
        CSG ();
        void NeighborFemto( vector<WayPoint> *path );
        double _Distance ( XYAXIS _Point , XYAXIS Point_ );
        void NeighborCSG_OSG();

    private :

        MSINFO* _msdata;
        FS_INFO* _fsdata_;
        LinkList *UserList;
        /*存放所有使用者的移動路徑*/
        vector<WayPoint>* U_Pathlist;
        vector<int>* v_fs_near;
        vector<int>* v_fs_near_CSG;
};

#endif
