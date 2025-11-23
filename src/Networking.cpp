#include "Networking.h"

#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#else
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include "upnpcommands.h"
#include "upnperrors.h"

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
	Connection CreateServer(uint16_t port, size_t maxPeers, size_t channelLimit)
	{
		Connection conn;
		conn.isServer = true;
		conn.address.host = ENET_HOST_ANY;
		conn.address.port = port;
		conn.host = enet_host_create(&conn.address, maxPeers, channelLimit, 0, 0);

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
	Connection CreateClient(size_t channelLimit)
	{
		Connection conn;
		conn.isServer = false;
		conn.address.host = ENET_HOST_ANY;
		conn.address.port = ENET_PORT_ANY;
		conn.host = enet_host_create(nullptr, 1, channelLimit, 0, 0);

		if (!conn.host)
		{
			debug::LogError("Failed to create ENet client host");
			return Connection();
		}

		debug::LogInfo("ENet client successfully created");
		return conn;
	}

	//Connect to an ENet server at ip:port, returns server's peer id on success
	int ConnectToServer(Connection& conn, const std::string& ip, uint16_t port, size_t numChannels)
	{
		//Resolve the host ip
		if (enet_address_set_host(&conn.address, ip.c_str()) < 0)
		{
			debug::LogError("Failed to resolve host " + ip);
			return -1;
		}
		conn.address.port = port;

		//Initiate the connection
		ENetPeer* server = enet_host_connect(conn.host, &conn.address, numChannels, 0);

		if (!server)
		{
			debug::LogError("No available peers at " + ip + ":" + std::to_string(port));
			return -1;
		}
		conn.peers[conn.nextPeerId++] = server;
		conn.closed = false;
		debug::LogInfo("Successfully connected to host at " + ip + ":" + std::to_string(port));
		return conn.nextPeerId - 1;
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
				Packet packet((char*)event.packet->data, event.packet->dataLength);
				packet.flags = (Packet::Flag)event.packet->flags;
				packet.chanelID = event.channelID;
				enet_packet_destroy(event.packet);

				//Call OnReceive if applicable
				if (onReceiveFunc)
					onReceiveFunc(info, packet);
				break;
			}

			case ENET_EVENT_TYPE_CONNECT: {
				//Get the peer id, ip, and port
				PeerInfo* info = new PeerInfo{ .id = conn.nextPeerId++, .port = event.peer->address.port };
				enet_address_get_host_ip(&event.peer->address, info->ip, sizeof(info->ip) - 1);
				event.peer->data = info;
				conn.peers[info->id] = event.peer;
				std::string infoString = (conn.isServer ? "client: " : "server: ") + std::string(info->ip) + ":" + std::to_string(info->port);

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
			for (auto peer : conn.peers)
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
	void OnReceive(const std::function<void(const PeerInfo&, Packet&)>& callback)
	{
		onReceiveFunc = callback;
	}

	//Returns the ipv4 addres of the first lan adapter with a default gateway, empty if none is found
	std::string GetLanIPv4()
	{
#ifdef _WIN32
		//Attempt to retrieve all network adapter addresses
		ULONG bufferSize = 20000;
		IP_ADAPTER_ADDRESSES* adapters = (IP_ADAPTER_ADDRESSES*)malloc(bufferSize);
		ULONG ret = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_GATEWAYS, NULL, adapters, &bufferSize);

		if (ret == NO_ERROR)
		{
			//Iterate the returned liked list of adapter addresses
			IP_ADAPTER_ADDRESSES* current = adapters;
			while (current)
			{
				if (current->FirstGatewayAddress)
				{
					//Iterate through 
					IP_ADAPTER_UNICAST_ADDRESS* ua = current->FirstUnicastAddress;
					while (ua)
					{
						SOCKADDR_IN* sa = (SOCKADDR_IN*)ua->Address.lpSockaddr;
						std::string addr(inet_ntoa(sa->sin_addr));

						//Skip reserved ips
						if(addr.find("127.") == std::string::npos && addr.find("169.254") == std::string::npos)
							return addr;

						ua = ua->Next;
					}
				}

				current = current->Next;
			}
		}
		else
		{
			debug::LogError("Failed to get LAN IP, GetAdaptersAddresses returned " + ret);
		}

		if (adapters)
			free(adapters);
#else
		//TODO: Implement for linux
		/*
		struct ifaddrs* ifaddr, * ifa;
		if (getifaddrs(&ifaddr) == -1) return NULL;
		for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
		{
			if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET)
			{
				char addr[INET_ADDRSTRLEN];
				struct sockaddr_in* sa = (struct sockaddr_in*)ifa->ifa_addr;
				inet_ntop(AF_INET, &sa->sin_addr, addr, sizeof(addr));
				if (is_private_ipv4(addr))
				{
					strncpy(ipbuf, addr, sizeof(ipbuf) - 1);
					freeifaddrs(ifaddr);
					return ipbuf;
				}
			}
		}
		freeifaddrs(ifaddr);
		*/
#endif
		return "";
	}

	//A list of all currently mapped ports by name
	std::unordered_map<std::string, UPNPPortInfo> mappedPorts;
	bool _wsaInitialized = false;

	//Attempt to open a upd port
	bool UPNPMapPort(uint16_t port, const std::string& name)
	{
		std::string sPort = std::to_string(port);
#ifdef _WIN32
		if (!_wsaInitialized)
		{
			WSADATA wsaData;
			int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (ret != UPNPCOMMAND_SUCCESS)
			{
				debug::LogError("WSAStartup failed with error " + std::string(strupnperror(ret)));
				return false;
			}
			_wsaInitialized = true;
		}
#endif

		//Find a upnp enabled router on the network
		UPNPDev* upnpDevices = nullptr;
		int ret = UPNPCOMMAND_SUCCESS;
		std::string lanIp = GetLanIPv4();
		upnpDevices = upnpDiscover(2000, lanIp.empty() ? nullptr : lanIp.c_str(), nullptr, UPNP_LOCAL_PORT_ANY, 0, 2, &ret);
		if (ret != UPNPCOMMAND_SUCCESS)
		{
			debug::LogWarning("Failed to discover UPNP devices: " + std::string(strupnperror(ret)));
			return false;
		}

		UPNPUrls upnpUrls;
		IGDdatas igdData;
		char lanAddress[64];
		char wanAddress[64];

		//Retrieve the Internet Gateway Devide of the router
		ret = UPNP_GetValidIGD(upnpDevices, &upnpUrls, &igdData, lanAddress, sizeof(lanAddress), wanAddress, sizeof(wanAddress));
		freeUPNPDevlist(upnpDevices);
		if (ret == UPNP_CONNECTED_IGD)
		{
			//Configure the port mapping to open it
			ret = UPNP_AddPortMapping(upnpUrls.controlURL, igdData.first.servicetype, sPort.c_str(), sPort.c_str(), lanAddress, name.c_str(), "UDP", "", "0");
			if (ret != UPNPCOMMAND_SUCCESS)
			{
				debug::LogWarning("Failed to map port: " + std::string(strupnperror(ret)));
				return false;
			}
		}
		else
		{
			debug::LogWarning("No valid IGD found from ip " + lanIp);
			return false;
		}

		mappedPorts[name] = UPNPPortInfo{ upnpUrls, igdData, sPort };
		debug::LogInfo("Succesfully mapped port " + sPort + " with name " + name);
		return true;
	}

	//Attempt to delete a udp port mapping by its name
	bool UPNPUnmapPort(const std::string& name)
	{
		if (!mappedPorts.contains(name))
		{
			debug::LogWarning("No mapped port named " + name);
			return false;
		}

		//Delete the port mapping
		UPNPPortInfo& info = mappedPorts[name];
		int ret = UPNP_DeletePortMapping(info.upnpUrls.controlURL, info.igdData.first.servicetype, info.port.c_str(), "UDP", 0);
		if (ret != UPNPCOMMAND_SUCCESS)
		{
			debug::LogWarning("Failed to delete port: " + std::string(strupnperror(ret)));
			return false;
		}

		debug::LogInfo("Deleted mapping on port " + info.port + " with name " + name);
		FreeUPNPUrls(&info.upnpUrls);
		return true;
	}

	//Attempt to delete all upd ports mapped by this program
	void UPNPUnmapAllPorts()
	{
		for (auto& port : mappedPorts)
		{
			UPNPUnmapPort(port.first);
		}
		mappedPorts.clear();
	}
}
