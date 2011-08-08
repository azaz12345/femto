#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "GenMultiUser.h"
#include "MobileStation.h"
#include "CSG.h"
#include "energy.h"

class Simulator
{
    public:
        Simulator();
        virtual ~Simulator();


        void Start();

        vector < vector<int>* >* FemtoListByFP_v;


    protected:



    private:

    GenMultiUserParam P1;
    GenMultiUser* MultiUser;

    CSG* _CSG ;
    vector<MSNODE*>* VQ;


    LinkList USERLIST;


};

#endif // SIMULATOR_H
