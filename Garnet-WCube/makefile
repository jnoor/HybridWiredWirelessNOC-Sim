##    DEFINE THE ROOT AND THE LIBRARIES
TARGET = garnetSimulator
BUILD_DIR = .
DOXYGEN = /usr/bin/doxygen
DEBUG = 1
##    DO NOT TOUCH BELOW THIS LINE
CC     = g++	
OPT    = -O2	
DEBUG  = -g
CFLAGS =  $(DEBUG) $(OTHER)

# INCLUDE = -I$(SYSTEMC)/include -I$(BUILD_DIR)
# LIB = -L$(SYSTEMC)/lib-linux
INCLUDE = -I$(BUILD_DIR)

CFLAGS += $(INCLUDE)
#default : all

all : $(TARGET)
##############################################################################
#                               Object Rules                                 #
##############################################################################
OBJS    = flitBuffer_d.o flit_d.o GarnetNetwork_d.o InputUnit_d.o NetworkInterface_d.o  NetworkLink_d.o OutputUnit_d.o OutVcState_d.o Router_d.o RoutingUnit_d.o SWallocator_d.o  Switch_d.o  VCallocator_d.o  VirtualChannel_d.o NoCInterface.o RouterAssignment.o main.o EventQueue.o MessageBuffer.o NetworkPower.o power_arbiter.o power_array.o power_bus.o power_crossbar.o power_ll.o power_router_init.o power_static.o power_utils.o  
##############################################################################

INCLUDE_FILES = RouterAssignment.h NoCInterface.h flitBuffer_d.h flit_d.h GarnetNetwork_d.h InputUnit_d.h NetworkInterface_d.h  NetworkLink_d.h OutputUnit_d.h OutVcState_d.h Router_d.h RoutingUnit_d.h SWallocator_d.h  Switch_d.h  VCallocator_d.h  VirtualChannel_d.h NetworkDefine.h Consumer.h EventQueue.h MessageBuffer.h NetworkMessage.h Common.h EventQueueNode.h MessageBufferNode.h NetworkPower.h

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) 


MessageBuffer.o : MessageBuffer.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) MessageBuffer.cpp
EventQueue.o : EventQueue.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) EventQueue.cpp
flitBuffer_d.o : flitBuffer_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) flitBuffer_d.cpp
flit_d.o : flit_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) flit_d.cpp
GarnetNetwork_d.o : GarnetNetwork_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) GarnetNetwork_d.cpp
InputUnit_d.o : InputUnit_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) InputUnit_d.cpp
NetworkInterface_d.o : NetworkInterface_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) NetworkInterface_d.cpp
NetworkLink_d.o : NetworkLink_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) NetworkLink_d.cpp
OutputUnit_d.o : OutputUnit_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) OutputUnit_d.cpp
OutVcState_d.o : OutVcState_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) OutVcState_d.cpp
Router_d.o : Router_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) Router_d.cpp
RoutingUnit_d.o : RoutingUnit_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) RoutingUnit_d.cpp
SWallocator_d.o : SWallocator_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) SWallocator_d.cpp
VirtualChannel_d.o : VirtualChannel_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) VirtualChannel_d.cpp
RouterAssignment.o : RouterAssignment.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) RouterAssignment.cpp
NoCInterface.o : NoCInterface.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) NoCInterface.cpp
main.o : main.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) main.cpp
Switch_d.o : Switch_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) Switch_d.cpp
VCallocator_d.o : VCallocator_d.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) VCallocator_d.cpp
NetworkPower.o : NetworkPower.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) NetworkPower.cpp
power_arbiter.o : power_arbiter.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) power_arbiter.cpp
power_array.o : power_array.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) power_array.cpp
power_bus.o : power_bus.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) power_bus.cpp
power_crossbar.o : power_crossbar.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) power_crossbar.cpp
power_ll.o : power_ll.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) power_ll.cpp
power_router_init.o : power_router_init.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) power_router_init.cpp
power_static.o : power_static.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) power_static.cpp
power_utils.o : power_utils.cpp $(INCLUDE_FILES)
	$(CC) -c $(CFLAGS) power_utils.cpp

clean: 
	rm -f $(TARGET) *.o
