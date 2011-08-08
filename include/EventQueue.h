#ifndef EVENTQUERE_H
#define EVENTQUERE_H

#include <set>

using namespace std;

class EventProcess{

public:

    virtual void ProcessEvent(int,void*)=0;


};


enum{
    INSER_NODE,
    RMV_NODE,
    CH_IDLE,
    CH_ACTIVE
};




typedef struct{

    double time_;
    class EventProcess* EvProc_;
    int EventType_;
    void* node_;

} Event;


struct sortByTime{

	bool operator () (const Event& b1, const Event& b2) const{

		return b1.time_ < b2.time_;
	}
};



class EventQueue
{
    public:
        EventQueue();

        virtual ~EventQueue();

        void Process();

        void pushEvent(Event Ev_);

        Event* popEvent();

        int size(){ return EvQueue.size();};

        multiset < Event ,sortByTime> EvQueue ;

        inline double getClock(){return currSimuTime;}

    protected:
/*
        void InsertUser();

        void RemoveUser(MSNODE* pCurr);

        void ChIdle();

        void ChActive();

*/
    private:
        double currSimuTime;




};

#endif // EVENTQUERE_H
