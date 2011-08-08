#ifndef CSG_H
#define CSG_H



#include "fbs.h"
#include "stdhead.h"
#include "PathGenerator.h"
#include "external_parameters.h"
#include <vector>

#define fs_CSG_Percent 0.0

using namespace std;

class CSG : public COORD
{
    public :
        CSG ();
        void NeighborFemto( vector<WayPoint> *path );

        void NeighborCSG_OSG();

        bool Femto_Use( int BSID , int MSID);

    private :

        MSINFO* _msdata;
        LinkList *UserList;
        /*存放所有使用者的移動路徑*/
//      vector<WayPoint>* U_Pathlist;
        vector<int>* v_fs_near;
        vector<int> Num_MS;
        vector < vector<int> >* v_fs_near_CSG;
};

#endif
