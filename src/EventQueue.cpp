#include "EventQueue.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>



EventQueue::EventQueue()
{
    //ctor
}

EventQueue::~EventQueue()
{
    //dtor
}

void
EventQueue::Process()
{
    Event* currEv;
    int EventType;
    while((currEv=popEvent())!=NULL){
        currSimuTime = currEv->time_;
        //printf("###########33\n");
        currEv->EvProc_->ProcessEvent(currEv->EventType_,currEv->node_);
        //printf("currEv->time_:%f\t",currEv->time_);
        //printf("currEv->EventType_:%d\n",currEv->EventType_);
        //system("pause");

    }




/*
    switch (currEv.type)
    {
        case INSER_NODE:
            InsertUser();
        break;
        case RMV_NODE:
            RemoveUser((MSNODE*)currEv.node);
        break;
        case CH_ACTIVE:

        break;
        case CH_IDLE:

        break;

    }
*/
}

void
EventQueue::pushEvent(Event Ev_)
{

    EvQueue.insert(Ev_);
}


Event*
EventQueue::popEvent()
{
    Event *tmp=new Event;
    multiset < Event,sortByTime>::iterator it;

    it =  EvQueue.begin();
    *tmp = (*it);


    if(it==EvQueue.end()) return NULL;
    EvQueue.erase(it);
    return tmp;
}

/*
void
EventQueue::InsertUser()
{
    UserList->insertLast( 0, 0, 1);
}
void0.
EventQueue::RemoveUser(MSNODE* pCurr)
{
    UserList->rmv_msnode(pCurr);
}
void
EventQueue::ChIdle()
{

}
void
EventQueue::ChActive()
{

}
*/


