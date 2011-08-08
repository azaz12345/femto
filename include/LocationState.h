#ifndef LOCATIONSTATE_H
#define LOCATIONSTATE_H

#include "stdhead.h"

class LocationState
{

public :
    int WhichSector(XYAXIS MSPosition, XYAXIS BSPosition);
    int WhichBlock(XYAXIS Position);
    bool IsOnStreet(XYAXIS Position);
    int DecidePosition(XYAXIS Position);
    int ComputePosition(float Coordinate);
    inline int HouseOrLittleStreet(int num);

private :
    int MsState;
    int MsStateXY;
    int num;

};

#endif
