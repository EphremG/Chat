/*
 * PacketHashing.cpp
 * Cpp file for the packet hashing
 * *      Author: Tigranuhi Mkhitaryan
 */

#include "PacketHashing.h"
#include <openssl/sha.h> // library for using Sha hashing functions 
#include <sstream>
#include <iomanip>
#include <ctime> // for determine the time 

PacketHashing::PacketHashing(void)
{
	m_lastGCCalled = time(0); // initial value object creation time
}


PacketHashing::~PacketHashing(void)
{
	 m_hashList.clear();
}

string PacketHashing::HashPacketSha256(string content)
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, content.c_str(), content.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool PacketHashing::IsPacketAlreadyExists(string content)
{
	string packetHash = HashPacketSha256(content); // hashing the packet's content 
	list<pktHash>::const_iterator iterator;        // create the iterator in order to be able to make a loop in the hash's list
	for (iterator = m_hashList.begin(); iterator != m_hashList.end(); ++iterator) 
	{
		pktHash hash = *iterator;
		if(hash.pktContent == packetHash)   
			return true;
	}
	return false;
}

void PacketHashing::ClearExpiredHashes()
{
	list<pktHash>::const_iterator iterator;
			for (iterator = m_hashList.begin(); iterator != m_hashList.end(); ++iterator) 
			{
				pktHash hash = *iterator;
				if(time(0) - hash.pktCreationTime >= PACKET_LIFE_TIME) // packet was created more than PACKET_LIFE_TIME
				{
					m_hashList.erase(iterator);
				}
			}
}

void PacketHashing::AddPacketToHashingList(string content)
{
	string packetHash = HashPacketSha256(content);
	if(!IsPacketAlreadyExists(content)) // if packet is not exist in the list, add it 
	{
		pktHash hash;
		hash.pktContent = packetHash;
		hash.pktCreationTime = time(0); // current date/time based on current system

		m_hashList.push_back(hash); 
	}
	else // if packet exist change the creation time of the packet in the list
	{
		list<pktHash>::const_iterator iterator;
		for (iterator = m_hashList.begin(); iterator != m_hashList.end(); ++iterator) 
		{
			pktHash hash = *iterator;
			if(hash.pktContent == packetHash)
			{
				hash.pktCreationTime = time(0);
				m_hashList.erase(iterator);
				m_hashList.push_back(hash);
				break;
			}			
		}
	}

	if(time(0) - m_lastGCCalled >= PACKET_LIFE_TIME) // if last time of calling GC was more than  PACKET_LIFE_TIME, will call again
	{
		m_lastGCCalled = time(0); // update the time
		ClearExpiredHashes(); 
	}
}