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
		ENetPeer* server = enet_host_connect(conn.host, &conn.address, numChannels, 0);

		if (!server)
		{
			debug::LogError("No available peers at " + ip + ":" + std::to_string(port));
			return false;
		}
		conn.peers[conn.nextPeerId++] = server;
		conn.closed = false;
		debug::LogInfo("Successfully connected to host at " + ip + ":" + std::to_string(port));
		return true;
	}

	//Update Enet, sends any queued packets, receives any pending packets, and calls the appropriate callbacs
	void UpdateEnet(Connection& conn)
	{
		//Get every event ENet has in queue
		ENetEvent event;
		while (enet_host_service(conn.host, &event, 0) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_RECEIVE: {
					//Parse peer info
					PeerInfo info;
					memcpy(&info, event.peer->data, sizeof(PeerInfo));
					//Parse event packet into Packet class
					Packet packet((char*) event.packet->data, event.packet->dataLength);
					packet.flags = (Packet::Flag) event.packet->flags;
					packet.chanelID = event.channelID;
					enet_packet_destroy(event.packet);

					//Call OnReceive if applicable
					if (onReceiveFunc)
						onReceiveFunc(info, packet);
					break;
				}

				case ENET_EVENT_TYPE_CONNECT: {
					//Get the peer id, ip, and port
					PeerInfo* info = new PeerInfo{.id = conn.nextPeerId++, .port = event.peer->address.port};
					enet_address_get_host_ip(&event.peer->address, info->ip, sizeof(info->ip) - 1);
					event.peer->data = info;
					conn.peers[info->id] = event.peer;
					std::string infoString = conn.isServer ? "client: " : "server: " + std::string(info->ip) + ":" + std::to_string(info->port);

					//Call OnConnect if applicable
					if (onConnectFunc)
					{
						if (!onConnectFunc(*info))
						{
							debug::LogWarning("Failed to connect to " + infoString + ", OnConnect returned false.");
							enet_peer_disconnect(event.peer, 0);
							return;
						}
					}
					debug::LogInfo("Connected to " + infoString);
					break;
				}

				case ENET_EVENT_TYPE_DISCONNECT: {
					//Parse peer info
					PeerInfo info;
					memcpy(&info, event.peer->data, sizeof(PeerInfo));

					//Remove the peer
					onDisconnectFunc(info);
					conn.peers.erase(info.id);
					debug::LogInfo("Peer " + std::to_string(info.id) + " with ip " + info.ip + " disconnected");
					break;
				}

				case ENET_EVENT_TYPE_NONE:
					return;
			}
		}
	}

	//Close this connection and disconnect all peers, no disconnect events will be received
	void CloseConnection(Connection& conn)
	{
		if (conn.host)
		{
			debug::LogInfo("Closing ENet connection and disconnecting all peers");
			for (auto peer: conn.peers)
				enet_peer_disconnect(peer.second, 0);
			conn.peers.clear();
			enet_host_flush(conn.host);
			enet_host_destroy(conn.host);
		}
		conn = Connection{};
	}

	//Send a packet to a specific peer, or if peer < 0 broadcast to all
	bool SendData(Connection& conn, const Packet& packet, int channel, int peer)
	{
		//Create an enet packet from the serialized data
		ENetPacket* enetPacket = packet.ToENetPacket();

		if (peer >= 0)
		{
			//Send to specific peer
			int ret = enet_peer_send(conn.peers[peer], channel, enetPacket);
			if (ret < 0)
			{
				debug::LogWarning("Failed to send ENet packet");
				enet_packet_destroy(enetPacket);
				return false;
			}
		}
		else
		{
			//Send to all peers
			enet_host_broadcast(conn.host, channel, enetPacket);
		}
		enet_host_flush(conn.host);
		return true;
	}

	//Set a function to call when a peer is connected
	void OnConnect(const std::function<bool(const PeerInfo&)>& callback)
	{
		onConnectFunc = callback;
	}
	//Set a function to call when a peer is disconnected
	void OnDisconnect(const std::function<void(const PeerInfo&)>& callback)
	{
		onDisconnectFunc = callback;
	}
	//Set a function to call when a packet is received
	void OnReceive(const std::function<void(const PeerInfo&, const Packet&)>& callback)
	{
		onReceiveFunc = callback;
	}
}
