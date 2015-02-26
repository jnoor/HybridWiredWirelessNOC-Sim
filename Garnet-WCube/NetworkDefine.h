#ifndef NET_DEFINE_H
#define NET_DEFINE_H

//Mishali: All Network related definitions should be placed here
//#define MESHDIM 10
//#define MESHDIM 4
//#define MESHNODES (MESHDIM * MESHDIM)
#define NUMBER_OF_VIRTUAL_NETWORKS 1
#define VCPERCLASS 16
#define BUFFERSIZE 8
#define ROUTING 0 //XYYX

#include "FlitLength.h"
#define MAXLINELENGTH 200
#define RECYCLE_LATENCY 1

/*
enum NodeType {
  CORE,
  L2BANK,
  MEMORY,
  EMPTY
};
*/
//Old declaration
//enum outPortMap {THERE=0,NORTH=1,SOUTH=2,EAST=3,WEST=4,RF=5};

enum outPortMap {NORTH=0,SOUTH=1,EAST=2,WEST=3,RF=4,THERE=5, RF0=6, RF1=7, RF2=8, RF3=9, NW=10, NE=11, SW=12, SE=13};


extern unsigned long long nocTime;
class EventQueue;
extern EventQueue* g_eventQueue_ptr;
extern bool deadlock;
extern unsigned long long deadlockTime;
extern bool canDetectDeadlock;
extern unsigned long long totalDeadlocks;
#define DEADLOCK_VNET 1
#define PORTS_PER_ROUTER 20
#define DIR_PER_ROUTER 20
//#define NOC_DEBUG 1
#define BUFF_THRESHOLD 50
//#define BUFF_THRESHOLD 20
//#define VCALLOC_THRESHOLD 200
//#define SELF_THRESHOLD 200
extern unsigned int vcallocThreshold;
extern unsigned int selfThreshold;
extern unsigned int MESHDIM;
extern unsigned int MESHNODES;
extern unsigned int MESHROUTERS;
extern unsigned int WCUBE_ON;
extern unsigned int NUM_RF_ROUTERS;
extern unsigned int ADAPTIVE_ROUTING;

#endif



