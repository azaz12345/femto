#include "CSG.h"




CSG::CSG ()
{
    v_fs_near_CSG = new vector < vector<int> >;
    //cdor
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
                        _temp_distance = distance(fsdata[BLOCK[s_block].FS_num[fap_ix]].position, it->xy);
                        if(_temp_distance < 0.2) //
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


};

void
CSG::NeighborCSG_OSG()
{
    int fs_near_CSG_num ,i = 0;
    double j;

    //vector < vector<int> > v_fs_near_CSG ;
    vector <int> Num_MS ;
    vector<int>::iterator its;

    fs_near_CSG_num = (int) (fs_CSG_Percent * v_fs_near->size());

    Search:
    for(vector<int>::iterator inter = v_fs_near->begin(); inter != v_fs_near->end(); inter++)
    {
        if( fsdata[*inter].fs_mode == 0 )
        {
            j= Rand();
            if(j <= fs_CSG_Percent && Num_MS.size() <fs_near_CSG_num )
            {
                Num_MS.push_back(*inter);
            }
        }
    }
        sort(Num_MS.begin(), Num_MS.end());
        its = unique(Num_MS.begin(), Num_MS.end());
        Num_MS.resize( its - Num_MS.begin() );

    if( Num_MS.size() < fs_near_CSG_num )
        goto Search ;



    v_fs_near_CSG->push_back( Num_MS );


};

bool CSG::Femto_Use( int BSID , int Group_)
{
    if( fsdata[BSID].fs_mode == 1 )
    {
        return true ;
    }
    else if ( fsdata[BSID].CSG_GROUP == Group_ )
    {
        return true ;
    }


    return false;

};

