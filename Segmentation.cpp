/*
* Segmentation.cpp
* Cpp file fo the packet segmentation
* Author: Tigranuhi Mkhitaryan, Alar Paas
*/

#include "Segmentation.h"

Segmentation::Segmentation()
{
	isFirstPacketPart = true; 
}


Segmentation::~Segmentation()
{
}

void Segmentation::SplitingPackets(Packet packet)
{
	if (isSegmentationNeeded(packet))
	{
		string data = packet.getterOf_Data();  // get data part of the initial packet
		PacketHeader header = packet.getterOf_PacketHeader();  // get header part of initial packet
		int numberofNeccesaryPackets;  
		if (data.length() % PACKET_CHUNCK != 0) // if remainder is not 0 
			numberofNeccesaryPackets = data.length() / PACKET_CHUNCK + 1; // we need to add +1 to the numberofNeccesaryPackets for extra last one
		else
			numberofNeccesaryPackets = data.length() / PACKET_CHUNCK; // if remainder is equal to zero

		int i = 0;
		while (data.length() > PACKET_CHUNCK) // segmentation is needed
		{
			header.orderNumber = i;
			header.lastPacketNumber = numberofNeccesaryPackets;  // number of total packets in the header
			string tmpData = data.substr(0, PACKET_CHUNCK); // get first 462 bytes from the  user Data
			data = data.substr(PACKET_CHUNCK, data.length() - PACKET_CHUNCK); // new data will be without 462 bytes taken away from the beginning
			header.crc = Packet::buildCRCWithHashMD5(tmpData); // to build a header crc for these "small packets"

			header.pID = rand() % UINT_MAX; // to get new packet ID for every "small packet"
			i++;

			Packet packetPart(header, tmpData); // building a new packet with new header and content
			packetsList.push_back(packetPart); // adding packet in the end of the list
		}
		if (data.length() > 0) // if we have extra packet then add it to the list
		{
			header.orderNumber = i; // to add sequence numbers one after another
			header.lastPacketNumber = numberofNeccesaryPackets;  // set total amount of packets in message
			header.crc = Packet::buildCRCWithHashMD5(data);  // to get CRC for packet
			header.pID = rand() % UINT_MAX;  // to set random packet ID
			Packet packetPart(header, data);
			packetsList.push_back(packetPart);
		}
	}
}

bool Segmentation::isSegmentationNeeded(Packet packet)  // if data needed to send is smaller than 462 bytes then segmentation is not needed
{
	string data = packet.getterOf_Data();
	if (data.length() < PACKET_CHUNCK)
	{
		return false;
	}
	return true;
}

list<Packet> Segmentation::getterOf_packetsList()
{
	return packetsList;
}


bool Segmentation::isReassemblingNeeded(Packet packet)  // to check if received packet is a lone one or part of the segmented message
{
	PacketHeader header = packet.getterOf_PacketHeader();
	return header.lastPacketNumber > 0;  // if  total amount of packets is above 0 what is set for a lone packet then we need to call reassembling
}

string Segmentation::reassemblingPackets(Packet packetPart)
{
	if (isReassemblingNeeded(packetPart))  
	{
		if (isFirstPacketPart)
		{
			isFirstPacketPart = false;
			PacketHeader header = packetPart.getterOf_PacketHeader();  // get the header of received packet
			reassembledPacket.header = header;
			reassembledPacket.mID = packetPart.getterOf_messageID(); // get and set a message ID

			int index = packetPart.getterOf_orderNumber(); // get the sequence number of received packet
			reassembledPacket.packetcontenet.push_back(packetPart.getterOf_Data()); // get the data from received packet
			reassembledPacket.packetcontenet.resize(packetPart.getterOf_lastPacket());
		}
		else
		{
			if (reassembledPacket.mID == packetPart.getterOf_messageID())  // if packet what we are already reassembling gets a new part coming in
			{
				int index = packetPart.getterOf_orderNumber(); // read the sequence number
				reassembledPacket.packetcontenet[index] = packetPart.getterOf_Data();  // get the data part
			}
		}
		if (isReassemblingPacketCompleted())  // if reassembling completed then build one big/original packet
			return convertVectorToOnePacket();
	}
		return "";
}

string Segmentation::convertVectorToOnePacket()  //  using vector to build up whole message from segmented data parts
{
	string data = "";
	if(isReassemblingPacketCompleted())
	{										
		std::vector<string>::iterator i = reassembledPacket.packetcontenet.begin();
		std::vector<string>::iterator end = reassembledPacket.packetcontenet.end();
		for(; i != end; i++){
			data += *i;
		}
	}
	return data;
}

bool Segmentation::isReassemblingPacketCompleted() //   to check if all packets are arrived or not
{
	//if((reassembledPacket.packetcontenet.size() == reassembledPacket.header.lastPacketNumber) || (reassembledPacket.header.lastPacketNumber = 0)) //  to compare that number of received packets equals to total amount of packets
	for(int i=0; i< reassembledPacket.header.lastPacketNumber; i++)	
	{
		if(reassembledPacket.packetcontenet[i] == "")
			return false;
	}
	return true; 
}