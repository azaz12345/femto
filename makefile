.SUFFIXES: .o .cpp 

LLOADFLAGS=  -lm -oCPPFLAGS= -I./include -I./

CC= g++ -g -c

PROJc= tools.o class_oper.o channel_allocation.o main.o mbs.o fbs.o pathloss.o ms_deployment.o sinr.o

projc:	$(PROJc)
	g++ $(PROJc) $(LLOADFLAGS) projc

$(PROJc): stdhead.h channel_allocation.h fbs.h external_parameters.h PathGenerator.h

tools.o: tools.cpp
	$(CC) tools.cpp $(CPPFLAGS)

class_oper.o: class_oper.cpp
	$(CC) class_oper.cpp $(CPPFLAGS)

operations.o: channel_allocation.cpp
	$(CC) channel_allocation.cpp $(CPPFLAGS)

main.o: main.cpp
	$(CC) main.cpp $(CPPFLAGS)

mbs.o: mbs.cpp
	$(CC) mbs.cpp $(CPPFLAGS)

fbs.o: fbs.cpp
	$(CC) fbs.cpp $(CPPFLAGS)

pathloss.o: pathloss.cpp
	$(CC) pathloss.cpp $(CPPFLAGS)

ms_deployment.o: ms_deployment.cpp
	$(CC) ms_deployment.cpp $(CPPFLAGS)

resource_allocation.o: sinr.cpp
	$(CC) sinr.cpp  $(CPPFLAGS)

