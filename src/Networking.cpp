#include "Networking.h"

namespace une::enet
{
	//Initalize the ENet library
	bool Init()
	{
		if (enet_initialize() != 0)
		{
			debug::LogError("Failed to initialize ENet");
			return false;
		}
		atexit(enet_deinitialize);
		debug::LogSpam("Successfully initialized ENet");
		return true;
	}

	//Create an ENet server host
	Connection CreateServer(uint16_t port, size_t maxPeers)
	{
		Connection conn;
		conn.isServer = true;
		conn.address.host = ENET_HOST_ANY;
		conn.address.port = port;
		conn.host = enet_host_create(&conn.address, maxPeers, numChannels, 0, 0);

		if (!conn.host)
		{
			debug::LogError("Failed to create ENet server host");
			return Connection();
		}

		debug::LogInfo("ENet server successfully created at port " + std::to_string(port));
		conn.closed = false;
		return conn;
	}

	//Create an ENet client host
	Connection CreateClient()
	{
		Connection conn;
		conn.isServer = false;
		conn.address.host = ENET_HOST_ANY;
		conn.address.port = ENET_PORT_ANY;
		conn.host = enet_host_create(nullptr, 1, numChannels, 0, 0);

		if (!conn.host)
		{
			debug::LogError("Failed to create ENet client host");
			return Connection();
		}

		debug::LogInfo("ENet client successfully created");
		return conn;
	}

	//Connect to an ENet server at ip:port, returns true on success
	bool ConnectToServer(Connection& conn, const std::string& ip, uint16_t port)
	{
		//Resolve the host ip
		if (enet_address_set_host(&conn.address, ip.c_str()) < 0)
		{
			debug::LogError("Failed to resolve host " + ip);
			return false;
		}
		conn.address.port = port;

		//Initiate the connection
		ENetPeer* peer = enet_host_connect(conn.host, &conn.address, numChannels, 0);

		if (!peer)
		{
			debug::LogError("No available peers at " + ip + ":" + std::to_string(port));
			return false;
		}
		conn.peers.push_back(peer);
		conn.closed = false;
		debug::LogInfo("Successfully connected to host at " + ip + ":" + std::to_string(port));
		return true;
	}

	//Update Enet, sends any queued packets, receives any pending packets, and calls the appropriate callbacs
	void UpdateEnet(Connection& conn, auto connectFunc, auto disconnectFunc, auto receiveFunc)
	{
		//Get the update event from Enet
		ENetEvent event;
		if (enet_host_service(conn.host, &event, 0) < 0)
		{
			debug::LogWarning("Failed to update ENet");
			return;
		}

		if (event.type == ENET_EVENT_TYPE_CONNECT)
		{
			char ipAddress[100];
			enet_address_get_host_ip(&event.peer->address, ipAddress, sizeof(ipAddress) - 1);
			printf("Client connected to %s:%u.\n", ipAddress, event.peer->address.port);
			// Store client id:
			int peerId = (int)conn.peers.size();
			conn.peers.push_back(event.peer);
			if (false == connectFunc(peerId))
			{
				enet_peer_reset(event.peer);
				conn.peers.pop_back();
			}
		}
		//Receiving data
		else if (event.type == ENET_EVENT_TYPE_RECEIVE)
		{
			//Parse event packet into Packet class
			Packet packet((char*)event.packet->data, event.packet->dataLength);
			packet.flags = (Packet::Flag)event.packet->flags;
			packet.chanelID = event.channelID;

			//Packet no longer needed
			enet_packet_destroy(event.packet);

			//Send to appropriate peer
			for (int i = 0; i < conn.peers.size(); ++i)
			{
				if (conn.peers[i] == event.peer)
				{
					msg.peerID = i;
					//Make sure receive callback returns true
					if (!receiveFunc(msg))
					{
						printf("Peer %d misbehave!\n", conn.msgBuffer.peerID);
					}
				}
			}
		}
		else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
		{
			//Loop through the peers to find the one that disconnected
			for (size_t i = 0; i < conn.peers.size(); ++i)
			{
				if (conn.peers[i] == event.peer)
				{
					disconnectFunc(i);
					char ip[100];
					enet_address_get_host_ip(&conn.peers[i]->address, ip, sizeof(ip) - 1);
					debug::LogInfo("Peer " + std::to_string(i) + " with ip " + ip + " disconnected");
					conn.peers.erase(conn.peers.begin() + i);
				}
			}
		}
	}

	void CloseConnection(Connection& conn)
	{
		if (conn.host != 0)
		{
			printf("Destroy ENet connection. ");
			printf("Disconnect all peers...");
			for (ENetPeer* peer : conn.peers)
			{
				printf(".");
				enet_peer_disconnect(peer, 0);
			}
			enet_host_flush(conn.host);
			conn.peers.clear();
			// Run host once
			printf(" done!.\nWait to close connection...");
			ENetEvent event;
			enet_host_service(conn.host, &event, 50);
			printf(" closed!\n");
			enet_host_destroy(conn.host);
			printf("ENet connection destroyed.\n");
		}
		conn = Connection{};
	}

	//Send a packet to a specific peer, or if peer < 0 broadcast to all
	int SendData(const Connection& conn, const Packet& packet, int channel, int peer)
	{
		//Create an enet packet from the serialized data
		ENetPacket* enetPacket = packet.ToENetPacket();

		if (peer >= 0)
		{
			//Send to specific peer
			int ret = enet_peer_send(conn.peers[peer], channel, enetPacket);
			if (ret < 0)
			{
				debug::LogWarning("Failed to send ENet packet!");
				enet_packet_destroy(enetPacket);
				return ret;
			}
		}
		else
		{
			//Send to all peers
			enet_host_broadcast(conn.host, channel, enetPacket);
		}
		enet_host_flush(conn.host);
		return 0;
	}
}