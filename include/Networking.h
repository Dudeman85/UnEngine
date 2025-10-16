#pragma once

#include <iostream>
#include <enet/enet.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>
#include <assert.h>

#include "Serialization.h"
#include "glm/ext/scalar_uint_sized.hpp"

namespace une::enet
{
	//How many channels should the enet host have
	inline uint32_t numChannels = 2;

	//ENet connection data
	struct Connection
	{
		bool isServer = false;
		bool closed = true;
		ENetHost* host = nullptr;
		std::vector<ENetPeer*> peers;
		ENetAddress address;
	};
	//Information about a peer stored in ENetPeer::data
	struct PeerInfo
	{

	};

	//Initalize the ENet library
	bool Init();
	//Create an ENet server host
	Connection CreateServer(uint16_t port, size_t maxPeers, size_t channelLimit = 2);
	//Create an ENet client host
	Connection CreateClient();
	//Connect to a server
	bool ConnectToServer(Connection& conn, const std::string& ip, uint16_t port);
	//Close this connection and disconnect all peers
	void CloseConnection(Connection& conn);
	//Update Enet, sends any queued packets, receives any pending packets, and calls the appropriate callbacs 
	void UpdateEnet(Connection& conn, auto connectFunc, auto disconnectFunc, auto receiveFunc, int timeOut);
	//Send a packet to a specific peer, or if peer < 0 broadcast to all
	int SendData(const Connection& conn, const Packet& packet, int channel, int peer = -1);
}
