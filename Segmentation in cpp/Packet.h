/* 
 * Packet.h
 * Header file fo the packet structure 
 * Author: Tigranuhi Mkhitaryan, Alar Paas 
*/

#pragma once
#include <string>

#define HOP_NUMBER 10 // TTL time -how many hops before dropping the packet
#define HEADER_SIZE 50 // - size of the header (without udp)

using namespace std;

struct PacketHeader
{
	char* crc; // should be 32 bytes 
	unsigned short int hop;
	unsigned int orderNumber;
	unsigned short int lastPacketNumber;
	unsigned short int sizeOfOriginalPacket;
	unsigned int pID; //packet ID
	unsigned int mID; //message ID
};

class Packet
{
	PacketHeader header;
	string data; // message from the user
public:
/*
 * Hashing any string to md5
 *
 * string content: content of the packet
*/
	static char* buildCRCWithHashMD5(string content);

	string getterOf_Data();
	void setterOf_Data(string data);
	unsigned int getterOf_messageID();
	unsigned int getterOf_packetID();
	unsigned int getterOf_orderNumber();
	void setterOf_orderNumber(unsigned int orderNumber);
	unsigned int getterOf_lastPacket();
	PacketHeader getterOf_PacketHeader();
	Packet(char* crc);
	Packet(PacketHeader header, string data);
	Packet(string data);
	~Packet();
};

