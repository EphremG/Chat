/*
* Packet.cpp
* Cpp file fo the packet structure
* Author: Tigranuhi Mkhitaryan, Alar Paas
*/

#include "Packet.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <openssl/md5.h>

Packet::Packet(char* crc)
{
	header.crc = crc;
	header.hop = HOP_NUMBER;
	header.orderNumber = 0;
	header.lastPacketNumber = 0;
	header.sizeOfOriginalPacket = HEADER_SIZE + sizeof data; // to get a size of the entire packet 

	srand(time_t(NULL));
	header.mID = rand() % UINT_MAX; //to assign random number as message ID from 0 till Max Int value
	header.pID = rand() % UINT_MAX; //to assign random number as packet ID from 0 till Max Int value
}

Packet::Packet(PacketHeader header, string data)
{
	this->header = header;
	this->data = data;
}

Packet::Packet(string data) 
{
	header.crc = buildCRCWithHashMD5(data);
	this->data = data;
	header.hop = HOP_NUMBER;
	header.orderNumber = 0;
	header.lastPacketNumber = 0;
	header.sizeOfOriginalPacket = HEADER_SIZE + data.length(); // size of original packet = header +full original message length

	header.mID = rand() % UINT_MAX;
	header.pID = rand() % UINT_MAX;
}

Packet::~Packet()
{
}

string Packet::getterOf_Data()
{
	return data;
}

void Packet::setterOf_Data(string data)
{
	this->data = data;
}

unsigned int Packet::getterOf_messageID()
{
	return header.mID;
}

unsigned int Packet::getterOf_packetID() 
{
	return header.pID;
}

unsigned int Packet::getterOf_orderNumber()
{
	return header.orderNumber;
}

void Packet::setterOf_orderNumber(unsigned int orderNumber)
{
	header.orderNumber = orderNumber;
}

unsigned int Packet::getterOf_lastPacket()
{
	return header.lastPacketNumber;
}

PacketHeader Packet::getterOf_PacketHeader()
{
	return header;
}

char* Packet::buildCRCWithHashMD5(string content)  // Build CRC with MD5 
{
	unsigned char digest[MD5_DIGEST_LENGTH];
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, content.c_str(), content.size());
	MD5_Final(digest, &ctx);

	char mdString [33];
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
		sprintf(&mdString[i * 2], "%02x", (unsigned int)digest[i]);

	return mdString;
}
