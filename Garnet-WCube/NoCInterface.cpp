#include "NoCInterface.h"
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <cmath>
#include "Common.h"

using std::cout;
using std::endl;
using std::cerr;
using std::vector;

//parameters: numberSimulators, NumberOfCores, NumberL2Banks, RouterAssignmentFile, LinkBandWidth, ShortcutFile, RoutingFile, DisabledFile, PCTShortcuts, LinkLatency, ScalePCTShortcut, Cycles, Accelerate, TraceFile, VC_T, SELF_T

NoCInterface::NoCInterface(unsigned int nSims, unsigned int nCores,
		unsigned int nL2Banks, char* routerAsmtFile,
		unsigned int linkBw, char* shortcutFile, 
		char* routingFile, char* disabledFile, unsigned int pctShortcuts, 
			   unsigned int linklatency, bool scalePctShortcut, 
			   unsigned long long cycles, unsigned int acc, char *trace, unsigned int vcalloc_t, unsigned self_t)
  : nSimulators(nSims), maxCores(nCores), numL2Banks(nL2Banks), accelerate(acc)
{
  debug=0;
  // debug=1;
  
  // which router attached to which node in NoC
  // R: Router -- must be specified
  // S: Simulator
  // TYPES
	  // B: L2 Bank
	  // C: Core
	  // M: Memory
	  // W: WCUBE
  // #: Comment Line
  routerdir = new RouterAssignment(nSimulators, routerAsmtFile);

  //  mynoc=new SimpleNoC(MESHDIM, linkBw, shortcutFile, routingFile, disabledFile, this, pctShortcuts, linklatency, scalePctShortcut);

  cout << "Creating Garnet Network..." << endl;
  garnet = new GarnetNetwork_d(MESHNODES, routingFile, shortcutFile, this, pctShortcuts, linkBw, vcalloc_t, self_t);
  cout << "Garnet Network Created!" << endl;
  traceFile = fopen(trace, "r");
  
  maxCycles = cycles;
}



NoCInterface::~NoCInterface()
{
  delete routerdir;
  delete garnet;
}

void NoCInterface::simulate()
{
  unsigned long long currCycle = 0;
  unsigned long long cycle = 0;
  int source = 0;
  char source_char;
  int dest =0;
  char dest_char;
  int d_size = 0;
  unsigned int sourceRouter = 0;
  unsigned int destRouter = 0;
  int broadcast = 0;
  int flag = 0;
  unsigned long long index=0;

  //open the trace file
  if (fscanf(traceFile, "%c %d %c %d %d %d %ld\n", &source_char,&source,&dest_char,&dest,&d_size,&broadcast,&cycle)==EOF)
    flag=1;
  
  for (;currCycle <= maxCycles; currCycle++) {
	  //potential off by one error off-by-one
    while (cycle<currCycle && flag==0){
	
	if (source_char=='C') //source is a core
	  sourceRouter = routerdir->getCoreRouter(0, source);
	else if (source_char=='B') //source is an L2 cache
	  sourceRouter = routerdir->getL2Router(source);
	//else for memory
	else ;
	  //	  sourceRouter = routerdir->getMemoryRouter(routerdir->getL2Router(dest));
	
	
	NodeID node;
	if (dest_char=='C'){
	  destRouter = routerdir->getCoreRouter(0, dest);
	  node = routerdir->getNodeID(CORE,dest);
	}else if (dest_char=='B'){
	  destRouter = routerdir->getL2Router(dest);
	  node = routerdir->getNodeID(L2BANK,dest);
	}else {
	  //destRouter = routerdir->getMemoryRouter(routerdir->getL2Router(source));
	  //node = routerdir->getNodeID(MEMORY,routerdir->getL2Router(source));
 	}
	
	//printf("hello: %d %d (src_router:%d  dst_router:%d dst_node:%d) %d %ld\n", source, dest, sourceRouter, destRouter, node, d_size,cycle);
	
	if (!accelerate) {
	  //	  mynoc->ProcessRequest(sourceRouter, destRouter,d_size, cycle);
	  if (dest_char!='M' && source_char!='M') {
	    garnet->insertIntoToQueue(source,0,sourceRouter,destRouter,broadcast,d_size,index,node);
	    index++;
	  }
	}else {
	  if (!broadcast){
	    //	    mynoc->ProcessRequest(sourceRouter, destRouter,d_size, cycle);
	    if (dest_char!='M' && source_char!='M') {
	    garnet->insertIntoToQueue(source,0,sourceRouter,destRouter,broadcast,d_size,index,node);
	    }
	  }else 
	    broadcastAccelerated++;
	  index++;
	}

	if (fscanf(traceFile, "%c %d %c %d %d %d %ld\n", &source_char,&source,&dest_char,&dest,&d_size,&broadcast,&cycle)== EOF)
	  flag=1;
	
      } 
	
    //    if (flag==1)
    //break;
    garnet->Tick();
  }
  cerr<<"Cycle count:"<<currCycle<<endl;
  cerr<<"Broadcast Accelerated "<<broadcastAccelerated<<endl;
}

vector<NodeInfo*> NoCInterface::getRouterToLocalNodes(int router)
{
  return routerdir->getRouterToLocalNodes(router);
}
