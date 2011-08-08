#ifndef PATHGENERATOR_H
#define PATHGENERATOR_H
#include "stdhead.h"
#include <vector>
#include "LocationState.h"
#include <omp.h>

#define DEBUG 0
//轉彎、直走、進房屋、出房屋的機率
#define BigTurn 0.25
#define BigStraight 0.5
#define LittleTurn 0.0
#define LittleStraight 1.0

#define InHome 0.1

#define S_H_P 0.34 //Stay_Home_Percent

using namespace std;

typedef struct
{
    XYAXIS xy;
    double speed;
} WayPoint;

class PathGenerator : public LocationState , private COORD
{

public:
    /*模擬時間(sec)*/

    PathGenerator( int Time, double ticktime , XYAXIS (*BSOxy)[NUM_CELL]);
    virtual ~PathGenerator();
    //將根據模擬的時間利用起始點來產生一條路徑
    //speed:節點的移動速度(m/s)
    void CreateNewPath(vector<WayPoint>* path, XYAXIS FirstPosition, int speed);
    bool Probability( double Prob ) ;
    int _CornerDistance ( int CheckCoordinate , int _MsDirection ,int *Cor , int &BigCorner , int &LittleCorner , double &HomeCorner) ;
    void CornerCheck ( int MsCorn , int _MsDirection_,int &BigCorner , int &LittleCorner , double &HomeCorner) ;
    XYAXIS HowXYAXIS( XYAXIS MsPositions , int Direct , int BigCorner , int LittleCorner ,int Corner_distance ,double HomeCorner) ;
    double Home_Street_Distance ( XYAXIS CheckDistance , int mode );
    double RandomDirect( void ) ;
    double TwoRandomDirect( double direction_2 );
    int ChangeToInt(double NumDouble);
    int ChangeToInt_Million(double _NumDouble );
    XYAXIS TickPoint (int _Speed , XYAXIS currPosition, XYAXIS _TurnCorner , double _direction_ );
    double Two_Point_Distance ( XYAXIS _Previous_Point , XYAXIS TurnPoint_ );
    void Mobility_Boundary ( XYAXIS *currPosition , XYAXIS *_TurnCorner);
    double csc(double z);


protected:
    //根據基準點 速度(m/sec) 方向 來產生下一個點
    XYAXIS NextPoint( XYAXIS DatumPosition, int Speed, double direction);
    XYAXIS TurnLine( XYAXIS MsPosition , int Speed , double MsDirection , double &HomeCorner);


private:

    const double SimulationTime;
    const double TickTime;
    XYAXIS (*BSOxy_)[NUM_CELL];
};

#endif // PATHGENERATOR_H

