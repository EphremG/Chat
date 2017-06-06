// ConsoleApplication6.cpp : Defines the entry point for the console application.
// Testing if segmentation and reassembling works as it is supposed to be!
// Author: Tigranuhi Mkhitaryan, Alar Paas

#include <iostream>
#include "Segmentation.h"
#include <string>
using namespace std;
int main()
{

	string payload;
	cout << "Testing the segementation! \n";
	cout << "--------------------------------------\n";
	cout << "Enter Your Msg: ";
	getline(cin, payload);
//	to read in entered message
 int n=1;

	Packet packet(payload);
	Segmentation segmentation;
	if (segmentation.isSegmentationNeeded(packet)){
		segmentation.SplitingPackets(packet);
		//if segmentation is needed then build up a list of small packets
	list<Packet> splitedPackets = segmentation.getterOf_packetsList();
	std::list<Packet>::iterator i = splitedPackets.begin();
	std::list<Packet>::iterator end = splitedPackets.end();
	srand((unsigned) time_t(NULL));
	for (; i != end; i++) {
		Packet packetPart = *i;
		PacketHeader header = packetPart.getterOf_PacketHeader();
		//display outcome of generated packets
		cout <<"Pkt"<<n<<"  "<<"Header: CRC: "<< header.crc<<"  HOP: "<< header.hop <<" orderNumber: "<< header.orderNumber <<" lastPacketNumber: "<< header.lastPacketNumber<<" sizeOfOriginalPacket: "  << header.sizeOfOriginalPacket <<" mID: "<< header.mID <<" pID: "<< header.pID <<"	Data: "<<packetPart.getterOf_Data() <<endl<<endl;
		n++;
	}
	n=0;
	i = splitedPackets.begin();
	for (; i != end; i++) {
		Packet packetPart = *i;
		if(segmentation.isReassemblingNeeded(packetPart))
		{
			cout<< "Reassembling step "<< n << ": "<<segmentation.reassemblingPackets(packetPart)<< endl; // display outcome of reassembling
		}
		n++;
	}
	}
	else
		cout<<"Segmentation not needed!!";
	


	//getchar();                                                         
    return 0;
}

