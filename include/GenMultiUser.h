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
    �إ�Num�ӨϥΪ̸�L�̦U�۪����u(�ھڮɶ��Ӳ��͸��u),
    Time�N�O�n�������ɶ�
    FemtoMode��1�N�O���Ҽ{Femtocell
    TrafficLoad
    Permutation->PUSC or FUSC �l���i�����t�覡�������
    */
    GenMultiUser(LinkList* Userlist_, int Num, int Time, int Permutation);
    GenMultiUser(LinkList* Userlist_, struct GenMultiUserParam& param);
    virtual ~GenMultiUser();

    /*��s�Ҧ�MS����m*/
    void UpdateAllPosition(MSNODE*& first, int i);
    /*��s�Y��MS����m,�]�N�O���u��smsdata����position*/
    void UpdateOnePosition(XYAXIS xy, MSINFO& currMsdata);

    vector<WayPoint>* getUserPathlist(){ return UserPathlist; };

protected:
    /*�s�W�@�ӨϥΪ�*/
    double addUser(double t_new,
                   double traffic_load,
                   int Femto_mode);
    /*���w�ƶq�إߦh�ӨϥΪ�*/
    double CreateMultiUser(int Num);

    /*�إߨC�ӨϥΪ̪����ʸ��|*/
    void CreatePathForAll(int Num);

    /*�إߨC�ӨϥΪ̪����y�_�l�ɶ�*/
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
    int SimulationTotalTime;//����n�������ɶ�
    int FemtoMode;

    double SimulationTime;//�ثe�������ɶ�

    LinkList *UserList;
    /*�s��Ҧ��ϥΪ̪����ʸ��|*/
    vector<WayPoint>* UserPathlist;

};

#endif // GENMULTIUSER_H
