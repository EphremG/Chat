/*
* Segmentation.h
* Header file fo the packet segmentation
* Author: Tigranuhi Mkhitaryan, Alar Paas
*/

#pragma once
#include "Packet.h"
#include <list>
#include <vector>

using namespace std;

#define PACKET_CHUNCK 462    //462 bytes as size of maximum input data from user to one packet

struct ReassembledPacket
{
	unsigned int mID;  //  message ID
	PacketHeader header;
	vector<string> packetcontenet;  //  data inside packet
};

class Segmentation
{
	list <Packet> packetsList; // class member wich will contain all splited packets
	bool isFirstPacketPart; 
	ReassembledPacket reassembledPacket; 
public:

/*
 * Checking is segmentation needed.
 * If yes, calculate nesseccary amount of packets, make a original header for small packets.
 * Split the contenet of big packet.
 * Build a new packets and add to the list.
 *
 * Packet packet : the big packet wich we need to split 
*/
	void SplitingPackets(Packet packet);

	list<Packet> getterOf_packetsList();

/*
 * Checking is segmentation needed. Comparing input data length with PACKET_CHUNCK.
 * If length of Data is bigger than PACKET_CHUNCK then segmentation is needed.
 *
 * Packet packet : the big packet wich we need to split
*/
	bool isSegmentationNeeded(Packet packet);

	bool isReassemblingNeeded(Packet packet);

	string reassemblingPackets(Packet packetPart);

	string convertVectorToOnePacket();

	bool isReassemblingPacketCompleted();

	Segmentation();
	~Segmentation();
};

