#ifndef GENMULTIUSER_H
#define GENMULTIUSER_H
#include "stdhead.h"
#include "PathGenerator.h"
#include <vector>
//#define DEBUG 1
#define FEMTOMODE 1


struct GenMultiUserParam{

	GenMultiUserParam(int SimuTotalTime,int NumUser,double TickTime,double ScanPeriod,int P):
	simulationtotaltime(SimuTotalTime),
	numuser(NumUser),
	ticktime(TickTime),
	scanperiod(ScanPeriod),
	permutation(P)
	{}

	int simulationtotaltime;
	int numuser;
	double ticktime;
	double scanperiod;
	int permutation;

};



using namespace std;
class GenMultiUser: private PathGenerator
{
public:

    /*
    建立Num個使用者跟他們各自的路線(根據時間來產生路線),
    Time就是要模擬的時間
    FemtoMode為1就是有考慮Femtocell
    TrafficLoad
    Permutation->PUSC or FUSC 子載波的分配方式分成兩種
    */
    GenMultiUser(LinkList* Userlist_, int Num, int Time, int Permutation);
    GenMultiUser(LinkList* Userlist_, struct GenMultiUserParam& param);
    virtual ~GenMultiUser();

    /*更新所有MS的位置*/
    void UpdateAllPosition(MSNODE*& first, int i);
    /*更新某個MS的位置,也就是說只更新msdata中的position*/
    void UpdateOnePosition(XYAXIS xy, MSINFO& currMsdata);

    vector<WayPoint>* getUserPathlist(){ return UserPathlist; };

protected:
    /*新增一個使用者*/
    double addUser(double t_new,
                   double traffic_load,
                   int Femto_mode);
    /*指定數量建立多個使用者*/
    double CreateMultiUser(int Num);

    /*建立每個使用者的移動路徑*/
    void CreatePathForAll(int Num);

    /*建立每個使用者的掃描起始時間*/
    void DecideScanStartTimForAll(double scanperiod, double ticktime);


    int GetNumUser()
    {
        return NumUser;
    }
    int GetSimulationTime()
    {
        return SimulationTime;
    }
    int GetFemtoMode()
    {
        return FemtoMode;
    }


private:

    int NumUser;
    int SimulationTotalTime;//整體要模擬的時間
    int FemtoMode;

    double SimulationTime;//目前的模擬時間

    LinkList *UserList;
    /*存放所有使用者的移動路徑*/
    vector<WayPoint>* UserPathlist;

};

#endif // GENMULTIUSER_H
