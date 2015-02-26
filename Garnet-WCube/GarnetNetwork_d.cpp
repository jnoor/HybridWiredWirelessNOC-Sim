/*
    Copyright (C) 1999-2008 by Mark D. Hill and David A. Wood for the
    Wisconsin Multifacet Project.  Contact: gems@cs.wisc.edu
    http://www.cs.wisc.edu/gems/

    --------------------------------------------------------------------

    This file is part of Garnet (Princeton's interconnect model),
    a component of the Multifacet GEMS (General Execution-driven 
    Multiprocessor Simulator) software toolset originally developed at 
    the University of Wisconsin-Madison.

    Garnet was developed by Niket Agarwal at Princeton University. Orion was
    developed by Princeton University.

    Substantial further development of Multifacet GEMS at the
    University of Wisconsin was performed by Alaa Alameldeen, Brad
    Beckmann, Jayaram Bobba, Ross Dickson, Dan Gibson, Pacia Harper,
    Derek Hower, Milo Martin, Michael Marty, Carl Mauer, Michelle Moravan,
    Kevin Moore, Andrew Phelps, Manoj Plakal, Daniel Sorin, Haris Volos, 
    Min Xu, and Luke Yen.
    --------------------------------------------------------------------

    If your use of this software contributes to a published paper, we
    request that you (1) cite our summary paper that appears on our
    website (http://www.cs.wisc.edu/gems/) and (2) e-mail a citation
    for your published paper to gems@cs.wisc.edu.

    If you redistribute derivatives of this software, we request that
    you notify us and either (1) ask people to register with us at our
    website (http://www.cs.wisc.edu/gems/) or (2) collect registration
    information and periodically send it to us.

    --------------------------------------------------------------------

    Multifacet GEMS is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    Multifacet GEMS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Multifacet GEMS; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307, USA

    The GNU General Public License is contained in the file LICENSE.

### END HEADER ###
*/
/*
 * GarnetNetwork_d.C
 *
 * Niket Agarwal, Princeton University
 * Edited by Mishali for MCSim
 * */



#include "NetworkDefine.h"
#include "GarnetNetwork_d.h"
#include "NetworkInterface_d.h"
#include "MessageBuffer.h"
#include "Router_d.h"
#include "NetworkLink_d.h"
#include "CreditLink_d.h"
#include "RouterAssignment.h"
#include "RoutingUnit_d.h"
#include "parm_technology.h"

//Extern variables 
unsigned long long nocTime=0;
EventQueue* g_eventQueue_ptr=NULL;
bool deadlock;
Time deadlockTime;
bool canDetectDeadlock;
unsigned int FLITLENGTH;
unsigned long long totalDeadlocks;
unsigned int vcallocThreshold;
unsigned int selfThreshold;

GarnetNetwork_d::GarnetNetwork_d(int nodes, char* routingFile, char* extraLinkFile, NoCInterface* nocInterface, unsigned int pctShortcuts, unsigned int linkBw, unsigned int vcalloc_t, unsigned int self_t)
  : node_stats(NULL)
{
  //Initialize event queue
  g_eventQueue_ptr = new EventQueue();
	nocInt = nocInterface;
	m_nodes = nodes; // Total nodes in network
	//	m_virtual_networks = num_vn; // Number of virtual networks = number of message classes in the coherence protocol
	m_virtual_networks = NUMBER_OF_VIRTUAL_NETWORKS;
	m_ruby_start = 0;
	m_flits_recieved = 0;
	m_flits_injected = 0;
	m_network_latency = 0.0;
	m_queueing_latency = 0.0;
	m_number_hops = 0.0;
	msgRFused = 0;
	num_wire_links = 0;
	//wireless_power = 0.0;

	m_router_ptr_vector.clear();

	// Allocate to and from queues
	m_toNetQueues.setSize(m_nodes); 	// Queues that are getting messages from protocol
	m_fromNetQueues.setSize(m_nodes); 	// Queues that are feeding the protocol
	m_in_use.setSize(m_virtual_networks);
  	m_ordered.setSize(m_virtual_networks);
    	for (int i = 0; i < m_virtual_networks; i++) 
	{
		m_in_use[i] = false;
		m_ordered[i] = false;
	}
    	
	for (int node = 0; node < m_nodes; node++) 
	{
		//Setting how many virtual message buffers will there be per Network Queue
		m_toNetQueues[node].setSize(m_virtual_networks);
		m_fromNetQueues[node].setSize(m_virtual_networks);

		for (int j = 0; j < m_virtual_networks; j++) 
		{ 
			m_toNetQueues[node][j] = new MessageBuffer();	// Instantiating the Message Buffers that interact with the coherence protocol
			m_toNetQueues[node][j]->setOrdering(false);//Mishali
			m_fromNetQueues[node][j] = new MessageBuffer();
			m_fromNetQueues[node][j]->setOrdering(false);
		}
	}


	m_routers = MESHROUTERS + NUM_RF_ROUTERS;

	//create routers
	for (int i=0; i<m_routers; i++) {
		m_router_ptr_vector.insertAtBottom(new Router_d(i, this,routingFile));
	}

	for (int i=0; i < m_nodes; i++) {
	  cout<<"Router number "<<i<<" and network interface created"<<endl;
		NetworkInterface_d *ni = new NetworkInterface_d(i, m_virtual_networks, this);
		ni->addNode(m_toNetQueues[i], m_fromNetQueues[i]);
		m_ni_ptr_vector.insertAtBottom(ni);
	}
	
	cout << "Finished creating routers!" << endl;

	baseLinkBw = linkBw;  // # bits on baseline links
        minLinkBw = 8; // # bits on minimum-width link ; used to be 24

        // check through map_file (first pass through...yeah hacky I know!) AKA Shortcut file
        // to determine minimum-width link in network
        // (at least we're not opening/closing this file in a loop!)
        //shortcut file.
        FILE* map_file=fopen(extraLinkFile,"r");
        int bw_multiplier;
        int s_r, s_c, d_r, d_c; // placeholders

        int count=0;
        while (1)
        	if (fscanf(map_file, "%d %d %d %d %d\n", &s_r, &s_c, &d_r, &d_c, &bw_multiplier)!=EOF)
        	{

        		if(bw_multiplier < minLinkBw)
        			minLinkBw = bw_multiplier;
        	}
        	else
        		break;

        fclose(map_file);
        FLITLENGTH = 4; //used to be minLinkBw/8

        // NOTE: for the current implementation we assume that each
        // link's bandwidth is some even multiple of the FLITLENGTH
        // (or minimum link bandwidth), and furthermore that each
        // link has an associated capacity...how many flits per cycle
        // it can consume
        // NOTE: this assumes ZERO changes in # of credits (set to 8
        // for now) and # buffer entries at each router
        // However, buffer sizes depend on the FLITLENGTH, as each
        // buffer entry holds a single flit
        // -- Adam 4/22/2008

        cout << "Creating Network..." << endl;
	createNetwork(extraLinkFile);//Mishali
	for(int i = 0; i < m_router_ptr_vector.size(); i++)
	{
		m_router_ptr_vector[i]->init();
	}
	
	deadlock = false;
	deadlockTime = 0;
	canDetectDeadlock = true;
	totalDeadlocks=0;
	vcallocThreshold = vcalloc_t;
	selfThreshold = self_t;
	msgInjected=0;
	msgDelivered=0;
	//Mishali: router stats
	m_p2pMsgCounter.setSize(m_routers);
	m_p2pByteCounter.setSize(m_routers);
	for(int i = 0; i < m_routers; i++)
          {
	    m_p2pMsgCounter[i].setSize(m_routers);
	    m_p2pByteCounter[i].setSize(m_routers);
            for(int j = 0; j < m_routers; j++) {
	      m_p2pMsgCounter[i][j] = 0; m_p2pByteCounter[i][j] = 0;
	    }
          }

        pctShort = pctShortcuts;
        
        node_stats = new node_stat[MESHNODES];
}

GarnetNetwork_d::~GarnetNetwork_d()
{
  DumpStats();
  printStats();
  cout<<m_nodes<<" nodes"<<":"<<m_router_ptr_vector.size()<<" routers"<<endl;
	cerr<<"Injected "<< msgInjected<<endl;
	cerr<<"Delivered "<< msgDelivered<<endl;
	for (int i = 0; i < m_nodes; i++) 
	{
		m_toNetQueues[i].deletePointers();
		m_fromNetQueues[i].deletePointers();
	}
	for (int i = 0; i < m_router_ptr_vector.size(); i++) {
	  cerr<<"Stalls for router "<<i<<" :"<<m_router_ptr_vector[i]->getVCStalls()<<endl;
	}
	m_router_ptr_vector.deletePointers();
	m_ni_ptr_vector.deletePointers();
	m_link_ptr_vector.deletePointers();
	m_creditlink_ptr_vector.deletePointers();
	//Delete all the port maps
	delete [] node_stats;
}

void GarnetNetwork_d::reset()
{
	for (int node = 0; node < m_nodes; node++) 
	{
		for (int j = 0; j < m_virtual_networks; j++) 
		{
			m_toNetQueues[node][j]->clear();
			m_fromNetQueues[node][j]->clear();
		}
	}
}

/* 
 * This function creates a link from the Network Interface (NI) into the Network. 
 * It creates a Network Link from the NI to a Router and a Credit Link from  
 * the Router to the NI
*/

//void GarnetNetwork_d::makeInLink(int src, SwitchID dest, int routing_table_entry, int link_latency, int bw_multiplier, bool isReconfiguration)
void GarnetNetwork_d::makeInLink(int src, SwitchID dest, int routing_table_entry, int link_latency, int bw_multiplier, bool isReconfiguration, unsigned int flitCapacity)
{
  cout<<"src"<<src<<"nodes"<<m_nodes<<endl;
	assert(src < m_nodes);
	
	if(!isReconfiguration)
	{	
	  //		NetworkLink_d *net_link = new NetworkLink_d(m_link_ptr_vector.size(), link_latency, this);
	  NetworkLink_d *net_link = new NetworkLink_d(m_link_ptr_vector.size(), link_latency, flitCapacity, -1, this);
		CreditLink_d *credit_link = new CreditLink_d(m_creditlink_ptr_vector.size());
		m_link_ptr_vector.insertAtBottom(net_link);
		m_creditlink_ptr_vector.insertAtBottom(credit_link);
		m_router_ptr_vector[dest]->addInPort(net_link, credit_link);
		m_ni_ptr_vector[src]->addOutPort(net_link, credit_link);
	}
	else 
	{
	  //		ERROR_MSG("Fatal Error:: Reconfiguration not allowed here");
	  cerr<<"Fatal Error: line 173"<<endl;
	  assert(0);
		// do nothing
	}
}

/* 
 * This function creates a link from the Network to a NI. 
 * It creates a Network Link from a Router to the NI and 
 * a Credit Link from NI to the Router 
*/

//void GarnetNetwork_d::makeOutLink(SwitchID src, int dest, int routing_table_entry, int link_latency, int link_weight, int bw_multiplier, bool isReconfiguration)
void GarnetNetwork_d::makeOutLink(SwitchID src, int dest, int routing_table_entry, int link_latency, int link_weight, int bw_multiplier, bool isReconfiguration, unsigned int flitCapacity)
{
	assert(dest < m_nodes);
	assert(src < m_router_ptr_vector.size());
	assert(m_router_ptr_vector[src] != NULL);
	int curr_port = m_router_ptr_vector[src]->get_num_outports();
        m_router_ptr_vector[src]->setNILinkMap(dest,curr_port);

	if(!isReconfiguration)
	{
	  //		NetworkLink_d *net_link = new NetworkLink_d(m_link_ptr_vector.size(), link_latency, this);
	  NetworkLink_d *net_link = new NetworkLink_d(m_link_ptr_vector.size(), link_latency, flitCapacity, THERE, this);
		CreditLink_d *credit_link = new CreditLink_d(m_creditlink_ptr_vector.size());
		m_link_ptr_vector.insertAtBottom(net_link);
		m_creditlink_ptr_vector.insertAtBottom(credit_link);

		m_router_ptr_vector[src]->addOutPort(net_link, routing_table_entry, link_weight, credit_link);
		m_ni_ptr_vector[dest]->addInPort(net_link, credit_link);
	} 
	else 
	{
	  //		ERROR_MSG("Fatal Error:: Reconfiguration not allowed here");
	  cerr<<"Fatal error:205"<<endl;
	  assert(0);
	  //do nothing
	}
}

/* 
 * This function creates a internal network links 
*/

//void GarnetNetwork_d::makeInternalLink(SwitchID src, SwitchID dest, int routing_table_entry, int link_latency, int link_weight, int bw_multiplier, bool isReconfiguration)
void GarnetNetwork_d::makeInternalLink(SwitchID src, SwitchID dest, int routing_table_entry, int link_latency, int link_weight, int bw_multiplier, bool isReconfiguration, unsigned int flitCapacity)
{
  //Add the if block so as to not the change the GEMS API
  int capacity = 0;
  int curr_port = m_router_ptr_vector[src]->get_num_outports();
  int src_col, src_row, dest_ccl, dest_row = 0;
  if (currLinkDirection == RF) {
    //    capacity = extraLinkCapacity;
    currLinkDirection = NORTH;//just to reinitialize the variable
    int route_find_index = RoutingUnit_d::getRouteFind(src,dest);
    //RF link signifies entering wireless NoC -- all destinations must route to wireless
    for( int i=0; i<m_routers;i++) {
      if( RoutingUnit_d::getRouteFind(src,i)  == route_find_index){
        m_router_ptr_vector[src]->setRFLinkMap(i,curr_port);
        //for every src,dest w my source that wants to use this newly created link => set RF link map.
      }
    }
   // m_router_ptr_vector[src]->setDestLinkMap(dest,curr_port);
    m_router_ptr_vector[src]->setRFLinkMap(dest,curr_port);
    
    fprintf(stderr, "RF number is: %d\n",curr_port);
    
  } else if (currLinkDirection == NW) {  
	    //    capacity = extraLinkCapacity;
	   m_router_ptr_vector[src]->setDirLinkMap((int)currLinkDirection,curr_port);
	   fprintf(stderr, "NW number is: %d\n",curr_port);
	    currLinkDirection = NORTH;//just to reinitialize the variable
	 
   } else if (currLinkDirection == NE) {
	    //    capacity = extraLinkCapacity;
	   m_router_ptr_vector[src]->setDirLinkMap((int)currLinkDirection,curr_port);
	   fprintf(stderr, "NE number is: %d\n",curr_port);
	    currLinkDirection = NORTH;//just to reinitialize the variable

  } else if (currLinkDirection == SW) {
	    //    capacity = extraLinkCapacity;
	   m_router_ptr_vector[src]->setDirLinkMap((int)currLinkDirection,curr_port);
	   fprintf(stderr, "SW number is: %d\n",curr_port);
	    currLinkDirection = NORTH;//just to reinitialize the variable

  } else if (currLinkDirection == SE) {
	    //    capacity = extraLinkCapacity;
	   m_router_ptr_vector[src]->setDirLinkMap((int)currLinkDirection,curr_port);
	   fprintf(stderr, "SE route_find is: %d\n",curr_port);
	    currLinkDirection = NORTH;//just to reinitialize the variable

  } else if (currLinkDirection == RF0) {  
	    //    capacity = extraLinkCapacity;
	   m_router_ptr_vector[src]->setDirLinkMap((int)currLinkDirection,curr_port);
	   fprintf(stderr, "RF0 number is: %d\n",curr_port);
	    currLinkDirection = NORTH;//just to reinitialize the variable
	 
  } else if (currLinkDirection == RF1) {
	    //    capacity = extraLinkCapacity;
	   m_router_ptr_vector[src]->setDirLinkMap((int)currLinkDirection,curr_port);
	   fprintf(stderr, "RF1 number is: %d\n",curr_port);
	    currLinkDirection = NORTH;//just to reinitialize the variable

  } else if (currLinkDirection == RF2) {
	    //    capacity = extraLinkCapacity;
	   m_router_ptr_vector[src]->setDirLinkMap((int)currLinkDirection,curr_port);
	   fprintf(stderr, "RF2 number is: %d\n",curr_port);
	    currLinkDirection = NORTH;//just to reinitialize the variable

  } else if (currLinkDirection == RF3) {
	    //    capacity = extraLinkCapacity;
	   m_router_ptr_vector[src]->setDirLinkMap((int)currLinkDirection,curr_port);
	   fprintf(stderr, "RF3 number is: %d\n",curr_port);
	    currLinkDirection = NORTH;//just to reinitialize the variable

  }else {

	  //normal case -- non-wireless link -- mesh link
    src_row = src/MESHDIM;
    src_col = src%MESHDIM;
    int destn = (src_row-1)*MESHDIM + src_col;
    int dests = (src_row+1)*MESHDIM + src_col;
    int deste = src_row*MESHDIM + (src_col+1);
    int destw = src_row*MESHDIM + (src_col-1);
    if (dest == destn)
      currLinkDirection = NORTH;
    else if (dest == dests)
      currLinkDirection = SOUTH;
    else if (dest == deste)
      currLinkDirection = EAST;
    else if (dest == destw)
      currLinkDirection = WEST;
    else {
      cerr<<"Garnet:Trying to create a topology other than mesh currently not supported"<<endl;
      assert(0);
    }
#ifdef NOC_DEBUG
    cerr<<"Adding the mapping for ("<<src<<","<<dest<<") dir "<<currLinkDirection<<" port "<<curr_port<<endl;
#endif
      m_router_ptr_vector[src]->setDirLinkMap((int)currLinkDirection,curr_port);
      fprintf(stderr, "%d MESH number is: %d\n", currLinkDirection, curr_port);
   // m_router_ptr_vector[src]->setDestLinkMap(dest,curr_port);
    


  }
  //m_router_ptr_vector[src]->setDirLinkMap((int)currLinkDirection,curr_port);
  m_router_ptr_vector[src]->setDestLinkMap(dest,curr_port);
  fprintf(stderr, "%d => %d use output %d\n", src, dest, curr_port);

  if(!isReconfiguration)
    {
      NetworkLink_d *net_link = new NetworkLink_d(m_link_ptr_vector.size(), link_latency, flitCapacity, currLinkDirection, this);
      CreditLink_d *credit_link = new CreditLink_d(m_creditlink_ptr_vector.size());
      m_link_ptr_vector.insertAtBottom(net_link);
      m_creditlink_ptr_vector.insertAtBottom(credit_link);
      
      m_router_ptr_vector[dest]->addInPort(net_link, credit_link);
      m_router_ptr_vector[src]->addOutPort(net_link, routing_table_entry, link_weight, credit_link);
    }
  else
    {	
      cerr<<"Fatal 230"<<endl;
      assert(0);
    }
}
/* Mishali -commented
void GarnetNetwork_d::checkNetworkAllocation(int id, bool ordered, int network_num)
{
	assert(id < m_nodes);
	assert(network_num < m_virtual_networks);

	if (ordered) 
	{
		m_ordered[network_num] = true;
	}
	m_in_use[network_num] = true;
}
*/

MessageBuffer* GarnetNetwork_d::getToNetQueue(int id, bool ordered, int network_num)
{
  //	checkNetworkAllocation(id, ordered, network_num);
	return m_toNetQueues[id][network_num];
}

MessageBuffer* GarnetNetwork_d::getFromNetQueue(int id, bool ordered, int network_num)
{
  //	checkNetworkAllocation(id, ordered, network_num);
	return m_fromNetQueues[id][network_num];
}

void GarnetNetwork_d::clearStats()
{
	m_ruby_start = nocTime;
}

Time GarnetNetwork_d::getRubyStartTime()
{
	return m_ruby_start;
}

void GarnetNetwork_d::printStats() const
{	double average_link_utilization = 0;
	Vector<double > average_vc_load;
	average_vc_load.setSize(m_virtual_networks*VCPERCLASS);	

	for(int i = 0; i < m_virtual_networks*VCPERCLASS; i++)
	{
		average_vc_load[i] = 0;
	}

	cout << endl;
	cout << "Network Stats" << endl;
	cout << "-------------" << endl;
	cout << endl;
	
	for(int i = 0; i < m_link_ptr_vector.size(); i++) 
	{
	
		average_link_utilization += (double(m_link_ptr_vector[i]->getLinkUtilization())) / (double(nocTime));
	
		Vector<int > vc_load = m_link_ptr_vector[i]->getVcLoad();
		for(int j = 0; j < vc_load.size(); j++)
		{
			assert(vc_load.size() == VCPERCLASS *m_virtual_networks);
			average_vc_load[j] += vc_load[j];
		}
	}
	
	average_link_utilization = average_link_utilization/m_link_ptr_vector.size();
	cout << "Average Link Utilization :: " << average_link_utilization << " flits/cycle" << endl;
	cout << "-------------" << endl;

	for(int i = 0; i < VCPERCLASS*NUMBER_OF_VIRTUAL_NETWORKS; i++)
	{
		average_vc_load[i] = (double(average_vc_load[i]) / (double(nocTime)));
		cout << "Average VC Load [" << i << "] = " << average_vc_load[i] << " flits/cycle " << endl;
	}
	cout << "-------------" << endl;

	//out << "Total flits injected = " << m_flits_injected << endl;
	//out << "Total flits recieved = " << m_flits_recieved << endl;
	cout << "Average network latency = " << ((double) m_network_latency/ (double) m_flits_recieved)<< endl;
	cout << "Average number hops = " << ((double) m_number_hops/ (double) msgDelivered) << endl;
	cout << "RF usage = " <<  msgRFused << "/" << msgDelivered << " (" << ((double) msgRFused/ (double) msgDelivered)*100.0 << " %)" << endl;
	//out << "Average queueing latency = " << ((double) m_queueing_latency/ (double) m_flits_recieved)<< endl;
	//	out << "Average latency = " << ((double)  (m_queueing_latency + m_network_latency) / (double) m_flits_recieved)<< endl;
	cout << "-------------" << endl;

	double m_total_link_power = 0.0;
	double m_total_router_power = 0.0;

	//m_link_ptr_vector.size()
	for(int i = 0; i < num_wire_links; i++)
	{
	  //Mishali:uncomment
	  m_total_link_power += m_link_ptr_vector[i]->calculate_power();
	}

	for(int i = 0; i < m_router_ptr_vector.size(); i++)
	{
	  //Mishali:uncomment
	  m_total_router_power += m_router_ptr_vector[i]->calculate_power();
	}
	
	double sim_cycles = g_eventQueue_ptr->getTime() - 0.0;
	double wireless_power = 4.5e-12*(msgRFused*FLITBITLENGTH* PARM_Freq/sim_cycles);
	//printf("parm_freq=%f\n", PARM_Freq);
	
	cout << "Total Link Power = " << m_total_link_power << " W " << endl;
	cout << "Total Router Power = " << m_total_router_power << " W " <<endl;
	cout << "Total Wireless-I Power = " << wireless_power << " W " <<endl;
	cout << "-------------" << endl;
	cout << "Router stats: # of msgs and # of bytes injected between a pair of routers" << endl;
/*	for(int i = 0; i < m_routers; i++)
	  {
		for(int j = 0; j < m_routers; j++)
	      {
		cout << "RouterA:" << i << ",RouterB:" << j << ",numMsgs:"
		     << m_p2pMsgCounter[i][j] << ",numBytes:" << m_p2pByteCounter[i][j]<<endl;
	      }
	  } */
	cout << "--------------" << endl;

	cout <<"Total number of deadlocks: "<<totalDeadlocks<<endl;
	
}

void GarnetNetwork_d::printConfig(ostream& out) const
{
	out << endl;
	out << "Network Configuration" << endl;
	out << "---------------------" << endl;
	out << "network: GarnetNetwork_d" << endl;
	out << endl;

	for (int i = 0; i < m_virtual_networks; i++) 
	{
		out << "virtual_net_" << i << ": ";
		if (m_in_use[i]) 
		{
			out << "active, ";
			if (m_ordered[i]) 
			{
				out << "ordered" << endl;
			} 
			else 
			{
				out << "unordered" << endl;
			}
		} 
		else 
		{
			out << "inactive" << endl;
		}
	}
  	out << endl;

	for(int i = 0; i < m_ni_ptr_vector.size(); i++)
	{
		m_ni_ptr_vector[i]->printConfig(out);
	}
	for(int i = 0; i < m_router_ptr_vector.size(); i++)
	{
		m_router_ptr_vector[i]->printConfig(out);
	}	

}
/*
 * Mishali: function definitions added for the interface
 */

// firstNonWhiteSpace
//   * given a c-style string (assumed NULL-delimited)
//   * finds first non-whitespace character (non-tab, non-space)
//     and returns it
//     (if no such char exists, return space ' ')
char firstNonWhiteSpace(char* line)
{
  for(int i = 0; i < strlen(line); i++)
    if(line[i] != '\t' && line[i] != ' ')
      return line[i];
  return ' ';
}

void GarnetNetwork_d::Tick()
{ 
  checkForDeadlock();
  g_eventQueue_ptr->triggerEvents(nocTime);
  popFromNetQueue();
  //Increment time
  nocTime++;
  if ((nocTime - lastPhaseInterval) >= NW_PHASE_INTERVAL)                                                                                                                         
    {
      lastPhaseInterval = nocTime;
      dumpClearPhaseStats();
    }
}


//Mishali: Added this function, it uses router assingment
//file to configure mesh
//So this function will create all of the links between the routers in the network.
	//It creates a standard mesh (not a torus) MESHDIMxMESHDIM
	//and then attaches all the 16 wireless links to 4 routers each
	//	as specified in the .cut shortcut file.
	//Finally, the wireless links are interconnected using the hypercube topology.
void GarnetNetwork_d::createNetwork(char* extraLinkFile)
{

  int routing_table_entry = 0;//dummy
  int link_latency = 1;
  int bw_multiplier = 0;
  bool isReconfiguration = false;
  int link_weight = 0;
  int row = 0;
  int col = 0;
  int dest = 0;
  int curr_port = 0;
  int concentration = MESHNODES/MESHROUTERS; //used to vary latencies

  vector<NodeInfo*> tempVector;
  for (int i=0; i<m_routers - NUM_RF_ROUTERS; i++) {
    //This can be changed later
    tempVector = nocInt->getRouterToLocalNodes(i);

    //instantiate each router and local node connection -- in-link => enter network, out-link => exit network
    for (int node = 0; node < tempVector.size(); node++) { 
    	if ((tempVector[node])->nodeID == 780) {
    		//printf("***** 780 attached to router[%d]\n", i);
    	}
    makeInLink((tempVector[node])->nodeID, i, routing_table_entry, link_latency, bw_multiplier, isReconfiguration, baseLinkBw / FLITBITLENGTH);
    makeOutLink(i, (tempVector[node])->nodeID, routing_table_entry,link_latency, link_weight, bw_multiplier, isReconfiguration, baseLinkBw / FLITBITLENGTH);
    }
  }

  //Connect the routers together into a mesh. Each router has up to 4 connections, NOT TAURUS
  cerr<<"Number of routers: "<<m_routers<<endl;
  cerr<<"Number of RF Routers: "<<NUM_RF_ROUTERS<<endl;
  for (int i=0; i<m_routers - NUM_RF_ROUTERS; i++) {
    row = i/MESHDIM; //mesh row number of router i
    col = i%MESHDIM; //mesh col number of router i

    //NORTH router
    if (row != 0){
      //HACK: to vary link latency for vertical links
      if (concentration>1) //more than one core per node/router
    	  link_latency=2;
      dest = (row-1)*MESHDIM + col;
      if (dest<m_routers)
    	  makeInternalLink(i, dest, routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, baseLinkBw / FLITBITLENGTH);
      link_latency=1;
    }

    //SOUTH router
    if (row != MESHDIM - 1){
      //HACK: to vary link latency for vertical links
      if (concentration>1)                                                                                                                                                
        link_latency=2;
      dest = (row+1)*MESHDIM + col;
      if (dest<m_routers)
    	  makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, baseLinkBw / FLITBITLENGTH);
      link_latency=1;
    }

    //EAST router
    if (col!=MESHDIM - 1){
      //HACK: to vary link latency for horizontal links (conc2 and conc4 designs)
      if (concentration > 1)
    	  link_latency=2;
      dest = row*MESHDIM + (col+1);
      if (dest<m_routers)
    	  makeInternalLink(i, dest, routing_table_entry, link_latency, link_weight,  bw_multiplier, isReconfiguration, baseLinkBw / FLITBITLENGTH);
      link_latency=1;
    }

    //WEST router
    if (col!=0) {
      //HACK: to vary link latency for horizontal links (conc2 and conc4 designs)
      if (concentration > 1)
        link_latency=2;
      dest = row*MESHDIM + (col-1);
      if (dest<m_routers)
    	  makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, baseLinkBw / FLITBITLENGTH);
      link_latency=1;
    }
  }
  
  
  //read from the router shortcut file (*.cut) to add extra links to the network (AKA Shortcuts)
  //each of these should signify a connection between a mesh router and a wireless router
  //mesh router is addressed with (r,c). wireless router is indexed as (mesh.rows(), WIRELESS_ROUTER_#)
  int s_r,s_c,d_r,d_c,s,d=0;
  int counter=0;
  //read the shortcuts file aka *.cut
  FILE* map_file=fopen(extraLinkFile,"r");
  while (1)
	  //                row1 col1 row2 col2 bandwidth
    if (fscanf(map_file, "%d %d %d %d %d\n", &s_r, &s_c, &d_r, &d_c, &bw_multiplier)!=EOF) {
    	s = (s_r*MESHDIM) + s_c; //source node/router #
    	d = (d_r*MESHDIM) + d_c; //dest node/router #
        unsigned int extraLinkCapacity = bw_multiplier / FLITBITLENGTH; //linkcapacity of shortcut
        currLinkDirection = RF; //means "move-to-wireless"
        //add RF shortcut from source base router to destination wireless router
        makeInternalLink(s, d,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, extraLinkCapacity);
        // connection between base nodes and RF routers aka not WCUBE nodes

        
       // I think this is to signify which attached base router is connected to each WCube transceiver
        //
        if (WCUBE_ON) {
        	if (counter == 0) {
        		currLinkDirection = NW;
        	} else if (counter == 1) {
        		currLinkDirection = NE;
        	} else if (counter == 2) {
        		currLinkDirection = SW;
        	} else if (counter == 3) {
        		currLinkDirection = SE;
        	} else {
        		printf("ERROR: Not Possible\n");
        	}
        	// make connection from wireless router to base router
        	makeInternalLink(d, s,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, extraLinkCapacity);
        	counter = (counter+1)%4;          
        }
		#ifdef NOC_DEBUG
		cerr<<"RF ("<<s<<","<<d<<")"<<endl;
		#endif
      } else
	break;
  fclose(map_file);
  
  num_wire_links = m_link_ptr_vector.size();
  
  
  // for WCube RF routers
  // add links in between wireless routers
  for (int i=m_routers - NUM_RF_ROUTERS; i<m_routers; i++) {
    row = i/MESHDIM;
    col = i%MESHDIM;
    link_latency = 1;
    fprintf(stderr,"assert that row -%d- == 16\n", row);
    if (col == 0) { // RF router <0000>
    	dest = row*MESHDIM + 8;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 4;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 2;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 1;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    	    	
    
    } else if (col == 1) { // RF router <0001>
    	dest = row*MESHDIM + 9;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 5;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 3;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 0;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 2) { // RF router <0010>
    	dest = row*MESHDIM + 10;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 6;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 0;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 3;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 3) { // RF router <0011>
    	dest = row*MESHDIM + 11;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 7;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 1;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 2;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 4) { // RF router <0100>
    	dest = row*MESHDIM + 12;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 0;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 6;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 5;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 5) { // RF router <0101>
    	dest = row*MESHDIM + 13;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 1;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 7;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 4;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 6) { // RF router <0110>
    	dest = row*MESHDIM + 14;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 2;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 4;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 7;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 7) { // RF router <0111>
    	dest = row*MESHDIM + 15;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 3;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 5;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 6;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 8) { // RF router <1000>
    	dest = row*MESHDIM + 0;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 12;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 10;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 9;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 9) { // RF router <1001>
    	dest = row*MESHDIM + 1;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 13;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 11;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 8;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 10) { // RF router <1010>
    	dest = row*MESHDIM + 2;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 14;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 8;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 11;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 11) { // RF router <1011>
    	dest = row*MESHDIM + 3;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 15;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 9;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 10;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 12) { // RF router <1100>
    	dest = row*MESHDIM + 4;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 8;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 14;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 13;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 13) { // RF router <1101>
    	dest = row*MESHDIM + 5;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 9;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 15;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 12;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 14) { // RF router <1110>
    	dest = row*MESHDIM + 6;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 10;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 12;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 15;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    } else if (col == 15) { // RF router <1111>
    	dest = row*MESHDIM + 7;
    	currLinkDirection = RF0;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 11;
    	currLinkDirection = RF1;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);
    	
    	dest = row*MESHDIM + 13;
    	currLinkDirection = RF2;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);

    	dest = row*MESHDIM + 14;
    	currLinkDirection = RF3;
    	if (dest<m_routers)
    		makeInternalLink(i, dest,  routing_table_entry, link_latency, link_weight, bw_multiplier, isReconfiguration, minLinkBw / FLITBITLENGTH);    
    	
    }
    

  }  
  
  //printf("*** wires: %d   total: %d\n", num_wire_links, m_link_ptr_vector.size());
  
  
}

void GarnetNetwork_d::insertIntoToQueue(int id, int network_num, int source, int dest, int broadcast, int size, unsigned long long index, NodeID node)
{
  NetworkMessage*  msg = new NetworkMessage(source, dest, size, index, node);
  m_toNetQueues[id][network_num]->enqueue(msg, 1); 
  #ifdef NOC_DEBUG
  cerr<<"Inserted "<<index<<" @"<<g_eventQueue_ptr->getTime()<<" (";
  cerr<<source<<","<<dest<<")"<<"nodeid"<<node<<endl;
  #endif
  msgInjected++;
  m_p2pMsgCounter[source][dest]++;
  m_p2pByteCounter[source][dest] += size;
}

void GarnetNetwork_d::popFromNetQueue()
{
  for (int i=0; i<m_nodes; i++) {
    for (int vnet=0; vnet<m_virtual_networks; vnet++) {
      NetworkMessage* msg=NULL;
      //      if (m_fromNetQueues[i][vnet]->isReady()) {
      while (m_fromNetQueues[i][vnet]->isReady()) {
	msg = m_fromNetQueues[i][vnet]->peekNetworkMessage();
	m_fromNetQueues[i][vnet]->dequeue();
	//Mishali: TODO not needed for trace mode
	//      nocinterface->NoCExit(msg->getIndex(),msg->getDest());
	unsigned long long index = msg->getIndex();
	#ifdef NOC_DEBUG
	cerr<<"Delivered "<<index<<" @"<<g_eventQueue_ptr->getTime()<<endl;
	#endif
	msgDelivered++;
	if (msg->useRF()) {  // if this msg used wireless backbone 
		msgRFused++;  
	}
	increment_hops(msg->getHops());
	delete msg;
      }
    }
  }
}

void GarnetNetwork_d::sendWaitVector(int source, int dest, bool *waitVector)
{ //printf("hi1\n");
  assert(dest<m_routers);
  m_router_ptr_vector[dest]->receiveWaitVector(source,waitVector); //printf("hi2\n");
  
}
void GarnetNetwork_d::checkForDeadlock()
{
  if (deadlock) {
    for (int i=0; i < m_routers; i++) {
      m_router_ptr_vector[i]->clearWaitVector();
    }
    deadlock = false;
  }
    
}

//To update router stats
void GarnetNetwork_d::updateRouterStats(int source, int dest, int bytes) 
{
  m_p2pMsgCounter[source][dest]++;
  m_p2pByteCounter[source][dest] += bytes;
}

void GarnetNetwork_d::dumpClearPhaseStats()
{
  unsigned long long overallL2Access = 0;
  unsigned long long overallNwInject = 0;
  unsigned long long overallExtraBwStall = 0;
  unsigned long long overallExtraFlitsSent = 0;
  unsigned long long overallBwStall = 0;
  unsigned long long overallFlitsSent = 0;
  int node = 0;
  for (int i=0; i < MESHDIM; i++)
    for (int j=0; j < MESHDIM; j++)
      {
	node = (i * MESHDIM) + j;
	if (node < m_routers) {
	node_stat* ptr = &(node_stats[node]);

	overallL2Access += ptr->phaseL2Access;
	overallNwInject += ptr->phaseNwInject;
	overallExtraBwStall += ptr->phaseExtraBwStall;
	overallExtraFlitsSent += ptr->phaseExtraFlitsSent;
	overallBwStall += ptr->phaseBwStall;
	overallFlitsSent += ptr->phaseFlitsSent;

      ptr->phaseL2Access = ptr->phaseNwInject = ptr->phaseExtraBwStall
                         = ptr->phaseExtraFlitsSent = ptr->phaseBwStall
	= ptr->phaseFlitsSent = 0;
	}
      }

  cout << "@" << nocTime << " PHASE L2 ACCESS : " << overallL2Access << endl;
  cout << "@" << nocTime << " PHASE NW INJECT : " << overallNwInject << endl;
  cout << "@" << nocTime << " PHASE RF BW STALL : " << overallExtraBwStall
       << endl;
  cout << "@" << nocTime << " PHASE RF FLITS SENT : " << overallExtraFlitsSent
       << endl;
  cout << "@" << nocTime << " PHASE BW STALL : " << overallBwStall << endl;
  cout << "@" << nocTime << " PHASE FLITS SENT : " << overallFlitsSent << endl;
}


void GarnetNetwork_d::DumpStats()
{
  int i, j;
  int node = 0;
  // print router stats: # msgs injected
  std::cout << "**************** <NOC STATS: " << nocTime
	    << " > *************************"
	    << "\n";
  for (i=0; i < MESHDIM; i++)
    for (j=0; j < MESHDIM; j++)
      {
	node = (i * MESHDIM) + j;
        if (node < m_routers) {
        node_stat* ptr = &(node_stats[node]);
	std::cout << "Router(" << i << "," << j << ") " << ptr->stat_c << " " << ptr->stat_n << " " << ptr->stat_e << " " << ptr->stat_s << " " << ptr->stat_w
		  << " " << ptr->stat_extra;
        ptr->stat_n=0;
        ptr->stat_e=0;
        ptr->stat_s=0;
        ptr->stat_w=0;
        ptr->stat_c=0;
        ptr->stat_extra=0;
	std::cout  << " " << ptr->stalls_bw << " " << ptr->stalls_q << std::endl;
        ptr->stalls_bw=0;
        ptr->stalls_q=0;
	}
      }
  std::cout << "**************** </NOC STATS: " << nocTime
	    << " > ***********************"
	    << "\n";
}
