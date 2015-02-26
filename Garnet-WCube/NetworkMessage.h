/*
 * NetworkMessage.h
 * 
 * Description: 
 *
 * Written by me
 *
 */

#ifndef NetworkMessage_H
#define NetworkMessage_H

#include "NetworkDefine.h"
#include "Common.h"
#include "EventQueue.h"

class NetworkMessage {
public:
  // Constructors
  NetworkMessage(int source, int dest, int size, unsigned long long index, NodeID node) 
    :m_source(source),m_dest(dest),m_size(size),m_index(index)
    {
      m_time = g_eventQueue_ptr->getTime();
      m_hops=0.0;
      m_use_rf = 0;
      setNodeID(node);
    }

  // Destructor
  ~NetworkMessage() { }
  
  int getDestination() {return m_dest;}
  int getSize() {return m_size;}
  int getSource() {return m_source;}
  int useRF() {return m_use_rf;}
  void rideRF() {m_use_rf = 1;}
  unsigned long long getIndex() {return m_index;}
  unsigned long long getTime() { return m_time;}
  void setDelayedCycles(const int& cycles) { m_DelayedCycles = cycles; }
  const int& getDelayedCycles() const {return m_DelayedCycles;}
  int& getDelayedCycles() {return m_DelayedCycles;}
  void setLastEnqueueTime(const Time& time) { m_LastEnqueueTime = time; }
  const Time& getLastEnqueueTime() const {return m_LastEnqueueTime;}
  Time& getLastEnqueueTime() {return m_LastEnqueueTime;}
  void incHops() { m_hops++; }
  double getHops() { return m_hops;}
  NodeID getNodeID(){return m_NodeID;}
  void setNodeID(NodeID id){m_NodeID = id;}
private:
  // Private Methods
  
  int m_source;
  int m_dest;
  int m_size;
  int m_use_rf;
  unsigned long long m_index;
  unsigned long long m_time;
  Time m_LastEnqueueTime; // my last enqueue time
  int m_DelayedCycles; // my delayed cycles
  double m_hops;
  NodeID m_NodeID;
};



#endif //NetworkMessage_H
