#include <iostream>
#include "NoCInterface.h"

using std::cout;
using std::cerr;
using std::endl;

unsigned int MESHDIM;
unsigned int MESHNODES;
unsigned int MESHROUTERS;
unsigned int WCUBE_ON;
unsigned int NUM_RF_ROUTERS;
unsigned int ADAPTIVE_ROUTING;


//The original runscript:
//./garnetSimulator 1 64 router_assignment10_cornermem.txt 64 cornermem8_256.cut cornermem8.route empty10.sub 0 5 temp.trace 20000 0 >& larger.out


int main( int argc , char* argv[] ) {
  int numL2Banks = 32; //number of L2 Banks
  int nSimulators = atoi(argv[1]); //number of simulators
  int nCores = atoi(argv[2]); //number of cores
  char* routerAsmtFile = argv[3]; //Router Assignment File
  int paramlinkBw = atoi(argv[4]); //bandwidth for router assignment file in bps
  char* shortcutFile = argv[5]; //the wireless link connection file
  char* routingFile = argv[6]; //static routing table: %d %d : output_port number_of_hops_away
  char* disabledFile = argv[7];
  int pctShortcutsARG = atoi(argv[8]); // i dont think this is used
  int linkLatencyARG = atoi(argv[9]); // not used
  unsigned int pctShortcuts = static_cast<unsigned int>(pctShortcutsARG);
  unsigned int linklatency = static_cast<unsigned int>(linkLatencyARG);
  unsigned int linkBw = static_cast<unsigned int>(paramlinkBw);
  char* traceFile = argv[10]; //trace file for insertion of messages
  bool scalePctShortcut = false;
  unsigned long long cycles = atoi(argv[11]);
  unsigned int accelerate = atoi(argv[12]);
  unsigned int vc_t = atoi(argv[13]); //VC Allocation Threshold (vcallocThreshold)
  unsigned int self_t = atoi(argv[14]); //self threshold for deadlocks (selfThreshold)

  //begin WCUBE Parameters
  MESHDIM = atoi(argv[15]);
  MESHNODES = atoi(argv[16]);
  MESHROUTERS = atoi(argv[17]);
  WCUBE_ON = atoi(argv[18]);
  NUM_RF_ROUTERS = atoi(argv[19]);
  ADAPTIVE_ROUTING = atoi(argv[20]);

  //create the actual simulator
  //parameters: numberSimulators, NumberOfCores, NumberL2Banks, RouterAssignmentFile, LinkBandWidth, ShortcutFile, RoutingFile, DisabledFile,
  //			PCTShortcuts, LinkLatency, ScalePCTShortcut, Cycles, Accelerate, TraceFile, VC_T, SELF_T
  //	 WCUBE: WCUBE MESH DIMENSIONS, NODES, ROUTERS, WCUBE_ON, NUM_RF_ROUTERS, ADAPTIVE_ROUTING
  NoCInterface  *NoCHandler = new NoCInterface(nSimulators, nCores, numL2Banks, routerAsmtFile, linkBw, shortcutFile,routingFile, disabledFile, pctShortcuts,linklatency, scalePctShortcut, cycles, accelerate,traceFile,vc_t,self_t);

  //run the simulation
  NoCHandler->simulate();
  delete NoCHandler;
  return 0;
}
