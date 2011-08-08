#include "PathGenerator.h"
#include "external_parameters.h"
#include <math.h>
#include "stdhead.h"
//#include <time.h>

#if DEBUG
#include <unistd.h>
#endif
/*
Position is init position of MS;
Time is simulation time;
Path include many waypoint;

*/

PathGenerator::PathGenerator( int Time, double ticktime ,XYAXIS (*BSOxy)[NUM_CELL] ):
	SimulationTime(Time),
	TickTime(ticktime)
{

    BSOxy_ = BSOxy ;

}

PathGenerator::~PathGenerator()
{
    //dtor
}


void
PathGenerator::CreateNewPath( vector<WayPoint> *path, XYAXIS FirstPosition, int Speed )
{

    XYAXIS currPosition;
    XYAXIS _TurnCorner ;
    XYAXIS _TickPoint ;
    WayPoint currPoint ;
    double  _direction ;
    double  HomeCorner = 0.0;

//  path->push_back(FirstPosition);
    /*根據時間來產生相應數量的waypoint  單位時間間隔為0.5*/

    /*隨機產生0~2*PI => 0~360度*/

    currPosition = FirstPosition;
    _direction = RandomDirect() ;
    currPoint.xy = currPosition ;
    currPoint.speed= Speed ;
    Mobility_Boundary(&currPosition , &_TurnCorner);
    path->push_back(currPoint);


    for(int i = 1; i<=(int)(SimulationTime/TickTime);)
    {
        if ( DecidePosition( currPosition ) == 0)       //MS in crossroad
            _direction = TwoRandomDirect(_direction) ;

        else if ( DecidePosition( currPosition ) == 1 ) //MS in Level_Street
            _direction = TwoRandomDirect(PI/2) ;

        else if ( DecidePosition( currPosition ) == 2 ) //MS in Vertical_Street
            _direction = TwoRandomDirect(PI) ;

        else if ( HomeCorner != 0.0 )
        {
            currPosition.x = currPosition.x + HomeCorner;
            currPosition.y = currPosition.y + Home_Street_Distance ( currPosition , 2 );
            Mobility_Boundary(&currPosition , &_TurnCorner);
            HomeCorner = 0.0;
            currPoint.xy = currPosition ;
            path->push_back(currPoint);

            i++;
        }

        if( DecidePosition( currPosition ) == 3 ) // MS in Home
        {
            int S_H_T = 0; //Stay home time
            while ( S_H_T < S_H_P*SimulationTime/TickTime && i<SimulationTime/TickTime )
            {
                Mobility_Boundary(&currPosition , &_TurnCorner);
                currPoint.xy = currPosition ;
                path->push_back(currPoint);

                S_H_T ++ ;
                i++;
            }
                currPosition.y = currPosition.y - Home_Street_Distance( currPosition ,1 );
                Mobility_Boundary(&currPosition , &_TurnCorner);
                _direction = TwoRandomDirect(PI/2) ;
                currPoint.xy = currPosition ;
                path->push_back(currPoint);

                i++;
        }



        _TurnCorner = TurnLine ( currPosition , Speed , _direction , HomeCorner) ;
		Mobility_Boundary(&currPosition , &_TurnCorner);

        while( ( currPosition.x != _TurnCorner.x ) || ( currPosition.y != _TurnCorner.y ) )
        {
            currPosition = TickPoint(Speed , currPosition ,_TurnCorner , _direction);
            Mobility_Boundary(&currPosition , &_TurnCorner);
            currPoint.xy = currPosition ;
            path->push_back(currPoint);

            i++ ;

            if (i>SimulationTime/TickTime )
                break ;
        }

		printf("#TEST1#\n");
#if DEBUG
        printf("產生的位置:(%f,%f)\n",currPosition.x,currPosition.y);
        printf("判斷位置為:%d\n",DecidePosition(currPosition));
        printf("產生轉彎的位置:(%f,%f)\n",_TurnCorner.x,_TurnCorner.y);
        system("PAUSE");
#endif //DEBUG
    };
};

/*時間單位先假設為0.5*/
XYAXIS
PathGenerator::NextPoint( XYAXIS DatumPosition,
                          int Speed,
                          double direction)
{

    XYAXIS NextPosition;//欲產生位置


#if DEBUG
    printf("direction:%f,cos:%f,sin:%f",direction,cos(direction),sin(direction));
    printf("\n\n");
    system("PAUSE");

#endif //DEBUG

    double MovingDistance = ( double ) Speed*1000/3600*0.5;//計算移動距離(m);除1000=>公尺轉公里

    NextPosition.x = DatumPosition.x + ( MovingDistance*cos(direction) );
    NextPosition.y = DatumPosition.y + ( MovingDistance*sin(direction) );


    return NextPosition;
};

XYAXIS PathGenerator::TurnLine( XYAXIS MsPosition , int Speed , double MsDirection , double &HomeCorner)
{
//    int Corner [ 13 ] = { 0 , 30 , 47 , 64 , 81 , 98 , 115 , 132 , 149 , 166 , 183 , 200 ,230 } ;

    int Corner [ 11 ] = { 0 , 20 , 40 , 60 , 80 , 115 , 150, 170 , 190 , 210 ,230 } ;
    int Corner_distance ;
    int BigCorner = 0 , LittleCorner = 0 ;
    int CheckX , CheckY ;
    int IntDirection ;

    XYAXIS TurnXYAXIS ;

    //換成公尺，並把原點改成大路的街道
        CheckX = MsPosition.x * 1000  ;
        CheckY = MsPosition.y * 1000  ;

        CheckX =  CheckX % 230;
        CheckY =  CheckY % 230;

        if ( CheckX < 0 )
            CheckX += 230 ;

        if ( CheckY < 0 )
            CheckY += 230 ;

        IntDirection = ChangeToInt( MsDirection );


    if ( IntDirection ==0 || IntDirection == 3 || IntDirection == 6 )
    {
       Corner_distance = _CornerDistance( CheckX , IntDirection , Corner  , BigCorner , LittleCorner , HomeCorner) ;//確定離最近的街道的距離
    }

    else if (IntDirection ==1 || IntDirection == 4 )
    {
        Corner_distance = _CornerDistance( CheckY , IntDirection , Corner  , BigCorner , LittleCorner , HomeCorner) ;
    }

        TurnXYAXIS = HowXYAXIS( MsPosition , IntDirection , BigCorner , LittleCorner , Corner_distance ,HomeCorner ) ; //轉彎那個點的XY值
        //printf("%f%f\n",TurnXYAXIS.x ,TurnXYAXIS.y );
        //system("pause");

        return TurnXYAXIS ;

};

int PathGenerator::ChangeToInt(double NumDouble)
{
    int MathInt = (int) floor( NumDouble );
    return MathInt ;
}

int PathGenerator::ChangeToInt_Million(double _NumDouble )
{
    int _NumDouble_ =  floor (_NumDouble * 100000) ;
    return _NumDouble_ ;
};
//機率
bool PathGenerator::Probability( double Prob )
{
    double y = Rand();

    if ( y <= Prob)
        return true ;
    else
        return false ;

};
//確定離最近的街道的距離

 int PathGenerator::_CornerDistance ( int CheckCoordinate , int _MsDirection ,int *Cor , int &BigCorner , int &LittleCorner,double &HomeCorner)
 {
//   int BigCornerWidth , LittleCornerWidth ;
     int Cornerdistance ;

        for ( int CornerNum =0 ; CornerNum < 11 ; CornerNum++ )
        {
            Cornerdistance = 0 ;

            Cornerdistance = Cor[ CornerNum ] - CheckCoordinate ;

            if ( _MsDirection == 0 || _MsDirection == 1 || _MsDirection == 6 )
            {

                if(Cornerdistance <= 35 && Cornerdistance >= 0)
                {
//                  Corner_distance = Cornerdistance ;

                    CornerCheck ( CornerNum ,_MsDirection, BigCorner , LittleCorner ,HomeCorner) ;

                    return Cornerdistance ;
                }
            }

           else if (  _MsDirection  == 3 ||  _MsDirection  == 4 )
                {
                  if(Cornerdistance <= 0 && Cornerdistance >= -35)
                  {
                    CornerCheck ( CornerNum , _MsDirection,  BigCorner , LittleCorner,HomeCorner) ;

                    return Cornerdistance ;
                  }
                }
        }
        return 0;
 };

//決定哪一個路口轉彎
void PathGenerator::CornerCheck ( int MsCorn , int _MsDirection_,int &BigCorner , int &LittleCorner,double &HomeCorner)
{
    int OriginCorn = MsCorn % 10 ;
    switch ( OriginCorn )
    {
      case 5 :


				if ( Probability( 2 * BigTurn ) == false )
            {
                BigCorner = BigCorner + 1 ;
                if( _MsDirection_== 3 || _MsDirection_ == 4)
                {
					if ( MsCorn < 0 )
                        MsCorn += 10;

                    CornerCheck( MsCorn - 1 ,_MsDirection_ , BigCorner ,LittleCorner, HomeCorner) ;



                }
                else
                     CornerCheck( MsCorn + 1 ,_MsDirection_ , BigCorner ,LittleCorner, HomeCorner ) ;

				break;
            }

            if ( Probability( InHome ) == true  )
            {
                if( _MsDirection_ == 3 )
                {
                    HomeCorner = 0.024;
                    break ;
                }
                else if ( _MsDirection_ == 0 || _MsDirection_ == 6 )
                {
                    HomeCorner = -0.024;
                    break ;
                }
            }

                break ;

      default :


            if ( Probability( 2 * LittleTurn ) == false )
            {
                LittleCorner = LittleCorner + 1 ;
                if( _MsDirection_ == 3 || _MsDirection_ == 4)
                {
                    if ( MsCorn < 0 )
                        MsCorn += 10;

                    CornerCheck( MsCorn - 1 ,_MsDirection_ , BigCorner ,LittleCorner,HomeCorner) ;


                }
                else
                    CornerCheck( MsCorn + 1 , _MsDirection_ , BigCorner ,LittleCorner,HomeCorner)  ;

				break ;
            }


            if ( Probability( InHome ) == true )
            {
                if( _MsDirection_ == 3 )
                {
                    HomeCorner = 0.01;
                    break ;
                }
                else if ( _MsDirection_ == 0 || _MsDirection_ == 6 )
                {
                    HomeCorner = -0.01;
                    break ;
                }
            }

			break ;

    }
};

//轉彎那個點的XY值
XYAXIS PathGenerator::HowXYAXIS( XYAXIS MsPositions , int Direct , int BigCorner , int LittleCorner ,int Corner_distance , double HomeCorner)
{

    XYAXIS TurnPosition; //決定轉彎的XY座標

        if (  Direct ==0 )
        {
            TurnPosition.x = MsPositions.x + ( ( double ) Corner_distance *0.001 ) + ( BigCorner * 0.03 ) + ( LittleCorner * 0.006 ) + HomeCorner ;
            TurnPosition.y = MsPositions.y ;
        }

        if ( Direct == 1 )
        {
            TurnPosition.x = MsPositions.x ;
            TurnPosition.y = MsPositions.y + ( ( double ) Corner_distance *0.001 ) + ( BigCorner * 0.03 ) + ( LittleCorner * 0.006 )  ;
        }

        if (  Direct == 3 )
        {
            TurnPosition.x = MsPositions.x - ( ( double ) Corner_distance *0.001 ) - ( BigCorner * 0.03 ) - ( LittleCorner * 0.006 ) - HomeCorner ;
            TurnPosition.y = MsPositions.y ;
        }

        if ( Direct == 4)
        {
            TurnPosition.x = MsPositions.x ;
            TurnPosition.y = MsPositions.y - ( ( double ) Corner_distance *0.001) - ( BigCorner * 0.03 ) - ( LittleCorner * 0.006 )   ;
        }




#if DEBUG
        printf ("\n距離%d\n大街道%d\n小街道%d\n角度:%f\n,座標:%f\t%f\n", Corner_distance , BigCorner , LittleCorner , Direct ,TurnPosition.x ,TurnPosition.y );
        system("pause") ;
#endif
        return TurnPosition ;
};

double PathGenerator::Home_Street_Distance ( XYAXIS CheckDistance , int mode )
 {
     int Corner [ 11 ] = { 0 , 20 , 40 , 60 , 80 , 115 , 150, 170 , 190 , 210 ,230 } ;
     int Street_Distance , _CheckDistance,A_CheckDistance;

    _CheckDistance = (ChangeToInt( CheckDistance.y*1000))% 230  ;

    if ( _CheckDistance <= 0)
        _CheckDistance = _CheckDistance + 230 ;

    Street_Distance = 0 ;

        for ( int _CornerNum =0 ; _CornerNum < 11 ; _CornerNum++ )
        {
            if ( CheckDistance.y > 0 )
                Street_Distance = Corner[ _CornerNum ] - _CheckDistance ;
            else
                Street_Distance = - Corner[ _CornerNum ] + _CheckDistance;

            if ( mode ==1 )            //計算離開屋子的距離
            {
               if(Street_Distance < 0 && Street_Distance >= -35)
                {
                    return abs( Street_Distance ) *0.001 ;
                }
            }
            else if ( mode == 2 )      //計算進屋子的距離
            {
                if(Street_Distance >= 0 && Street_Distance <= 35)
                {
                    if( _CornerNum % 10 == 5 )
                    {
                        return 0.02;
                    }

                    else
                        return 0.008;
                }
            }
        }
        return 0.0;
 };


double PathGenerator::RandomDirect( void )
{
            double direction_1 =0.0 ;

            direction_1 = Rand()*(PI*2);

            if( (direction_1<PI/4 && direction_1 >=0) ||
                    (direction_1 <=2*PI &&  direction_1>=7*PI/4) )
            {
                direction_1 = 0;
            }
            else if( direction_1<3*PI/4 && direction_1>=PI/4 )
            {
                direction_1 = PI/2;
            }
            else if( direction_1<5*PI/4 && direction_1>=3*PI/4 )
            {
                direction_1 = PI;
            }
            else if( direction_1<7*PI/4 && direction_1>=5*PI/4 )
            {
                direction_1 = 3*PI/2;
            }
            else{
                printf("漏網之魚\n");
                printf("direction:%f",direction_1);
            }

            return direction_1 ;

};

double PathGenerator::TwoRandomDirect( double direction_2 )
{
    double Two_Random = Rand();

    if ( Two_Random <=0.5 )
        direction_2 = direction_2 + PI/2 ;

    else
        direction_2 = direction_2 - PI/2 ;

    if (  direction_2 >= 2*PI  )
        direction_2 = direction_2 - 2* PI ;

    else if( direction_2 < 0.0 )
        direction_2 = direction_2 + 2* PI ;

    return direction_2 ;
};

XYAXIS PathGenerator::TickPoint (int _Speed , XYAXIS currPosition, XYAXIS _TurnCorner ,double _direction_  )
{
    int Ms_Distance_M , Two_Point_Distance_X_M , Two_Point_Distance_Y_M , IntDirection;
    double  Two_Point_Distance_X , Two_Point_Distance_Y ;
    double Ms_Distance ;
    XYAXIS _TickPoint ;

    IntDirection = ChangeToInt( _direction_ );


    Two_Point_Distance_X = fabs ( _TurnCorner.x  -  currPosition.x )  ;
    Two_Point_Distance_Y = fabs ( _TurnCorner.y  -  currPosition.y )  ;

    Ms_Distance = ( double )_Speed * 0.00028 * TickTime;  // 0.00028 (1/3600) 公里

    Two_Point_Distance_X_M = ChangeToInt_Million ( Two_Point_Distance_X  );
    Two_Point_Distance_Y_M = ChangeToInt_Million ( Two_Point_Distance_Y  );
    Ms_Distance_M = ChangeToInt_Million ( Ms_Distance );


    if( (IntDirection == 0 ) || (IntDirection == 1) ||(IntDirection == 6) )
    {
        if ( Two_Point_Distance_X_M > Ms_Distance_M )
        {
            _TickPoint.x = currPosition.x + Ms_Distance ;
            _TickPoint.y = currPosition.y ;
        }
        else if ( Two_Point_Distance_Y_M > Ms_Distance_M )
        {
            _TickPoint.x = currPosition.x  ;
            _TickPoint.y = currPosition.y + Ms_Distance ;
        }
        else
        {
            _TickPoint.x = _TurnCorner.x  ;
            _TickPoint.y = _TurnCorner.y  ;
        }
    }
    else if ( (IntDirection == 3 ) || (IntDirection == 4) )
    {
        if ( Two_Point_Distance_X_M > Ms_Distance_M )
        {
            _TickPoint.x = currPosition.x - Ms_Distance ;
            _TickPoint.y = currPosition.y ;
        }
        else if ( Two_Point_Distance_Y_M > Ms_Distance_M )
        {
            _TickPoint.x = currPosition.x  ;
            _TickPoint.y = currPosition.y - Ms_Distance ;
        }
        else
        {
            _TickPoint.x = _TurnCorner.x  ;
            _TickPoint.y = _TurnCorner.y  ;
        }
    }


    return _TickPoint ;

};

double PathGenerator::Two_Point_Distance ( XYAXIS _Previous_Point , XYAXIS TurnPoint_ )
{
    double Two_Point_Distance_X , Two_Point_Distance_Y ;

    Two_Point_Distance_X = TurnPoint_.x - _Previous_Point.x ;
    Two_Point_Distance_Y = TurnPoint_.y - _Previous_Point.y ;

    return sqrt ( pow(Two_Point_Distance_X , 2)+pow ( Two_Point_Distance_Y , 2) );

};


void PathGenerator::Mobility_Boundary ( XYAXIS *currPosition , XYAXIS *_TurnCorner)
{
    int Int_D, Int_R , Int_CR, PI_M , Boundary_M , Different_M ,Different_C_M, angle_M;
    double Boundary , angle_;
    double c , d, e , f ;
    XYAXIS Mobility_Boundary_XYAXIS;

    POLAXIS  Mobility_Boundary_polar = cart2pol((*BSOxy_)[0], *currPosition);
    POLAXIS  Mobility_TurnCorner_polar = cart2pol((*BSOxy_)[0], *_TurnCorner);

    Int_R =  ChangeToInt_Million( Mobility_Boundary_polar.r) ;
    Int_D =  ChangeToInt_Million( Mobility_Boundary_polar.th ) ;
    Int_CR=  ChangeToInt_Million( Mobility_TurnCorner_polar.r) ;
    PI_M  =  ChangeToInt_Million( PI );


    angle_ = (PI-PI/3-(double)((int)((fabs(Mobility_Boundary_polar.th)/PI)*180)%60)/180*PI);

//    angle_M = ChangeToInt_Million( angle_ );

    Boundary = fabs ( R*sin(PI/3)*csc(angle_) ) ;

    Boundary_M = ChangeToInt_Million( Boundary );

    Different_M = Int_R - Boundary_M ;
    Different_C_M = Int_CR - Boundary_M;

    if ( Different_M >=0 )
    {
        if ( Mobility_Boundary_polar.th > 0.0 )
        {
            Mobility_Boundary_polar.th -= PI ;
            Mobility_TurnCorner_polar.th -= PI;
        }
        else
        {
            Mobility_Boundary_polar.th += PI ;
            Mobility_TurnCorner_polar.th += PI ;
        }

        Mobility_Boundary_polar.r = (( Boundary_M - Different_M ) *0.00001 )  ;
        Mobility_TurnCorner_polar.r = (( Boundary_M - Different_C_M ) *0.00001 ) ;

/*
        *currPosition = pol2cart ( Mobility_Boundary_polar );
        *_TurnCorner = pol2cart ( Mobility_TurnCorner_polar );
*/
        XYAXIS  aa = pol2cart ( Mobility_Boundary_polar );
        XYAXIS  bb = pol2cart ( Mobility_TurnCorner_polar );

        c = ChangeToInt_Million( aa.x )*0.00001;
        d = ChangeToInt_Million( aa.y )*0.00001;
        e = ChangeToInt_Million( bb.x )*0.00001;
        f = ChangeToInt_Million( bb.y )*0.00001;

        currPosition->x = c;
        currPosition->y = d;
        _TurnCorner->x = e;
        _TurnCorner->y = f;

    }
};
double PathGenerator::csc(double z)
{
    if ( z == 0.0)
    {
        return 1.0;
    }
    else
  return 1.0 / sin(z);
};
