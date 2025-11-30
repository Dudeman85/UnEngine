#include "UnEngine.h"
#include "Vector.h"
#include "debug/Logging.h"
#include "Serialization.h"

#include <vector>

#include "Networking.h"
#include "renderer/UserInterface.h"

//Determines what kind of packet is sent or received
enum class MessageType : int8_t { AddPlayer, RemovePlayer, ConnectionReceived, PositionInfo, AllPositionInfo };

int main()
{
	//Set logging level and outputs
	debug::logOutputs.push_back({ new std::ofstream{"log.txt"}, false });
	debug::logOutputs[0].second = true;
	debug::verbosity = debug::Verbosity::Warning;

	bool isHost = false;
	uint16_t port = 0;
	std::string ip;
	std::string buff;

	//Get server info from user
	std::cout << "Are you hosting? (y/n) ";
	std::cin >> buff;
	isHost = buff == "y";
	if (!isHost)
	{
		std::cout << "Enter server ip: ";
		std::cin >> ip;
		std::cout << "Enter server port: ";
		std::cin >> buff;
		port = std::stoi(buff);
	}
	else
	{
		std::cout << "What port do you want to use? ";
		std::cin >> buff;
		port = std::stoi(buff);
	}

	//Setup une scene
	une::Window* window = une::CreateMainWindow(800, 600, "Window");

	une::EngineInit();
	une::enet::Init();
	une::renderer::SetBackgroundColor(une::Color(32, 32, 32));

	ecs::Entity camera = ecs::NewEntity();
	ecs::AddComponent(camera, une::Camera{ .viewport = {0, 0, 1, 1} });
	ecs::AddComponent(camera, une::Transform{ .position = {0, 0, 3000} });
	une::CameraSystem::MakeOrtho(camera, 800, 600);

	std::string assets = UNE_EXAMPLE_PROJECT_LOCATION "/assets/";
	une::Texture texture(assets + "strawberry.png");
	une::Font font(assets + "Coolvetica Rg Cond.otf", 64);

	//Networking code
	std::unordered_map<uint16_t, ecs::Entity> playerEntities;
	une::enet::Connection connection;
	uint16_t playerId = UINT16_MAX;
	uint16_t serverId = 0;
	if (isHost)
	{
		ecs::Entity player = ecs::NewEntity();
		ecs::AddComponent(player, une::SpriteRenderer{ .texture = &texture });
		ecs::AddComponent(player, une::Transform{ .position = {0, 0, 0}, .rotation = {0, 0, 0}, .scale = 5 });
		ecs::AddComponent(player, une::TextRenderer{ .font = &font, .text = "Host", .color = une::Color(200, 0, 0, 255) });
		playerEntities[UINT16_MAX] = player;
		playerId = UINT16_MAX;

		if (!une::enet::UPNPMapPort(port, "UnEngineTest"))
		{
			debug::LogError("Failed to open port, server could not be created");
			system("pause");
			return -1;
		}
		connection = une::enet::CreateServer(port, 16);

		une::enet::OnConnect([&connection, &texture, &font, &playerEntities](const une::enet::PeerInfo& info)
			{
				debug::LogInfo("New Client connected");

				//Send current state to newly connected client
				une::Packet p1;
				p1.Write(MessageType::ConnectionReceived);
				p1.Write(info.id);
				//Other player info
				p1.Write(playerEntities.size());
				for (auto player : playerEntities)
				{
					p1.Write(player.first);
					p1.Write(ecs::GetComponent<une::Transform>(player.second).position);
				}
				une::enet::SendData(connection, p1, 1, info.id);

				//Broadcast new entity to all other clients
				une::Packet p2;
				p2.Write(MessageType::AddPlayer);
				p2.Write(info.id);
				une::enet::SendData(connection, p2, 1);

				//Add new entity for connected client
				ecs::Entity e = ecs::NewEntity();
				ecs::AddComponent(e, une::SpriteRenderer{ .texture = &texture });
				ecs::AddComponent(e, une::Transform{ .position = {0, 0, 0}, .rotation = {0, 0, 0}, .scale = 5 });
				ecs::AddComponent(e, une::TextRenderer{ .font = &font, .text = std::to_string(info.id), .color = une::Color(0, 0, 200, 255) });
				playerEntities[info.id] = e;

				return true;
			});

		une::enet::OnDisconnect([&playerEntities, &connection](const une::enet::PeerInfo& info)
			{
				debug::LogInfo("Client diconnected");

				ecs::DestroyEntity(playerEntities[info.id]);
				playerEntities.erase(info.id);

				//Broadcast disconnect to all other clients
				une::Packet p2;
				p2.Write(MessageType::RemovePlayer);
				p2.Write(info.id);
				une::enet::SendData(connection, p2, 1);
			});

		une::enet::OnReceive([&playerEntities](const une::enet::PeerInfo& info, une::Packet& packet)
			{
				MessageType type = packet.Read<MessageType>().first;
				switch (type)
				{
				case MessageType::PositionInfo:
				{
					une::Vector3 pos = packet.Read<une::Vector3>().first;
					une::TransformSystem::SetPosition(playerEntities[info.id], pos);
					break;
				}
				default:
					break;
				}
			});
	}
	else
	{
		connection = une::enet::CreateClient(2);
		serverId = une::enet::ConnectToServer(connection, ip, port);

		une::enet::OnReceive([&playerEntities, &playerId, &texture, &font](const une::enet::PeerInfo& info, une::Packet& packet)
			{
				MessageType type = packet.Read<MessageType>().first;
				switch (type)
				{
				case MessageType::AddPlayer:
				{
					debug::LogInfo("AddPlayer");

					uint16_t id = packet.Read<uint16_t>().first;
					if (id != playerId)
					{
						//Add new entity for connected client
						ecs::Entity e = ecs::NewEntity();
						ecs::AddComponent(e, une::SpriteRenderer{ .texture = &texture });
						ecs::AddComponent(e, une::Transform{ .position = {0, 0, 0}, .rotation = {0, 0, 0}, .scale = 5 });
						ecs::AddComponent(e, une::TextRenderer{ .font = &font, .text = std::to_string(id), .color = une::Color(0, 0, 200, 255) });
						playerEntities[id] = e;
					}
					break;
				}
				case MessageType::RemovePlayer:
				{
					debug::LogInfo("RemovePlayer");

					uint16_t id = packet.Read<uint16_t>().first;
					ecs::DestroyEntity(playerEntities[id]);
					playerEntities.erase(id);
					break;
				}
				case MessageType::ConnectionReceived:
				{
					debug::LogInfo("ConnectionReceived");

					//Add entity for this player
					uint16_t id = packet.Read<uint16_t>().first;
					playerId = id;
					ecs::Entity e = ecs::NewEntity();
					ecs::AddComponent(e, une::SpriteRenderer{ .texture = &texture });
					ecs::AddComponent(e, une::Transform{ .position = {0, 0, 0}, .rotation = {0, 0, 0}, .scale = 5 });
					ecs::AddComponent(e, une::TextRenderer{ .font = &font, .text = std::to_string(id), .color = une::Color(0, 0, 200, 255) });
					playerEntities[playerId] = e;

					//Add entities for all other players
					size_t numPlayers = packet.Read<size_t>().first;
					for (size_t i = 0; i < numPlayers; i++)
					{
						uint16_t id2 = packet.Read<uint16_t>().first;
						une::Vector3 pos = packet.Read<une::Vector3>().first;

						//Add new entity for connected client
						ecs::Entity e2 = ecs::NewEntity();
						ecs::AddComponent(e2, une::SpriteRenderer{ .texture = &texture });
						ecs::AddComponent(e2, une::Transform{ .position = pos, .rotation = {0, 0, 0}, .scale = 5 });
						if (id2 == UINT16_MAX)
							ecs::AddComponent(e2, une::TextRenderer{ .font = &font, .text = "Host", .color = une::Color(200, 0, 0, 255) });
						else
							ecs::AddComponent(e2, une::TextRenderer{ .font = &font, .text = std::to_string(id2), .color = une::Color(0, 0, 200, 255) });
						playerEntities[id2] = e2;
					}
					break;
				}
				case MessageType::AllPositionInfo:
				{
					size_t numPlayers = packet.Read<size_t>().first;
					for (size_t i = 0; i < numPlayers; i++)
					{
						uint16_t id = packet.Read<uint16_t>().first;
						une::Vector3 pos = packet.Read<une::Vector3>().first;

						if (id != playerId)
							if (playerEntities.contains(id))
								une::TransformSystem::SetPosition(playerEntities[id], pos);
					}
					break;
				}
				default:
					break;
				}
			});
	}

	//Game loop
	while (!window->ShouldClose())
	{
		une::BeginFrame();
		une::enet::UpdateEnet(connection);
		
		if (playerEntities.contains(playerId))
		{
			if (glfwGetKey(window->glWindow, GLFW_KEY_RIGHT))
			{
				une::TransformSystem::Translate(playerEntities[playerId], 2, 0, 0);
			}
			if (glfwGetKey(window->glWindow, GLFW_KEY_LEFT))
			{
				une::TransformSystem::Translate(playerEntities[playerId], -2, 0, 0);
			}
			if (glfwGetKey(window->glWindow, GLFW_KEY_UP))
			{
				une::TransformSystem::Translate(playerEntities[playerId], 0, 2, 0);
			}
			if (glfwGetKey(window->glWindow, GLFW_KEY_DOWN))
			{
				une::TransformSystem::Translate(playerEntities[playerId], 0, -2, 0);
			}
			if (glfwGetKey(window->glWindow, GLFW_KEY_W))
			{
				une::TransformSystem::Translate(camera, 0, 10, 0);
			}
			if (glfwGetKey(window->glWindow, GLFW_KEY_A))
			{
				une::TransformSystem::Translate(camera, -10, 0, 0);
			}
			if (glfwGetKey(window->glWindow, GLFW_KEY_S))
			{
				une::TransformSystem::Translate(camera, 0, -10, 0);
			}
			if (glfwGetKey(window->glWindow, GLFW_KEY_D))
			{
				une::TransformSystem::Translate(camera, 10, 0, 0);
			}

			//Send movement packets
			if (isHost)
			{
				une::Packet packet;
				packet.Write(MessageType::AllPositionInfo);
				packet.Write(playerEntities.size());
				for (auto player : playerEntities)
				{
					packet.Write(player.first);
					packet.Write(ecs::GetComponent<une::Transform>(player.second).position);
				}
				une::enet::SendData(connection, packet, 1);
			}
			else
			{
				une::Packet packet;
				packet.Write(MessageType::PositionInfo);
				packet.Write(ecs::GetComponent<une::Transform>(playerEntities[playerId]).position);
				une::enet::SendData(connection, packet, 1, serverId);
			}
		}

		//Update engine systems and receive enet packets
		une::Update();

		une::EndFrame();
	}

	une::enet::UPNPUnmapAllPorts();
	return 0;
}
