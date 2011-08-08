#include "LocationState.h"
#include <math.h>


enum State   { Crossroads, Level_Street, Vertical_Street , Home } MsState;
enum StateXY { Big_Street, Little_Street, House = 4 } MsStateXY;

int LocationState::WhichSector(XYAXIS MSPosition, XYAXIS BSPosition){

    int SecotrIndex = 0;

    double sector_angle = 360/(double)NUM_SECTOR;

    double angle = atan2(MSPosition.y - BSPosition.y, MSPosition.x - BSPosition.x);

    if(angle>=0)//-pi~pi => 0~360 degree
        angle=angle*180/PI;
    else
        angle=angle*180/PI+360;
    //choose the serving sector
    SecotrIndex=(INT16)floor((angle+sector_angle/2)/sector_angle);
    if(SecotrIndex>=NUM_SECTOR)
        SecotrIndex=SecotrIndex-(INT16)NUM_SECTOR;//0~NUM_SECTOR-1
    return SecotrIndex;

}



/*輸出值為0~119,因為Block[120]*/
int LocationState::WhichBlock(XYAXIS Position){
    /*找出座標最接近於哪條路,分成X軸平行的路跟Y軸平行的路*/
    int      NearestStreetIndexForX = (int) floor(Position.x/block_unit)+6;
    int      NearestStreetIndexForY = abs( (int) floor(Position.y/block_unit)-4);

    return (int)(NearestStreetIndexForY * 12 + NearestStreetIndexForX);
}

bool LocationState::IsOnStreet(XYAXIS Position){

    if( DecidePosition(Position)==0 ||
        DecidePosition(Position)==1 ||
        DecidePosition(Position)==2){

        return true;//street
    }
    else{
        return false;//house
    }
}

int LocationState::DecidePosition(XYAXIS Position)
{

    int DecideX = ComputePosition(fabs(Position.x*1000));
    int DecideY = ComputePosition(fabs(Position.y*1000));

    int DecideXY = DecideX + DecideY;
//    printf("DecideX:%dDecideY:%d\n",DecideX,DecideY);
    if( DecideXY < 4 )
        MsState = Crossroads;
    else if ( (DecideX == 4) &&(DecideY <= 1) )
        MsState = Level_Street;
    else if ( (DecideY == 4) &&(DecideX <= 1) )
        MsState = Vertical_Street;
    else if ( DecideXY == 8)
        MsState = Home ;

    return MsState;

}

/*
    Big Street:     0
    Little Street:  1
    House:          3

*/

int LocationState::ComputePosition(float Coordinate)
{

    int ComputeCoordinate = (( int )Coordinate )%230;

    if ( ComputeCoordinate < 0 )
    {
        ComputeCoordinate += 230 ;
    }

    if( ComputeCoordinate >= 100 && ComputeCoordinate <=130 )
    {
        MsStateXY = Big_Street;
    }
    else
    {
        if(ComputeCoordinate>130) ComputeCoordinate-=30;
        if ( (ComputeCoordinate %20 <3 ) ||  (ComputeCoordinate %20 >17 ) )
            MsStateXY = Little_Street;

        else
            MsStateXY = House;

    }

    return MsStateXY;

}
/*
int LocationState::ComputePosition(float Coordinate)
{


    int ComputeCoordinate = ( int )Coordinate%230; //230為半個街區長+大街道寬+半個街區長

    if ( ComputeCoordinate < 0 )
    {
        ComputeCoordinate += 230 ;
    }

    if( ComputeCoordinate >= 100 && ComputeCoordinate <= 130)
    {
        MsStateXY = Big_Street;
    }
    else
    {

    	if(ComputeCoordinate>130) ComputeCoordinate-=30;

        if (  ((ComputeCoordinate )%20 <3 ) ||   ((ComputeCoordinate )%20 >17 ) )
            MsStateXY = Little_Street;

        else
            MsStateXY = House;

    }

    return MsStateXY;

}
*/
inline int LocationState::HouseOrLittleStreet(int num)
{
    if( num >=130)
        return num-130;
    else
        return num;
}
