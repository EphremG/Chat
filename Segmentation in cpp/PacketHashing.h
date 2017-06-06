/*
 * PacketHashing.h
 * Header file for the packet hashing
 * *      Author: Tigranuhi Mkhitaryan
 */

#pragma once

#include <list>

#define PACKET_LIFE_TIME 600000 // 10 minutes to miliseconds 

using namespace std;

struct pktHash
{
	string pktContent; // content Hashes 
	time_t pktCreationTime;
};

class PacketHashing
{
	time_t m_lastGCCalled; // remember the last time then GC was called
	list<pktHash> m_hashList; // list of hashes and their creation times

/*
 * Any string argument convert to hash by sha256 algorithm and returen already hashed value.
 *
 * string content: the content of the packet 
 */
	string HashPacketSha256(string content); 
	
/*
 * Checking present system time, make a loop to hash's list and compare each paket's creation time to present time.
 * If packet was created more than PACKET_LIFE_TIME will be removed from the list. 
 */
	void ClearExpiredHashes();
public:
	PacketHashing(void);
	~PacketHashing(void);

/*
 * Checking if packet already exist in the hash's list. 
 * If no, Hashing the new packet and add it to the hash's list.
 * If yes, changing the creation time of the packet in the list.
 * Calles the GC every 10 minutes 
 *
 * string content: the content of the packet
 */
	void AddPacketToHashingList(string content);

/*
 * Hashing the content of the packet.
 * Make a loop to hash's list and compare each paket's hash value.
 * Checking if the packet already exist in the hash's list.
 *
 * string content: the content of the packet
 */
	bool IsPacketAlreadyExists(string content);
};

