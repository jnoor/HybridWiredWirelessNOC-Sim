
#ifndef _NoCInterface_H_
#define _NoCInterface_H_

#include <string>
#include <vector>
#include <stdio.h>

#include "GarnetNetwork_d.h"
#include "RouterAssignment.h"

class GarnetNetwork_d;

using std::vector;

class NoCInterface
{
  unsigned int nSimulators;
  unsigned int numL2Banks;
  unsigned int maxCores;
  unsigned long long maxCycles;
  FILE *traceFile;
  int debug;
  RouterAssignment* routerdir;
  int broadcastAccelerated;
  unsigned int accelerate;
 public:
  //  SimpleNoC * mynoc;
  GarnetNetwork_d* garnet;

  NoCInterface(unsigned int nSims, unsigned int nCores, unsigned int nL2Banks,
               char* routerAsmtFile, unsigned int linkBw, char* shortcutFile,
	       char* routingFile, char* disabledFile, unsigned int pctShortcuts, 
	       unsigned int linklatency, bool scalePctShortcut, unsigned long long cycles, unsigned int acc, char *trace, unsigned int vcalloc_t, unsigned int self_t);
  ~NoCInterface();

  // public wrapper for RouterAssignment
  int getLocalNodeOfRouter(unsigned int routerId, NodeType& type,
                           int& simId)
  {
    return routerdir->getLocalNodeOfRouter(routerId, type, simId);
  }
  void simulate();
  vector<NodeInfo*> getRouterToLocalNodes(int router);
};
#endif//NoCInterface_H
