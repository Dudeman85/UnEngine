#pragma once

#include <string>
#include <functional>
#include <unordered_map>

#include "enet/enet.h"
#include "miniupnpc.h"

#include "Serialization.h"

namespace une::enet
{
	//ENet connection data
	struct Connection
	{
		bool isServer = false;
		bool closed = true;
		ENetHost* host = nullptr;
		std::unordered_map<uint16_t, ENetPeer*> peers;
		uint16_t nextPeerId = 0;
		ENetAddress address;
	};
	//Information about a peer stored in ENetPeer::data
	struct PeerInfo
	{
		uint16_t id = 0;
		char ip[40];
		uint16_t port = 0;
	};

	//How many channels should the enet host have
	inline uint32_t numChannels = 2;
	inline std::function<bool(const PeerInfo&)> onConnectFunc;
	inline std::function<void(const PeerInfo&)> onDisconnectFunc;
	inline std::function<void(const PeerInfo&, const Packet&)> onReceiveFunc;

	//Initalize the ENet library
	bool Init();
	//Create an ENet server host
	Connection CreateServer(uint16_t port, size_t maxPeers, size_t channelLimit = 2);
	//Create an ENet client host
	Connection CreateClient();
	//Connect to a server
	bool ConnectToServer(Connection& conn, const std::string& ip, uint16_t port);
	//Close this connection and disconnect all peers, no disconnect events will be received
	void CloseConnection(Connection& conn);
	//Update Enet, sends any queued packets, receives any pending packets, and calls the appropriate callbacs 
	void UpdateEnet(Connection& conn);
	//Send a packet to a specific peer, or if peer < 0 broadcast to all
	bool SendData(Connection& conn, const Packet& packet, int channel, int peer = -1);

	//Set a function to call when a peer is connected
	void OnConnect(const std::function<bool(const PeerInfo&)>& callback);
	//Set a function to call when a peer is disconnected
	void OnDisconnect(const std::function<void(const PeerInfo&)>& callback);
	//Set a function to call when a packet is received
	void OnReceive(const std::function<void(const PeerInfo&, const Packet&)>& callback);

	struct UPNPPortInfo
	{
		UPNPUrls upnpUrls;
		IGDdatas igdData;
		std::string port;
	};

	//Attempt to open a upd port
	bool UPNPMapPort(const std::string& port, const std::string& name);
	//Attempt to delete a udp port mapping by its name
	bool UPNPUnmapPort(const std::string& name);
	//Attempt to delete all upd ports mapped by this program
	void UPNPUnmapAllPorts();
}
