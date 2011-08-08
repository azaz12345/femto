#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "GenMultiUser.h"
#include "MobileStation.h"


class Simulator
{
    public:
        Simulator();
        virtual ~Simulator();


        void Start();
    protected:
    private:
	LinkList USERLIST;
    GenMultiUserParam P1;
    GenMultiUser MultiUser;




};

#endif // SIMULATOR_H
