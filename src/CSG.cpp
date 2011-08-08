#include "CSG.h"



CSG::CSG ()
{


};

void
CSG::NeighborFemto( vector<WayPoint> *path )
{
    int  nearest_x_street_index  ,nearest_y_street_index ,  fs_near_num = 0 , temp_block , fap_ix , s_block,_size;
    double _temp_distance=0.0;
    _size = path->size();
    v_fs_near = new vector<int>;
    vector<int>::iterator it;


    for(vector<WayPoint>::iterator it = path->begin(); it != path->end(); it++)
    {
        U_Pathlist = new vector<WayPoint>[_size];

        //找出MS在哪個BLOCK
        nearest_x_street_index= (int) floor(it->xy.x/block_unit)+6;
        nearest_y_street_index= abs( (int) floor(it->xy.y/block_unit)-4);
        temp_block = (int) (nearest_y_street_index * 12 + nearest_x_street_index);

        //找附近9個BLOCK
        for( int i=0; i <= 2 ; i++ )
        {
            for( int j = 11; j <=13; j++ )
            {
                fap_ix =0;
                s_block = temp_block - (j-(i*12));

                if(s_block>=0 && s_block <120)
                {
                    while(fap_ix < BLOCK[s_block].FS_num_length)
                    {
                        _temp_distance = _Distance(fsdata[BLOCK[s_block].FS_num[fap_ix]].position, it->xy);
                        if(_temp_distance < 0.3) //
                        {

                                v_fs_near->push_back(BLOCK[s_block].FS_num[fap_ix]);
                                fs_near_num ++;
                        }
                        fap_ix ++;
                    } //end of while
                } //end of if
            }
        }
    }
        sort(v_fs_near->begin(), v_fs_near->end());
        it = unique(v_fs_near->begin(), v_fs_near->end());
        v_fs_near->resize( it - v_fs_near->begin() );

        printf("fs_near_num %d", v_fs_near->size());
/*
    for(vector<int>::iterator inter = v_fs_near->begin(); inter != v_fs_near->end(); inter++)
    {
        printf("\nfs_near %d", *inter);
    }
*/
//        system("pause");
};

void
CSG::NeighborCSG_OSG( )
{

    int fs_near_CSG_num ,i = 0;
    double j;
    v_fs_near_CSG = new vector<int>;

    for(vector<int>::iterator inter = v_fs_near->begin(); inter != v_fs_near->end(); inter++)
    {
        if( fsdata[*inter].fs_mode == 0 )
        {
            fs_near_CSG_num = fs_CSG_Percent * v_fs_near->size();
            j= Rand();
            if( v_fs_near_CSG->size() <fs_near_CSG_num )
            {
                if(  j  <=fs_CSG_Percent)
                {
                    v_fs_near_CSG->push_back(*inter);
                }
            }
        }
    }

    for(vector<int>::iterator inter = v_fs_near_CSG->begin(); inter != v_fs_near_CSG->end(); inter++)
    {
        printf("\nfs_near %d", *inter);
        system("PAUSE");
    }
};



double CSG::_Distance ( XYAXIS _Point , XYAXIS Point_ )
{
    double Two_Point_Distance_X , Two_Point_Distance_Y ;

    Two_Point_Distance_X = _Point.x - Point_ .x ;
    Two_Point_Distance_Y = _Point.y - Point_ .y ;

    return sqrt ( pow(Two_Point_Distance_X , 2)+pow ( Two_Point_Distance_Y , 2) );

};

