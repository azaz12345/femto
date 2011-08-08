#include "GenMultiUser.h"
#include "external_parameters.h"
#include "stdhead.h"
#include "MacroBS.h"
#include <vector>

#define TICKTIME 0.5

GenMultiUser::GenMultiUser(LinkList* Userlist_, int Num, int Time, int Permutation):
    PathGenerator(Time,TICKTIME, &BSOxy_MAI),
    FemtoMode(FEMTOMODE),
    NumUser(Num),
    SimulationTotalTime(Time),
    SimulationTime(0.0) //0.0second
{
    //�إߦsUSER���e��
    UserList = Userlist_;
    /*********************�ݭק�************************/
    UserList->DL_sub_channel_sum = new int**[NUM_CELL];
    for(int i = 0; i < NUM_CELL; i++)
        UserList->DL_sub_channel_sum[i] = new int*[NUM_SECTOR];
    for(int i = 0; i < NUM_CELL; i++)
    {
        for(int j = 0; j < NUM_SECTOR; j++)
            UserList->DL_sub_channel_sum[i][j] = new int[Sub_channel_num];
    }

    UserList->DL_FS_sub_channel = new int*[NUM_SECTOR];
    for(int i = 0; i < NUM_SECTOR; i++)
        UserList->DL_FS_sub_channel[i] = new int[FS_Sub_channel_num];
    /*******************************************************/
    UserList->DL_sub_channel_initial();
    UserList->sys_setup();

    //�Q��Userlist�Ӳ���Femto����m(LinkList�����󦳲���Femtocell��m���\��]�N�Ofs_position)
    //�إߪ���m�|�s��fsdata �o�ӬO�@��extern�����ܼ�

    UserList->fs_position(Permutation);//Permutation->PUSC or FUSC
    //�ھڿ�J���ƶq�Ӳ���MS
    CreateMultiUser(NumUser);
    /*************************�إߦU�ӨϥΪ̪����ʸ��|�æs�_��*******************************/
    CreatePathForAll(NumUser);

    /******************************�إߤj��a�x��VectorFP****************************************/
	MacroBS MBS;
	MBS.buildAllFP();
    //ctor

}

GenMultiUser::GenMultiUser(LinkList* Userlist_, struct GenMultiUserParam& param):
    PathGenerator(param.simulationtotaltime, param.ticktime,&BSOxy_MAI),
    FemtoMode(FEMTOMODE),
    NumUser(param.numuser),
    SimulationTotalTime(param.simulationtotaltime),
    SimulationTime(0.0) //0.0second
{

	//�إߦsUSER���e��
    UserList = Userlist_;


    /*********************�ݭק�************************/
    UserList->DL_sub_channel_sum = new int**[NUM_CELL];
    for(int i = 0; i < NUM_CELL; i++)
        UserList->DL_sub_channel_sum[i] = new int*[NUM_SECTOR];
    for(int i = 0; i < NUM_CELL; i++)
    {
        for(int j = 0; j < NUM_SECTOR; j++)
            UserList->DL_sub_channel_sum[i][j] = new int[Sub_channel_num];
    }

    UserList->DL_FS_sub_channel = new int*[NUM_SECTOR];
    for(int i = 0; i < NUM_SECTOR; i++)
        UserList->DL_FS_sub_channel[i] = new int[FS_Sub_channel_num];

	/*******************************************************/
	printf("777777777777777\n");
	UserList->DL_sub_channel_initial();
	printf("8888888888888888\n");
    UserList->sys_setup();
	printf("99999999999999\n");
    //�Q��Userlist�Ӳ���Femto����m(LinkList�����󦳲���Femtocell��m���\��]�N�Ofs_position)
    //�إߪ���m�|�s��fsdata �o�ӬO�@��extern�����ܼ�

    UserList->fs_position(param.permutation);//Permutation->PUSC or FUSC

    //�ھڿ�J���ƶq�Ӳ���MS
    CreateMultiUser(NumUser);
    /*************************�إߦU�ӨϥΪ̪����ʸ��|�æs�_��*******************************/
    CreatePathForAll(NumUser);

	MacroBS MBS;
	MBS.buildAllFP();


	DecideScanStartTimForAll(param.scanperiod,param.ticktime);

}

GenMultiUser::~GenMultiUser()
{

	delete [] UserPathlist;

	for(int i = 0; i < NUM_SECTOR; i++)
		delete [] UserList->DL_FS_sub_channel[i];

	delete [] UserList->DL_FS_sub_channel;

	for(int i = 0; i < NUM_CELL; i++)
    {
        for(int j = 0; j < NUM_SECTOR; j++)
		delete [] UserList->DL_sub_channel_sum[i][j];
    }

    for(int i = 0; i < NUM_CELL; i++)
        delete [] UserList->DL_sub_channel_sum[i];

	delete [] UserList->DL_sub_channel_sum;



    //dtor
}

/*��s�C�@�ӨϥΪ̪���m*/
void
GenMultiUser::UpdateAllPosition(MSNODE*& first, int i = 0){
    //vector<XYAXIS>::iterator msdata_ = UserPathlist[i].begin() + (SimulationTime/TICKTIME);
/**********���j***********
    if(i==0){
        SimulationTime += TICKTIME;
    }

    //XYAXIS xy_ = UserPathlist[i][SimulationTime/TICKTIME];
    //XYAXIS xy_;
#if 0
    double x=first->msdata.position.x;
    double y=first->msdata.position.y;

    printf("��%d�ӨϥΪ̪�msdata��position�|����s��:(%f,%f)\n",i,x,y);
#endif
    UpdateOnePosition( UserPathlist[i][SimulationTime/TICKTIME], first->msdata);
#if 0
    printf("��%d�ӨϥΪ̪�msdata��position��s�ᬰ  :(%f,%f)\n",i,first->msdata.position.x,first->msdata.position.y);
    printf("�ۮt                                    :(%f,%f)\n\n",x - UserPathlist[i][SimulationTime/TICKTIME].x, y - UserPathlist[i][SimulationTime/TICKTIME].y );
    system("PAUSE");
#endif
    if(first->pNext != NULL){
        i = i + 1;
        UpdateAllPosition(first->pNext, i);
    }
*/
/**********�D���j*************/


    MSNODE* currMSNODE = first;
//start for openMP
    MSNODE** arrayMSNODE = new MSNODE*[NumUser];

    for(int i=0;currMSNODE != NULL;i++){
        arrayMSNODE[i] = currMSNODE;
        currMSNODE = currMSNODE->pNext;
    }

    //#pragma omp parallel for
    for(int j=0; j<NumUser;j++){

#if 0
    double x=arrayMSNODE[j]->msdata.position.x;
    double y=arrayMSNODE[j]->msdata.position.y;

    printf("��%d�ӨϥΪ̪�msdata��position�|����s��:(%f,%f)\n",j,x,y);
#endif

        UpdateOnePosition( UserPathlist[j][SimulationTime/TICKTIME].xy, arrayMSNODE[j]->msdata);

#if 0
    printf("��%d�ӨϥΪ̪�msdata��position��s�ᬰ  :(%f,%f)\n",j,arrayMSNODE[j]->msdata.position.x,arrayMSNODE[j]->msdata.position.y);
    printf("�ۮt                                    :(%f,%f)\n\n",x - arrayMSNODE[j]->msdata.position.x, y - arrayMSNODE[j]->msdata.position.y );
    system("PAUSE");
#endif



    }
	SimulationTime += TICKTIME;

    delete [] arrayMSNODE;
//end for openMP
/*
    for(int j=0; currMSNODE != NULL;j++){

#if 1
    double x=currMSNODE->msdata.position.x;
    double y=currMSNODE->msdata.position.y;

    printf("��%d�ӨϥΪ̪�msdata��position�|����s��:(%f,%f)\n",i,x,y);
#endif

        UpdateOnePosition( UserPathlist[j][SimulationTime/TICKTIME], currMSNODE->msdata);

#if 1
    printf("��%d�ӨϥΪ̪�msdata��position��s�ᬰ  :(%f,%f)\n",i,currMSNODE->msdata.position.x,currMSNODE->msdata.position.y);
    printf("�ۮt                                    :(%f,%f)\n\n",x - currMSNODE->msdata.position.x, y - currMSNODE->msdata.position.y );
    system("PAUSE");
#endif

        currMSNODE = currMSNODE->pNext;
    }
*/
}

/*��s�Y�ӨϥΪ̪���m�ھ�PathGenerator*/
void
GenMultiUser::UpdateOnePosition(XYAXIS xy, MSINFO& currMsdata)
{
	currMsdata.preposition = currMsdata.position;
    currMsdata.position = xy;


}


double
GenMultiUser::addUser(double t_new, double traffic_load, int Femto_mode)
{
    return UserList->insertLast( t_new, traffic_load, Femto_mode);
}

/*�ھڿ�J���ƥ�,�ӥ[�J���ƶq���ϥΪ�*/
double
GenMultiUser::CreateMultiUser(int Num)
{

    for(int i=0; i<Num; i++)
    {
#if DEBUG
        printf("#DEBUG#: in CreateMultiUser %d\n",i);
#endif
        addUser(0.0, 1, FemtoMode);//��1��2�ӰѼƧ����S�N�q
    }

	//initial MS ID
	MSNODE* currMS = UserList->pFirst;

	for( int j=0; currMS != NULL; j++){
		currMS->msdata.ID = j;
		currMS = currMS->pNext;
	}


}

/*���C�ӨϥΪ̫إߦۤv�����ʸ��|
  �Q��PathGenerator�����\��Ӳ��͸��|�æs��UserPathlist��
*/
void
GenMultiUser::CreatePathForAll(int Num)
{

    UserPathlist = new vector<WayPoint>[Num];
    //UserList->pFirst->msdata.position;
    MSNODE* currMSNODE = UserList->pFirst;
    for(int i=0; i<Num; i++)
    {
        //�Ѽ�1�O�ΨӦs���|��
        //�Ѽ�2�O���|���_�l�I(�]�N�O��l�ƨϥΪ̮ɤ@�}�l���w����m)
        //�Ѽ�3�O�t��(���t�ץ��ӷ|�[�J�ھڨϥΪ̪������Ӱ�����)
        CreateNewPath(&UserPathlist[i],currMSNODE->msdata.position,5);

        printf("�ϥΪ�%d���|�w�إߧ���\n",i);
#if 0
        for(vector<WayPoint>::iterator it = UserPathlist[i].begin(); it != UserPathlist[i].end(); it++)
        {

            printf("�ϥΪ�%d�y��:(%f,%f)\n", i, it->xy.x, it->xy.y);

        }

#endif

        currMSNODE = currMSNODE->pNext;
    }


}



void
GenMultiUser::DecideScanStartTimForAll(double scanperiod, double ticktime){

	MSNODE* currMSNODE = UserList->pFirst;

	while(currMSNODE!=0){


		currMSNODE->msdata.ScanPeriod = scanperiod;
		currMSNODE->msdata.ScanStartTickTime = ((int)(Rand()*(scanperiod/ticktime)))%((int)(scanperiod/ticktime));

		/*�Ȯɥ[�b�o�t��*/
		currMSNODE->msdata.speed =5;

//		printf("ScanStartTime:%f\n",currMSNODE->msdata.ScanStartTickTime);
//		system("PAUSE");

		currMSNODE = currMSNODE->pNext;
	}




}
