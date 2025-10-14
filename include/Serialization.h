#pragma once
#include <string>
#include <cstring>
#include <span>
#include <type_traits>

#include "enet/enet.h"

namespace une
{
	//Serialize an aggregate type with contiguous memory, make sure to not have any heap data
	template<typename T>
	[[nodiscard]] char* Serialize(T type)
	{
		static_assert(std::is_standard_layout_v<T>, "Default Serialize function must only be used with contiguous data types");

		//Create the buffer and copy the object to it
		char* buff = new char[sizeof(T)];
		memset(buff, 0, sizeof(T));
		memcpy(buff, &type, sizeof(T));

		return buff;
	}

	//Deserialize an aggregate type with contiguous memory, make sure to not have any heap data
	template<typename T>
	T Deserialize(const char* data)
	{
		static_assert(std::is_standard_layout_v<T>, "Default Serialize function must only be used with contiguous data types");

		T result;
		memcpy(&result, data, sizeof(T));
		return result;
	}

	//Special case for strings
	template<>
	[[nodiscard]] inline char* Serialize<std::string>(std::string type)
	{
		char* buff = new char[type.size() + 1];
		memset(buff, 0, type.size() + 1);
		memcpy(buff, type.data(), type.size());
		return buff;
	}
	template<>
	std::string inline Deserialize<std::string>(const char* data)
	{
		std::string result(data);
		return result;
	}

	//A binary array of data typically used with ENet networking
	class Packet
	{
	public:
		enum Flag
		{
			Reliable = ENET_PACKET_FLAG_RELIABLE,
			Unreliable = ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT,
			Unsequenced = ENET_PACKET_FLAG_UNSEQUENCED
		};

		Packet() {};
		Packet(char* c, size_t s)
		{
			data.reserve(s);
			data.insert(data.end(), c, c + s);
		}

		//Deserialize an object and move the head
		template<typename T>
		T Read()
		{
			if (sizeof(T) > data.size() - readHead)
			{
				debug::LogError("Could not read type from packet, out of range!");
				return T();
			}
			const char* slice = std::span(data.data() + readHead, sizeof(T)).data();
			readHead += sizeof(T);
			return Deserialize<T>(slice);
		}
		//Deserialize an object but don't move the head
		template<typename T>
		T Peek()
		{
			if (sizeof(T) > data.size() - readHead)
			{
				debug::LogError("Could not read type from packet, out of range!");
				return T();
			}
			const char* slice = std::span(data.data() + readHead, sizeof(T)).data();
			return Deserialize<T>(slice);
		}
		//Serialize an object to the end of the data
		template<typename T>
		void Write(T obj)
		{
			char* c = Serialize(obj);
			data.insert(data.end(), c, c + sizeof(T));
		}

		//Create an enet packet from serialized data
		ENetPacket* ToENetPacket(Flag flags)
		{
			ENetPacket* packet = enet_packet_create(nullptr, data.size(), flags);
			memcpy(packet->data, data.data(), data.size());
			return packet;
		}

		std::vector<char> data;
		size_t readHead = 0;
		//These are set automatically
		int peerID = -1;
		int chanelID = 0;
	};
}
